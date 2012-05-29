/*

  ** AlienFX Light Control Program **
  -- Controls the accessory lights for AlienWare M11X R3 --

*/

#include <stdio.h>
#include <cstdlib>
#include <string>
#include <string.h>
#include <iostream>
#include <libusb-1.0/libusb.h>

using namespace std;

const int SEND_REQUEST_TYPE = 0x21;
const int SEND_REQUEST = 0x09;
const int SEND_VALUE = 0x202;
const int SEND_INDEX = 0x00;
const int READ_REQUEST_TYPE = 0xa1;
const int READ_REQUEST = 0x01;
const int READ_VALUE = 0x101;
const int READ_INDEX = 0x0;
const int ALIENFX_VID = 0x187c;
const int ALIENFX_PID_AURORA_NONALX = 0x513;
const int ALIENFX_PID_ALLPOWERFULL_M11XR3 = 0x522;
const int ALIENFX_PID_ALLPOWERFULL_M11X = 0x514;
const int ALIENFX_PID_ALLPOWERFULL = 0x512;
const int ALIENFX_PID_AREA51 = 0x511;
const int AURORA_NONALX = 5;
const int ALLPOWERFULL_M11XR3 = 4;
const int ALLPOWERFULL_M11X = 3;
const int ALLPOWERFULL = 2;
const int AREA51 = 1;
const int NOT_FOUND = 0;
const int SEND_DATA_SIZE = 9;
//const int READ_DATA_SIZE (alienFXid == ALLPOWERFULL_ALIENFX ? 8 : 9) //is this needed?
const int READ_DATA_SIZE = 9;

const int STATE_BUSY = 0x11;
const int STATE_READY = 0x10;
const int STATE_UNKNOWN_COMMAND = 0x12;
const int SUPPORTED_COMMANDS = 15;
const int COMMAND_END_STORAGE = 0x00;// = End Storage block (See storage)
const int COMMAND_SET_MORPH_COLOR = 0x01;// = Set morph color (See set commands)
const int COMMAND_SET_BLINK_COLOR = 0x02;// = Set blink color (See set commands)
const int COMMAND_SET_COLOR = 0x03;// = Set color (See set commands)
const int COMMAND_LOOP_BLOCK_END = 0x04;// = Loop Block end (See loops)
const int COMMAND_TRANSMIT_EXECUTE = 0x05;// = End transmition and execute
const int COMMAND_GET_STATUS = 0x06;// = Get device status (see get device status)
const int COMMAND_RESET = 0x07;// = Reset (See reset)
const int COMMAND_SAVE_NEXT = 0x08;// = Save next instruction in storage block (see storage)
const int COMMAND_SAVE = 0x09;// = Save storage data (See storage)
const int COMMAND_BATTERY_STATE = 0x0F;// = Set batery state (See set commands)
const int COMMAND_SET_SPEED = 0x0E;// = Set display speed (see set speed)
const int RESET_TOUCH_CONTROLS = 0x01;
const int RESET_SLEEP_LIGHTS_ON = 0x02;
const int RESET_ALL_LIGHTS_OFF = 0x03;
const int RESET_ALL_LIGHTS_ON = 0x04;
const int DATA_LENGTH = 9;
const int FILL_BYTE = 0x00;
const int BLOCK_LOAD_ON_BOOT = 0x01;
const int BLOCK_STANDBY = 0x02;
const int BLOCK_AC_POWER = 0x05;
const int BLOCK_CHARGING = 0x06;
const int BLOCK_BATT_CRITICAL = 0x07;
const int BLOCK_BAT_POWER = 0x08;

const int START_BYTE_M11XR3 = 0x02;

const int START_BYTE_M11X = 0x02;
const int REGION_M11X_RIGHT_KEYBOARD = 0x0001;
const int REGION_M11X_RIGHT_SPEAKER = 0x0020;
const int REGION_M11X_LEFT_SPEAKER = 0x0040;
const int REGION_M11X_ALIEN_NAME = 0x0100;
const int REGION_M11X_MEDIA_BAR = 0x0800;
const int REGION_M11X_POWER_BUTTON = 0x6000;

const int START_BYTE_NONALX = 0x02;

