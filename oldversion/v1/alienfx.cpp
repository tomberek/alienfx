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
const int SEND_DATA_SIZE = 9;
//const int READ_DATA_SIZE (alienFXid == ALLPOWERFULL_ALIENFX ? 8 : 9) //is this needed?
const int READ_DATA_SIZE = 9;

const int STATE_BUSY = 0x11;
const int STATE_READY = 0x10;
const int STATE_UNKNOWN_COMMAND = 0x12;
const int SUPPORTED_COMMANDS = 15;
const int COMMAND_END_STORAGE = 0x00;// = End Storage block (See storage)
const int COMMAND_SET_COLOUR_MORPH = 0x01;// = Set morph color (See set commands)
const int COMMAND_SET_COLOUR_BLINK = 0x02;// = Set blink color (See set commands)
const int COMMAND_SET_COLOUR_FIXED = 0x03;// = Set color (See set commands)
const int COMMAND_LOOP_BLOCK_END = 0x04;// = Loop Block end (See loops)
const int COMMAND_TRANSMIT_EXECUTE = 0x05;// = End transmition and execute
const int COMMAND_GET_STATUS = 0x06;// = Get device status (see get device status)
const int COMMAND_RESET = 0x07;// = Reset (See reset)
const int COMMAND_SAVE_NEXT = 0x08;// = Save next instruction in storage block (see storage)
const int COMMAND_SAVE = 0x09;// = Save storage data (See storage)
const int COMMAND_REBOOT_CHIP = 0X0C; //Reboot Chip
const int COMMAND_SET_SPEED = 0x0E;// = Set display speed (see set speed)
const int COMMAND_BATTERY_STATE = 0x0F;// = Set battery state (See set commands)
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
const int MAX_SPEED = 1000;
const int MIN_SPEED = 100;
const int STEP_SPEED = 100;
const int ALIENFX_VID = 0x187c;
const int ALIENFX_PID_AURORA_NONALX = 0x513;
const int ALIENFX_PID_ALLPOWERFULL_M11XR3 = 0x522;
const int ALIENFX_PID_ALLPOWERFULL_M11X = 0x514;
const int ALIENFX_PID_ALLPOWERFULL = 0x512;
const int ALIENFX_PID_AREA51 = 0x511;
const int ALIENFX_PID_UNKNOWN = 0x520;
const int NOT_FOUND = 0;
const int START_BYTE_M11XR3 = 0x02;
const int START_BYTE_M11X = 0x02;
const int START_BYTE_NONALX = 0x02;
const int START_BYTE_ALLP = 0x02;
const int START_BYTE_AREA51 = 0x00;
const int DEFAULT_USB_SLEEP = 275000; //usec!

libusb_context* context;
libusb_device_handle* alienFx;
int alienFXid, START_BYTE, magicnum, preset, usb_sleep=DEFAULT_USB_SLEEP+1;
char magic[20][24]; //20 iterations of 23-bytes each
bool user_is_root, debug, debug2, help, reset, reboot, saving, speed, darkness, wait;

void detach(libusb_device_handle* device){
  int r = libusb_kernel_driver_active(device, 0);
  if (r == 1)
    r = libusb_detach_kernel_driver(device, 0);
}

void attach(libusb_device_handle* device){
  libusb_attach_kernel_driver(device, 0);
}


int WriteDevice(unsigned char* pData, int pDataLength){
 if (debug2) { printf("Debug: USB Write"); for (int i=0;i<pDataLength;i++) { printf("[%i]",pData[i]); } printf("\n"); }
 int v = libusb_control_transfer(alienFx, SEND_REQUEST_TYPE, SEND_REQUEST, SEND_VALUE, SEND_INDEX, pData,pDataLength,0);
 usleep(DEFAULT_USB_SLEEP);//sdk recommends 250msec
 return v;
}


