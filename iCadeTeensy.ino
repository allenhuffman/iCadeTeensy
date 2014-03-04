/*-----------------------------------------------------------------------------

Teensy iCade Input
by Allen C. Huffman (alsplace@pobox.com)

Monitor digital inputs, then emit a USB keyboard character mapped to an iCade
button depending on the pin status. The character will be the "hold" character
for pin connected (N.O. button push) and "release" character for pin
disconnected (N.O. button released). 

Pin 11 is reserved for blinking the onboard LED as a heartbeat "we are alive"
indicator.

This software was written to allow a Teensy 2.0 to interface between arcade
buttons and an iPad via USB and Camera Connector Kit.

2012-12-04 0.0 allenh - Initial version, based on my ArduinoAIDI code.

-----------------------------------------------------------------------------*/
#define VERSION "0.0"
#define LED_OFF

//#include <EEPROM.h>
//#include <avr/wdt.h>

/*
iCade keyboard mappings.
See developer doc at: http://www.ionaudio.com/products/details/icade

   WE     YT UF IM OG
AQ<-->DC
   XZ     HR JN KP LV

Atari joystick port, looking at the male DB9 on the Atari.
See: http://old.pinouts.ru/Inputs/JoystickAtari2600_pinout.shtml

 \1 2 3 4 5/  Up Dn Lt Rt PA
  \6 7 8 9/    Bt +5  Gd PB
*/

/*
  The following I/O pins will be used as digital inputs
  for each specific iCade function.
*/
#define UP_PIN    0
#define DOWN_PIN  1
#define LEFT_PIN  2
#define RIGHT_PIN 3
#define BTN1_PIN  4
#define BTN2_PIN  5
#define BTN3_PIN  6
#define BTN4_PIN  7
#define BTN5_PIN  8
#define BTN6_PIN  9
#define BTN7_PIN  10
#define BTN8_PIN  12

/*
  The following keys are the iCade sequence (hold, release)
  for each function. Send "W" to indicate UP, and "E" when
  UP is released.
*/
#define UP_KEYS    "we"
#define DOWN_KEYS  "xz"
#define LEFT_KEYS  "aq"
#define RIGHT_KEYS "dc"
#define BTN1_KEYS  "yt"
#define BTN2_KEYS  "uf"
#define BTN3_KEYS  "im"
#define BTN4_KEYS  "og"
#define BTN5_KEYS  "hr"
#define BTN6_KEYS  "jn"
#define BTN7_KEYS  "kp"
#define BTN8_KEYS  "lv"

#define DI_PIN_COUNT  12   // 12 pins used.
#define DI_PIN_START  1    // First I/O pin.
#define DI_PIN_END    20   // Last I/O pin.

byte myPins[DI_PIN_COUNT] =
  {UP_PIN, DOWN_PIN, LEFT_PIN, RIGHT_PIN,
  BTN1_PIN, BTN2_PIN, BTN3_PIN, BTN4_PIN,
  BTN5_PIN, BTN6_PIN, BTN7_PIN, BTN8_PIN};

char iCadeKeymap[][DI_PIN_COUNT] =
  {UP_KEYS, DOWN_KEYS, LEFT_KEYS, RIGHT_KEYS,
  BTN1_KEYS, BTN2_KEYS, BTN3_KEYS, BTN4_KEYS,
  BTN5_KEYS, BTN6_KEYS, BTN7_KEYS, BTN8_KEYS};

char iCadeDesc[][DI_PIN_COUNT] =
  {"Up", "Down", "Left", "Right",
  "Btn1", "Btn2", "Btn3", "Btn4",
  "Btn5", "Btn6", "Btn7", "Btn8"};

/* We want a very short debounce delay for an arcade controller. */
#define DI_DEBOUNCE_MS 10 // 100ms (1/10th second)

#define LED_PIN 11
#define LEDBLINK_MS 1000

/*---------------------------------------------------------------------------*/

/* For I/O pin status and debounce. */
unsigned int  digitalStatus[DI_PIN_COUNT];          // Last set PIN mode.
unsigned long digitalDebounceTime[DI_PIN_COUNT];    // Debounce time.
//unsigned long digitalCounter[DI_PIN_COUNT];         // Times button pressed.
unsigned int  digitalDebounceRate = DI_DEBOUNCE_MS; // Debounce rate.

/* For the blinking LED (heartbeat). */
unsigned int  ledStatus = LOW;             // Last set LED mode.
unsigned long ledBlinkTime = 0;            // LED blink time.
unsigned int  ledBlinkRate = LEDBLINK_MS;  // LED blink rate.

unsigned int pinsOn = 0;

/*---------------------------------------------------------------------------*/

void setup()
{
  // Just in case it was left on...
  //wdt_disable();

  // Initialize the serial port.
  Serial.begin(9600);

  // Docs say this isn't necessary for Uno.
  //while(!Serial) { }

  showHeader();
  
  // Initialize watchdog timer for 2 seconds.
  //wdt_enable(WDTO_4S);

  // LOW POWER MODE!
  // Pins default to INPUT mode. To save power, turn them all to OUTPUT
  // initially, so only those being used will be turn on. See:
  // http://www.pjrc.com/teensy/low_power.html
  for (int thisPin=0; thisPin < DI_PIN_COUNT; thisPin++ )
  {
    pinMode(thisPin, OUTPUT);
  }

  // Disable Unused Peripherals
  ADCSRA = 0;

  // Initialize the pins and digitalPin array.
  for (int thisPin=0; thisPin < DI_PIN_COUNT; thisPin++ )
  {
    // Set pin to be digital input using pullup resistor.
    pinMode(myPins[thisPin], INPUT_PULLUP);
    // Set the current initial pin status.
    digitalStatus[thisPin] = HIGH; //digitalRead(thisPin+DI_PIN_START);
    // Clear debounce time.
    digitalDebounceTime[thisPin] = 0;
    //digitalCounter[thisPin] = 0;
  }

  // Set LED pin to output, since it has an LED we can use.
  pinMode(LED_PIN, OUTPUT);

  Serial.println("Ready.");
}

