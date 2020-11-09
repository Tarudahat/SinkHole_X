
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <nf_lib.h>
#include "structs.c"

const int map_layer = {3};
const int item_layer = {2};
const int menu_layer = {1};
const int text_layer = {0};

struct Player player = {0, "SCORE STRING THING", 0, 0, 0, 0, 1, 0};

struct Timer hole_timer;
struct Timer update_hole_timer;
struct Timer speed_item;
struct Timer invert_item;
struct Timer items_timer = {10};

touchPosition touchXY;

u8 frame_in_sec = {0};
u32 current_msec; //approximation of what msec, from start of the game
u32 current_sec;  //sec since the start of game, using console time lags to much
s8 level = {0};	  //0=plains (OG) 1=Highway 2=Snow field
u8 difficulty;	  //1=easy 2=normal 3=hard 4=impossible
u8 scroll_x;

bool paused = {false};

bool enemies_used = {false};
bool car_enemies_used = {false};

u8 total_enemies;
struct enemy_group car_enemies;

struct enemy_group empty_group;

//--NDS functions--
void init(void)
{
	//set seed for rnd
	srand((unsigned)time(0));

	swiWaitForVBlank(); //vsync
	NF_SetRootFolder("NITROFS");
	NF_Set2D(0, 0);
	NF_Set2D(1, 0);

	NF_InitSpriteBuffers();
	NF_InitSpriteSys(0);

	NF_InitTiledBgBuffers();
	NF_InitTiledBgSys(0);
	NF_InitTiledBgSys(1);

	NF_InitTextSys(0);
	NF_InitTextSys(1);

	//load assets
	//Sprites							|->the img
	//					name     , ram-slot  , w, h
	NF_LoadSpriteGfx("sprites/car", 0, 16, 16);
	NF_LoadSpritePal("sprites/car", 0);

	NF_LoadSpriteGfx("sprites/car_enemy", 1, 16, 16);
	NF_LoadSpritePal("sprites/car_enemy", 1);

	//in which vram? screen,vram,ram,animframes?
	NF_VramSpriteGfx(0, 0, 0, false);
	NF_VramSpritePal(0, 0, 0);

	//                     |-> img it will be using		|-> 0-128 slots to contain sprite
	//            screen, ram-slot,                  Vram-slot,
	NF_VramSpriteGfx(0, 1, 1, false);
	NF_VramSpritePal(0, 1, 1);

	//BG

	NF_LoadTiledBg("BG/layer_2", "item_layer", 512, 256);
	NF_LoadTiledBg("BG/map0", "map0", 512, 256);
	NF_LoadTiledBg("BG/map1", "map1", 512, 256);
	NF_LoadTiledBg("BG/map2", "map2", 512, 256);

	//menus
	NF_LoadTiledBg("menu/main_menu0", "main_menu0", 256, 256);
	NF_LoadTiledBg("menu/main_menu1", "main_menu1", 256, 256);
	NF_LoadTiledBg("menu/main_menu2", "main_menu2", 256, 256);
	NF_LoadTiledBg("menu/clear_screen", "clear_screen", 256, 256);
	NF_LoadTiledBg("menu/game_over", "game_over", 256, 256);
	NF_LoadTiledBg("menu/pause_menu", "pause_menu", 256, 256);
	NF_LoadTiledBg("menu/diff_menu", "diff_menu", 256, 768);
	//font
	NF_LoadTextFont16("font/font16", "font", 256, 256, 0);
	//----

	NF_CreateTextLayer16(0, text_layer, 0, "font");
	NF_CreateTextLayer16(1, text_layer, 0, "font");
	NF_CreateTiledBg(0, item_layer, "item_layer"); //items and sinkholes layer

	//bottom screen stuff
	NF_LoadTiledBg("menu/temp_map", "temp_screen", 256, 256);
	NF_CreateTiledBg(1, menu_layer, "temp_screen");
}