int ReadDevice(unsigned char* pData, int pDataLength){
 if (debug2) { printf("Debug: USB Read"); for (int i=0;i<pDataLength;i++) { printf("[%i]",pData[i]); } printf("\n"); }
 int v = libusb_control_transfer(alienFx, READ_REQUEST_TYPE, READ_REQUEST, READ_VALUE, READ_INDEX, pData,pDataLength,0);
 return v;
}

int AlienfxInit(){
  libusb_init(&context);
  if (debug2) libusb_set_debug (context, 3);
  if (alienFXid == 0) alienFXid = ALIENFX_PID_ALLPOWERFULL_M11XR3;
  if (debug2) printf("Debug: USB ProductID set to: %llx. Connecting...", *(unsigned int *)&alienFXid);
  alienFx = libusb_open_device_with_vid_pid(context, ALIENFX_VID, alienFXid);
  START_BYTE = START_BYTE_M11XR3;
  if(alienFx == NULL){
    if (debug2) printf("failed.\n");
    alienFXid = ALIENFX_PID_AREA51;
    if (debug2) printf("Debug: USB ProductID set to: %llx. Connecting...", *(unsigned int *)&alienFXid);
    alienFx = libusb_open_device_with_vid_pid(context, ALIENFX_VID, alienFXid);
    START_BYTE = START_BYTE_AREA51;
    if(alienFx == NULL){
      if (debug2) printf("failed.\n");
      alienFXid = ALIENFX_PID_ALLPOWERFULL_M11X;
      if (debug2) printf("Debug: USB ProductID set to: %llx. Connecting...", *(unsigned int *)&alienFXid);
      alienFx = libusb_open_device_with_vid_pid(context, ALIENFX_VID, alienFXid);
      START_BYTE = START_BYTE_M11X;
      if(alienFx == NULL){
        if (debug2) printf("failed.\n");
        alienFXid = ALIENFX_PID_ALLPOWERFULL;
        if (debug2) printf("Debug: USB ProductID set to: %llx. Connecting...", *(unsigned int *)&alienFXid);
        alienFx = libusb_open_device_with_vid_pid(context, ALIENFX_VID, alienFXid);
        START_BYTE = START_BYTE_ALLP;
        if(alienFx == NULL){
          if (debug2) printf("failed.\n");
          alienFXid = ALIENFX_PID_AURORA_NONALX;
          if (debug2) printf("Debug: USB ProductID set to: %llx. Connecting...", *(unsigned int *)&alienFXid);
          alienFx = libusb_open_device_with_vid_pid(context, ALIENFX_VID, alienFXid);
          START_BYTE = START_BYTE_NONALX;
          if(alienFx == NULL){
            if (debug2) printf("failed.\n");
            alienFXid = ALIENFX_PID_UNKNOWN;
            if (debug2) printf("Debug: USB ProductID set to: %llx. Connecting...", *(unsigned int *)&alienFXid);
            alienFx = libusb_open_device_with_vid_pid(context, ALIENFX_VID, alienFXid);
            START_BYTE = START_BYTE_M11XR3;
            if(alienFx == NULL) {
              if (debug2) printf("failed.\nDebug: Exhausted list of known product ID's.\n");
              return NOT_FOUND;
            } else { if (debug2) printf("success!\n"); };
          } else { if (debug2) printf("success!\n"); };
        } else { if (debug2) printf("success!\n"); };
      }else { if (debug2) printf("success!\n"); };
    }else { if (debug2) printf("success!\n"); };
  }else { if (debug2) printf("success!\n"); };
  detach(alienFx);
  int res = libusb_claim_interface(alienFx,0);
  if(res < 0) return NOT_FOUND;
  return alienFXid;
}

void AlienfxDeinit(){
  if (debug2) printf("Debug: Ordering USB disconnect...");
  libusb_release_interface(alienFx, 0);
  attach(alienFx);
  libusb_close(alienFx);
  libusb_exit(context);
  if (debug2) printf("done.\n");
}


