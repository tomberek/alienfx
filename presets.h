/*

  Includes the presets used by the "-p" command option.

*/

const int MAX_PRESET = 20;    //if you add presets, increment this number!

const int preset_size[] = { 6,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1 }; //how many magics per preset?

const char preset_data[][6][8] = {
	{ "MF00FF0","MFF00F0","M0F00FF","M0FF00F","M00FF0F","MF0FF00" }, //01 Morph: Smooth Rainbow
	{ "MFFF444","M444FFF" },	//02 Morph: Grey/White
	{ "MFF00F0","M0F0FF0" },	//03 Morph: Yellow/Green
	{ "M00FF00","MF0000F" },	//04 Morph: Blue/Red
	{ "MFFF000","M000FFF" },	//05 Morph: Black/White
	{ "MF0770F","M70FF07" },	//06 Morph: Pink/Purple
	{ "M00F70F","M70F00F" },	//07 Morph: Blue/Purple
	{ "MF60FF0","MFF0F60" },	//08 Morph: Orange/Yellow
	{ "MF07FF0","MFF0F07" },	//09 Morph: Pink/Yellow
	{ "FF48" },	//10 Fixed: Soft Pink
	{ "FF00" },	//11 Fixed: Red
	{ "FFF0" },	//12 Fixed: Yellow
	{ "F0F0" },	//13 Fixed: Green
	{ "F0FF" },	//14 Fixed: Cyan
	{ "F00F" },	//15 Fixed: Blue
	{ "FF60" },	//16 Fixed: Orange
	{ "F111" },	//17 Fixed: Dark Night
	{ "BF00" },	//18 Blink: Red
	{ "B0F0" },	//19 Blink: Green
	{ "B00F" }	//20 Blink: Blue
};

const char preset_name[][22] = {
	{ "Morph: Smooth Rainbow" },
	{ "Morph: Grey/White" },
	{ "Morph: Yellow/Green" },
	{ "Morph: Blue/Red" },
	{ "Morph: Black/White" },
	{ "Morph: Pink/Purple" },
	{ "Morph: Blue/Purple" },
	{ "Morph: Orange/Yellow" },
	{ "Morph: Pink/Yellow" },
	{ "Fixed: Soft Pink" },
	{ "Fixed: Red" },
	{ "Fixed: Yellow" },
	{ "Fixed: Green" },
	{ "Fixed: Cyan" },
	{ "Fixed: Blue" },
	{ "Fixed: Orange" },
	{ "Fixed: Dark Night" },
	{ "Blink: Red" },
	{ "Blink: Green" },
	{ "Blink: Blue" }
};