const int START_BYTE_ALLP = 0x02;
const int REGION_ALLP_RIGHT_KEYBOARD = 0x0001;
const int REGION_ALLP_RIGHT_SPEAKER = 0x0020;
const int REGION_ALLP_LEFT_SPEAKER = 0x0040;
const int REGION_ALLP_ALIEN_NAME = 0x0100;
const int REGION_ALLP_MIDDLE_RIGHT_KEYBOARD = 0x0002;
const int REGION_ALLP_LEFT_KEYBOARD = 0x0004;
const int REGION_ALLP_MIDDLE_LEFT_KEYBOARD = 0x0008;
const int REGION_ALLP_POWER_BUTTON_2 = 0x0010;
const int REGION_ALLP_ALIEN_HEAD = 0x0080;
const int REGION_ALLP_TOUCH_PAD = 0x0200;
const int REGION_ALLP_MEDIA_BAR = 0x1c00;
const int REGION_ALLP_POWER_BUTTON = 0x2000;
const int REGION_ALLP_POWER_BUTTON_EYES = 0x4000;

const char *AREA51_KEY_BOARD = "KeyBoard";
const char *AREA51_KB = "KB";
const char *AREA51_LIGHTPIPE = "Lightpipe";
const char *AREA51_LP = "LP";
const int START_BYTE_AREA51 = 0x00;
const int REGION_AREA51_TOUCH_PAD = 0x000001;
const int REGION_AREA51_LIGHTPIPE = 0x000020;
const int REGION_AREA51_ALIEN_LOGO = 0x000080;
const int REGION_AREA51_ALIEN_HEAD = 0x000100;
const int REGION_AREA51_KEY_BOARD = 0x000400;
const int REGION_AREA51_TOUCH_PANEL = 0x010000;
const int REGION_AREA51_POWER_BUTTON = 0x008000;

libusb_context* context;
libusb_device_handle* alienFx;
int alienFXid, START_BYTE;
bool user_is_root, args_ok, debug=false, debug2=false, help=false;
int a_colour[3];
char commandline;

void detach(libusb_device_handle* device){
  int r = libusb_kernel_driver_active(device, 0);
  if (r == 1)
    r = libusb_detach_kernel_driver(device, 0);
}

void attach(libusb_device_handle* device){
  libusb_attach_kernel_driver(device, 0);
}


int WriteDevice(unsigned char* pData, int pDataLength){
 if (debug2) { for (int i=0;i<pDataLength;i++) { printf("[%i]",pData[i]); } printf("\n"); }
  return libusb_control_transfer(alienFx, SEND_REQUEST_TYPE, SEND_REQUEST, SEND_VALUE, SEND_INDEX, pData,pDataLength,0);
}


int ReadDevice(unsigned char* pData, int pDataLength){
  return libusb_control_transfer(alienFx, READ_REQUEST_TYPE, READ_REQUEST, READ_VALUE, READ_INDEX, pData,pDataLength,0);
 if (debug2) { for (int i=0;i<pDataLength;i++) { printf("[%i]",pData[i]); } printf("\n"); }
}

int AlienfxInit(){
  libusb_init(&context);
  if (debug2) libusb_set_debug (context, 3);
  alienFx = libusb_open_device_with_vid_pid(context, ALIENFX_VID, ALIENFX_PID_ALLPOWERFULL);
  //check if we found the device by attempting to open the device
  alienFXid = ALLPOWERFULL;
  if(alienFx == NULL){
    alienFx = libusb_open_device_with_vid_pid(context, ALIENFX_VID, ALIENFX_PID_AREA51);
    alienFXid = AREA51;
    START_BYTE = START_BYTE_AREA51;
    if(alienFx == NULL){
      alienFx = libusb_open_device_with_vid_pid(context, ALIENFX_VID, ALIENFX_PID_ALLPOWERFULL_M11X);
      alienFXid = ALLPOWERFULL_M11X;
      START_BYTE = START_BYTE_M11X;
      if(alienFx == NULL){
        alienFx = libusb_open_device_with_vid_pid(context, ALIENFX_VID, ALIENFX_PID_ALLPOWERFULL_M11XR3);
        alienFXid = ALLPOWERFULL_M11XR3;
        START_BYTE = START_BYTE_M11XR3;
        if(alienFx == NULL){
          alienFx = libusb_open_device_with_vid_pid(context, ALIENFX_VID, ALIENFX_PID_AURORA_NONALX);
          alienFXid = AURORA_NONALX;
          START_BYTE = START_BYTE_NONALX;
          if(alienFx == NULL) return NOT_FOUND;
        }
      }
    }
  }
  detach(alienFx);
  int res = libusb_claim_interface(alienFx,0);
  if(res < 0) return NOT_FOUND;
  return alienFXid;
}