char hextoint(char c) {
	if (c >= '0' && c <= '9') return c-'0';
	if (c >= 'A' && c <= 'F') return c-'A'+10;
	if (c >= 'a' && c <= 'f') return c-'a'+10;
	return 0;
}

void print_help() {
	printf("Alienware AlienFX Light Manipulation Program v1.0\n");
	printf("Usage:    alienfx [-c cmd] [-p hex] [-u] [-v]\n");
	printf("   or     alienfx -R\n");
	printf("   or     alienfx -h\n");
	printf("  -c <M>[RGB[RGB[ZZZZZZZZZZZZZZZZ]]]   Colour command\n");
	printf("  -d <num>   Set morph/flash delay time. (1 to 10)\n");
	printf("  -h         This helpful message\n");
	printf("  -p <num>   Apply a preset. Use '0' to get a list. Implies ignorance of -c.\n");
	printf("  -P <hex>   Specify the hex ProductID of lightchip\n");
	printf("  -r         Reset chip; software reset of lightchip\n");
	printf("  -R         Reboot chip; hardware reset of lightchip\n");
	printf("  -s         Permanently save the settings into the EEPROM\n");
	printf("  -t <num>   Increase USB timing delay by increment (1 to 100)\n");
	printf("  -u         Show USB debug if needed\n");
	printf("  -v         Show verbose debug output\n");
	printf("  -w         Check for lightchip ready (excess)\n");
	printf("Each colour command is made up of M (mode), RGB (colour) and Z (zones).\n");
	printf("Mode is either B=Blink, M=Morph or F=Fixed\n");
	printf("RGB is three single-digits of hex. Specify two colours, even if 2nd unused.\n");
	printf("Zones are either 1 or 0. 1 is on, 0 is off. There are sixteen zones.\n");
	printf("Zone default is to set all zones to on(1).\n");
	printf("If called with no arguments the lights will silently turn off.\n");
	printf("See 'man alienfx' for more information.\n");
}

bool parse_arguments(int argc, char *argv[], char **envp) {
	//Trap username from environment
	char *var,tag[5],uservar[9],arg[2],setzones[16];
	int i;
        while(*envp != NULL) {
		var = *(envp++);
		if (strcmp(var,"USER=root")|strcmp(var,"USERNAME=root")) user_is_root=true;
	}
	bool colour_option;
        while(*argv != NULL) {
		var = *(argv++);
		//if (debug2) printf("Debug: Var: \"%s\"\n", var);
		if (!strcmp(var,"-c")) { //colour command
			var = *(argv++);
			memset(magic[magicnum],0,23);
			strncpy(magic[magicnum],var,23);
			if (debug) printf("Debug: received argument '%s'\n", magic[magicnum]);
			if (!((magic[magicnum][0]=='B')|(magic[magicnum][0]=='M')|(magic[magicnum][0]=='F')|
				(magic[magicnum][0]=='b')|(magic[magicnum][0]=='m')|(magic[magicnum][0]=='f')))
					help = true;
			for (i=1; i<7; i++) {
				if (i==4) colour_option = true;
				if (!((magic[magicnum][i]=='0')|(magic[magicnum][i]=='1')|(magic[magicnum][i]=='2')
				|(magic[magicnum][i]=='3')|(magic[magicnum][i]=='4')|(magic[magicnum][i]=='5')
				|(magic[magicnum][i]=='6')|(magic[magicnum][i]=='7')|(magic[magicnum][i]=='8')
				|(magic[magicnum][i]=='9')|(magic[magicnum][i]=='A')|(magic[magicnum][i]=='B')
				|(magic[magicnum][i]=='C')|(magic[magicnum][i]=='D')|(magic[magicnum][i]=='E')
				|(magic[magicnum][i]=='F')|(magic[magicnum][i]=='a')|(magic[magicnum][i]=='b')
				|(magic[magicnum][i]=='c')|(magic[magicnum][i]=='d')|(magic[magicnum][i]=='e')
				|(magic[magicnum][i]=='f')|colour_option)) help = true; //look, i know there are better ways ok
			}
			for (i=7; i<24; i++) {
				if (!((magic[magicnum][i]=='0')|(magic[magicnum][i]=='1')|(magic[magicnum][i]==0))) help = true;
			}
			magicnum++;
		}
		if (!strcmp(var,"-v")) { debug = true;  }  //alienfx debugging
		if (!strcmp(var,"-u")) { debug2 = true; }  //libusb debugging
		if (!strcmp(var,"-w")) { wait = true;   }  //usb in waiting
		if (!strcmp(var,"-h")) { help = true;   }  //help output
		if (!strcmp(var,"-R")) { reboot = true; }  //COMMAND_REBOOT_CHIP
		if (!strcmp(var,"-s")) { saving = true; }  //saving (save to CMOS not available)
		if (!strcmp(var,"-d")) { speed = atoi(*(argv++)); }  //flash/morph speed
		if (!strcmp(var,"-r")) { darkness = true; }//Plunge into darkness before set colours
		if (!strcmp(var,"-P")) { sscanf(*(argv++), "%llx", (unsigned int *)&alienFXid); }  //USB ProductID, ugly!
		if (!strcmp(var,"-t")) { usb_sleep = DEFAULT_USB_SLEEP * atoi(*(argv++)); } //USB delay
		if (!strcmp(var,"-p")) { preset = atoi(*(argv++)); } //presets!
	}
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
		usleep(DEFAULT_USB_SLEEP);
		memset(thedata, FILL_BYTE, DATA_LENGTH);
		ReadDevice(thedata, DATA_LENGTH);
		ready = (thedata[0] == STATE_READY);
		if (debug) {
			if (ready) printf("Debug: Device now ready\n");
			if (thedata[0] == STATE_BUSY) printf("Debug: Device is busy\n");
			if (thedata[0] == STATE_UNKNOWN_COMMAND) printf("Debug: Device claims 'unknown command'\n");
		}
		if (!ready & (usb_sleep > DEFAULT_USB_SLEEP)) usleep(usb_sleep);
	}
	return ready;
}

