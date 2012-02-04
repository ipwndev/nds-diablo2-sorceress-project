#include "ulib.h"

/*
	TODO: Calculer la taille totale de la VRAM textures en fonction de l'allocation des banques


*/

//On commence au d�but de la VRAM mapp�e en LCD
void *ul_texVramBase = (void*)0x06800000;
//128 ko par d�faut
int ul_texVramSize = 128 << 10;
//Banque A seulement par d�faut
UL_BANKS ul_texVramBanks = UL_BANK_A;
u8 ul_optimizeTextureSize = 1;
//By default, initialize every texture to zero
u8 ul_initTexturesToZero = UL_TEXINITZERO_ALL;

#define DEFAULT_TABLE_SIZE 1024

int ulTexRamBlocksMax, ulTexRamBlocksNb;

typedef struct		{
	u16 offset, size;
} UL_TEXRAMBLOCK;

#define isBlockFree(i)				(ulTexRamBlocks[i].size & 0x8000)
#define getBlockSize(i)				(ulTexRamBlocks[i].size & 0x7fff)
#define getBlockOffset(i)			(ulTexRamBlocks[i].offset)

#define setBlockFree(i, free)		(ulTexRamBlocks[i].size = (ulTexRamBlocks[i].size & ((free)? 0xffff : 0x7fff)) | ((free)? 0x8000 : 0))
#define setBlockSize(i, nsize)		(ulTexRamBlocks[i].size = (ulTexRamBlocks[i].size & ~0x7fff) | (nsize))
#define setBlockOffset(i, noffset)	(ulTexRamBlocks[i].offset = noffset)

// >> 3 dans textureparams et << 4 (mult. 16 ici)
#define getTextureOffset(name) ((ulTextureParams[name] & 0xFFFF) >> 1)

UL_TEXRAMBLOCK *ulTexRamBlocks;

void ulTexVramInit()		{
	ulTexRamBlocksMax = DEFAULT_TABLE_SIZE;
	ulTexRamBlocksNb = 1;
	ulTexRamBlocks = (UL_TEXRAMBLOCK*)malloc(ulTexRamBlocksMax * sizeof(UL_TEXRAMBLOCK));
	//Premier bloc: libre, taille totale de la VRAM, adresse 0
	setBlockOffset(0, 0);
	//La taille en blocs doit �tre divis�e par 16 puisqu'on n'utilise pas des octets sinon il serait impossible de coder toute la VRAM sur 16 bits
	setBlockSize(0, ul_texVramSize >> 4);
	setBlockFree(0, 1);
}

int ulTexVramAllocBlock(int blockSize)		{
	int i;

	//Le bloc ne peut pas �tre de taille nulle ou n�gative
	if (blockSize <= 0)
		return -1;

	//La taille est toujours multiple de 16 - arrondir au bloc sup�rieur
	if (blockSize & 15)
		blockSize += 16;
	blockSize >>= 4;

	for (i=0;i<ulTexRamBlocksNb;i++)		{
		//Ce bloc est-il suffisant?
		if (isBlockFree(i) && getBlockSize(i) >= blockSize)
			break;
	}

	//Aucun bloc libre
	if (i >= ulTexRamBlocksNb)
		return -1;

	//Pile la m�moire qu'il faut? - pas g�r�, il faut toujours que le dernier bloc soit marqu� comme libre (m�me s'il reste 0 octet) pour ulSetTexVramParameters
	if (getBlockSize(i) == blockSize && i != ulTexRamBlocksNb - 1)			{
		//Il n'est plus libre
		setBlockFree(i, 0);

	}
	else		{
		//On va ajouter un nouveau bloc
		ulTexRamBlocksNb++;

		//Plus de m�moire pour le tableau? On l'aggrandit
		if (ulTexRamBlocksNb >= ulTexRamBlocksMax)			{
			UL_TEXRAMBLOCK *oldBlock = ulTexRamBlocks;
			ulTexRamBlocksMax += DEFAULT_TABLE_SIZE;
			ulTexRamBlocks = (UL_TEXRAMBLOCK*)realloc(ulTexRamBlocks, ulTexRamBlocksMax);

			//V�rification que la m�moire a bien pu �tre allou�e
			if (!ulTexRamBlocks)		{
				ulTexRamBlocks = oldBlock;
				ulTexRamBlocksMax -= DEFAULT_TABLE_SIZE;
				//Pas assez de m�moire
				return -1;
			}
		}

		//D�calage pour ins�rer notre nouvel �l�ment
		memmove(ulTexRamBlocks + i + 1, ulTexRamBlocks + i, sizeof(UL_TEXRAMBLOCK) * (ulTexRamBlocksNb - i - 1));

		//Remplissons notre nouveau bloc
		setBlockSize(i, blockSize);
		//Il a l'adresse du bloc qui �tait l� avant
		setBlockOffset(i, getBlockOffset(i + 1));
		//Il n'est pas libre
		setBlockFree(i, 0);

		//Pour le prochain, sa taille diminue
		setBlockSize(i + 1, getBlockSize(i + 1) - blockSize);
		//ATTENTION: calcul d'offset
		setBlockOffset(i + 1, getBlockOffset(i + 1) + blockSize);
	}

	//Note: il faut traduire l'offset en vraie adresse
	return getBlockOffset(i);
}