void AlienfxDeinit(){
  libusb_release_interface(alienFx, 0);
  attach(alienFx);
  libusb_close(alienFx);
  libusb_exit(context);
}

void print_help() {
	printf("Alienware AlienFX Light Manipulation Program v0.9\n");
	printf("Usage:    alienfx [-r n] [-b b] [-g n] [-v] [-u]\n");
	printf("   or     alienfx -h\n");
	printf("  -r <n>     Red pigment\n");
	printf("  -g <n>     Green pigment\n");
	printf("  -b <n>     Blue pigment\n");
	printf("  -v         Show verbose debug output\n");
	printf("  -u         Requires -v, show USB debug if needed\n");
	printf("  -h         This helpful message\n");
	printf("Pigments must be more than zero and less than 256.\n");
	printf("If called with no arguments the lights will turn off.\n");
	printf("Examples:\n");
	printf("  Bright red:     alienfx -r 255\n");
	printf("  Bright green:   alienfx -g 255\n");
	printf("  Bright blue:    alienfx -b 255\n");
	printf("  Bright pink:    alienfx -r 255 -b 255\n");
	printf("  Bright yellow:  alienfx -r 255 -g 255\n");
	printf("  Bright aqua:    alienfx -g 255 -b 255\n");
	printf("  Night mode:     alienfx -r 16 -g 16 -b 16\n");
}

bool parse_arguments(int argc, char *argv[], char **envp) {
	//Trap username from environment
	char *var,tag[5],uservar[9],arg[2];
        while(*envp != NULL) {
		var = *(envp++);
		strncpy(tag, var, 5);
		tag[5] = 0;
		if (!strcmp(tag,"USER=")) strncpy(uservar, var, 9);
	}
	uservar[9] = 0;
	if (!strcmp(uservar, "USER=root")) user_is_root=true;
	//commandline = **(argv++); //called ELF is 1st arg (this don't work for some reason. char copy?)
        while(*argv != NULL) {
		var = *(argv++);
		if (!strcmp(var,"-r")) { a_colour[1] = atoi(*(argv++)); } //red
		if (!strcmp(var,"-g")) { a_colour[2] = atoi(*(argv++)); } //green
		if (!strcmp(var,"-b")) { a_colour[3] = atoi(*(argv++)); } //blue
		if (!strcmp(var,"-v")) { debug = true; }   //alienfx debugging
		if (!strcmp(var,"-u")) { debug2 = true; }  //libusb debugging
		if (!strcmp(var,"-h")) { help = true;   }  //help output
	}
	if (a_colour[1] < 0) a_colour[1] = 0; if (a_colour[1] > 255) a_colour[1] = 255;
	if (a_colour[2] < 0) a_colour[2] = 0; if (a_colour[2] > 255) a_colour[2] = 255;
	if (a_colour[3] < 0) a_colour[3] = 0; if (a_colour[3] > 255) a_colour[3] = 255;
	return true;
}

bool usb_is_ready() {
	bool ready;
	unsigned char thedata[DATA_LENGTH];
	while (!ready) {
		memset(thedata, FILL_BYTE, DATA_LENGTH);
		thedata[0] = START_BYTE;
		thedata[1] = COMMAND_GET_STATUS;
		WriteDevice(thedata, DATA_LENGTH);
		memset(thedata, FILL_BYTE, DATA_LENGTH);
		ReadDevice(thedata, DATA_LENGTH);
		ready = (thedata[0] == STATE_READY);
		if (debug) {
			if (ready) printf("Debug: Device now ready\n");
			if (thedata[0] == STATE_BUSY) printf("Debug: Device is busy\n");
			if (thedata[0] == STATE_UNKNOWN_COMMAND) printf("Debug: Device claims 'unknown command'\n");
		}
		if (!ready) usleep(250000); else usleep(10000);
	}
	return ready;
}

