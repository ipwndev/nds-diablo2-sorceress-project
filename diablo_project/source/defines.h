#ifndef _DEFINES_H
#define _DEFINES_H
/////////////////
/////////////////
/////Defines/////
/////////////////
/////////////////
//#define Test
//#define NOSPAWN


#include "soundbank.h" //include all audio defines

#define VERSION             "1.1.2"
//version number two digits per member and 3 for last one ex : 1.1.2=>01.01.002=>01 01 002 or 12.32.2=>12 32 002
#define VERSION_NUMBER      0101002
#define PARSERROR           topPrintf(130,45,"Error parsing data")
#define ERROR(text...)         topPrintf(130,45, ##text)


#define MUSIC_OFF

//#define RESETKEYS 0xF03 //combo of keys to reset game
#define SKILLMENU_KEY   ul_keys.pressed.select

#define _GFX_ALIGN __attribute__((aligned (4)))

#define ISINBOX(X,Y,x,y,u,v) ((X) >= (x) && (X) <= ((x) + (u)) && (Y) >=(y) && (Y) <=((y)+(v)))
#define STYLUSBOX(X,Y,u,v) ISINBOX(ul_keys.touch.x,ul_keys.touch.y,X,Y,u,v)//ul_keys.touch.x >= (X) && ul_keys.touch.x <= ((X) + (u)) && ul_keys.touch.y >=(Y) && ul_keys.touch.y <=((Y)+(v)))

#define norm_fix(x)			((x)<<8)
#define fix_norm(x)			((x)>>8)
#define dir_angle(x)		(((x)<<6)-160)
#define angle_dir(x)		( ((x+160)&511)>>6 )
#define mdir_angle(x)		(((x)<<5)-144)
#define mangle_dir(x)		( ((x+144)&511)>>5 )
#define fix_mult(x,y)		(((x)*(y))>>8)
#define ratio_fix(x,y)		((256*(x))/(y))
#define fixed_fraction(x)	((x)&&0xff)
#define SOUND(x)            (SoundInfo){(u8*)x,(u32)x##_size,0,16384,127,64,false,0,0}

#define SPEED				256
#define SPEED2			    192
#define CAMERA_X        	112
#define CAMERA_Y	        60
#define CHARMID_X           128
#define CHARMID_Y           86
#define CHARFEET_X          128
#define CHARFEET_Y          107
#define PAUSEKEY            ul_keys.pressed.start

#define SKILLNUMBER         8
#define BGOBJ_AB            25
#define NWALKABLETILE       8
#define TILENB              16
#define VOIDTILE            10
//#define MAPSIZE_X           59 //size in number of tiles
//#define MAPSIZE_Y           47
int MAPSIZE_X; //no more a define, we have more than one map
int MAPSIZE_Y;

#define MAX_DATA			5
#define MAX_BGDATA			5
#define MAX_DATASPRITES     21
#define MAX_MDATA			6
#define MAX_OBJECT			100
#define MAX_BGOBJECT        100
#define MAX_MISSILE			100
#define MAX_FX 				200
#define MAX_FXDATA 			11
#define MAX_SPRITES 		300
#define MAX_AURAS           1

//formulae
#define MONSTERLIFE(life)   ((MonBaseLife*(life))/100+512)>>9

//counters
#define VBL                 0
#define TOPSCREEN           1
#define TALKING             2

//status, bitmasks

#define S_ALARMED           1
#define S_ATTACKED          2
#define S_DEAD              4
#define S_COLD              8
#define S_FROZEN            16
#define S_INFIRE            32
#define S_POISONNED         64
#define S_STUNNED           128

#endif