void afx_cmd(unsigned char cmd, unsigned char block, unsigned char data1, unsigned char data2, unsigned char data3, unsigned char data4, unsigned char data5, bool readycheck) {
	if (debug) printf("Debug: Device command\n");
	unsigned char thedata[DATA_LENGTH];
	memset(thedata, FILL_BYTE, DATA_LENGTH);
	thedata[0] = START_BYTE;
	thedata[1] = cmd;
	thedata[2] = block;
	thedata[3] = data1;
	thedata[4] = data2;
	thedata[5] = data3;
	thedata[6] = data4;
	thedata[7] = data5;
	if (readycheck) {
		if (usb_is_ready()) WriteDevice(thedata, DATA_LENGTH);
		if (usb_sleep > DEFAULT_USB_SLEEP) usleep(usb_sleep);
	} else WriteDevice(thedata, DATA_LENGTH);
}

void afx_set(unsigned char cmd, unsigned char r, unsigned char g, unsigned char b, bool zones[16]) {
	if (debug) printf("Debug: Set fixed zone colour\n");
	int RG = r & 0xF0; RG = RG | ((g >> 4) & 0x0F);
	int B = b & 0xF0;
	int z1, z2;
	if (debug) printf("Debug: Set argument with colours %i red, %i green, %i blue.\n", r, g, b);
	z1 = (zones[0]<<7)+(zones[1]<<6)+(zones[2]<<5)+(zones[3]<<4)+(zones[4]<<3)+(zones[5]<<2)+(zones[6]<<1)+zones[7];
	z2 = (zones[8]<<7)+(zones[9]<<6)+(zones[10]<<5)+(zones[11]<<4)+(zones[12]<<3)+(zones[13]<<2)+(zones[14]<<1)+zones[15];
	afx_cmd(cmd, 255, 255, z1, z2, RG, B, wait);
	if (usb_sleep > DEFAULT_USB_SLEEP) usleep(usb_sleep);
}

