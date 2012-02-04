#include "ulib.h"

s32 ul_currentDepth;
u8 ul_autoDepth;
u8 ul_colorKeyEnabled;
UL_COLOR ul_colorKeyValue;
unsigned long ul_colorKeyValue32;
u8 ul_frameNumber, ul_dualScreenMode;
const int ul_pixelWidth[] = {0, 8, 2, 4, 8, 0, 8, 16, 16};
u8 ul_screenClippingChanged = 0;
u8 ul_drawingStarted = 0;
//UL_IMAGE *ul_lastTexture = NULL;
u8 ul_firstPaletteColorOpaque = 0;

//a global copy of sprite attribute memory
SpriteEntry ulDualScreenSprites[128];

//Taille des formats de pixel
const u8 ul_pixelSizes[]=		{
	0,										//Non utilis�
	8, 									//UL_PF_PAL5_A3
	2, 									//UL_PF_PAL2
	4, 									//UL_PF_PAL4
	8, 									//UL_PF_PAL8
	0, 									//UL_PF_COMPRESSED
	8, 									//UL_PF_PAL3_A5
	16, 									//UL_PF_5551
	16, 									//UL_PF_5550
};

//Taille des palettes (2^n)
const u8 ul_paletteSizes[]=		{
	0,										//Non utilis�
	5, 									//UL_PF_PAL5_A3: 32 couleurs
	2, 									//UL_PF_PAL2: 4 couleurs
	4, 									//UL_PF_PAL4: 16 couleurs
	8, 									//UL_PF_PAL8: 256 couleurs
	0, 									//UL_PF_COMPRESSED
	3, 									//UL_PF_PAL3_A5: 8 couleurs
	0, 									//UL_PF_5551
	0, 									//UL_PF_5550
};

//Nombre de pixels par octet en fonction du format (seuls 1, 2, 4 et 8 support�s)
const u8 ul_pixelsPerByte[] = {0, 8, 4, 0, 2, 0, 0, 0, 1};

void ulInitGfx()
{
	//set mode 0, enable BG0 and set it to 3D
	videoSetMode(MODE_0_3D);
	vramSetBankA(VRAM_A_TEXTURE);
	
	glInit();
	glViewport(0,0,255,191);
	
//	glClearColor(0,0,0);
	BG_PALETTE[0] = RGB15(0, 0, 0);
	
//	GFX_CLEAR_COLOR = RGB15(31, 31, 31) | (15<<16);
	glClearDepth(0x7FFF);

	ul_autoDepth = 1;
	ul_dualScreenMode = 0;
	
	//Il faudra de toute fa�on la mettre � jour...
//	ul_lastTexture = (UL_IMAGE*)-1;
}

void ulInitDualScreenMode()		{
	videoSetMode(MODE_0_3D);
	
	// Init the Sub screen (test if eli can set this up correctly)
	// Set the Sub screen for 15 bit 2D
	videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_2D_BMP_256);
	REG_BG2CNT_SUB = BG_BMP16_256x256;
	REG_BG2PA_SUB = 256;
	REG_BG2PB_SUB = 0;
	REG_BG2PC_SUB = 0;
	REG_BG2PD_SUB = 256;
	REG_BG2Y_SUB = 0;
	REG_BG2X_SUB = 0;
	
	vramSetBankC(VRAM_C_SUB_BG);
	vramSetBankD(VRAM_D_SUB_SPRITE);
	
	ul_frameNumber = 0;
	ul_dualScreenMode = 1;

	int i, x, y;

	for (i = 0; i < 128; i++)
	{
	   ulDualScreenSprites[i].attribute[0] = ATTR0_DISABLED;
	   ulDualScreenSprites[i].attribute[1] = 0;
	   ulDualScreenSprites[i].attribute[2] = 0;
	   ulDualScreenSprites[i].attribute[3] = 0;
   }

	i = 0;
   for (y = 0; y < 3; y++)
   for (x = 0; x < 4; x++)
   {
	   ulDualScreenSprites[i].attribute[0] = ATTR0_BMP | ATTR0_SQUARE | (64 * y);
	   ulDualScreenSprites[i].attribute[1] = ATTR1_SIZE_64 | (64 * x);
	   ulDualScreenSprites[i].attribute[2] = ATTR2_ALPHA(1) | (8 * 32 * y) | (8 * x);
      i++;
   }
}

void ulStopDualScreenMode()		{
   ul_dualScreenMode = 0;
}
   