void render(void)
{
	if (player.player_state != 1)
	{
		//clear bottom screen
		NF_ClearTextLayer16(1, text_layer);

		//display score
		sprintf(player.score_str, "Score:%lli", player.score);
		NF_WriteText16(1, text_layer, 5, 5, player.score_str);
	}
	else
	{
		NF_ClearTextLayer16(1, text_layer);
	}

	NF_SpriteOamSet(0);
	NF_SpriteOamSet(1);

	NF_UpdateVramMap(0, item_layer);
	NF_UpdateVramMap(0, map_layer);
	NF_UpdateVramMap(0, menu_layer);
	NF_UpdateTextLayers();
	swiWaitForVBlank();
	// Update the OAM
	oamUpdate(&oamMain);
	oamUpdate(&oamSub);
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

void make_16x16_tile(u16 tile_id, u8 layer, u16 x, u16 y, u8 mode)
{ //mode 1 => tile map position | mode 0 => on screen for player

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

void create_sprite(u8 index, u8 asset)
{
	NF_CreateSprite(0, index, asset, asset, 0, 0);
	NF_EnableSpriteRotScale(0, index, index, true); //using index for arg. 3 so that they have separate rotations
	NF_SpriteLayer(0, index, item_layer);
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

	if (player.player_x >= 80 && player.player_x <= 144)
	{
		scroll_x = player.player_x - 80;
	}

	NF_ScrollBg(0, item_layer, scroll_x, 0);
	NF_ScrollBg(0, map_layer, scroll_x, 0);
}

void spawn_player()
{
	player.player_x = rand_(19) * 16 + 8;
	player.player_y = rand_(12) * 16 + 8;

	//make the player face the correct direction
	NF_SpriteRotScale(0, 0, 0, 256, 256);

	if (player.player_x >= 80 && player.player_x <= 144)
	{
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

	NF_ScrollBg(0, item_layer, scroll_x, 0);
	NF_ScrollBg(0, map_layer, scroll_x, 0);
}

void spawn_enemy(u8 enemy_type, u8 direction_, u16 enemy_x_, u16 enemy_y_)
{
	if (enemy_type == 0)
	{
		car_enemies.group_members++;
		total_enemies++;

		car_enemies.enemy_id[car_enemies.group_members] = total_enemies;
		create_sprite(total_enemies, 1);

		NF_SpriteRotScale(0, total_enemies, 128, 256, 256); //turn to the right
		car_enemies.enemy_direction[car_enemies.group_members] = direction_;
		if (direction_ == -1)
		{
			NF_SpriteRotScale(0, total_enemies, 384, 256, 256); //turn to the left
		}

		car_enemies.enemy_x[car_enemies.group_members] = enemy_x_ * 16 + 8;
		car_enemies.enemy_y[car_enemies.group_members] = enemy_y_ * 16 + 8;
		NF_MoveSprite(0, total_enemies, car_enemies.enemy_x[car_enemies.group_members], car_enemies.enemy_y[car_enemies.group_members]);
		//ew_enemy.enemy_type = 0
	}
}

void update_car_enemy(u8 enemy_)
{
	car_enemies.enemy_x[enemy_] += car_enemies.enemy_direction[enemy_] * ((int)(difficulty / 4) + 1);
	if (car_enemies.enemy_x[enemy_] <= -32)
	{
		car_enemies.enemy_direction[enemy_] = 1;
		NF_SpriteRotScale(0, car_enemies.enemy_id[enemy_], 128, 256, 256); //turn right
		car_enemies.enemy_y[enemy_] -= 7 * 16;
	}
	else if (car_enemies.enemy_x[enemy_] >= 288)
	{
		car_enemies.enemy_direction[enemy_] = -1;
		NF_SpriteRotScale(0, car_enemies.enemy_id[enemy_], 384, 256, 256); //turn left
		car_enemies.enemy_y[enemy_] += 7 * 16;
	}

	if (car_enemies.anim_delay <= current_msec) // anim_delay needs to be updated when iterating trough the enemies
	{
		car_enemies.current_frame++;
		if (car_enemies.current_frame >= 5)
			car_enemies.current_frame = 0;
	}
	else
	{
		car_enemies.anim_delay = current_msec + 125;
	}

	//NF_SpriteFrame(0, car_enemies.enemy_id[enemy_], car_enemies.current_frame);
	NF_MoveSprite(0, car_enemies.enemy_id[enemy_], car_enemies.enemy_x[enemy_], car_enemies.enemy_y[enemy_]);
	//make sure that we do stuff inbounds
	if ((car_enemies.enemy_x[enemy_] <= 256 * 2) & (car_enemies.enemy_x[enemy_] >= 0))
	{
		if ((car_enemies.enemy_y[enemy_] <= 192) & (car_enemies.enemy_y[enemy_] >= 0))
		{

			make_16x16_tile(50, item_layer, even(car_enemies.enemy_x[enemy_] / 8 + scroll_x / 8), car_enemies.enemy_y[enemy_] / 8 + 1, 1);
		}
	}
}

void get_enemy_collision()
{

	//update enemies here
	if (car_enemies_used == true)
	{
		for (u8 enemy_index = 1; enemy_index <= car_enemies.group_members; enemy_index++) //start from 1 bc 0 is for player
		{
			update_car_enemy(enemy_index);
		}
	}

	/*for enemy_ in enemies.get_children():
		
		if enemy_used[1]==true and enemy_.enemy_type==0:
			update_car_enemy(enemy_)

		if enemy_used[2]==true and enemy_.enemy_type==1:
			map_object_layer.set_cell(enemy_.position.x/64, enemy_.position.y/64+1, -1)
			update_snow_enemy(enemy_)
		if enemy_used[3]==true and enemy_.enemy_type==2:
			update_snow_ball_enemy(enemy_)

		if enemy_.collided_with_player == true:
			player.player_state=1*/
}

void clear_enemies()
{
	for (u8 enemy = 1; enemy <= total_enemies; enemy++)
	{
		NF_DeleteSprite(0, enemy);
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
	else if (strcmp(str_, "snow_grass") == 0)
	{
		prop_id = 1;
	}
	else
	{
		//invalid prop
	}

	if (strcmp(str_, "grass") == 0)
	{
		for (u8 i = 0; i < (rand_(10) + 25); i++)
		{
			make_16x16_tile(2, layer_, even(rand_(80)), even(rand_(48)), 1);
		}
	}
	else if (strcmp(str_, "snow_grass") == 0)
	{
		for (u8 i = 0; i < (rand_(5) + 12); i++)
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
	char map_name[10];
	sprintf(map_name, "map%i", level);

	NF_CreateTiledBg(0, map_layer, map_name); //map layer

	enemies_used = true;
	if (map_index == 0)
	{
		add_object(map_layer, "grass");
		enemies_used = false;
	}
	else if (map_index == 1)
	{
		//spawn_car enemies
		car_enemies_used = true;
		spawn_enemy(0, 1, 4, 0);
		spawn_enemy(0, 1, 6, 0);
	}
	else if (map_index == 2)
	{
		add_object(map_layer, "snow_grass");
	}
}

void update_holes()
{
	for (u8 x = 0; x < 40; x += 2)
	{
		for (u8 y = 0; y < 24; y += 2)
		{
			if (NF_GetTileOfMap(0, item_layer, x, y) > 0 && NF_GetTileOfMap(0, item_layer, x, y) < 16 && x % 2 == 0 && y % 2 == 0)
			{
				make_16x16_tile(NF_GetTileOfMap(0, item_layer, x, y) + 4, item_layer, x, y, 1);
			}
		}
	}
}

void clear_map(u8 layer, u16 tile, u8 mode)
{ //mode -> 0 fill map | mode -> 1 switch to empty map
	switch (mode)
	{
	case 0:
		for (u8 x = 0; x < 40; x += 1)
		{
			for (u8 y = 0; y < 24; y += 1)
			{
				NF_SetTileOfMap(0, layer, x, y, tile);
			}
		}
		break;
	case 1:
		NF_CreateTiledBg(0, layer, "clear_screen");
		break;
	}
}

void reset()
{
	//-reset-
	//reset player stuff
	player.score = 0;
	player.player_state = 0;
	player.bridges = 0;
	player.speed = 1;
	scroll_x = 0;
	//reset timers
	frame_in_sec = 0;
	current_msec = 0;
	current_sec = 0;
	hole_timer.delay = 0;

	update_hole_timer.delay = 0;
	speed_item.delay = 0;
	invert_item.delay = 0;
	items_timer.delay = 12;
	//reset sinkholes & items
	clear_map(item_layer, 0, 0);
	//reset map
	switch (level)
	{
	case 0:
		clear_map(map_layer, 1, 0);
		break;
	}
	if (enemies_used == true)
	{
		//reset enemies
		clear_enemies();
		total_enemies = 0;
		//reset enemy data
		if (car_enemies_used == true)
		{
			car_enemies = empty_group;
		}
	}
	car_enemies_used = false;
	gen_map(level);
	swiWaitForVBlank();
	//-------
}

void difficulty_menu();

void main_menu(void)
{
	swiWaitForVBlank();
	level = 0;
	player.player_state = 1;
	struct Timer input_delay = {0};
	bool selecting = {true};
	char map_name[11];
	//make a temp maps
	NF_CreateTiledBg(0, map_layer, "map0");
	NF_CreateTiledBg(0, menu_layer, "main_menu0");
	while (selecting)
	{
		scanKeys(); //get  button input

		if ((keysHeld() > 0) & (input_delay.delay < current_msec))
		{
			if (keysHeld() & KEY_LEFT)
			{
				level--;
			}
			else if (keysHeld() & KEY_RIGHT)
			{
				level++;
			}
			if (level > 2)
			{
				level = 0;
			}
			else if (level < 0)
			{
				level = 2;
			}

			sprintf(map_name, "main_menu%i", level);
			NF_CreateTiledBg(0, menu_layer, map_name);

			input_delay.delay = current_msec + 175;
		}

		if (keysHeld() & KEY_A)
		{
			selecting = false;
		}
		render();
		swiWaitForVBlank();
		update_current_time();
	}
	clear_map(menu_layer, 0, 1);
	NF_CreateTiledBg(0, menu_layer, "diff_menu");
	swiWaitForVBlank();
}

void difficulty_menu()
{
	difficulty = 3;
	player.player_state = 1;
	struct Timer input_delay = {0};
	bool selecting = true;
	//make a temp maps
	NF_CreateTiledBg(0, map_layer, "map0");
	NF_CreateTiledBg(0, menu_layer, "diff_menu");
	NF_ScrollBg(0, menu_layer, 0, 192 * (difficulty - 1));
	render();
	swiDelay(1000000);
	while (selecting)
	{

		scanKeys(); //get  button input

		if ((keysHeld() > 0) & (input_delay.delay < current_msec))
		{
			if (keysHeld() & KEY_LEFT)
			{
				difficulty--;
			}
			else if (keysHeld() & KEY_RIGHT)
			{
				difficulty++;
			}
			if (difficulty > 4)
			{
				difficulty = 1;
			}
			if (difficulty < 1)
			{
				difficulty = 4;
			}
			swiWaitForVBlank();
			render();
			input_delay.delay = current_msec + 175;
		}

		if (keysHeld() & KEY_A)
		{
			selecting = false;
		}
		else if (keysHeld() & KEY_B)
		{
			reset();
			swiWaitForVBlank();
			NF_CreateTiledBg(0, menu_layer, "main_menu0");
			main_menu();
		}

		NF_ScrollBg(0, menu_layer, 0, 192 * (difficulty - 1));
		swiWaitForVBlank();
		render();
		update_current_time();
	}
	swiWaitForVBlank();
}

//pre-define pause function?
void pause_game();

void spawn_hole()
{
	u8 hole_x;
	u8 hole_y;

	hole_x = even(rand_(80));
	hole_y = even(rand_(48));

	if (NF_GetTileOfMap(0, item_layer, hole_x, hole_y) > 8 && NF_GetTileOfMap(0, item_layer, hole_x, hole_y) < 20)
	{
		update_holes();
		spawn_hole();
	}
	else
	{
		make_16x16_tile(1, item_layer, hole_x, hole_y, 1);
	}
}

void replace_item(u8 tile, s8 tile_2)
{
	if (get_player_tile(item_layer) == tile)
	{
		make_16x16_tile(tile_2, item_layer, player.collision_x, player.collision_y, 1);
	}
	else if (get_player_tile(item_layer) == tile + 1)
	{
		make_16x16_tile(tile_2, item_layer, player.collision_x - 1, player.collision_y, 1);
	}
	else if (get_player_tile(item_layer) == tile + 2)
	{
		make_16x16_tile(tile_2, item_layer, player.collision_x, player.collision_y - 1, 1);
	}
	else if (get_player_tile(item_layer) == tile + 3)
	{
		make_16x16_tile(tile_2, item_layer, player.collision_x - 1, player.collision_y - 1, 1);
	}
}

void do_physics()
{
	if (get_player_tile(item_layer) > 8)
	{
		if (get_player_tile(item_layer) < 20)
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
		else if (get_player_tile(item_layer) >= 21 && get_player_tile(item_layer) <= 24)
		{
			player.speed = 2;
			player.player_state = 3;
			replace_item(21, 50);
			speed_item.delay = current_sec + 5;
		}
		else if (get_player_tile(item_layer) >= 33 && get_player_tile(item_layer) <= 36)
		{
			player.speed = -1;
			player.player_state = 2;
			replace_item(33, 50);
			invert_item.delay = current_sec + 4;
		}
		else if (get_player_tile(item_layer) >= 29 && get_player_tile(item_layer) <= 32)
		{
			player.bridges += 1;
			replace_item(29, 50);
		}
		else if (get_player_tile(item_layer) >= 25 && get_player_tile(item_layer) <= 28)
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
	NF_CreateTiledBg(0, menu_layer, "game_over");
	render();
	//display score
	NF_ClearTextLayer16(0, text_layer);
	NF_WriteText16(0, text_layer, 10, 6, player.score_str);
	NF_UpdateTextLayers();

	while (keysHeld() != (KEY_A || KEY_START))
	{
		scanKeys(); //get input
	}
	clear_map(menu_layer, 0, 1);
	NF_ClearTextLayer16(0, text_layer);
	NF_UpdateTextLayers();
	reset();
	spawn_player();
	swiWaitForVBlank();
}

int main(void)
{
	init();

	main_menu();
	difficulty_menu();
	clear_map(menu_layer, 0, 1);

	//create player sprite
	create_sprite(0, 0);

	reset();
	spawn_player();

	while (1)
	{
		scanKeys(); //get  button input
		touchRead(&touchXY);

		u32 button = keysHeld();
		if ((button == KEY_START))
		{
			pause_game();
		}
		player_movement(button);

		//--timers--
		if (hole_timer.delay <= current_sec)
		{
			spawn_hole();
			hole_timer.delay = current_sec + 4 - difficulty;
		}

		if (update_hole_timer.delay <= current_msec)
		{
			update_holes();
			update_hole_timer.delay = current_msec + 450;
		}

		if (items_timer.delay <= current_sec)
		{
			add_object(item_layer, "item");
			items_timer.delay = current_sec + 7 + difficulty;
		}
		//----------
		//--handle player state--
		switch (player.player_state)
		{
		case 1:
			game_over();
			break;
		case 2:
			if (player.score <= 11)
			{
				player.score = 0;
			}
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

void pause_game()
{
	s8 selected = {0};
	paused = true;
	NF_CreateTiledBg(0, menu_layer, "pause_menu");
	while (paused)
	{
		swiWaitForVBlank();
		scanKeys(); //scan input
		switch (keysHeld())
		{
		case KEY_UP:
			selected--;
			if (selected <= 0)
			{
				selected = 1;
			}
			break;
		case KEY_DOWN:
			selected++;
			if (selected >= 1)
			{
				selected = 0;
			}
			break;
		case KEY_A:
			switch (selected)
			{
			case 0:
				paused = false;
				break;
			case 1:
				paused = false;
				NF_CreateTiledBg(0, menu_layer, "clear_screen");
				reset();
				main();
				break;
			}
			break;
		default:
			for (u8 i = 0; i < 60; i++)
			{
				iprintf(" ");
			}
			break;
		}
		render();
	}
	NF_CreateTiledBg(0, menu_layer, "clear_screen");
}
