#include "interface.h"
#include "uldata.h"
#include "Sor_cantuseyet.h"

extern int curMaxSprite;
bool dialbox=0;
extern const unsigned short skillmenu_map[24][32];
extern u8 skillsLevels[SKILLNUMBER];
const u8 requiredLevel[SKILLNUMBER]={1/*firebolt*/,1/*icebolt*/,15/*iceorb*/,5/*firewall*/,10/*blaze*/,15/*hydra*/,1/*chargedbolt*/,5/*teleport*/};


extern u8 skillpoints;
void save ()
{
	FILE *save_file = fopen("fat:/d2save.sav", "wb");
    if (!save_file)PA_OutputText(1,0,0,"Can't open save, emulator?");
	fwrite(&hero.stats, 1, sizeof(hero.stats), save_file);
	fwrite(&skillsLevels, 1, sizeof(skillsLevels), save_file);

	fclose(save_file);
}

void load()
{
    FILE *save_file = fopen("fat:/d2save.sav", "rb");
    if (!save_file)PA_OutputText(1,0,0,"Can't open save, emulator?");
    fread(&hero.stats, 1, sizeof(hero.stats), save_file);
	fread(&skillsLevels, 1, sizeof(skillsLevels), save_file);
	fclose(save_file);
}
void pause (u8 *quitcondition)//with booloean parameter checked at each frame
{
    int i;
    int sprite=-1;
    int frameNumber=0, animStage=0;
    UL_IMAGE *pentacle = ulLoadImageFilePNG((void*)pentacle_png, (int)pentacle_png_size, UL_IN_RAM, UL_PF_PAL4),
                         *pausesprite = ulLoadImageFilePNG((void*)pause_png, (int)pause_png_size, UL_IN_RAM, UL_PF_PAL2);
    while (!(*quitcondition))
    {
        ulStartDrawing2D();
        ulSetDepth(0);
        //Fond sur l'�cran du bas
        ulDrawMap(Mymap);


        for (i=0; i<=curMaxSprite ; i++)
        {
            sprite =i;
            while (sprite != -1)
            {
                if(sprites[sprite].used)
                {
                    UL_IMAGE *spriteimage;
                    spriteimage=spritedatabase[sprites[sprite].sprite].image;

                    spriteimage->x=sprites[sprite].x;
                    spriteimage->y=sprites[sprite].y;

                    //Tile to display
                    ulSetImageTileSize (
                        spriteimage,
                        spritedatabase[sprites[sprite].sprite].sizex*sprites[sprite].column,																			//image, starting x
                        spritedatabase[sprites[sprite].sprite].sizey*(sprites[sprite].animStage+sprites[sprite].startframe),	//starting y : size*(frame+startframe)
                        spritedatabase[sprites[sprite].sprite].sizex,																// size x
                        spritedatabase[sprites[sprite].sprite].sizey);															// size y

                    ulMirrorImageH( spriteimage, sprites[sprite].flippedh);
                    ulMirrorImageV( spriteimage, sprites[sprite].flippedv);
                    ulSetDepth(sprites[sprite].prio);
                    glPolyFmt(POLY_ALPHA(sprites[sprite].abcoeff) | POLY_CULL_NONE | POLY_ID(sprites[sprite].prio));

                    ulDrawImage(spriteimage);

                }
                sprite =-1;
            }
        }




        ulSetDepth(255);
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(0));
        pausesprite->x=70;
        pausesprite->y=80;
        pentacle->y=65;



        frameNumber++;
        if (frameNumber % 8  == 0) animStage = (animStage + 1) % 8 ;
        ulSetImageTileSize (pentacle, 0,	64*animStage, 64, 64);
        pentacle->x=12;
        ulDrawImage(pentacle);
        pentacle->x=187;
        ulDrawImage(pentacle);
        ulDrawImage(pausesprite);
        ulEndDrawing();
        PA_WaitForVBL();
    }
    ulDeleteImage (pentacle);
    ulDeleteImage (pausesprite);
}
/*

void dialogbox()
{

    if (!dialbox)
    {
        //PA_ShowBg(0,1);
        PA_EasyBgLoad(0, 1, dialog);
        dialbox=1;
    }
    else if (dialbox)
    {
     PA_HideBg(0, 1);
     dialbox=0;
    }
}

*/

