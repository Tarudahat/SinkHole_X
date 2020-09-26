
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <nf_lib.h>
#include "structs.c"

struct Player player = {0, 0, 0, 0, 0, 1};
struct PrintConsole bottomScreen;
u8 level;
u8 scroll_x;

//--needed functions--
void init(void)
{
	//DEBUG: level select
	level = 1;

	NF_Set2D(0, 0);
	consoleDemoInit();

	consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
	consoleSelect(&bottomScreen);

	swiWaitForVBlank(); //vsync
	NF_SetRootFolder("NITROFS");
	NF_Set2D(0, 0);
	NF_InitSpriteBuffers();
	NF_InitSpriteSys(0);
	NF_InitTiledBgBuffers();
	NF_InitTiledBgSys(0);
	//load assets
	//Sprites
	//					name     , id  , w, h
	NF_LoadSpriteGfx("sprites/car", 0, 16, 16);
	NF_LoadSpritePal("sprites/car", 0);
	//NF_LoadSpriteGfx("sprites/bola", 1, 32, 32); // Bola azul
	//NF_LoadSpritePal("sprites/bola", 1);

	//in which vram? screen,vram,ram,animframes?
	NF_VramSpriteGfx(0, 0, 0, false);
	NF_VramSpritePal(0, 0, 0);
	//BG
	NF_LoadTiledBg("BG/maps", "maps", 512, 768);
	NF_LoadTiledBg("BG/layer_2", "item_layer", 512, 768);

	NF_CreateTiledBg(0, 0, "maps");		  //layer map
	NF_CreateTiledBg(0, 1, "item_layer"); //layer items and sinkholes

	//NF_VramSpriteGfx(0, 1, 1, false);
	//NF_VramSpritePal(0, 1, 1);
}

void render(void)
{
	// Update the OAM array
	NF_SpriteOamSet(0);
	NF_SpriteOamSet(1);

	iprintf("\x1b[1;1H Score:%lli", player.score);

	//--debug--:
	//iprintf("\x1b[5;1H tile layer0:%04i", NF_GetTileOfMap(0, 0, player.player_x / 8, player.player_y / 8));
	//iprintf("\x1b[6;1H player_x: %03i", player.player_x);
	//iprintf("\x1b[7;1H player_x: %01i,%01i", (int)((float)((float)(player.player_x - 8) / 16) * 10), (int)(((player.player_x - 8) / 16) * 10));
	//---------

	swiWaitForVBlank(); // Wait for vertical sync
	// Update the OAM
	oamUpdate(&oamMain);
	oamUpdate(&oamSub);
}
//--------------------

//--game functions--

int rand_(u16 rnd_max)
{
	u8 rnd_output;
	srand((unsigned)time(0) + keysHeld());

	rnd_output = rand() % rnd_max;
	if (rnd_output <= 0)
	{
		rnd_output = 1;
	}
	return rnd_output;
}

void player_movement(int keys)
{

	if (keys & KEY_UP)
	{
		player.player_y -= 1 * player.speed;

		player.score += 10 * player.speed;
		NF_SpriteRotScale(0, 0, 0, 256, 256);
	}
	else if (keys & KEY_DOWN)
	{
		player.player_y += 1 * player.speed;
		player.score += 10 * player.speed;
		NF_SpriteRotScale(0, 0, 256, 256, 256);
	}
	if (keys & KEY_LEFT)
	{
		player.player_x -= 1 * player.speed;
		player.score += 10 * player.speed;
		NF_SpriteRotScale(0, 0, 384, 256, 256);
	}
	else if (keys & KEY_RIGHT)
	{
		player.player_x += 1 * player.speed;
		player.score += 10 * player.speed;
		NF_SpriteRotScale(0, 0, 128, 256, 256);
	}

	if (player.player_x < -16)
	{
		player.player_x = 240;
		scroll_x = 64;
	}
	if (player.player_x > 240)
	{
		player.player_x = -16;
		scroll_x = 0;
	}

	if (player.player_y < -16)
	{
		player.player_y = 192 - 16;
	}
	if (player.player_y > 192 - 16)
	{
		player.player_y = -16;
	}

	NF_MoveSprite(0, 0, player.player_x, player.player_y);

	//scrolling

	if (player.player_x < 144 && player.player_x > 79)
	{
		scroll_x = player.player_x - 80;
	}

	NF_ScrollBg(0, 0, scroll_x, level * 192);
}

void spawn_player()
{
	player.player_x = rand_(15) * 16 + 8;
	player.player_y = rand_(10) * 16 + 8;

	if (player.player_x < 144 && player.player_x > 79)
	{
		if ((int)((float)((float)(player.player_x - 8) / 16) * 10) != (int)(((player.player_x - 8) / 16) * 10))
		{
			player.player_x += 8;
		}

		scroll_x = player.player_x - 80;
	}
	if (player.player_x >= 232)
	{
		player.player_x = 232;
	}
	if (player.player_x > 144)
	{
		scroll_x = 144 - 80;
	}

	NF_ScrollBg(0, 0, scroll_x, level * 192);
}

int main(void)
{
	init();

	//creat player sprite and enable rot.
	NF_CreateSprite(0, 0, 0, 0, 0, 0);
	NF_EnableSpriteRotScale(0, 0, 0, true);

	//NF_CreateSprite(0, 1, 1, 1, 0, 0);

	touchPosition touchXY;
	spawn_player();

	while (1)
	{

		scanKeys(); //get  button input
		touchRead(&touchXY);

		int button = keysHeld();

		player_movement(button);

		render();
		rand();
	}

	return 0;
}