/*---------------------------------------------------------------------------*/

void loop()
{
  // Tell the watchdog timer we are still alive.
  //wdt_reset();

#ifndef LED_OFF
  // LED blinking heartbeat. Yes, we are alive.
  if ( (long)(millis()-ledBlinkTime) >= 0 )
  {
    // Toggle LED.
    if (ledStatus==LOW)  // If LED is LOW...
    {
      ledStatus = HIGH;  // ...make it HIGH.
    } else {
      ledStatus = LOW;   // ...else, make it LOW.
    }
    // Set LED pin status.
    if (pinsOn==0) digitalWrite(LED_PIN, ledStatus);
    // Reset "next time to toggle" time.
    ledBlinkTime = millis()+ledBlinkRate;
  }
#endif

  // Check for serial data.
  if (Serial.available() > 0) {
    // If data ready, read a byte.
    int incomingByte = Serial.read();
    // Parse the byte we read.
    switch(incomingByte)
    {
      case '?':
        showStatus();
        break;
      default:
        break;
    }
  }
  
  /*-------------------------------------------------------------------------*/

  // Loop through each Digital Input pin.
  for (int thisPin=0; thisPin < DI_PIN_COUNT; thisPin++ )
  {
    // Read the pin's current status.
    unsigned int status = digitalRead(myPins[thisPin]);

    // In pin status has changed from our last toggle...
    if (status != digitalStatus[thisPin])
    {
      // Remember when it changed, starting debounce mode.
      // If not currently in debounce mode,
      if (digitalDebounceTime[thisPin]==0)
      {
        // Set when we can accept this as valid (debounce is considered
        // done if the time gets to this point with the status still the same).
        digitalDebounceTime[thisPin] = millis()+digitalDebounceRate;
      }

      // Check to see if we are in debounce detect mode.
      if (digitalDebounceTime[thisPin]>0)
      {
        // Yes we are. Have we delayed long enough yet?
        if ( (long)(millis()-digitalDebounceTime[thisPin]) >= 0 )
        {
            // Yes, so consider it switched.
            // If pin is Active LOW,
            if (status==LOW)
            {
              // Emit BUTTON PRESSED string.
              Serial.print(iCadeDesc[thisPin]);
              Serial.print(" pressed  (sending ");
              Serial.print(iCadeKeymap[thisPin][0]);
              Serial.println(" to iCade).");
              Keyboard.print(iCadeKeymap[thisPin][0]);
              //digitalCounter[thisPin]++;
              pinsOn++;
#ifndef LED_OFF
              digitalWrite(LED_PIN, HIGH);
#endif
            } else {
              // Emit BUTTON RELEASED string.
              Serial.print(iCadeDesc[thisPin]);
              Serial.print(" released (sending ");
              Serial.print(iCadeKeymap[thisPin][1]);
              Serial.println(" to iCade).");
              Keyboard.print(iCadeKeymap[thisPin][1]);
              if (pinsOn>0) pinsOn--;
              if (pinsOn==0) digitalWrite(LED_PIN, LOW);
            }
            // Remember current (last set) status for this pin.
            digitalStatus[thisPin] = status;
            // Reset debounce time (disable, not looking any more).
            digitalDebounceTime[thisPin] = 0;
        } // End of if ( (long)(millis()-digitalDebounceTime[thisPin]) >= 0 )
        
      } // End of if (digitalDebounceTime[thisPin]>0)
    }
    else // No change? Flag no change.
    {
      // If we were debouncing, we are no longer debouncing.
      digitalDebounceTime[thisPin] = 0;
    }
  } // End of (int thisPin=0; thisPin < DI_PIN_COUNT; thisPin++ )
}

/*---------------------------------------------------------------------------*/

void showHeader()
{
  int i;
  // Emit some startup stuff to the serial port.
  Serial.print("iCadeTeensy ");
  Serial.print(VERSION);
  Serial.println(" by Allen C. Huffman (alsplace@pobox.com)");
  Serial.print(DI_PIN_COUNT);
  Serial.print(" DI Pins (");
  for (i=0; i<DI_PIN_COUNT; i++)
  {
    Serial.print(myPins[i]);
    Serial.print("=");
    Serial.print(iCadeDesc[i]);
    Serial.print(" ");
  }
  Serial.print("), ");
  Serial.print(digitalDebounceRate);
  Serial.println("ms Debounce.");
}

/*---------------------------------------------------------------------------*/

void showStatus()
{
  showDigitalInputStatus();
}

/*---------------------------------------------------------------------------*/

void showDigitalInputStatus()
{
  Serial.print("DI: ");

  for (int thisPin=0; thisPin < DI_PIN_COUNT; thisPin++ )
  {
    // Read the pin's current status.
    Serial.print(iCadeDesc[thisPin]);
    Serial.print("=");
    Serial.print(digitalRead(myPins[thisPin]));
    Serial.print(" ");
    //Serial.print(" (");
    //Serial.print(digitalCounter[thisPin]);
    //Serial.print(") ");
  }
  Serial.println("");
}

/*---------------------------------------------------------------------------*/

// End of file.

