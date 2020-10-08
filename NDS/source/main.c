
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <nf_lib.h>
#include "structs.c"

struct Player player = {0, 0, 0, 0, 0, 1, 0};
struct PrintConsole bottomScreen;

struct Timer hole_timer;
struct Timer update_hole_timer;
struct Timer speed_item;
struct Timer invert_item;
struct Timer items_timer = {12, false};

touchPosition touchXY;

u8 frame_in_sec = {0};
u32 current_msec;	 //approximation of what msec, from start of the game
u32 current_sec;	 //sec since the start of game, using console time lags to much
u8 level = {0};		 //0=plains (OG) 1=Highway 2=Snow field
u8 difficulty = {3}; //1=easy 2=normal 3=hard 4=impossible
u8 scroll_x;

bool enemies_used = {true};
struct Enemies car_enemies;

//--NDS functions--
void init(void)
{
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
	//NF_LoadSpriteGfx("sprites/car_enemy", 1, 16, 16); // Bola azul
	//NF_LoadSpritePal("sprites/car_enemy", 1);

	//in which vram? screen,vram,ram,animframes?
	NF_VramSpriteGfx(0, 0, 0, false);
	NF_VramSpritePal(0, 0, 0);

	//NF_VramSpriteGfx(0, 1, 1, false);
	//NF_VramSpritePal(0, 1, 1);
	//BG

	NF_LoadTiledBg("BG/layer_2", "item_layer", 512, 256);
	NF_LoadTiledBg("BG/map0", "map0", 512, 256);
	NF_LoadTiledBg("BG/map1", "map1", 512, 256);

	NF_CreateTiledBg(0, 0, "item_layer"); //items and sinkholes layer
}

int get_player_tile(u8 layer)
{

	//uncomment to disable collision;
	//return -1;

	if (NF_GetTileOfMap(0, layer, (player.player_x + 16 + scroll_x - 5) / 8, (player.player_y + 16 - 3) / 8) > 1)
	{
		player.collision_x = (player.player_x + 16 + scroll_x - 5) / 8;
		player.collision_y = ((player.player_y + 16 - 3) / 8);
		return NF_GetTileOfMap(0, layer, (player.player_x + 16 + scroll_x - 5) / 8, (player.player_y + 16 - 3) / 8);
	}
	if (NF_GetTileOfMap(0, layer, (player.player_x + 16 + scroll_x - 5) / 8, (player.player_y + 16 + 4) / 8) > 0)
	{
		player.collision_x = (player.player_x + 16 + scroll_x - 5) / 8;
		player.collision_y = ((player.player_y + 16 + 4) / 8);
		return NF_GetTileOfMap(0, layer, (player.player_x + 16 + scroll_x - 5) / 8, (player.player_y + 16 + 4) / 8);
	}
	if (NF_GetTileOfMap(0, layer, (player.player_x + 16 + scroll_x + 5) / 8, (player.player_y + 16 - 3) / 8) > 0)
	{
		player.collision_x = (player.player_x + 16 + scroll_x + 5) / 8;
		player.collision_y = ((player.player_y + 16 - 3) / 8);
		return NF_GetTileOfMap(0, layer, (player.player_x + 16 + scroll_x + 5) / 8, (player.player_y + 16 - 3) / 8);
	}
	if (NF_GetTileOfMap(0, layer, (player.player_x + 16 + scroll_x + 5) / 8, (player.player_y + 16 + 4) / 8) > 0)
	{
		player.collision_x = (player.player_x + 16 + scroll_x + 5) / 8;
		player.collision_y = ((player.player_y + 16 + 4) / 8);
		return NF_GetTileOfMap(0, layer, (player.player_x + 16 + scroll_x + 5) / 8, (player.player_y + 16 + 4) / 8);
	}
	else
	{
		return -1;
	}
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
	//iprintf("\x1b[5;1H hole_timer.delay:%01li", hole_timer.delay);
	//iprintf("\x1b[6;1H update_hole_timer.delay:%01li", update_hole_timer.delay);
	//iprintf("\x1b[7;1H invert_item.delay:%01li", invert_item.delay);
	//iprintf("\x1b[8;1H speed_timer.delay:%01li", speed_item.delay);
	//iprintf("\x1b[9;1H MSEC:%01li", current_msec);
	//iprintf("\x1b[10;1H SEC:%01li", current_sec);
	//iprintf("\x1b[11;1H Player_State:%01i", player.player_state);
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
		NF_SetTileOfMap(0, layer, (x + 16 + scroll_x) / 8, (y + 16) / 8, tile_id);
		NF_SetTileOfMap(0, layer, (x + 16 + scroll_x) / 8 + 1, (y + 16) / 8, tile_id + 1);
		NF_SetTileOfMap(0, layer, (x + 16 + scroll_x) / 8, (y + 16) / 8 + 1, tile_id + 2);
		NF_SetTileOfMap(0, layer, (x + 16 + scroll_x) / 8 + 1, (y + 16) / 8 + 1, tile_id + 3);
		break;

	case 1:
		NF_SetTileOfMap(0, layer, x, y, tile_id);
		NF_SetTileOfMap(0, layer, x + 1, y, tile_id + 1);
		NF_SetTileOfMap(0, layer, x, y + 1, tile_id + 2);
		NF_SetTileOfMap(0, layer, x + 1, y + 1, tile_id + 3);
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
	current_msec += 17; //16.666...
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

	NF_ScrollBg(0, 0, scroll_x, 0);
	NF_ScrollBg(0, 1, scroll_x, 0);
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

	NF_ScrollBg(0, 0, scroll_x, 0);
	NF_ScrollBg(0, 1, scroll_x, 0);
}

