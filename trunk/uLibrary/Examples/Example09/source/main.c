/*
	Example 09

	Same as Example02b except that a music is played meanwhile using PALib.
*/

#include <ulib/ulib.h>
#include <PA9.h>
#include "james_gif.h"
#include "bear_gif.h"
#include "music_mod.h"

//Our image contains 3 frames, one next to the other vertically, and 32 pixel wide.
#define FRAME_HEIGHT 32

// Helper that may be useful for defining several files
// In this case don't forget to use the special bin2o in the makefile for your projects too
#define RAM_FILE_ENTRY(name, symbol)	{name, (void*)symbol, sizeof(symbol), &VF_MEMORY}

//Files stored in RAM.
UL_VIRTUALFILENAME ram_names[] =		{
	RAM_FILE_ENTRY("james.gif", james_gif),
	RAM_FILE_ENTRY("bear.gif", bear_gif),
};

int main()
{
	UL_IMAGE *imgJames, *imgBear;
	int animStage = 0, frameNumber = 0;
	//The frame numbers that will be used for animations
	const int animPositions[] = {0, 1, 2, 1};
	int i, j, fade = 0x1f;

	PA_Init();    // PA Init...
	PA_InitVBL();	// VBL Init...
	PA_InitSound(); 	// Sound Init, for the mod player...
	PA_PlayMod(music_mod);		// Play a mod file

	//Initialize �Library
	ulInit(UL_INIT_ALL);
	//Initialize the graphical part
	ulInitGfx();
	//Initialize the text part
	ulInitText();

	//Register the file names for the RAM file system. This is not obligatory, you could directly pass "pointer, size" for file loading routines instead of "name, 0".
	ulSetVirtualFilenameList(ram_names, ulNumberof(ram_names));

	//Show some splash screens!
	ulSetMainLcd(1);
	ulShowSplashScreen(1);
	ulShowSplashScreen(2);

	//We will use the touchpad this time
	ulSetMainLcd(0);
	//Ensure the second screen is black
	SUB_BLEND_CR = BLEND_FADE_BLACK | BLEND_SRC_BG0 | BLEND_SRC_BG1 | BLEND_SRC_BG2 | BLEND_SRC_BG3;
	SUB_BLEND_Y = 0x1f;
	BLEND_CR = BLEND_FADE_BLACK | BLEND_SRC_BG0 | BLEND_SRC_BG1 | BLEND_SRC_BG2 | BLEND_SRC_BG3;
	BLEND_Y = fade;

	//Use bright pink as a transparent color
	ulSetTransparentColor(RGB15(31, 0, 31));
	{
		//We load our James character image. Note that the format is PAL4 (16 colors). It is enough for our image but may not for others.
		imgJames = ulLoadImageFileGIF("james.gif", 0, UL_IN_VRAM, UL_PF_PAL4);
		imgBear = ulLoadImageFileGIF("bear.gif", 0, UL_IN_VRAM, UL_PF_PAL8);
	}
	ulDisableTransparentColor();

	//Define the image center, so that the stylus will control the center of the image. By default, images are specified by their top-left corner (that's the fastest). Usually you should not have to change the image center except for rotation.
	imgJames->centerX = imgJames->sizeX / 2;
	imgJames->centerY = FRAME_HEIGHT / 2;

	while(1)
	{
	   //Read keys
	   ulReadKeys(0);

	   //We place our character to the same position as the touchpad stylus, making it follow it
	   imgJames->x = ul_keys.touch.x;
	   imgJames->y = ul_keys.touch.y;

	   frameNumber++;

	   //Each 8 frames, we increment the frame stage counter, so that we display the next frame
	   if (frameNumber % 8 == 0)
		   //Increment the animation counter
		   animStage = (animStage + 1) % 4;

	   //Start our drawing
		ulStartDrawing2D();

		//Draw some bears in the background!
		if (imgBear)			{
		   ulSetImageTint(imgBear, RGB15(16, 16, 16));
			for (j=0;j<SCREEN_HEIGHT;j+=imgBear->sizeY)			{
				for (i=0;i<SCREEN_WIDTH;i+=imgBear->sizeX)			{
			   	ulDrawImageXY(imgBear, i, j);
				}
			}
		}

		//Set up the sprite part to draw only the desired image part. We could do it by ourself using the offsetX0, ... members.
		ulSetImageTileSize(imgJames,
			0,														//Image frames are aligned vertically, so we always begin to the left
			FRAME_HEIGHT * animPositions[animStage],	//Top position: animPositions defines the frame numbers in the image for each animation step (0, 1, 2, 1). Each frame is FRAME_HEIGHT tall, so we just have to multiply to get the actual position of the frame on the image!
			imgJames->sizeX,									//Every frame takes the entire image width
			FRAME_HEIGHT);										//Height of a frame

		//Draw our character on the screen!
		ulDrawImage(imgJames);

		//End the drawing
		ulEndDrawing();

		//Wait the VBlank (synchronize at 60 fps)
		ulEndFrame();
		PA_WaitForVBL();

		//Fade the screen
		fade = ulMax(fade - 1, 0);
		BLEND_Y = fade;
	}

	return 0;
}