void afx_load_on_boot() {
// this tells the chip to restore command buffer on laptop power-on
	if (debug) printf("Debug: Instruction store\n");
	unsigned char thedata[DATA_LENGTH];
	memset(thedata, FILL_BYTE, DATA_LENGTH);
	thedata[0] = START_BYTE;
	thedata[1] = COMMAND_SAVE_NEXT;
//	thedata[2] = BLOCK_LOAD_ON_BOOT;
	if (usb_is_ready()) WriteDevice(thedata, DATA_LENGTH);
	usleep(50000);
}

void afx_done() {
	if (debug) printf("Debug: Instruction done\n");
	unsigned char thedata[DATA_LENGTH];
	memset(thedata, FILL_BYTE, DATA_LENGTH);
	thedata[0] = START_BYTE;
	thedata[1] = COMMAND_LOOP_BLOCK_END;
	if (usb_is_ready()) WriteDevice(thedata, DATA_LENGTH);
	usleep(50000);
}

void reset_device(char type) {
// this turns the lights off, empties the data buffer for new schema
	if (debug) printf("Debug: Device reset\n");
	unsigned char thedata[DATA_LENGTH];
	memset(thedata, FILL_BYTE, DATA_LENGTH);
	thedata[0] = START_BYTE;
	thedata[1] = COMMAND_RESET;
	thedata[2] = type;
	WriteDevice(thedata, DATA_LENGTH);
	usleep(50000); //won't allow other commands after reset unless delay, wtf?
}

void afx_save() {
	//tell keychip to store data
	if (debug) printf("Debug: Device save\n");
	unsigned char thedata[DATA_LENGTH];
	memset(thedata, FILL_BYTE, 9);
	thedata[0] = START_BYTE;
	thedata[1] = COMMAND_SAVE;
	if (usb_is_ready()) WriteDevice(thedata, DATA_LENGTH);
	usleep(50000);
}

void afx_commit() {
	//tell keychip to make it effective
	if (debug) printf("Debug: Device commit\n");
	unsigned char thedata[DATA_LENGTH];
	memset(thedata, FILL_BYTE, 9);
	thedata[0] = START_BYTE;
	thedata[1] = COMMAND_TRANSMIT_EXECUTE;
	if (usb_is_ready()) WriteDevice(thedata, DATA_LENGTH);
	usleep(150000); //won't set lights unless delay, wtf?
}

void afx_set(char r, char g, char b) {
	if (debug) printf("Debug: Set colour\n");
	int RG = r & 0xF0;
	RG = RG | ((g >> 4) & 0x0F);
	int B = b & 0xF0;
/*	char region;
// regions not used - didn't work on my kit (M11xR3), sorry.
	if (alienFXid == ALLPOWERFULL_M11X_ALIENFX)
		region = REGION_M11X_RIGHT_KEYBOARD & REGION_M11X_RIGHT_SPEAKER & REGION_M11X_LEFT_SPEAKER &
				REGION_M11X_ALIEN_NAME & REGION_M11X_MEDIA_BAR & REGION_M11X_POWER_BUTTON;
	if (alienFXid == ALLPOWERFULL_ALIENFX)
		region = REGION_ALLP_RIGHT_KEYBOARD & REGION_ALLP_RIGHT_SPEAKER & REGION_ALLP_LEFT_SPEAKER &
				REGION_ALLP_ALIEN_NAME & REGION_ALLP_MIDDLE_RIGHT_KEYBOARD & REGION_ALLP_LEFT_KEYBOARD &
				REGION_ALLP_MIDDLE_LEFT_KEYBOARD & REGION_ALLP_POWER_BUTTON_2 & REGION_ALLP_ALIEN_HEAD &
				REGION_ALLP_TOUCH_PAD & REGION_ALLP_MEDIA_BAR & REGION_ALLP_POWER_BUTTON &
				REGION_ALLP_POWER_BUTTON_EYES;
	if (alienFXid == AREA51_ALIENFX)
		region = REGION_AREA51_TOUCH_PAD & REGION_AREA51_LIGHTPIPE & REGION_AREA51_ALIEN_LOGO &
				REGION_AREA51_ALIEN_HEAD & REGION_AREA51_KEY_BOARD & REGION_AREA51_TOUCH_PANEL &
				REGION_AREA51_POWER_BUTTON;
	char b1 = (region >> 16) & 0xFF;
	char b2 = (region >> 8) & 0xFF;
	char b3 = region & 0xFF; */
	unsigned char thedata[DATA_LENGTH];
	memset(thedata, FILL_BYTE, DATA_LENGTH);
	thedata[0] = START_BYTE;
	thedata[1] = COMMAND_SET_COLOR;
	//I believe the next 4 bytes are regions, set all bits to 1 :-)
	thedata[2] = 0xFF;      //block
	thedata[3] = 0xFF;//b1; //unknown
	thedata[4] = 0xFF;//b2; //zone1!
	thedata[5] = 0xFF;//b3; //zone2!
	thedata[6] = RG;	//r and g
	thedata[7] = B,0x00;	//b
	if (usb_is_ready()) WriteDevice(thedata, DATA_LENGTH);
	usleep(10000);
}