void afx_set2(unsigned char cmd, unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2, bool zones[16]) {
	if (debug) printf("Debug: Set zone multi-colour\n");
//don't ask.
	int RG = r1 & 0xF0; RG = RG | ((g1 >> 4) & 0x0F);
	int BR = b1 & 0xF0; BR = BR | ((r2 >> 4) & 0x0F);
	int GB = g2 & 0xF0; GB = GB | ((b2 >> 4) & 0x0F);
//uuhhh.
	int z1, z2;
	z1 = (zones[0]<<7)+(zones[1]<<6)+(zones[2]<<5)+(zones[3]<<4)+(zones[4]<<3)+(zones[5]<<2)+(zones[6]<<1)+zones[7];
	z2 = (zones[8]<<7)+(zones[9]<<6)+(zones[10]<<5)+(zones[11]<<4)+(zones[12]<<3)+(zones[13]<<2)+(zones[14]<<1)+zones[15];
	if (debug) printf("Debug: Set argument with double colours %i red, %i green, %i blue AND %i red, %i green, %i blue.\n", r1, g1, b1, r2, g2, b2);
	unsigned char thedata[DATA_LENGTH+1];
	memset(thedata, FILL_BYTE, DATA_LENGTH+1);
	thedata[0] = START_BYTE;
	thedata[1] = cmd;
	thedata[2] = 0;
	thedata[3] = 0;
	thedata[4] = z1;
	thedata[5] = z2;
	thedata[6] = RG;
	thedata[7] = BR;
	thedata[8] = GB;
	if (usb_is_ready()) WriteDevice(thedata, DATA_LENGTH+1);
	if (usb_sleep > DEFAULT_USB_SLEEP) usleep(usb_sleep);
}

void afx_spd(int speed) {
//call this with arg >1 and <10
	speed = speed * 100;
	if(speed > MAX_SPEED) speed = MAX_SPEED;
	if(speed < MIN_SPEED) speed = MIN_SPEED;
	//speed = speed / STEP_SPEED; // wtf?
	//speed = speed * STEP_SPEED; // wtf?
	int b1 = (speed >> 8) & 0xFF;
	int b2 = speed & 0xFF;
	afx_cmd(COMMAND_SET_SPEED, b1, b2, 0, 0, 0, 0, wait);
	if (usb_sleep > DEFAULT_USB_SLEEP) usleep(usb_sleep);
}

