
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

//--NDS functions--
void init(void)
{
	//DEBUG: level select
	level = 0;

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

int get_player_tile(u8 layer)
{
	return NF_GetTileOfMap(0, layer, (player.player_x + 16 + scroll_x) / 8, (player.player_y + 16 + level * 192) / 8);
}

void render(void)
{
	// Update the OAM array
	NF_SpriteOamSet(0);
	NF_SpriteOamSet(1);

	iprintf("\x1b[1;1H Score:%lli", player.score);

	//--debug--:
	iprintf("\x1b[5;1H tile layer0:%04i", get_player_tile(0));
	//iprintf("\x1b[6;1H player_x: %03i", player.player_x);
	//iprintf("\x1b[7;1H player_x: %01i,%01i", (int)((float)((float)(player.player_x - 8) / 16) * 10), (int)(((player.player_x - 8) / 16) * 10));
	//---------

	swiWaitForVBlank(); // Wait for vertical sync
	// Update the OAM
	oamUpdate(&oamMain);
	oamUpdate(&oamSub);
	NF_UpdateVramMap(0, 0);
	NF_UpdateVramMap(0, 1);
}

void make_16x16_tile(u16 tile_id, u8 layer, u16 x, u16 y, u8 mode)
{
	//mode 1 => tile map position | mode 0 => tilemap position
	switch (mode)
	{
	case 0:
		NF_SetTileOfMap(0, layer, (x + 16 + scroll_x) / 8, (y + 16 + level * 192) / 8, tile_id);
		NF_SetTileOfMap(0, layer, (x + 16 + scroll_x) / 8 + 1, (y + 16 + level * 192) / 8, tile_id + 1);
		NF_SetTileOfMap(0, layer, (x + 16 + scroll_x) / 8, (y + 16 + level * 192) / 8 + 1, tile_id + 2);
		NF_SetTileOfMap(0, layer, (x + 16 + scroll_x) / 8 + 1, (y + 16 + level * 192) / 8 + 1, tile_id + 3);
		break;

	case 1:
		NF_SetTileOfMap(0, layer, x, y + ((level * 192) / 8), tile_id);
		NF_SetTileOfMap(0, layer, x + 1, y + ((level * 192) / 8), tile_id + 1);
		NF_SetTileOfMap(0, layer, x, y + ((level * 192) / 8) + 1, tile_id + 2);
		NF_SetTileOfMap(0, layer, x + 1, y + ((level * 192) / 8) + 1, tile_id + 3);
		break;
	}
}

int rand_(u16 rnd_max)
{
	u8 rnd_output;

	rnd_output = rand() % rnd_max;
	rnd_output = rnd_output / 2;
	if (rnd_output <= 0)
	{
		rnd_output = 1;
	}
	return rnd_output;
}

int even(int input_num)
{

	if ((input_num % 2) == 1)
	{
		input_num--;
	}
	return input_num;
}
//--------------------

//--game functions--

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

void add_object(u8 layer_, char *str_)
{
	u8 prop_id;
	if (strcmp(str_, "grass") == 0)
	{
		prop_id = 1;
	}
	else if (strcmp(str_, "item") == 0)
	{
		prop_id = rand_(3) + 5;
	}
	else
	{
		//invalid prop
	}

	if (strcmp(str_, "grass") == 0)
	{
		for (u8 i = 0; i < (rand_(10) + 25); i++)
		{
			make_16x16_tile(11, layer_, even(rand_(80)), even(rand_(48) - 1), 1);
		}
	}
	else if (strcmp(str_, "item") == 0)
	{

		make_16x16_tile(prop_id, layer_, even(rand_(80)), even(rand_(48)), 1);
	}
}

void gen_map(u8 map_index)
{
	if (map_index == 0)
	{
		add_object(0, "grass");
	}
}

int main(void)
{
	init();

	//creat player sprite and enable rot.
	NF_CreateSprite(0, 0, 0, 0, 0, 0);
	NF_EnableSpriteRotScale(0, 0, 0, true);

	//NF_CreateSprite(0, 1, 1, 1, 0, 0);

	touchPosition touchXY;
	gen_map(level);
	spawn_player();

	while (1)
	{
		scanKeys(); //get  button input
		touchRead(&touchXY);

		int button = keysHeld();

		player_movement(button);
		render();
	}

	return 0;
}
