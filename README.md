iCadeTeensy
===========

Digital input to iCade USB for Teensy 2.0, Leonardo, etc.

This code reads digital inputs of a Teensy 2.0 and emits USB keyboard presses in iCade format.

NOTES
=====

Here is the writeup I posted to the www.prjc.com forums documenting the research I did for this project:

http://forum.pjrc.com/threads/2271-Tennsy-2-0-as-an-iCade-emulator-for-iPad-(with-an-Atari-2600-joystick)?highlight=atari

This is a summary of my efforts to get a Teensy (or Arduino Leonardo) acting as an iCade interface between arcade joysticks (or even an Atari 2600 joystick) and an iPad. I present all my documentation here in case it is of use to others.

First, some quick background on how I ended up here. In October 2012, I was helping with some haunted house attractions in Des Moines, Iowa, and we were having difficulty getting our $150 input boards to work (which would be used to trigger effects in a haunted house). The software we were evaluating was able to read serial commands, and I thought perhaps something like an Arduino might work. After a $35 trip to RadioShack, I was writing my first Arduino code and we ended up driving two of our attractions with them. While my experience with these types of hardware is new, I have a background with computers back to the VIC-20, and I used to work for Microware, the company that made the OS-9 embedded operating system.

My former understanding was that the iCade was a bluetooth device, and I has assumed the only way to interact with it was via Bluetooth. While there were Bluetooth Shields for the Arduinos, they added quite a bit of cost so I never persued them. Recently, I ran across a blogger that discovered the iCade keyboard commands worked over a USB connection (via Apple's Camera Connector Kit). He had been creating a USB interface for a Super Nintendo controller:

http://biosrhythm.com/?p=933

This led me to think I could use an Arduino and hook the USB side to my iPad and roll my own interface. I soon found out that the USB port on Arduino was just a serial device and could not act like a USB keyboard. Further research led me to discover that you could build a USB interface with a few parts at a cost of a few bucks:

http://petrockblog.wordpress.com/2012/05/19/usb-keyboard-with-arduino-and-v-usb-library-an-example/

Using that, and some open source libraries, you could write USB keyboard commands that a computer would see as input.

I then found out that the Arduino Duo could have the firmware reloaded with a version that made it look like a USB keyboard/mouse device, but to load code, it had to be running the serial port firmware. It looked like a hassel (load code, load USB keyboard firmware, test, load USB serial firmware, load new code, repeat) but it wouldn't require buying any parts or building anything.

Then, I read a blog comment thread somewhere that mentioned Teensy, and I ended up on this site and found there was a $16 ($19 with header pins) Arduino-like computer that has USB keyboard/mouse support built in. Last Friday, I e-mailed in a question, which was quickly answered, then placed an order. The item was shipped the same day and I had it in my post office box the following Monday (wow!).

On Monday, I got the Teensy up and running and, using the Teensyduino add-on, I was able to quickly modify my haunted house code to send iCade keys instead of my previous serial commands. I had used "A" when a switch was pressed, and "a" when it was released, so my changes were very minor - basically, just making it use a Keyboard.print() with the iCade sequence instead of a Serial.print().

The next issue was hooking it up to an iPad for testing. I was going to need a powered hub, since the iPad provided very little power (20mA, I think). When first released, iOS would put out 100mA, but this was changed with iOS 4.2:

http://9to5mac.com/2010/12/03/ios-4-2-emits-less-usb-power-on-ipad-camera-connection-kit-crippled/

I priced USB hubs and was prepared to buy a cheap one, and then wondered just how much power Teensy 2.0 actually used:

http://www.pjrc.com/teensy/low_power.html

Might I actually be able to run it off the iPad USB power? I gave it a try...and it worked! The iPad complained about an unsupported USB device, as expected, but the push buttons I had wired up on a breadboard were correctly sending out up, down, left, right and fire buttons to the Atari's Greatest Hits app. I was stunned. (I was so paranoid of power drain, I even removed the blinking LED status indicator from my program.)

The next day (today), I dug out an Atari 2600 joystick (a replica that came with the Flashback 2 unit). Using just some female-end connector wires (that came from RadioShack), I was able to wire ground and five I/O pins from the Teensy header pins to the joystick's DB9 connector and successfully use it in the Atari app, Activision app, and Midway Arcade app. Some apps recognized it automatically, and some required setting it to "Arcade Controller." (Apple doesn't let the apps call it iCade mode, apparently.)

So, Apple Camera Connector Kit, USB to USB mini cable, Teensy 2.0, and some wires to an Atari Joystick was all I needed to use an joystick with the iPad. Next I would like to build my own arcade surface with a joystick and eight buttons and have my own home-made iCade controller. Software could allow toggling between iCade mode and PC mode, like the SNES-USB project does.

A few final notes about what I discovered. I knew one unusual thing about the iCade was that it send one character to button-down, and another one for button-up. If you moved the joystick up, it would send a "W". Once you released the joystick (back to center), it would send an "E". I expect they had a good reason for needing this, but I thought USB keyboards already had key down, key up messages.

Anyway, due to my assumption that iCade was a Bluetooth-only device, and needed to be paired with the iOS device to work, I expected there was some setup/handshake that apps did which would prevent them from seeing anything else. As it turns out, apps apparently just read keypresses from keyboards (bluetoorh or USB) and that's it. I was surprised to find that, when using a Bluetooth keyboard on my iPad, I could just press those iCade keys and make apps respond as if they had an iCade hooked up! (It's not very practical; the keymap is scattered across the keyboard, and having to press one key to indicate up, then another to indicate stop, is not very fun.)

Perhaps this writeup will be of use to someone. I plan to post my work to my www.appleause.com blog, but wanted to share it here first.

The final tidbit was discovering that a few Arduinos can also toggle to keyboard USB, like Teensy does. I have picked up an Arduino Leonardo for $24.95 and plan to make my source code work with it as well (mostly just changing I/O pin mappings).

If anyone is interested, I will post my work-in-progress source code.

...okay, no this is all here so others may be able to find all the bits and pieces I was looking for in one place.

-- Allen in Iowa

Here are other some related links:

iCade product page:
http://www.ionaudio.com/products/details/icade

iCade developer information (with keymap):
http://www.ionaudio.com/downloads/ION%20Arcade%20Dev%20Resource%20v1.5.pdf
