
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <nf_lib.h>
#include "structs.c"

struct Player player = {0, 0, 0, 0, 0, 1};
struct PrintConsole bottomScreen;
struct Timer hole_timer;
struct Timer update_hole_timer;

touchPosition touchXY;

u8 frame_in_sec = {0};
u16 current_msec;	 //approximation of what msec, from start of the game
u64 current_sec;	 //sec since the start of game, using console time lags to much
u8 level;			 //0=plains (OG) 1=Highway 2=Snow field
u8 difficulty = {2}; //1=easy 2=normal 3=hard 4=impossible
u8 scroll_x;

//--NDS functions--
void init(void)
{
	//DEBUG: level select
	level = 0;

	//set seed for rnd
	srand((unsigned)time(0));

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
	NF_LoadTiledBg("BG/layer_2", "item_layer", 512, 768);
	NF_LoadTiledBg("BG/maps", "maps", 512, 768);

	NF_CreateTiledBg(0, 0, "item_layer"); //layer items and sinkholes
	NF_CreateTiledBg(0, 1, "maps");		  //layer map

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
	//iprintf("\x1b[5;1H tile top layer:%04i", get_player_tile(0));
	//iprintf("\x1b[6;1H tile bottom layer:%04i", get_player_tile(1));
	//iprintf("\x1b[8;1H update_hole_timer.delay:%04i", update_hole_timer.delay);
	//iprintf("\x1b[9;1H MSEC:%04i", current_msec);
	//iprintf("\x1b[10;1H SEC:%lli", current_sec);
	//update_hole_timer.delay
	//iprintf("\x1b[6;1H player_x: %03i", player.player_x);
	//iprintf("\x1b[7;1H player_x: %01i,%01i", (int)((float)((float)(player.player_x - 8) / 16) * 10), (int)(((player.player_x - 8) / 16) * 10));
	//---------

	swiWaitForVBlank(); // Wait for vertical sync
	// Update the OAM
	oamUpdate(&oamMain);
	oamUpdate(&oamSub);
	NF_UpdateVramMap(0, 1);
	NF_UpdateVramMap(0, 0);
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

	if (rnd_output < 0)
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

void update_current_time()
{
	frame_in_sec++;
	current_msec += 16;
	if (frame_in_sec == 59)
	{
		current_sec++;
		frame_in_sec = 0;
	}
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
	NF_ScrollBg(0, 1, scroll_x, level * 192);
}

void spawn_player()
{
	player.player_x = rand_(19) * 16 + 8;
	player.player_y = rand_(12) * 16 + 8;

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
	NF_ScrollBg(0, 1, scroll_x, level * 192);
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
		for (u8 i = 0; i < (rand_(8) + 27); i++)
		{
			make_16x16_tile(11, layer_, even(rand_(80)), even(rand_(48)), 1);
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
		add_object(1, "grass");
	}
}

void update_holes()
{
	for (u8 x = 0; x < 40; x += 2)
	{
		for (u8 y = 0; y < 24; y += 2)
		{
			if (NF_GetTileOfMap(0, 0, x, y) > 0 && NF_GetTileOfMap(0, 0, x, y) < 16 && x % 2 == 0 && y % 2 == 0)
			{
				make_16x16_tile(NF_GetTileOfMap(0, 0, x, y) + 4, 0, x, y, 1);
			}
		}
	}
}

void spawn_hole()
{
	u8 hole_x;
	u8 hole_y;

	hole_x = even(rand_(80));
	hole_y = even(rand_(48));

	/*I'll have to fix this later, it made the game freeze 

	if (NF_GetTileOfMap(0, 0, hole_x / 8, hole_y / 8) > 0)
	{
		update_holes();
		rand_(99);
		spawn_hole();
	}
	*/
	make_16x16_tile(1, 0, hole_x, hole_y, 1);
}

void do_physics()
{
	if (get_player_tile(0) > 8)
	{
		if (get_player_tile(0) < 20)
		{
			/*if player.bridges>0:
				replace_item(map_object_layer,2,9)
				replace_item(map_object_layer,3,9)
				replace_item(map_object_layer,4,9)
				player.bridges-=1
			else:*/
			player.player_state = 1;
		}
	}
}

void game_over()
{
	iprintf("\x1b[8;12H\x1b[31;1m U DED\x1b[39m");
}

int main(void)
{
	init();

	//create player sprite and enable rot.
	NF_CreateSprite(0, 0, 0, 0, 0, 0);
	NF_EnableSpriteRotScale(0, 0, 0, true);

	//NF_CreateSprite(0, 1, 1, 1, 0, 0);
	gen_map(level);
	spawn_player();

	while (1)
	{
		scanKeys(); //get  button input
		touchRead(&touchXY);

		int button = keysHeld();
		player_movement(button);

		//--timers--
		if (hole_timer.do_action == false && hole_timer.delay <= current_sec)
		{
			hole_timer.do_action = true;
		}
		if (hole_timer.do_action == true)
		{
			spawn_hole();
			hole_timer.delay = current_sec + 4 - difficulty;
			hole_timer.do_action = false;
		}

		if (update_hole_timer.do_action == false && update_hole_timer.delay <= current_msec)
		{
			update_hole_timer.do_action = true;
		}
		if (update_hole_timer.do_action == true)
		{
			update_holes();
			update_hole_timer.delay = current_msec + 450;
			update_hole_timer.do_action = false;
		}
		//----------
		//--handle player state--
		switch (player.player_state)
		{
		case 1:
			game_over();
			break;
		default:
			break;
		}
		//----------

		do_physics();
		render();
		update_current_time();
	}

	return 0;
}