//Note: il faut traduire une vraie adresse en offset
int ulTexVramFreeBlock(int blockOffset)		{
	int i, j, updateNeeded;

	for (i=0;i<ulTexRamBlocksNb;i++)		{
		if (getBlockOffset(i) == blockOffset)
			break;
	}

	//Impossible de trouver le bloc
	if (i >= ulTexRamBlocksNb)
		return 0;

	//Le bloc est maintenant libre ^^
	setBlockFree(i, 1);

	//Bon maintenant reste � "assembler" les blocs libres adjacents
	do		{
		updateNeeded = 0;
		for (j=0;j<ulTexRamBlocksNb-1;j++)			{
			//Cherchons deux blocs adjacents
			if ((isBlockFree(j) && isBlockFree(j + 1))
				|| (isBlockFree(j) && getBlockSize(j) == 0))			{
				//Assemblons ces blocs maintenant
				int newSize = getBlockSize(j) + getBlockSize(j + 1), newAdd = getBlockOffset(j);
				memmove(ulTexRamBlocks + j, ulTexRamBlocks + j + 1, sizeof(UL_TEXRAMBLOCK) * (ulTexRamBlocksNb - j - 1));
				setBlockOffset(j, newAdd);
				setBlockSize(j, newSize);
				//Le bloc entre deux est supprim�
				ulTexRamBlocksNb--;
				//ATT: On devra refaire un tour pour v�rifier si de nouveaux blocs n'ont pas �t� cr��s
				updateNeeded = 1;
			}
		}

	} while (updateNeeded);

	return 1;
}


//Inspir�e de la fonction de libnds
//Attention: sizeX et sizeY sont les VRAIES tailles (en pixel)! Elles seront align�es aux prochaines puissances de deux!
int ulTexImage2D(int target, int empty1, int type,
                 int sizeX, int sizeY,
                 int empty2, int param,
                 uint8* texture) {
//---------------------------------------------------------------------------------
	uint32 size = 0;
	int32 texId;
	uint32* addr;
	//Unoptimized sizeX
	int uoSizeX = sizeX;
//  uint32 vramTemp;

	sizeX = 1 << ulGetPowerOf2Count(sizeX);
	//L'optimisation pour les tailles de palettes permet de r�duire l'espace vertical allou� (pas d'alignement � une puissance de deux)
	if (!ul_optimizeTextureSize)
		sizeY = 1 << ulGetPowerOf2Count(sizeY);
	size = (sizeX * sizeY * ul_pixelSizes[type]) >> 3;

	texId = ulTexVramAllocBlock(size);
	if (texId < 0)
		return 0;
	addr = ulTexVramOffsetToAddress(texId);

   swiWaitForVBlank();
	// unlock texture memory
	ulChangeVramAllocation(ul_texVramBanks, UL_BANK_TYPE_LCD);

	if (type == GL_RGB) {
		// We do GL_RGB as GL_RGBA, but we set each alpha bit to 1 during the copy
		u16 * src = (u16*)texture;
		u16 * dest = (u16*)addr;

		//Valeur de la texture: utilis� pour GFX_TEX_FORMAT
		ulTexParameter(ulGetPowerOf2Count(sizeX) - 3, ulGetPowerOf2Count(sizeY) - 3, addr, GL_RGBA, param);

		if (texture)		{
		   int number = 0;
			while (size--) {
			   //Do not make opaque pixels that are outside of the screen
			   if (number < uoSizeX)
					*dest++ = *src | (1 << 15);
				else
					*dest++ = *src & 0x7fff;
				src++;
				number++;
				if (number >= sizeX)
					number -= sizeX;
			}
		}
	}
	else		{
		// For everything else, we do a straight copy
		ulTexParameter(ulGetPowerOf2Count(sizeX) - 3, ulGetPowerOf2Count(sizeY) - 3, addr, type, param);
		if (texture)
			swiCopy((uint32*)texture, addr , (size >> 2) | COPY_MODE_WORD);
	}

	//No texture passed - eventually fill the texture with zero
	if (!texture)			{
	   if (ul_initTexturesToZero & UL_TEXINITZERO_VRAM)
		   memset(addr, 0, size);
	}

 	ulChangeVramAllocation(ul_texVramBanks, UL_BANK_TYPE_TEXTURE);
	return 1;
}

#define UL_MAX_TEXTURES 128
#define TEXTURE_FREE (~0)
int *ulTextureParams;
int ulTextureNb;
int ulTextureMax;
int ulTextureActive;

void ulInitTextures()		{
   ulTextureActive = 0;
	ulTextureMax = UL_MAX_TEXTURES;
	ulTextureParams = (int*)malloc(sizeof(ulTextureParams[0]) * ulTextureMax);
	memset(ulTextureParams, 0xff, sizeof(ulTextureParams[0]) * ulTextureMax);
	ulTextureNb = 0;
}

