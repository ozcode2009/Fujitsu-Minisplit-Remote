# Fujitsu Minisplit Remote
This was created when a remote for a Fujitsu Minisplit was lost. It is based off the implementation of the Fujitsu protocol in the IRremote8266 project as well as documentation of the protocol from a PDF on Remote Central uploaded by David Abrams, but rebuilt for a Raspberry Pi Pico.

## Usage
To use this remote, upload the code to a Pi Pico. Then, connect to the Pi Pico with a serial console.

Send to the Pi Pico in integers, comma separated, with no spaces, and in accordance with the PDF in the repo:
- The desired temperature
- The integer one (1) to indicate that the unit should be on
- The desired mode
- The desired swing state
- The desired fan speed

Hex codes for temperatures, modes, swing states, and fan speeds can be found in the PDF in the repo; they will have to be converted to integers to use.

To turn the unit off, simply type "off" into the serial console.

Each second, the Pi Pico will bit-bang the newest data it has received through the IR_LED_PIN.

## Credits
The PDF used was from this page on Remote Central and is credited to David Abrams: https://files.remotecentral.com/view/7198-18795-1/fujitsu_air_conditioners_generic.html