void skillmenu(bool levelup)
{
    bool endloop=0;
    const void (*skillfunction[SKILLNUMBER])(int,int,u16)= {(void*)&firebolt,(void*)&icebolt,(void*)&iceorb,(void*)&blaze,(void*)&firewall,(void*)&hydra,(void*)&chargedbolt,(void*)&teleport};
    const u8 skillx[SKILLNUMBER]= {12,12,60,60,108,156,12,60};
    const u8 skilly[SKILLNUMBER]= {12,60,60,12,12,12,108,108};
    const u8 skillframe[SKILLNUMBER]= {0,2,8,3,4,7,1,5};
    u8 i;

    UL_IMAGE *skillicons = ulLoadImageFilePNG((void*)sorts_png, (int)sorts_png_size, UL_IN_RAM, UL_PF_PAL4);
    UL_IMAGE *numberslot = ulLoadImageFilePNG((void*)number_png, (int)number_png_size, UL_IN_RAM, UL_PF_PAL2);
    UL_IMAGE *exitbtn=ulLoadImageFilePNG((void*)exit_png, (int)exit_png_size, UL_IN_VRAM, UL_PF_PAL4);
//    UL_IMAGE *skillbg = ulLoadImageFilePNG((void*)skillmenubg_png, (int)skillmenubg_png_size, UL_IN_RAM, UL_PF_PAL4);
//UL_MAP *skillbg = ulCreateMap(mapTiles,map,8,8,60,45,UL_MF_U16);
    UL_IMAGE *skillTiles = ulLoadImageFilePNG((void*)skilltiles_png, (int)skilltiles_png_size, UL_IN_VRAM, UL_PF_PAL4);
    UL_MAP *skillbg = ulCreateMap(skillTiles,/*Tileset*/skillmenu_map,8,8,/*Tiles size*/32,24,/*Map size*/UL_MF_U16);//Map format

    while (!endloop)
    {

    ulStartDrawing2D();
    ulSetDepth(0);
    //Fond sur l'�cran du bas
    ulDrawMap(skillbg);
    //ulDrawImage(skillbg);

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(0));

    for(i=0; i<SKILLNUMBER; i++)
    {
        ulSetDepth(1);
        numberslot->x=skillx[i]+30;
        numberslot->y=skilly[i]+20;
        ulDrawImage(numberslot);
        ulSetDepth(2);
        skillicons->x=skillx[i];
        skillicons->y=skilly[i];
        ulSetImageTileSize (skillicons,0,32*skillframe[i],32,32);
        if (!skillsLevels[i])
        {
            ulSetImageTint(skillicons,0x14d4);
        }
        else ulSetImageTint(skillicons,0x7fff);
        ulDrawImage(skillicons);
        ulSetDepth(3);
        ulPrintf_xy(skillx[i]+33,skilly[i]+24,"%i",skillsLevels[i]);
    }
    ulPrintf_xy(130,1,"Points remaining : %i",skillpoints);
    exitbtn->x=212;
    exitbtn->y=156;
    ulDrawImage(exitbtn);
    ulEndDrawing();



        PA_WaitForVBL();
        if(Stylus.Newpress)
        {
            for(i=0; i<SKILLNUMBER; i++)
            {
                if(Stylus.X>skillx[i]&&Stylus.X<(skillx[i]+32)&&Stylus.Y>skilly[i]&&Stylus.Y<(skilly[i]+32))
                {
                    if (levelup&&skillpoints&&(hero.stats.lvl>=requiredLevel[i]))
                    {
                        //should ask for confirmation
                        skillsLevels[i]++;
                        skillpoints--;
                        //ulPrintf_xy(skillx[i]+33,skilly[i]+24,"%i",skillsLevels[i]);
                    }
                    else if (skillsLevels[i]&&!(levelup&&skillpoints))
                    {
                        currentSkill[Pad.Held.L]=i;
                        sortchoisi[Pad.Held.L]=(void*)skillfunction[i];
                        PA_SetSpriteAnim(1, 3+Pad.Held.L, skillframe[i]);
                        i=SKILLNUMBER;
                        endloop=1;
                    }
                    else AS_SoundDirectPlay(0,SOUND(Sor_cantuseyet));//AS_SoundQuickPlay((u8*)Sor_cantuseyet);
                }
                else if (Stylus.X>212&&Stylus.X<244&&Stylus.Y>156&&Stylus.Y<188) endloop=1;
            }
        }

    }



    ulDeleteImage   (skillicons);
    ulDeleteImage   (numberslot);
    ulDeleteImage   (skillTiles);
    ulDeleteImage   (exitbtn);
    ulDeleteMap     (skillbg);
}



void death()
{
    int i;
    extern int currentMap;
    for (i=0; i<MAX_DATASPRITES; i++)
    {
        ulDeleteImage(spritedatabase[i].image);
    }
    ulDeleteImage(mapTiles);
    ulDeleteMap(Mymap);
    QuickTopScreenRefresh();
    UL_IMAGE *deathscreen = ulLoadImageFilePNG((void*)deathscreen_png, (int)deathscreen_png_size, UL_IN_RAM, UL_PF_PAL8);
    ulStartDrawing2D();
    ulSetDepth(0);
    ulDrawImage(deathscreen);
    ulEndDrawing();
    PA_WaitForVBL();
    while (!(Pad.Newpress.Anykey||Stylus.Newpress))PA_WaitForVBL();

    ulDeleteImage(deathscreen);
    hero.stats.vie_restante=hero.stats.vie_max;
    for(i=0; i<MAX_OBJECT; i++)
    {
        deleteobject(i);
    }
    QuickTopScreenRefresh();
    myulInitData(1);
    changemap(currentMap,1);
}
