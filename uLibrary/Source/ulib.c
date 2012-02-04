#include <nds.h>
#include <stdlib.h>
#include <math.h>
#include <nds/system.h>

#include "ulib.h"

s16 ul_cos[512], ul_sin[512];

void ulTexVramInit();
void ulInitTextures();
void ulTexPalInit();

void _initSinTables()		{
	int i;
	for (i=0;i<512;i++)			{
		ul_sin[i] = (sin(i * 3.1415926535898f / 256.0) * 256.0) + 0.5;
		ul_cos[i] = (cos(i * 3.1415926535898f / 256.0) * 256.0) + 0.5;
	}
}

void ulInit(UL_INIT_FLAGS flags)		{
   if (flags == UL_INIT_ALL)			{
		powerOn(POWER_ALL);

		//irqs are nice
//		irqInit();
		irqEnable(IRQ_VBLANK);
	}

	_initSinTables();
	ulTexVramInit();
	ulInitTextures();
	ulTexPalInit();
	VirtualFileInit();
}

void ulTerminate()	{
}

int ulGetPowerOf2Count(int value)		{
	int i;
	for (i=31;i>=0;i--)			{
		if (value & (1<<i))
			break;
	}
	if (value & ~-(1<<i))
		i++;

	return i;
}

/*
	DEBUG
*/
int ul_isConsoleInited = 0;