void ulSetRegCapture(bool enable, uint8 srcBlend, uint8 destBlend, uint8 bank, uint8 offset, uint8 size, uint8 source, uint8 srcOffset)
{
   uint32 value = 0;
   if (enable)
      value |= 1 << 31; // 31 is enable
   value |= 3 << 29; // 29-30 seems to have something to do with the blending
   value |= (srcOffset & 0x3) << 26; // capture source offset is 26-27
   value |= (source & 0x3) << 24; // capture source is 24-25
   value |= (size & 0x3) << 20; // capture data write size is 20-21
   value |= (offset & 0x3) << 18; // write offset is 18-19 
   value |= (bank & 0x3) << 16; // vram bank select is 16-17 
   value |= (srcBlend & 0xFF) << 8; // graphics blend evb is 8..12
   value |= (destBlend & 0xFF) << 0; // ram blend EVA is bits 0..4 
   
   REG_CAPTURE = value;
}

void ulEndFrame()		{
   //Si le clipping a chang�, on doit obligatoirement le restaurer avant la fin
	if (ul_screenClippingChanged)
		ulResetScreenClipping();

	if (ul_dualScreenMode)			{
		if (ul_frameNumber & 1)			{
		   vramSetBankC(VRAM_C_SUB_BG);
		   vramSetBankD(VRAM_D_LCD);
		   ulSetRegCapture(true, 0, 31, 3, 0, 3, 0, 0);
		}
		else	{
	      vramSetBankC(VRAM_C_LCD);
	      vramSetBankD(VRAM_D_SUB_SPRITE);
	      ulSetRegCapture(true, 0, 31, 2, 0, 3, 0, 0);
		}
		
	   lcdSwap();
	   dmaCopy(ulDualScreenSprites, OAM_SUB, 128 * sizeof(SpriteEntry));
	}
	ul_frameNumber++;
}

/*static void glReset(void) {
	while (GFX_STATUS & (1<<27)); // wait till gfx engine is not busy
  
	// Clear the FIFO
	GFX_STATUS |= (1<<29);

	// Clear overflows for list memory
	GFX_CONTROL = enable_bits = ((1<<12) | (1<<13)) | GL_TEXTURE_2D;
	glResetMatrixStack();

	GFX_TEX_FORMAT = 0;
	GFX_POLY_FORMAT = 0;
  
	glMatrixMode(GL_PROJECTION);
	glIdentity();

	glMatrixMode(GL_MODELVIEW);
	glIdentity();
}*/

void ulStartDrawing2D()
{
   //This should be done only the first time
   if (!ul_drawingStarted)			{
		// Reset the screen and setup the view
//		glReset();
				
		//Active l'alpha blending
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
	
		ulDisableEdgeAntialiasing();
		GFX_CONTROL &= ~GL_OUTLINE;

		GFX_COLOR = 0x7fff;

		//DS specific, several attributes can be set here
		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

		ul_currentDepth = 0;
		ul_drawingStarted = 1;
	}
	
	//Set up 2D, again if it was already done
	ulResetScreenView();

//	glPushMatrix();
	
	glMatrixMode(GL_TEXTURE);
	//Load identity
	MATRIX_IDENTITY = 0;
	
	glMatrixMode(GL_MODELVIEW);
	MATRIX_IDENTITY = 0;
	
	// Set the current matrix to be the model matrix
	glMatrixMode(GL_PROJECTION);
}

void ulEndDrawing()
{
   if (ul_drawingStarted)		{
		glPopMatrix(1);
		GFX_FLUSH = 0;
	}
	ul_drawingStarted = 0;
}

void ulSyncFrame()		{
   //Do it by ourself, in case the user forgot it...
   ulEndDrawing();
	swiWaitForVBlank();
	ulEndFrame();
}

void ulSetScreenWindow(int x0, int y0, int x1, int y1)		{
	glMatrixMode(GL_PROJECTION);
	MATRIX_IDENTITY = 0;
	glOrthof32(0, x1, y1, 0, -2045, 1);
	glViewport(0, 192 - y1, x1, 192);
	glTranslate3f32(x0, y0, 0);
	ul_screenClippingChanged = 1;
}

void ulSetScreenClipping(int x0, int y0, int x1, int y1)		{
	glMatrixMode(GL_PROJECTION);
	MATRIX_IDENTITY = 0;
	glOrthof32(x0, x1, y1, y0, -2045, 1);
	glViewport(x0, 192 - y1, x1, 192 - y0);
	ul_screenClippingChanged = 1;
}

