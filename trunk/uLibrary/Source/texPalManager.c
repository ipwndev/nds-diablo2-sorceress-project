#include "ulib.h"

/*
	PALETTE

	Les adresses (paletteID) sont � l'octet pr�s, partant de 0 (d�but de la bank allou�e aux TEX_PALETTE)
*/

//On commence au d�but de la VRAM mapp�e en LCD
void *ul_texPalVramBase = (void*)VRAM_F;
//16 ko de RAM par d�faut
int ul_texPalVramSize = 16 << 10;
//Banque A seulement par d�faut
UL_BANKS ul_texPalVramBanks = UL_BANK_F;

#define DEFAULT_TABLE_SIZE 128

int ulTexPalBlocksMax, ulTexPalBlocksNb;

typedef struct		{
	u16 offset, size;
} UL_TEXPALBLOCK;

#define isBlockFree(i)					(ulTexPalBlocks[i].size & 0x8000)
#define getBlockSize(i)					(ulTexPalBlocks[i].size & 0x7fff)
#define getBlockOffset(i)				(ulTexPalBlocks[i].offset)

#define setBlockFree(i, free)			(ulTexPalBlocks[i].size = (ulTexPalBlocks[i].size & ((free)? 0xffff : 0x7fff)) | ((free)? 0x8000 : 0))
#define setBlockSize(i, nsize)		(ulTexPalBlocks[i].size = (ulTexPalBlocks[i].size & ~0x7fff) | (nsize))
#define setBlockOffset(i, noffset)	(ulTexPalBlocks[i].offset = noffset)

UL_TEXPALBLOCK *ulTexPalBlocks;

void ulTexPalInit()		{
	ulTexPalBlocksMax = DEFAULT_TABLE_SIZE;
	ulTexPalBlocksNb = 1;
	ulTexPalBlocks = (UL_TEXPALBLOCK*)malloc(ulTexPalBlocksMax * sizeof(UL_TEXPALBLOCK));
	//Premier bloc: libre, taille totale de la VRAM, adresse 0
	setBlockOffset(0, 0);
	setBlockSize(0, ul_texPalVramSize >> 3);
	setBlockFree(0, 1);
}