int ulGetNextAvailableTexture()			{
	int iterate = 0;

	while (iterate < 3)		{
		//Trouve la prochaine texture libre
		while (ulTextureNb < ulTextureMax
			&& ulTextureParams[ulTextureNb] != TEXTURE_FREE)			{
			ulTextureNb++;
		}

		//Pas de texture disponible, on recommence � z�ro
		if (ulTextureNb >= ulTextureMax)			{
			iterate++;

			//On a d�j� fait une fois un tour complet + un tour partiel mais on n'a rien trouv� => plus de m�moire
			if (iterate == 2)			{
				int *oldPtr = ulTextureParams;
				//On rajoute un peu de m�moire
				ulTextureParams = (int*)realloc(ulTextureParams, sizeof(ulTextureParams[0]) * (ulTextureMax + UL_MAX_TEXTURES));
				//L'allocation a foir� => Plus de m�moire
				if (!ulTextureParams)			{
					ulTextureParams = oldPtr;
					break;
				}
				//Remplit les prochaines �l�ments du tableau pour les marquer comme "libres"
				memset(ulTextureParams + ulTextureMax, 0xff, sizeof(ulTextureParams[0]) * UL_MAX_TEXTURES);
				ulTextureMax += UL_MAX_TEXTURES;
			}

			ulTextureNb = 0;
		}
		else
			//On a trouv� notre texture =)
			break;
	}

	if (iterate >= 3)
		//Rien � faire: (vraiment) plus de m�moire
		return -1;
	else
		//La prochaine fois, on ne prendra plus la m�me (post incr�mentation)
		return ulTextureNb++;
}

int ulGenTextures(int n, int *names) 		{
	int index;

	for(index = 0; index < n; index++) {
		int newTexture = ulGetNextAvailableTexture();

		if (newTexture >= 0)		{
			names[index] = newTexture;
			//Pour �viter de le consid�rer comme pris
			ulTextureParams[newTexture] = 0;
		}
		else
			return 0;
	}

	return 1;
}

void ulFreeTextures(int n, int *names)		{
	int index;
	for (index=0;index<n;index++)			{
	   //Pas libre => � lib�rer
	   if (ulTextureParams[names[index]] != TEXTURE_FREE)			{
			ulTexVramFreeBlock(getTextureOffset(names[index]));
	   }
		ulTextureParams[names[index]] = TEXTURE_FREE;
	}
}

//Param�trage d'une texture
void ulTexParameter(uint8 sizeX, uint8 sizeY,
					uint32* addr,
					uint8 mode,
					uint32 param)
{
	ulTextureParams[ulTextureActive] = param | (sizeX << 20) | (sizeY << 23) | (((uint32)addr >> 3) & 0xFFFF) | (mode << 26);
}

void ulBindTexture(int target, int name) {
//	if (name == 0)
//		GFX_TEX_FORMAT = 0;
//	else
		GFX_TEX_FORMAT = ulTextureParams[name];

	ulTextureActive = name;
}

//Permet d'utiliser une texture de UL avec la librairie VideoGL int�gr�e � libnds
void ulBindTextureToGl(int target, int name)		{

	DynamicArraySet( &glGlob->texturePtrs, glGlob->activeTexture, ulTextureParams[name] );
	glBindTexture(target, name);
}

//1 on success, 0 on failure
int ulSetTexVramParameters(int activeBanks, void *baseAddr, int totalSize)			{
   int curVramSize = ul_texVramSize >> 4;
   int blockNum = ulTexRamBlocksNb - 1;
   int sizeDiff;

	//La taille est toujours multiple de 16 - arrondir au bloc sup�rieur
	if (totalSize & 15)
		totalSize += 16;
	totalSize >>= 4;

	//Diff�rence de taille (n�gatif pour r�duction, positif pour aggrandissement)
   sizeDiff = totalSize - curVramSize;

   //Le dernier bloc est TOUJOURS libre, m�me s'il reste 0 octet. Cf la bidouille dans ulTexVramAlloc
   if (isBlockFree(blockNum) && getBlockSize(blockNum) + sizeDiff >= 0)			{
      setBlockSize(blockNum, getBlockSize(blockNum) + sizeDiff);
	   ul_texVramBase = baseAddr;
	   ul_texVramSize = totalSize;
	   ul_texVramBanks = activeBanks;
	   ulChangeVramAllocation(ul_texVramBanks, UL_BANK_TYPE_TEXTURE);
   }
   else
   	return 0;
   return 1;
}

int ulGetTexVramAvailMemory()		{
   return getBlockSize(ulTexRamBlocksNb - 1) << 4;
}

int ulGetTexVramTotalMemory()			{
   return ul_texVramSize;
}

int ulGetTexVramUsedMemory()			{
   return ul_texVramSize - ulGetTexVramAvailMemory();
}