void ulResetScreenClipping()			{
	glMatrixMode(GL_PROJECTION);
	MATRIX_IDENTITY = 0;
	glOrthof32(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -2045, 1);
	glViewport(0, 0, 255, 191);
	ul_screenClippingChanged = 0;
}

void ulResetScreenView()		{
	glMatrixMode(GL_PROJECTION);
	MATRIX_IDENTITY = 0;
	glOrthof32(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -4090, 1);
}

void ulMoveScreenView(int posX, int posY)			{
   MATRIX_TRANSLATE = posX;
   MATRIX_TRANSLATE = posY;
   MATRIX_TRANSLATE = 0;
}

void ulScaleScreenView(int scaleX, int scaleY)		{
   MATRIX_SCALE = scaleX;
   MATRIX_SCALE = scaleY;
   MATRIX_SCALE = inttof32(1);
}

void ulRotateScreenView(int angle)			{
	int32 sinus = ulSin(angle, inttof32(1));
	int32 cosinus = ulCos(angle, inttof32(1));
	
	MATRIX_MULT3x3 = cosinus;
	MATRIX_MULT3x3 = sinus;
	MATRIX_MULT3x3 = 0;
	
	MATRIX_MULT3x3 = - sinus;
	MATRIX_MULT3x3 = cosinus;
	MATRIX_MULT3x3 = 0;
	
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = inttof32(1);
}

//Coeff: 0 - 31 (opacit�)
//Effect: 
void ulSetAlpha(UL_SPECIAL_EFFECT effect, int coeff, int polygroup)		{
	//Active l'alpha blending
	if (effect != UL_FX_ALPHA)
		coeff = 31;
	glPolyFmt(POLY_ALPHA(coeff) | POLY_CULL_NONE | POLY_ID(polygroup));
}

UL_IMAGE *ulCreateImage(int width, int height, int location, int format, int palCount)		{
	UL_IMAGE *img;
	int textureID, success = 1;
	
	//Cr�ation de l'image
	img = malloc(sizeof(UL_IMAGE));
	if (img)			{
		memset(img, 0, sizeof(UL_IMAGE));
		img->format = format;
		img->textureID = -1;
		img->paletteID = -1;
		img->location = location;

		img->sizeX = width;
		img->sizeY = height;
		img->sysSizeX = 1 << ulGetPowerOf2Count(width);
		//The height doesn't have to be rounded to a power of two, ulTexImage2D will do the job to create a texture which is big enough but where only a part of data are actually used.
		if (ul_optimizeTextureSize)
			img->sysSizeY = height;
		else if (ul_initTexturesToZero & UL_TEXINITZERO_RAM)
			img->sysSizeY = 1 << ulGetPowerOf2Count(height);

		//Alloue la m�moire
		if (location == UL_IN_RAM)			{
		   int size = (ul_pixelSizes[format] * img->sysSizeX * img->sysSizeY) >> 3;
			img->texture = malloc(size);
			img->imgState = UL_STATE_RAM_BLOCK;
			if (!img->texture)
				success = 0;
			else
			   //For textures in RAM, we first initialize them to zero
			   memset(img->texture, 0, size);
		}
		else if (location == UL_IN_VRAM)			{
		   int texGen = TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT;
		   if (ul_firstPaletteColorOpaque == 2)
			   texGen &= ~GL_TEXTURE_COLOR0_TRANSPARENT;

			//On cr�e l'image directement en VRAM
			ulGenTextures(1, &textureID);
			ulBindTexture(0, textureID);
			if (ulTexImage2D(0, 0, img->format, img->sizeX, img->sizeY, 0, texGen, NULL))			{
				//Lib�re si on n'en a plus besoin
				if (img->imgState == UL_STATE_RAM_BLOCK)			{
					free(img->texture);
					img->texture = NULL;
				}
				//Syst�me
				img->textureID = textureID;
				img->imgState = UL_STATE_VRAM;
			}
			else
				success = 0, img->imgState = 0;
		}
		
		if (success)
			success = ulCreateImagePalette(img, location, palCount);

		//Initialise les propri�t�s de l'image
		ulResetImageProperties(img);
		
		//Il y a eu un probl�me?
		if (!success)			{
		   ulDeleteImage(img);
		   img = NULL;
		}
	}
	return img;
}

