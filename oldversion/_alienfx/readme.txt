=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
           Alienware AlienFX Light Manipulation Program
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Building source code requires libusb-1.0 headers.
If you are missing them on Debian, issue command:
  apt-get install libusb-1.0-0-dev

To build source:
  cd alienfx/
  make

To install to userland:
  sudo make install
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Usage:    alienfx [-r n] [-b b] [-g n] [-v] [-u]
   or     alienfx -h
  -r <n>     Red pigment
  -g <n>     Green pigment
  -b <n>     Blue pigment
  -v         Show verbose debug output
  -u         Requires -v, show USB debug if needed
  -h         This helpful message
Pigments must be more than zero and less than 256.
If called with no arguments the lights will turn off.
Examples:
  Bright red:     alienfx -r 255
  Bright green:   alienfx -g 255
  Bright blue:    alienfx -b 255
  Bright pink:    alienfx -r 255 -b 255
  Bright yellow:  alienfx -r 255 -g 255
  Bright aqua:    alienfx -g 255 -b 255
  Night mode:     alienfx -r 16 -g 16 -b 16
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Supported models:  Now, before you complain about it not turning your
kit into a geekoid disco ball, respect the fact many revisions of the
AlienWare light chip have been made.   Generally, the 'lsusb' command
will reveal the product-ID of your chip.
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
I have built this on 64-bit hardware, though I include 32-bit support
for completeness - and this is untested!  64-bit is known working.
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
     This AlienFX program is not endorsed by Alienware or Dell.
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