int main(int argc, char *argv[], char **envp) {
	if (debug) printf("Debug: Hit main code block. Parsing arguments...");
	parse_arguments(argc, argv, envp);
	if (help) { print_help(); return 0; }
	if (debug) {
		if (user_is_root) printf("Debug: User is root.\n"); else printf("Debug: User is NOT root!\n");
		printf("Debug: Colour argument requested: %d red, %d green, %d blue\n", a_colour[1]&255, a_colour[2]&255, a_colour[3]&255);
		printf("Debug: Attempting to connect AlienFX chip by USB...");
	}
	//open the chip
	if (AlienfxInit()) {
		if (debug) {
			printf("success.\n");
			if (alienFXid == AREA51) printf("Debug: Found AlienFX Area51 chip\n");
			if (alienFXid == ALLPOWERFULL) printf("Debug: Found AlienFX AllPowerfull chip\n");
			if (alienFXid == ALLPOWERFULL_M11X) printf("Debug: Found AlienFX AllPowerfull M11X chip\n");
			if (alienFXid == ALLPOWERFULL_M11XR3) printf("Debug: Found AlienFX AllPowerfull M11X R3 chip\n");
			if (alienFXid == AURORA_NONALX) printf("Debug: Found AlienFX Aurora (non-ALX) chip\n");
		}
/////////////////////////////////
		if ((a_colour[1] == 0x00) && (a_colour[2] == 0x00) && (a_colour[3] == 0x00)) {
			if (debug) printf("Debug: No colour specified, assuming darkness.\n");
			reset_device(RESET_ALL_LIGHTS_OFF);
			afx_commit();
//This commented out code in both parts of the IF statement are attempts to permanently store
//the new colour settings in the EEPROM chip - however, this fails to work properly. I am still
//experimenting with it to see what works.  At present, on my model (M11XR3) the lights reset
//to WHITE upon a laptop power cycle.
//			afx_load_on_boot();
//			reset_device(RESET_ALL_LIGHTS_OFF);
//			afx_done();
//			afx_save();
//			afx_commit();
		} else {
			reset_device(RESET_ALL_LIGHTS_ON);
			afx_set(a_colour[1], a_colour[2], a_colour[3]);
			reset_device(RESET_ALL_LIGHTS_ON);
//			afx_done();
			afx_commit();

//			afx_load_on_boot();
//			afx_set(a_colour[1], a_colour[2], a_colour[3]);
//			afx_done();
//			afx_save();
//			afx_commit();
		}
//////////////////////////////////
		//close the chip
		if (debug) printf("Debug: Attempting USB disconnect...");
		AlienfxDeinit();
		if (debug) printf("done.\n");
	} else {
		if (debug) printf("failed.\n");
		printf("Error: ");
		if (user_is_root)
			printf("AlienFX not detected.\n");
		else
			printf("you are not root, maybe you lack permission or AlienFX is missing?\n");
		return 1;
	}
	//everything went well, exit 0
	if (debug) printf("Debug: Exiting.\n");
	return 0;
} //EOF