void spawn_enemy(u8 enemy_index, u8 direction_, s16 enemy_x_, s16 enemy_y_)
{
	switch (enemy_index)
	{
		//car enemy
	case 0:
		car_enemies.enemy_x[enemy_index] = enemy_x_;
		car_enemies.enemy_y[enemy_index] = enemy_y_;
		car_enemies.enemy_direction[enemy_index] = direction_;
		NF_CreateSprite(0, enemy_index, 1, 1, car_enemies.enemy_x[enemy_index], car_enemies.enemy_y[enemy_index]);
		NF_EnableSpriteRotScale(0, enemy_index, enemy_index, true);
		NF_SpriteRotScale(0, enemy_index, 128, 256, 256);
		if (direction_ == -1)
		{
			NF_SpriteRotScale(0, enemy_index, 384, 256, 256);
		}
		car_enemies.member++;
		break;
	}
}

void update_car_enemy(u8 enemy_id)
{
	car_enemies.enemy_x[enemy_id] = car_enemies.enemy_direction[enemy_id] * (difficulty + 1.5);
	if (car_enemies.enemy_x[enemy_id] < -130)
	{
		car_enemies.enemy_direction[enemy_id] = 1;
		NF_SpriteRotScale(0, enemy_id, 128, 256, 256);
		car_enemies.enemy_y[enemy_id] -= 7 * 16;
	}
	else if (car_enemies.enemy_x[enemy_id] > 256 + 130)
	{
		car_enemies.enemy_direction[enemy_id] = -1;
		NF_SpriteRotScale(0, enemy_id, 384, 256, 256);
		car_enemies.enemy_y[enemy_id] += 7 * 16;
	}

	if (car_enemies.anim_frame[enemy_id] == 3)
	{
		car_enemies.anim_frame[enemy_id] = 0;
	}
	NF_SpriteFrame(0, enemy_id, car_enemies.anim_frame[enemy_id]);
	car_enemies.anim_frame[enemy_id]++;
	make_16x16_tile(50, 0, car_enemies.enemy_x[enemy_id] / 8, car_enemies.enemy_y[enemy_id] / 8, 1);
}

void get_enemy_collision()
{
	//update enemies here
	//car enemies
	for (u8 i = 0; i < car_enemies.member; i++)
	{
		update_car_enemy(i);
	}
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
		prop_id = 21 + rand_(7) * 4;
	}
	else
	{
		//invalid prop
	}

	if (strcmp(str_, "grass") == 0)
	{
		for (u8 i = 0; i < (rand_(8) + 27); i++)
		{
			make_16x16_tile(2, layer_, even(rand_(80)), even(rand_(48)), 1);
		}
	}
	else if (strcmp(str_, "item") == 0)
	{

		make_16x16_tile(prop_id, layer_, even(rand_(80)), even(rand_(48)), 1);
	}
}

void gen_map(u8 map_index)
{
	switch (map_index)
	{
	case 0:
		NF_CreateTiledBg(0, 1, "map0"); //map layer
		break;
	case 1:
		NF_CreateTiledBg(0, 1, "map1"); //map layer
		break;
	}

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

void replace_item(u8 tile, s8 tile_2)
{
	if (get_player_tile(0) == tile)
	{
		make_16x16_tile(tile_2, 0, player.collision_x, player.collision_y, 1);
	}
	else if (get_player_tile(0) == tile + 1)
	{
		make_16x16_tile(tile_2, 0, player.collision_x - 1, player.collision_y, 1);
	}
	else if (get_player_tile(0) == tile + 2)
	{
		make_16x16_tile(tile_2, 0, player.collision_x, player.collision_y - 1, 1);
	}
	else if (get_player_tile(0) == tile + 3)
	{
		make_16x16_tile(tile_2, 0, player.collision_x - 1, player.collision_y - 1, 1);
	}
}

void do_physics()
{
	if (get_player_tile(0) > 8)
	{
		if (get_player_tile(0) < 20)
		{
			if (player.bridges > 0)
			{
				replace_item(9, 37);
				replace_item(13, 37);
				replace_item(17, 37);
				player.bridges -= 1;
			}
			else
			{
				player.player_state = 1;
			}
		}
		else if (get_player_tile(0) >= 21 && get_player_tile(0) <= 24)
		{
			player.speed = 2;
			player.player_state = 3;
			replace_item(21, 50);
			speed_item.delay = current_sec + 5;
		}
		else if (get_player_tile(0) >= 33 && get_player_tile(0) <= 36)
		{
			player.speed = -1;
			player.player_state = 2;
			replace_item(33, 50);
			invert_item.delay = current_sec + 4;
		}
		else if (get_player_tile(0) >= 29 && get_player_tile(0) <= 32)
		{
			player.bridges += 1;
			replace_item(29, 50);
		}
		else if (get_player_tile(0) >= 25 && get_player_tile(0) <= 28)
		{
			player.score += 10000;
			replace_item(25, 50);
		}
	}
	if (enemies_used == true)
	{
		get_enemy_collision();
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

	//spawn_enemy(1, 1, 32, 32);
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

		if (items_timer.do_action == false && items_timer.delay <= current_sec)
		{
			items_timer.do_action = true;
		}
		if (items_timer.do_action == true)
		{
			add_object(0, "item");
			items_timer.delay = current_sec + 9 + difficulty;
			items_timer.do_action = false;
		}
		//----------
		//--handle player state--
		switch (player.player_state)
		{
		case 1:
			game_over();
			break;
		case 2:
			if (invert_item.delay <= current_sec)
			{
				player.speed = 1;
				player.player_state = 0;
			}
			break;
		case 3:
			if (speed_item.delay <= current_sec)
			{
				player.speed = 1;
				player.player_state = 0;
			}
			break;
		default:
			break;
		}
		//-----------

		do_physics();
		render();
		update_current_time();
	}

	return 0;
}