int main(int argc, char *argv[], char **envp) {
	memset(magic,0,20*20);
	if (debug) printf("Debug: Hit main code block. Parsing arguments...\n");
	parse_arguments(argc, argv, envp);
	if (help) { print_help(); return 0; }
	if (debug) { if (user_is_root) printf("Debug: Detection suggests user may be root.\n"); else printf("Debug: Detection suggests user may NOT be root!\n"); }
	//open the chip
	if (AlienfxInit()) {
		if (debug) printf("Debug: Found an AlienFX chip.\n");
		if (reboot) {
			if (debug) printf("Debug: Reboot of lightchip requested.\n");
			afx_cmd(COMMAND_REBOOT_CHIP, 0, 0, 0, 0, 0, 0, false);
			AlienfxDeinit();
			sleep(3); //if you delay here, it prevents the next cmd in a script file from error.
			if (!AlienfxInit()) { //chances are we can continue...
				if (debug) printf("Debug: Exiting.\n");
				return 1;
			}
		}
		if (speed > 0) {
			if (debug) printf("Debug: Setting speed to %i of 10\n", speed);
			afx_spd(speed);
		}
		if (darkness|(magicnum==0)) {
			if (debug) printf("Debug: Turning lights off.\n");
			afx_cmd(COMMAND_RESET, RESET_ALL_LIGHTS_OFF, 0, 0, 0, 0, 0, wait);
		}
		bool zones[16]; unsigned char R1,G1,B1,R2,G2,B2;
		if (debug) printf("Debug: Magic Number [%i]\n", magicnum);
		afx_cmd(COMMAND_RESET, RESET_ALL_LIGHTS_ON, 0, 0, 0, 0, 0, wait);
		for (int i=0; i<magicnum; i++) { //magic loop
			if (debug) printf("Debug: Magic Buffer [%s]\n", magic[i]);
			R1 = hextoint(magic[i][1]); G1 = hextoint(magic[i][2]); B1 = hextoint(magic[i][3]);
			R2 = hextoint(magic[i][4]); G2 = hextoint(magic[i][5]); B2 = hextoint(magic[i][6]);
			memset(zones,true,16);
			if (R1>0) R1*=16; if (G1>0) G1*=16; if (B1>0) B1*=16;
			if (R2>0) R2*=16; if (G2>0) G2*=16; if (B2>0) B2*=16;
			for (int j=0;j<16;j++) if ((magic[i][j+7]=='0')) zones[j] = false;
			if (debug) {
				printf("Zones: 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16\n");
				printf("Zones:  %i  %i  %i  %i  %i  %i  %i  %i  %i  %i  %i  %i  %i  %i  %i  %i\n",
				 zones[0], zones[1], zones[2], zones[3], zones[4], zones[5], zones[6], zones[7], zones[8],
				 zones[9], zones[10], zones[11], zones[12], zones[13], zones[14], zones[15]);
			}
			if ((R1==0)&(G1==0)&(B1==0)&(R2==0)&(G2==0)&(B2==0)) {
				if (debug) printf("Debug: No colour specified, assuming darkness.\n");
				if (debug & darkness) printf("Debug: And you have already specified darkness? wth.\n");
				afx_cmd(COMMAND_RESET, RESET_ALL_LIGHTS_OFF, 0, 0, 0, 0, 0, wait);
				magic[i][0] = 0; i = magicnum;
			}
			if ((magic[i][0]=='F')|(magic[i][0]=='f')) {
				if (debug) printf("Debug: Fixed colours to zones\n");
				afx_cmd(COMMAND_SAVE_NEXT, 0, 0, 0, 0, 0, 0, wait);
				afx_set(COMMAND_SET_COLOUR_FIXED,R1,G1,B1,zones);
//				continue;
			} else if ((magic[i][0]=='M')|(magic[i][0]=='m')) {
				if (debug) printf("Debug: Morph colours to zones\n");
				afx_cmd(COMMAND_SAVE_NEXT, 0, 0, 0, 0, 0, 0, wait);
				afx_set2(COMMAND_SET_COLOUR_MORPH,R1,G1,B1,R2,G2,B2,zones);
//				continue;
			} else if ((magic[i][0]=='B')|(magic[i][0]=='b')) {
				if (debug) printf("Debug: Blink colours to zones.\n");
				afx_cmd(COMMAND_SAVE_NEXT, 0, 0, 0, 0, 0, 0, wait);
				afx_set2(COMMAND_SET_COLOUR_BLINK,R1,G1,B1,R2,G2,B2,zones);
//				continue;
			}
			if (usb_sleep > DEFAULT_USB_SLEEP) usleep(usb_sleep);
		} //end loop "process magic[x][y]"
		afx_cmd(COMMAND_LOOP_BLOCK_END, 0, 0, 0, 0, 0, 0, wait);
		afx_cmd(COMMAND_TRANSMIT_EXECUTE, 0, 0, 0, 0, 0, 0, wait);
		if (saving) {   //Save for boot-time colouring
			if (debug) printf("Debug: Sorry, saving to EEPROM not available.\n");
		}
		//close the chip
		AlienfxDeinit();
	} else {
		if (debug) printf("Debug: USB driver could not locate AlienFX chip.  Consult manpage?\n");
		if (debug2) printf("Debug: Use the -p setting to specify the product ID of your AlienFX chip.\n");
		if (debug2) printf("Debug: You may find the product ID by running the 'lsusb' command.\n");
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
