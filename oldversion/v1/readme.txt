=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                Alienware AlienFX Light Manipulation Program
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Building source code requires libusb-1.0 headers.
If you are missing them on Debian, issue command:
  apt-get install libusb-1.0-0-dev

To build source:
  cd alienfx/
  make

To install to userland:
  sudo make install
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Alienware AlienFX Light Manipulation Program v1.0
Usage:    alienfx [-c cmd] [-p hex] [-u] [-v]
   or     alienfx -R
   or     alienfx -h
  -c <M>[RGB[RGB[ZZZZZZZZZZZZZZZZ]]]   Colour command
  -d <num>   Set morph/flash delay time. (1 to 10)
  -h         This helpful message
  -p <num>   Apply a preset. Use '0' to get a list. Implies ignorance of -c.
  -P <hex>   Specify the hex ProductID of lightchip
  -r         Reset chip; software reset of lightchip
  -R         Reboot chip; hardware reset of lightchip
  -s         Permanently save the settings into the EEPROM
  -t <num>   Increase USB timing delay by increment (1 to 100)
  -u         Show USB debug if needed
  -v         Show verbose debug output
Each colour command is made up of M (mode), RGB (colour) and Z (zones).
Mode is either B=Blink, M=Morph or F=Fixed
RGB is three single-digits of hex. Specify two colours, even if 2nd unused.
Zones are either 1 or 0. 1 is on, 0 is off. There are sixteen zones.
Zone default is to set all zones to on(1).
If called with no arguments the lights will silently turn off.
See 'man alienfx' for more information.
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Supported models:  Now, before you complain about it not turning your kit
into a flashing disco pad, respect the fact many revisions of the AlienWare
light chip have been made.   Generally, the 'lsusb' command will reveal the
product-ID of your chip.

Currently, the follow models are known and programmed for:
      Model                  Vendor:Product
	M11X                   187c:0514
	M11X R3                187c:0522	
	M15X                   187c:0512
	M17X                   187c:0512
	Aurora (non-ALX)       187c:0513
	Area51                 187c:0511

If your USB device is not listed, please output the result of the 
'lsusb -v' (run it as root) and mail it to <simon@tibble.net> and I
will do my best to work with you by email to get the program working
on your equipment.

I have built this on 64-bit hardware, though I include a 32-bit binary for
completeness - and this is untested!  64-bit is known working.
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
         This AlienFX program is not endorsed by Alienware or Dell.
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