int ulTexPalAllocBlock(int blockSize, int format)		{
	int i, align;

	align = (format == GL_RGB4) ? 8 : 16;
   
	//La taille est toujours multiple de 8 ou 16 - arrondir au bloc sup�rieur
//	if (initialBlockSize & (align - 1))
//		initialBlockSize += align - (initialBlockSize & (align - 1));
	if (blockSize & 7)
		blockSize += 8;
	blockSize >>= 3;

	for (i=0;i<ulTexPalBlocksNb;i++)		{
	   if (!isBlockFree(i))
			continue;
		//Si le bloc n'est pas align�, il faut un peu plus d'espace
		if ((getBlockOffset(i) & 1) && align == 16)				{
			//Bloc suffisant?
			if (getBlockSize(i) >= blockSize + 1)		{
			   //Le bloc pr�c�dent sera aggrandi de fa�on � ce que le nouveau d�bute � la position align�e
				if (i > 0)			{
					setBlockSize(i - 1, getBlockSize(i - 1) + 1);
					setBlockSize(i, getBlockSize(i) - 1);
					setBlockOffset(i, getBlockOffset(i) + 1);
				}
				break;
			}
		}
		else			{
			//Ce bloc est-il suffisant?
			if (getBlockSize(i) >= blockSize)
				break;
		}
	}

	//Aucun bloc libre
	if (i >= ulTexPalBlocksNb)
		return -1;

	//Pile la m�moire qu'il faut? - supprim� pour la m�me raison que ulTexVramAllocBlock
	if (getBlockSize(i) == blockSize && i != ulTexPalBlocksNb - 1)			{
		//Il n'est plus libre
		setBlockFree(i, 0);
	}
	else		{
		//On va ajouter un nouveau bloc
		ulTexPalBlocksNb++;
		
		//Plus de m�moire pour le tableau? On l'aggrandit
		if (ulTexPalBlocksNb >= ulTexPalBlocksMax)			{
			UL_TEXPALBLOCK *oldBlock = ulTexPalBlocks;
			ulTexPalBlocksMax += DEFAULT_TABLE_SIZE;
			ulTexPalBlocks = (UL_TEXPALBLOCK*)realloc(ulTexPalBlocks, ulTexPalBlocksMax);

			//V�rification que la m�moire a bien pu �tre allou�e
			if (!ulTexPalBlocks)		{
				ulTexPalBlocks = oldBlock;
				ulTexPalBlocksMax -= DEFAULT_TABLE_SIZE;
				//Pas assez de m�moire
				return -1;
			}
		}

		//D�calage pour ins�rer notre nouvel �l�ment
		memmove(ulTexPalBlocks + i + 1, ulTexPalBlocks + i, sizeof(UL_TEXPALBLOCK) * (ulTexPalBlocksNb - i - 1));
		
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
int ulTexPalFreeBlock(int blockOffset)		{
	int i, j, updateNeeded;

	for (i=0;i<ulTexPalBlocksNb;i++)		{
		if (getBlockOffset(i) == blockOffset)
			break;
	}

	//Impossible de trouver le bloc
	if (i >= ulTexPalBlocksNb)
		return 0;


	//Le bloc est maintenant libre ^^
	setBlockFree(i, 1);

	//Bon maintenant reste � "assembler" les blocs libres adjacents
	do		{
		updateNeeded = 0;
		for (j=0;j<ulTexPalBlocksNb-1;j++)			{
			//Cherchons deux blocs adjacents
			if ((isBlockFree(j) && isBlockFree(j + 1))
				|| (isBlockFree(j) && getBlockSize(j) == 0))			{
				//Assemblons ces blocs maintenant
				int newSize = getBlockSize(j) + getBlockSize(j + 1), newAdd = getBlockOffset(j);
				memmove(ulTexPalBlocks + j, ulTexPalBlocks + j + 1, sizeof(UL_TEXPALBLOCK) * (ulTexPalBlocksNb - j - 1));
				setBlockOffset(j, newAdd);
				setBlockSize(j, newSize);
				//Le bloc entre deux est supprim�
				ulTexPalBlocksNb--;
				//ATT: On devra refaire un tour pour v�rifier si de nouveaux blocs n'ont pas �t� cr��s
				updateNeeded = 1;
			}
		}

	} while (updateNeeded);

	return 1;
}

void uluTexUnloadPal(u32 addr)		{
	addr = ulTexPalAddressToOffset(addr);
	ulTexPalFreeBlock(addr);
}

//---------------------------------------------------------------------------------
//Alignement sup�rieur (16, 32 => 32 / 64, 32 => 64)
#define alignVal(val, to)  (((val) & ((to)-1))? ((val) & ~((to)-1)) + (to) : (val))

void ulTexLoadPal(u16* pal, u16 count, u32 addr) {
 	ulChangeVramAllocation(ul_texPalVramBanks, UL_BANK_TYPE_LCD);

	// Words are 32 bits => copy number of colors / 2 words
	swiCopy(pal, ulGetPaletteAddress(addr) , (count >> 1) | COPY_MODE_WORD);

 	ulChangeVramAllocation(ul_texPalVramBanks, UL_BANK_TYPE_TEXPAL);
}

int uluTexLoadPal(u16* pal, u16 count, uint8 format)		 {
	//Ca c'est la taille totale de la m�moire
	int memSize;
	int addr;

	// D�termine la taille de la palette en octets
	memSize = alignVal(count << 1, 1 << 3);
	//Dans les formats autres que GL_RGB4, on doit aligner � 16 octets au lieu de 8
	addr = (int)ulTexPalOffsetToAddress(ulTexPalAllocBlock(memSize, format));
   
	if (addr >= 0 && pal)
		// Every color is 2 bytes
		ulTexLoadPal(pal, memSize >> 1, (u32) addr);

	return addr;
}

//1 on success, 0 on failure
int ulSetTexPalVramParameters(int activeBanks, void *baseAddr, int totalSize)			{
   int curVramSize = ul_texPalVramSize >> 4;
   int blockNum = ulTexPalBlocksNb - 1;
   int sizeDiff;

	//La taille est toujours multiple de 8 - arrondir au bloc sup�rieur
	if (totalSize & 7)
		totalSize += 8;
	totalSize >>= 3;

	//Diff�rence de taille (n�gatif pour r�duction, positif pour aggrandissement)
   sizeDiff = totalSize - curVramSize;
   
   //Le dernier bloc est TOUJOURS libre, m�me s'il reste 0 octet. Cf ulTexPalAllocBlock.
   if (isBlockFree(blockNum) && getBlockSize(blockNum) + sizeDiff >= 0)			{
      setBlockSize(blockNum, getBlockSize(blockNum) + sizeDiff);
	   ul_texPalVramBase = baseAddr;
	   ul_texPalVramSize = totalSize;
	   ul_texPalVramBanks = activeBanks;
	   ulChangeVramAllocation(ul_texPalVramBanks, UL_BANK_TYPE_TEXPAL);
   }
   else
   	return 0;
   return 1;
}

int ulGetTexPalAvailMemory()		{
   return getBlockSize(ulTexPalBlocksNb - 1) << 3;
}   

int ulGetTexPalTotalMemory()			{
   return ul_texPalVramSize;
}

int ulGetTexPalUsedMemory()			{
   return ul_texPalVramSize - ulGetTexPalAvailMemory();
}

