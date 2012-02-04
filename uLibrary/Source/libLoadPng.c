#include "ulib.h"
#include "libpng/png.h"
#include "libpng/pngconf.h"
#include "libpng/zlib.h"
#include "libpng/zconf.h"

//Lecture / �criture des PNG
static void ulPngReadFn(png_structp png_ptr, png_bytep data, png_size_t length)			{
	VIRTUAL_FILE *f = (VIRTUAL_FILE*)png_get_io_ptr(png_ptr);
	VirtualFileRead(data, length, 1, f);
}

// Other instances of this function may exist elsewhere!
static bool isColorTransparent32(u8 r, u8 g, u8 b, u8 a)
{
	if (ul_colorKeyEnabled == 32)
	{
		u32 color = RGBA32(r, g, b, a);
		if (color == ul_colorKeyValue32 || ((ul_colorKeyValue32 & 0xff000000) == 0 && (color & 0xff000000) == 0))
			return true;
	}
	else if (ul_colorKeyEnabled == 16)
	{
		if ((r >> 3) == (ul_colorKeyValue & 0x1f) &&
			(g >> 3) == ((ul_colorKeyValue>>5) & 0x1f) &&
			(b >> 3) == ((ul_colorKeyValue>>10) & 0x1f))
			return true;
	}
	return false;
}

/*static void errorOutput (png_structp png_ptr, const char *message)
{
	ulDebug(message);
	longjmp(png_ptr->jmpbuf, 1);
}*/

/*void* ulPngStandardGetMemory(u32* width, u32* height, u32 flags, png_info* pi)		{
	return malloc(ul_pixelSizes[flags & 15] * (*width) * (*height));
}*/

//Calcule le carr� de la valeur fournie en param�tre
static int square(int value)		{
	return value * value;
}

//fnGetMemory(width, height, flags, png_info). Peut modifier width et height aux valeurs adapt�es (genre 250 => 256).
UL_IMAGE *ulLoadImagePNG(VIRTUAL_FILE *f, int location, int pixelFormat)
{
	const size_t nSigSize=8;
	int transparentColor = -1;
	u8 signature[ nSigSize ];
	int i, j, dynamicPaletteRequired = 0, dynamicColorsUsed = 1;
//	u32 theTransparentColor = 0, transpActive = 0;
	UL_IMAGE *img = NULL;

	if (VirtualFileRead(signature, sizeof(u8), nSigSize, f) == 0)
	{
		goto error;
	}
	
	if ( png_check_sig(signature, nSigSize) == 0 )
	{
		goto error;
	}
	
	png_struct *	pPngStruct = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );

	if ( pPngStruct == NULL)
	{
		goto error;
	}

	png_info *	pPngInfo = png_create_info_struct( pPngStruct );

	if ( pPngInfo == NULL )
	{
		png_destroy_read_struct( &pPngStruct, NULL, NULL );
		goto error;
	}

	if ( setjmp( pPngStruct->jmpbuf ) != 0 )
	{
		png_destroy_read_struct( &pPngStruct, NULL, NULL );
		goto error;
	}
	
	png_set_read_fn(pPngStruct, f, (png_rw_ptr)ulPngReadFn);
//	png_set_error_fn(pPngStruct, 0, &errorOutput, NULL);
//	png_init_io( pPngStruct, &f );
	png_set_sig_bytes( pPngStruct, nSigSize );
	png_read_png( pPngStruct, pPngInfo, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | /*PNG_TRANSFORM_EXPAND |*/ PNG_TRANSFORM_BGR, NULL );

	png_uint_32 width = pPngInfo->width;
	png_uint_32 height = pPngInfo->height;
	png_uint_32 depth = pPngInfo->bit_depth;
	int color_type = pPngInfo->color_type;

	//On supporte uniquement les palettes 8 bits et moins
/*	if (depth > 8 && color_type == PNG_COLOR_TYPE_PALETTE)			{
		png_destroy_read_struct( &pPngStruct, &pPngInfo, NULL );
		goto error;
	}*/

	png_byte **pRowTable = pPngInfo->row_pointers;
	unsigned char r=0, g=0, b=0, a=0;

	//Pas de palette dans le PNG mais notre pixelformat en veut une?
	if (!pPngInfo->num_palette && ul_pixelWidth[pixelFormat] <= 8)
		dynamicPaletteRequired = 1;

	//Gestion de la couleur transparente - Retir�, devrait �tre fonctionnel mais pas test�, on utilisera le color key
