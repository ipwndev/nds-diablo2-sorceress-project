//map.png
//Converti avec GBA Graphics par Br�nni
//Map ("","map.til.c",0,ffffffff)
//Taille: 8*8 * 60*45
//M�moire: 5400 octets

#include "../ulScreenDisplay.h"
#include "../uldata.h"
#include "../objects.h"
#include "../interface.h"
#include "Map.h"

int currentMap;
u16* map_col;
void changemap(u8 mapnb,bool reset)
{
    int i;
    switch(mapnb)
    {
    case 0:
        if (reset)
        {
            ulDeleteImage(mapTiles);
            ulDeleteMap(Mymap);
            for (i=0; i<MAX_OBJECT; i++)
            {
                deleteobject(i);
            }
            for (i=0; i<MAX_BGOBJECT; i++)
            {
                deleteBgObject(i);
            }
            map_col=tilescollision_map;
            MAPSIZE_X=59;
            MAPSIZE_Y=47;
            curMaxObject=0;
            curMaxBgObject=0;
            SpawnObjects();

        }
        mapTiles = ulLoadImageFilePNG((void*)map_png, (int)map_png_size, UL_IN_VRAM, UL_PF_PAL4);
        Mymap = ulCreateMap(mapTiles,/*Tileset*/map,8,8,/*Tiles size*/60,45,/*Map size*/UL_MF_U16);//Map format
        hero.x=norm_fix(63);
        hero.y=norm_fix(5);
        currentMap=0;

        if (hero.stats.lvl>=5)
        {
            int objectnb;
            objectnb=getUnusedBgObject();
            newObject((46<<3)+4, (39<<3)+8, &bgobjects[objectnb],objectnb, &bgdata[2] ,1);
            for (i=0;i<MAX_BGOBJECT;i++){if(bgobjects[i].datanb==2)bgobjects[i].ai=&waypointmenu;}
        }
        break;
    case 1:
        if (reset)
        {
            ulDeleteImage(mapTiles);
            ulDeleteMap(Mymap);
            for (i=0; i<MAX_OBJECT; i++)
            {
                deleteobject(i);
            }
            for (i=0; i<MAX_BGOBJECT; i++)
            {
                deleteBgObject(i);
            }
            map_col=duducol_map;
            MAPSIZE_X=129;
            MAPSIZE_Y=102;
            curMaxObject=0;
            curMaxBgObject=0;
            SpawnObjects();
        }
        mapTiles = ulLoadImageFilePNG((void*)dudu_png, (int)dudu_png_size, UL_IN_VRAM, UL_PF_PAL4);
        Mymap = ulCreateMap(mapTiles,/*Tileset*/dudu_map,8,8,/*Tiles size*/129,102,/*Map size*/UL_MF_U16);//Map format
        hero.x=norm_fix(240);
        hero.y=norm_fix(430);
        currentMap=1;
        for (i=0;i<MAX_BGOBJECT;i++){if(bgobjects[i].datanb==2)bgobjects[i].ai=&waypointmenu;}
        break;
    }
    for (i=1; i<MAX_DATASPRITES; i++)
    {
        ulUnrealizeImage(spritedatabase[i].image);
    }
}