//Copie une image en VRAM pour qu'on puisse l'utiliser
int ulRealizeImage(UL_IMAGE *img)		{
	int textureID, success = 0;
	if (img->texture /*&& img->textureID == -1*/ && img->imgState != UL_STATE_VRAM)			{
	   int texGen = TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT;
	   if (ul_firstPaletteColorOpaque == 2)
		   texGen &= ~GL_TEXTURE_COLOR0_TRANSPARENT;

		//Si une texture existe d�j�, on la supprime
		if (img->textureID != -1)		{
			int texID = img->textureID;
			ulFreeTextures(1, &texID);
		}
		ulGenTextures(1, &textureID);
		ulBindTexture(0, textureID);
		if (ulTexImage2D(0, 0, img->format, img->sizeX, img->sizeY, 0, texGen, (u8*)img->texture))			{
			//Lib�re si on n'en a plus besoin
			if (img->imgState == UL_STATE_RAM_BLOCK)			{
				free(img->texture);
			}
			img->texture = NULL;
			//Syst�me
			img->textureID = textureID;
			img->imgState = UL_STATE_VRAM;
			success = 1;
		}
	}
	
	//Ne continue que si le premier a r�ussi
	if (success)			{
		if (img->palette /*&& img->paletteID == -1*/ && img->palState != UL_STATE_VRAM)			{
			//Si la palette existe d�j�, on la supprime
			if (img->paletteID != -1)			{
				uluTexUnloadPal(img->paletteID);
			}
			//Aligner la taille � 2
			if (img->palCount & 1)
				img->palCount++;
			img->paletteID = uluTexLoadPal(img->palette, img->palCount, img->format);
			if (img->paletteID >= 0)			{
				img->palState = UL_STATE_VRAM;
				//Lib�re si on n'en a plus besoin
				if (img->palState == UL_STATE_RAM_BLOCK)			{
					free(img->palette);
				}
				img->palette = NULL;
				success = 1;
			}
		}
	}
	return success;
}

void ulSetTexture(UL_IMAGE *img)		{
//   if (img == ul_lastTexture)
//   	return;

//	ul_lastTexture = img;
	
   if (!img)		{
      GFX_TEX_FORMAT = 0;
      return;
   }

   //On s'assure que l'image est bien en m�moire vid�o
   if (img->imgState != UL_STATE_VRAM)
   	ulRealizeImage(img);
   
	//Textur�
	if (img->textureID >= 0)			{
		ulBindTexture(GL_TEXTURE_2D, img->textureID);

		//Palette
		if (img->format != GL_RGB)
			GFX_PAL_FORMAT = ((u32)img->paletteID) >> (4 - (img->format == GL_RGB4));
	}
	else
		GFX_TEX_FORMAT = 0;
}

void ulResetImageProperties(UL_IMAGE *img)		{
	//Positions
	img->x = img->y = 0;

	//Offset
	img->offsetX0 = img->offsetY0 = 0;
	img->offsetX1 = img->sizeX;
	img->offsetY1 = img->sizeY;

	//Rotation	
	img->centerX = img->centerY = img->angle = 0;

	//Scaling
	img->stretchX = img->sizeX;
	img->stretchY = img->sizeY;
	
	//Teinte
	img->tint1 = img->tint2 = img->tint3 = img->tint4 = 0x7fff;
}

void ulDeleteImage(UL_IMAGE *img)			{
   int texId = img->textureID;
   
   //Supprime la texture en fonction de sa localisation
	if (img->imgState == UL_STATE_RAM_BLOCK)
		free(img->texture);
	else if (img->imgState == UL_STATE_VRAM)
	   ulFreeTextures(1, &texId);
	   
	if (img->palState == UL_STATE_RAM_BLOCK)
		free(img->palette);
	else if (img->palState == UL_STATE_VRAM)
		uluTexUnloadPal(img->paletteID);
		
	free(img);
}

//Est-ce vraiment utile? J'en doute, mais bon.
void ulSetScreenBackgroundColor(UL_COLOR color)			{
	GFX_CLEAR_COLOR = color;
}
   
void ulSetTransparentColor(unsigned long color)		{
	if (color & 0xffff0000)
	{
		ul_colorKeyEnabled = 32;
		ul_colorKeyValue32 = color;
		// For compatibility
		ul_colorKeyValue = RGB15((color & 0xff) >> 3, (color & 0xff00) >> 11, (color & 0xff0000) >> 19);
	}
	else
	{
		ul_colorKeyEnabled = 16;
		ul_colorKeyValue = color;
	}
}