/*	if (pPngInfo->num_trans > 0)			{
	   if (pPngInfo->num_palette > 0)		{
			for (i=0;i<pPngInfo->num_trans;i++)		{
			   if (pPngInfo->trans[i] < 128)
			   	transpActive = 1, theTransparentColor = i;
			}
		}
		else
	   	transpActive = 1, theTransparentColor = ((pPngInfo->trans_values.red) | ((pPngInfo->trans_values.green) << 8) | ((pPngInfo->trans_values.blue) << 16));
	}*/
	
	//Cr�e l'image dans laquelle on mettra notre bitmap
	//On charge toujours en RAM d'abord car l'acc�s � la VRAM n'est pas permis en 8 bits
	img = ulCreateImage(width, height, UL_IN_RAM, pixelFormat, 0);
	if (img)
	{
	   	//S'il y a besoin d'une palette...
			if (ul_pixelWidth[pixelFormat] <= 8)				{
				//!!! Note qu'il faudrait aussi prendre en compte num_trans pour la couleur transparente
				if (!dynamicPaletteRequired)		{
					// Faire attention de ne pas cr�er trop de couleurs, mais en r�server une pour le d�calage si jamais aucune couleur transparente n'a �t� trouv�e
					ulCreateImagePalette(img, UL_IN_RAM, ulMin(pPngInfo->num_palette + 1, 1 << ul_paletteSizes[pixelFormat]));
				}
				else		{
					//Palette dynamique, on cr�e le max possible de couleurs; on verra plus tard
					ulCreateImagePalette(img, UL_IN_RAM, 1 << ul_paletteSizes[pixelFormat]);
				}
				
				if (img->palette && !dynamicPaletteRequired)			{
				   //Make sure to not use too much colors!
				   pPngInfo->num_palette = ulMin(pPngInfo->num_palette, img->palCount);
				   
					//Nous devons trouver la couleur transparente (color key) parmis toutes les couleurs de la palette
					if (ul_colorKeyEnabled)			{
						for (i=0;i<pPngInfo->num_palette;i++)			{
							if (isColorTransparent32(pPngInfo->palette[i].red, pPngInfo->palette[i].green, pPngInfo->palette[i].blue, 0xff))
								transparentColor = i;
						}
					}
					
					//Toujours pas trouv� de couleur transparente??? - on d�cale tout d'un alors pour que la couleur 0 ne soit pas utilis�e
					if (transparentColor == -1 && !ul_firstPaletteColorOpaque)			{
					   transparentColor = pPngInfo->num_palette;
					   ((u16*)img->palette)[0] = 0;
						for (i=0;i<pPngInfo->num_palette;i++)		{
						   //V�rifie qu'on ne d�borde pas
						   if (i + 1 < img->palCount)
						   	((u16*)img->palette)[i + 1] = RGB15(pPngInfo->palette[i].red >> 3, pPngInfo->palette[i].green >> 3, pPngInfo->palette[i].blue >> 3) /*| 1 << 15*/;
						}
					}
					else		{
					   //Tenons compte de la couleur transparente (il faut la mettre en premi�re position pour qu'elle soit transparente)
						i = pPngInfo->num_palette;
						j = pPngInfo->num_palette;
						while (--i >= 0)  {
						   if (i == transparentColor)
						   	((u16*)img->palette)[0] = RGB15(pPngInfo->palette[i].red >> 3, pPngInfo->palette[i].green >> 3, pPngInfo->palette[i].blue >> 3);
						   else
						      //Cette couleur est affich�e (0x8000 = alpha � 1)
								((u16*)img->palette)[--j] = RGB15(pPngInfo->palette[i].red >> 3, pPngInfo->palette[i].green >> 3, pPngInfo->palette[i].blue >> 3) | 1 << 15;
						}
					}
				}
			}
			
			u16 *p_dest2 = (u16*)img->texture;
			u8  *p_dest1 = (u8*) img->texture;
			int x, y;
			int color_per_entry = 8 / depth;
			int color_offset, pixel_value = 0;
			int mask = (1 << depth) - 1;

			for ( y = 0; y < height; ++y )
			{
				const png_byte *	pRow = pRowTable[ y ];

				for ( x = 0; x < width; ++x )
				{
					switch ( color_type )
					{
					case PNG_COLOR_TYPE_GRAY:
						r = g = b = *pRow++;
						if ( r == 0 && g == 0 && b == 0 )	a = 0x00;
						else								a = 0xff;
						break;
					case PNG_COLOR_TYPE_GRAY_ALPHA:
						r = g = b = *pRow++;
						if ( r == 0 && g == 0 && b == 0 )	a = 0x00;
						else								a = 0xff;
						pRow++;
						break;

					case PNG_COLOR_TYPE_RGB:
						b = *pRow++;
						g = *pRow++;
						r = *pRow++;
						a = 0xff;
						break;

					case PNG_COLOR_TYPE_RGB_ALPHA:
						b = *pRow++;
						g = *pRow++;
						r = *pRow++;
						a = *pRow++;
						break;

					case PNG_COLOR_TYPE_PALETTE:
						color_offset = x % color_per_entry;
						pixel_value = (*pRow >> (8 - depth * (color_offset + 1))) & mask;
						//Dernier pixel pour cet octet
						if (x % color_per_entry == color_per_entry - 1)
							pRow++;
						b = pPngInfo->palette[pixel_value].blue;
						g = pPngInfo->palette[pixel_value].green;
						r = pPngInfo->palette[pixel_value].red;
						a = 0xff;
						break;
					}
					
					//Il y a eu un swap entre la couleur transparente et la couleur z�ro
			      if (pixel_value == transparentColor)
			      	pixel_value = 0;
			      else if (pixel_value < transparentColor)
			      	pixel_value++;
					
					if (isColorTransparent32(r, g, b, a))
						a = 0;

/*					//Check avec la couleur transparente
					if ((r | g << 8 | v << 16) == theTransparentColor)
						a = 0;*/

					//Palette dynamique
					if (dynamicPaletteRequired && img->palette)			{
					   u16 color = RGB15(r >> 3, g >> 3, b >> 3);
					   u16 *pal = (u16*)img->palette;
					   int firstColor = 0;

						//Si la premi�re couleur est transparente, on ne peut pas l'utiliser
//						if (ul_firstPaletteColorOpaque && dynamicColorsUsed == 0)
//							firstColor = dynamicColorsUsed = 1;
						if (!ul_firstPaletteColorOpaque)
							firstColor = 1;

						if (a == 0 && !ul_firstPaletteColorOpaque)			{
						   //Pas d'alpha (invisible) => couleur 0
						   i = 0;
						}
						else		{
						   //Parcourt la palette pour voir si elle se trouve dans la palette
						   for ( i = firstColor;
								   i < dynamicColorsUsed && pal[i] != color;
									i++);
						}
					
						//Trouv�?
						if (i < dynamicColorsUsed)
							pixel_value = i;
						else		{
						   //Reste-t-il des couleurs libres?
						   if (dynamicColorsUsed < img->palCount)		{
						      //Ajoute la couleur � la palette
						      pal[dynamicColorsUsed] = color;
						      //Et l'utilise comme couleur pour le pixel courante
						      pixel_value = dynamicColorsUsed;
						      dynamicColorsUsed++;
						   }
						   else		{
						      //A�e! Plus de place... on va essayer de trouver une couleur proche
							   int minDistance = 0;
							   for (i = 0; i < dynamicColorsUsed; i++)		{
									//Calcule la distance entre les couleurs dans le cube colorim�trique
									int distance =
										square(r - ulGetColorRed(pal[i])) +
										square(g - ulGetColorGreen(pal[i])) +
										square(b - ulGetColorBlue(pal[i]));
									
									//Est-ce celle qui convient le mieux pour l'instant? (la premi�re fois c'est forc�ment le cas)
									if (distance < minDistance || i == 0)		{
									   minDistance = distance;
									   pixel_value = i;
									}
								}
							}
						}   
					}

					if (pixelFormat == UL_PF_5551)
						p_dest2[x] = RGB15(r >> 3, g >> 3, b >> 3) | (a >> 7) << 15;
					else if (pixelFormat == UL_PF_5550)
						p_dest2[x] = RGB15(r >> 3, g >> 3, b >> 3);
					else if (pixelFormat == UL_PF_PAL8)
						p_dest1[x] = pixel_value;
					else if (pixelFormat == UL_PF_PAL5_A3)
						p_dest1[x] = (pixel_value & 31) | (a & ~31);
					else if (pixelFormat == UL_PF_PAL3_A5)
						p_dest1[x] = (pixel_value & 7) | (a & ~7);
					else if (pixelFormat == UL_PF_PAL4)				{
						p_dest1[x >> 1] &= ~(15 << ((x & 1) << 2));
						p_dest1[x >> 1] |= (pixel_value & 15) << ((x & 1) << 2);
					}
					else if (pixelFormat == UL_PF_PAL2)				{
						p_dest1[x >> 2] &= ~(3 << ((x & 3) << 1));
						p_dest1[x >> 2] |= (pixel_value & 3) << ((x & 3) << 1);
					}
				}

				p_dest1 += (img->sysSizeX * ul_pixelSizes[pixelFormat]) >> 3;
				p_dest2 += img->sysSizeX;
			}
	}

	png_destroy_read_struct( &pPngStruct, &pPngInfo, NULL );
	
	//On doit reconvertir en palette
//	if (wantedPixelFormat != pixelFormat)
//	   img = ulConvertImageToPaletted(img, UL_IN_RAM, wantedPixelFormat);

	if (location == UL_IN_VRAM)			{
		if (!ulRealizeImage(img))			{
		   ulDeleteImage(img);
			return NULL;
		}
	}
	
error:
	return img;
}