const unsigned short map[45][60]=
{
    {
        0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008,
        0x0009, 0x000a, 0x000b, 0x000c, 0x0003, 0x0004, 0x0005, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d,
        0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
        0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005,
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005
    },
    {
        0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016,
        0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x0013, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f,
        0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d,
        0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013,
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013
    },
    {
        0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028,
        0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f, 0x0030,
        0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035,
        0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033,
        0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031,
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031
    },
    {
        0x0037, 0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e,
        0x003f, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046,
        0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e,
        0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c,
        0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a,
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a
    },
    {
        0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058,
        0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f, 0x0060,
        0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068,
        0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066,
        0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e,
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e
    },
    {
        0x006f, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076,
        0x0077, 0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e,
        0x007f, 0x0080, 0x0081, 0x0082, 0x0006, 0x0007, 0x0008, 0x000d,
        0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
        0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005,
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005
    },
    {
        0x000f, 0x0010, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088,
        0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 0x0090,
        0x0091, 0x0092, 0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f,
        0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d,
        0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013,
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013
    },
    {
        0x0021, 0x0022, 0x0023, 0x0024, 0x0093, 0x0094, 0x0095, 0x0096,
        0x0097, 0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e,
        0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035,
        0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033,
        0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031,
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031
    },
    {
        0x0037, 0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x009f, 0x00a0,
        0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x004a, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e,
        0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c,
        0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a,
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a
    },
    {
        0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066, 0x0067,
        0x00a7, 0x00a8, 0x00a9, 0x00aa, 0x006c, 0x006d, 0x006e, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068,
        0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066,
        0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e,
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e
    },
    {
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d,
        0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
        0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005,
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005
    },
    {
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f,
        0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d,
        0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013,
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013
    },
    {
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035,
        0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033,
        0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031,
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031
    },
    {
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e,
        0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c,
        0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a,
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a
    },
    {
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068,
        0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066,
        0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e,
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e
    },
    {
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d,
        0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
        0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005,
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005
    },
    {
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f,
        0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d,
        0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013,
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013
    },
    {
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035,
        0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033,
        0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031,
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031
    },
    {
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e,
        0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c,
        0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a,
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a
    },
    {
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068,
        0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066,
        0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e,
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e
    },
    {
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d,
        0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
        0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005,
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005
    },
    {
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f,
        0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d,
        0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013,
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013
    },
    {
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035,
        0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033,
        0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031,
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031
    },
    {
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e,
        0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c,
        0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a,
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a
    },
    {
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068,
        0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066,
        0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e,
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e
    },
    {
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d,
        0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
        0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005,
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005
    },
    {
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f,
        0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d,
        0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013,
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013
    },
    {
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035,
        0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033,
        0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031,
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031
    },
    {
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e,
        0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c,
        0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a,
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a
    },
    {
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068,
        0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066,
        0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e,
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e
    },
    {
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d,
        0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
        0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005,
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005
    },
    {
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f,
        0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d,
        0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013,
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013
    },
    {
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035,
        0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033,
        0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031,
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031
    },
    {
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e,
        0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c,
        0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a,
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a
    },
    {
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068,
        0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066,
        0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e,
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e
    },
    {
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d,
        0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
        0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005,
        0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008,
        0x0009, 0x000a, 0x000b, 0x000c, 0x0003, 0x0004, 0x0005, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005
    },
    {
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f,
        0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d,
        0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013,
        0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016,
        0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x0013, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013
    },
    {
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035,
        0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033,
        0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031,
        0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028,
        0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f, 0x0030,
        0x0022, 0x0023, 0x0024, 0x0031
    },
    {
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e,
        0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c,
        0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a,
        0x0037, 0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e,
        0x003f, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046,
        0x0047, 0x0048, 0x0049, 0x004a
    },
    {
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068,
        0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066,
        0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e,
        0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058,
        0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f, 0x0060,
        0x0061, 0x0062, 0x0063, 0x0064
    },
    {
        0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003,
        0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d, 0x000e, 0x0001,
        0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x000d,
        0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
        0x0008, 0x000d, 0x000e, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005,
        0x006f, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076,
        0x0077, 0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e,
        0x007f, 0x0080, 0x0081, 0x0082
    },
    {
        0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011,
        0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f, 0x0020, 0x000f,
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d, 0x001e, 0x001f,
        0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x001d,
        0x001e, 0x001f, 0x0020, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013,
        0x000f, 0x0010, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088,
        0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 0x0090,
        0x0091, 0x0092, 0x0012, 0x0013
    },
    {
        0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023,
        0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0021,
        0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035,
        0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031, 0x0032, 0x0033,
        0x0034, 0x0035, 0x0036, 0x0021, 0x0022, 0x0023, 0x0024, 0x0031,
        0x0021, 0x0022, 0x0023, 0x0024, 0x0093, 0x0094, 0x0095, 0x0096,
        0x0097, 0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e,
        0x0022, 0x0023, 0x0024, 0x0031
    },
    {
        0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050,
        0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e,
        0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x004c,
        0x004d, 0x004e, 0x004f, 0x0037, 0x0038, 0x0050, 0x0049, 0x004a,
        0x0037, 0x0038, 0x0050, 0x0049, 0x004a, 0x004b, 0x009f, 0x00a0,
        0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x004a, 0x0037,
        0x0038, 0x0050, 0x0049, 0x004a
    },
    {
        0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
        0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066, 0x0067, 0x0068,
        0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066,
        0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e,
        0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x0065, 0x0066, 0x0067,
        0x00a7, 0x00a8, 0x00a9, 0x00aa, 0x006c, 0x006d, 0x006e, 0x006a,
        0x006b, 0x006c, 0x006d, 0x006e
    }
};