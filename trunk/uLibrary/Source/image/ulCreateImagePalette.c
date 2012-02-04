#include "ulib.h"

int ulCreateImagePalette(UL_IMAGE *img, int location, int palCount)			{
   int success = 1;
   
	//Cr�e une palette si l'image peut en avoir une
	if (palCount && ulImageIsPaletted(img) && img->palState == UL_STATE_NONE)			{
		//L'endroit o� mettre la palette et l'image doivent �tre identiques!
		if (img->imgState != UL_STATE_NONE)
			location = img->location;
	   
		if (location == UL_IN_RAM)			{
			//Chaque pixel fait 2 octets
			img->palette = malloc(2 * palCount);
			if (img->palette)			{
				img->palCount = palCount;
				img->palState = UL_STATE_RAM_BLOCK;
			}
			else
				success = 0;
		}
		else if (location == UL_IN_VRAM)			{
		   //Directement en VRAM
			img->palCount = palCount;
			//Aligner la taille � 2
			if (img->palCount & 1)
				img->palCount++;
			img->paletteID = uluTexLoadPal(NULL, img->palCount, img->format);
			if (img->paletteID >= 0)
				img->palState = UL_STATE_VRAM;
			else
				success = 0;
		}   
	}
	else		{
		img->palette = NULL;
		img->palCount = 0;
	}
	return success;


/*	if (ulImageIsPaletted(img) && !img->palette)			{
		//Chaque pixel fait 2 octets - toujours 15 bits
		img->palette = malloc(2 * palCount);
		img->palCount = palCount;
		img->palState = UL_STATE_RAM_BLOCK;
	}*/



}

