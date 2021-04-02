#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fat.h>

#include <string.h>
#include <filesystem.h>

#include <nf_lib.h>
#include "structs.c"
#include "rivers.h"

const int map_layer = {3};
const int item_layer = {2};
const int menu_layer = {1};
const int text_layer = {0};

struct Player player = {0, "Score:0             ", 0, 0, 0, 0, 1, 0};

struct Timer hole_timer;
struct Timer update_hole_timer;
struct Timer speed_item;
struct Timer invert_item;
struct Timer items_timer = {7};
struct Timer roller;

FILE *save_file;
struct SaveStruct SaveData;

touchPosition touchXY;

u8 frame_in_sec = {0};
u32 current_msec; //approximation of what msec, from start of the game
u32 current_sec;  //sec since the start of game, using console time lags to much
s8 level = {0};	  //0=plains (OG) 1=Highway 2=Snow field
u8 difficulty;	  //1=easy 2=normal 3=hard 4=impossible
u8 scroll_x;
u8 prev_scroll_x;
s8 enemy_scroll;
s16 big_enemy_scroll;
u8 x_offset = 0;

bool exit_menus={false};
bool goto_title={false};
bool reset_game = {false};
bool touch_text_render = {true};

bool enemies_used = {false};
bool car_enemies_used = {false};
bool snow_enemies_used = {false};
bool fire_enemies_used = {false};

bool enemies_deleted = {false};
bool spawn_offset = {false};
u8 total_enemies;
struct enemy_group car_enemies;

struct enemy_group snow_enemies;
struct enemy_group ball_enemies;
struct enemy_group fire_enemies;
struct enemy_group shadow_enemies;

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
	NF_InitSpriteSys(1);

	NF_InitTiledBgBuffers();
	NF_InitTiledBgSys(0);
	NF_InitTiledBgSys(1);

	NF_InitTextSys(0);
	NF_InitTextSys(1);

	//load assets
	//Sprites							|->the img
	//					name     , ram-slot  , w, h
	//top-screen
	NF_LoadSpriteGfx("sprites/car", 0, 16, 16);
	NF_LoadSpritePal("sprites/car", 0);

	NF_LoadSpriteGfx("sprites/car_enemy", 1, 16, 16);
	NF_LoadSpritePal("sprites/car_enemy", 1);

	NF_LoadSpriteGfx("sprites/snow_man", 2, 16, 32);
	NF_LoadSpritePal("sprites/snow_man", 2);

	NF_LoadSpriteGfx("sprites/ball", 3, 8, 8);
	NF_LoadSpritePal("sprites/ball", 3);

	NF_LoadSpriteGfx("sprites/car_roller", 4, 16, 16);
	NF_LoadSpritePal("sprites/car_roller", 4);

	NF_LoadSpriteGfx("sprites/fire_ball", 6, 16, 16);
	NF_LoadSpritePal("sprites/fire_ball", 6);

	NF_LoadSpriteGfx("sprites/shadow", 7, 16, 16);
	NF_LoadSpritePal("sprites/shadow", 7);

	//touch screen
	NF_LoadSpriteGfx("sprites/cursor", 5, 64, 64); //144 32
	NF_LoadSpritePal("sprites/cursor", 5);

	//in which vram? screen,vram,ram,animframes?
	NF_VramSpriteGfx(0, 0, 0, false);
	NF_VramSpritePal(0, 0, 0);

	//                     |-> img it will be using		|-> 0-128 slots to contain sprite
	//            screen, ram-slot,                  Vram-slot,
	//top-screen
	NF_VramSpriteGfx(0, 1, 1, false);
	NF_VramSpritePal(0, 1, 1);

	NF_VramSpriteGfx(0, 2, 2, false);
	NF_VramSpritePal(0, 2, 2);

	NF_VramSpriteGfx(0, 3, 3, false);
	NF_VramSpritePal(0, 3, 3);

	NF_VramSpriteGfx(0, 4, 4, false);
	NF_VramSpritePal(0, 4, 4);

	NF_VramSpriteGfx(0, 6, 6, false);
	NF_VramSpritePal(0, 6, 6);

	NF_VramSpriteGfx(0, 7, 7, false);
	NF_VramSpritePal(0, 7, 7);

	//touch screen
	NF_VramSpriteGfx(1, 5, 0, false);
	NF_VramSpritePal(1, 5, 0);
	//BG

	NF_LoadTiledBg("BG/layer_2", "item_layer", 512, 256);
	NF_LoadTiledBg("BG/map0", "map0", 512, 256);
	NF_LoadTiledBg("BG/map1", "map1", 512, 256);
	NF_LoadTiledBg("BG/map2", "map2", 512, 256);
	NF_LoadTiledBg("BG/map3", "map3", 512, 256);

	//menus
	//top screen
	NF_LoadTiledBg("menu/main_menu0", "main_menu0", 256, 256);
	NF_LoadTiledBg("menu/main_menu1", "main_menu1", 256, 256);
	NF_LoadTiledBg("menu/main_menu2", "main_menu2", 256, 256);
	NF_LoadTiledBg("menu/main_menu3", "main_menu3", 256, 256);
	NF_LoadTiledBg("menu/clear_screen", "clear_screen", 256, 256);
	NF_LoadTiledBg("menu/game_over", "game_over", 256, 256);
	NF_LoadTiledBg("menu/pause_menu_top", "pause_menu_top", 256, 256);
	NF_LoadTiledBg("menu/diff_menu", "diff_menu", 256, 768);
	NF_LoadTiledBg("menu/title", "title", 256, 256);
	NF_LoadTiledBg("menu/power_msg", "power_msg", 256, 256);

	//bottom screen
	NF_LoadTiledBg("menu/game_touch", "game_touch", 256, 256);
	NF_LoadTiledBg("menu/title_bottom", "title_bottom", 256, 256);
	NF_LoadTiledBg("menu/temp_map", "temp_screen", 256, 256);
	NF_LoadTiledBg("menu/pause_menu", "pause_menu", 256, 256);
	NF_LoadTiledBg("menu/game_over_touch", "game_over_touch", 256, 256);
	NF_LoadTiledBg("menu/main_menu_touch", "main_menu_touch", 256, 256);

	//font
	NF_LoadTextFont16("font/font16", "font", 256, 256, 0);
	//----

	NF_CreateTextLayer16(0, 0, 0, "font");
	NF_CreateTextLayer16(1, 0, 0, "font");
	NF_CreateTiledBg(0, 2, "item_layer"); //items and sinkholes layer

	//create tilemaps so they can be used


	NF_CreateTiledBg(0, map_layer, "map0");

}

void save()
{
	fatInitDefault();

	SaveData.hi_scores[3]=5;
	SaveData.total_score=69420;
	save_file = fopen("./SinkHole.sav","wb");
	fwrite(&SaveData,1,sizeof(SaveData),save_file);
	fclose(save_file);	
}

void load()
{
	fatInitDefault();

	save_file = fopen("./SinkHole.sav","rb");
	fread(&SaveData,1,sizeof(SaveData),save_file);
	fclose(save_file);

}

int get_player_tile(u8 layer);

void render(void)
{
	if (player.player_state != 1 && touch_text_render == true)
	{
		//clear bottom screen
		NF_ClearTextLayer16(1, text_layer);

		//display score
		sprintf(player.score_str, "Score:%lli", player.score);
		NF_WriteText16(1, text_layer, 3, 1, player.score_str);

		//display bridges
		char *bridges_str="AAAAAAAAAAAAAAAAA";
		sprintf(bridges_str, "%0i",player.bridges);
		NF_WriteText16(1,text_layer,10,2,bridges_str);

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
	// Update the OAM -> Object Attribute Memory
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

int even(int input_num)
{

	if ((input_num % 2) == 1)
	{
		input_num--;
	}

	return input_num;
}

void make_16x16_tile(u8 screen, u16 tile_id, u8 layer, s16 x, s16 y, u8 mode)
{ //mode 1 => tile map position | mode 0 => on screen for player | mode 2 => empty tile
	if (tile_id == 0)
	{
		mode = 2;
	}

	switch (mode)
	{
	case 0:
		if (x <= 0)
		{
			x = -1;
		}
		if (y <= 0)
		{
			y = -1;
		}
		NF_SetTileOfMap(screen, layer, even((x + 16 + scroll_x) / 8), even((y + 16) / 8), 0);
		NF_SetTileOfMap(screen, layer, even((x + 16 + scroll_x) / 8) + 1, even((y + 16) / 8), 0);
		NF_SetTileOfMap(screen, layer, even((x + 16 + scroll_x) / 8), even((y + 16) / 8) + 1, 0);
		NF_SetTileOfMap(screen, layer, even((x + 16 + scroll_x) / 8) + 1, even((y + 16) / 8) + 1, 0);
		break;

	case 1:
		NF_SetTileOfMap(screen, layer, x, y, tile_id);
		NF_SetTileOfMap(screen, layer, x + 1, y, tile_id + 1);
		NF_SetTileOfMap(screen, layer, x, y + 1, tile_id + 2);
		NF_SetTileOfMap(screen, layer, x + 1, y + 1, tile_id + 3);
		break;
	case 2:
		NF_SetTileOfMap(screen, layer, x, y, 0);
		NF_SetTileOfMap(screen, layer, x + 1, y, 0);
		NF_SetTileOfMap(screen, layer, x, y + 1, 0);
		NF_SetTileOfMap(screen, layer, x + 1, y + 1, 0);
		break;
	}
}

void array2river(u8 layer, u8 river_, u8 array[10][12][20])
{
	for (u8 i_x = 0; i_x < 20; i_x++)
	{
		for (u8 i_y = 0; i_y < 12; i_y++)
		{
			make_16x16_tile(0, array[river_][i_y][i_x], layer, i_x * 2, i_y * 2, 1);
		}
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

void create_sprite(u8 screen, u8 index, u8 asset, u8 mode)
{
	//mode 0 -> enemy  |  mode 1 -> player

	NF_CreateSprite(screen, index, asset, asset, 0, 0);
	NF_EnableSpriteRotScale(screen, index, index, true); //using index for arg. 3 so that they have separate rotations

	switch (mode)
	{
	case 0:
		NF_SpriteLayer(screen, index, menu_layer);
		break;

	case 1:
		NF_SpriteLayer(screen, index, item_layer);
		break;
	}
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

	big_enemy_scroll = 0;

	if (player.player_x < -16)
	{
		player.player_x = 240;
		scroll_x = 64;
		big_enemy_scroll = -63;
	}
	if (player.player_x > 240)
	{
		player.player_x = -16;
		scroll_x = 0;
		big_enemy_scroll = 63;
	}

	if (player.player_y < -16)
	{
		player.player_y = 192 - 16;
	}
	if (player.player_y > 192 - 16)
	{
		player.player_y = -16;
	}

	//scrolling

	NF_MoveSprite(0, 0, player.player_x, player.player_y);

	if (player.player_x >= 80 && player.player_x <= 144)
	{
		scroll_x = player.player_x - 80;
	}
	if (player.player_x >= 144)
	{
		scroll_x = 144 - 80;
	}

	NF_ScrollBg(0, item_layer, scroll_x, 0);
	NF_ScrollBg(0, map_layer, scroll_x, 0);
}

void spawn_player()
{

	//create player sprite
	create_sprite(0, 0, 0, 1);
	NF_SpriteLayer(0, 0, item_layer);

	player.player_x = rand_(19) * 16 + 8;
	player.player_y = rand_(12) * 16 + 8;

	//make the player face the correct direction
	NF_SpriteRotScale(0, 0, 0, 256, 256);
	spawn_offset = false;
	if (player.player_x >= 80 && player.player_x <= 144)
	{
		player.player_x += 4;
		spawn_offset = true;
		scroll_x = player.player_x - 80;
	}

	if (player.player_x >= 232)
	{
		player.player_x = 232;
	}

	if (player.player_x >= 144)
	{
		scroll_x = 144 - 80;
	}

	NF_MoveSprite(0, 0, player.player_x, player.player_y);
	NF_ScrollBg(0, item_layer, scroll_x, 0);
	NF_ScrollBg(0, map_layer, scroll_x, 0);
	if (get_player_tile(item_layer) >= 1)
	{
		spawn_player();
	}
}

int inworld(s32 input)
{
	s32 output;
	output = input + (enemy_scroll * abs(player.speed)) + big_enemy_scroll;
	return output;
}

void spawn_enemy(u8 enemy_type, s8 direction_, s16 enemy_x_, s16 enemy_y_)
{
	x_offset = 0;
	if (spawn_offset != false)
	{
		x_offset = 4;
	}

	total_enemies++;
	enemies_deleted = false;
	if (enemy_type == 0)
	{
		car_enemies.group_members++;

		car_enemies.enemy_id[car_enemies.group_members] = total_enemies;
		create_sprite(0, total_enemies, 1, 0);

		NF_SpriteRotScale(0, total_enemies, 128, 256, 256); //turn to the right
		car_enemies.enemy_direction[car_enemies.group_members] = direction_;
		if (direction_ == -1)
		{
			NF_SpriteRotScale(0, total_enemies, 384, 256, 256); //turn to the left
		}

		car_enemies.enemy_x[car_enemies.group_members] = enemy_x_ * 16 + 8 + x_offset;
		car_enemies.enemy_y[car_enemies.group_members] = enemy_y_ * 16 + 8;
		NF_MoveSprite(0, total_enemies, car_enemies.enemy_x[car_enemies.group_members], car_enemies.enemy_y[car_enemies.group_members]);
	}
	else if (enemy_type == 1)
	{
		snow_enemies.group_members++;

		snow_enemies.enemy_id[snow_enemies.group_members] = total_enemies;

		snow_enemies.enemy_x[snow_enemies.group_members] = enemy_x_ * 16 + 8 + x_offset;
		snow_enemies.enemy_y[snow_enemies.group_members] = enemy_y_ * 16;

		create_sprite(0, total_enemies, 2, 0);

		NF_MoveSprite(0, total_enemies, snow_enemies.enemy_x[snow_enemies.group_members], snow_enemies.enemy_y[snow_enemies.group_members]);

		total_enemies++;

		ball_enemies.group_members++;

		ball_enemies.enemy_id[ball_enemies.group_members] = total_enemies;

		ball_enemies.enemy_y[ball_enemies.group_members] = enemy_y_ * 16 + 8;

		ball_enemies.target_x[ball_enemies.group_members] = player.player_x;
		ball_enemies.target_y[ball_enemies.group_members] = player.player_y;

		ball_enemies.enemy_x[ball_enemies.group_members] = -220;
		ball_enemies.can_spawn[ball_enemies.group_members] = true;

		create_sprite(0, total_enemies, 3, 0);

		NF_MoveSprite(0, total_enemies, ball_enemies.enemy_x[ball_enemies.group_members], ball_enemies.enemy_y[ball_enemies.group_members]);
	}
	else if (enemy_type == 2)
	{	

		fire_enemies.group_members++;

		fire_enemies.enemy_id[fire_enemies.group_members] = total_enemies;

		fire_enemies.enemy_x[fire_enemies.group_members] = shadow_enemies.enemy_x[shadow_enemies.group_members];
		fire_enemies.enemy_y[fire_enemies.group_members] = shadow_enemies.enemy_y[shadow_enemies.group_members] - (32 + rand_(5) * 16);

		create_sprite(0, total_enemies, 6, 0);

		NF_MoveSprite(0, total_enemies, fire_enemies.enemy_x[fire_enemies.group_members], fire_enemies.enemy_y[fire_enemies.group_members]);

		total_enemies++;

		shadow_enemies.group_members++;

		shadow_enemies.enemy_id[shadow_enemies.group_members] = total_enemies;

		shadow_enemies.enemy_x[shadow_enemies.group_members] = enemy_x_ * 16 + 8 + x_offset;
		shadow_enemies.enemy_y[shadow_enemies.group_members] = enemy_y_ * 16 + 8;

		create_sprite(0, total_enemies, 7, 0);

		NF_MoveSprite(0, total_enemies, shadow_enemies.enemy_x[shadow_enemies.group_members], shadow_enemies.enemy_y[shadow_enemies.group_members]);

	}
}

void update_enemy_scroll()
{

	if (prev_scroll_x > scroll_x)
	{
		enemy_scroll = 1;
	}
	else if (prev_scroll_x < scroll_x)
	{
		enemy_scroll = -1;
	}
	if (prev_scroll_x == scroll_x)
	{
		enemy_scroll = 0;
	}

	prev_scroll_x = scroll_x;
}

void update_car_enemy(u8 enemy_)
{
	u8 half = 0;
	if (frame_in_sec % 2 == 1)
	{
		half = 1;
	}
	//update position
	car_enemies.enemy_x[enemy_] += inworld(car_enemies.enemy_direction[enemy_] * ((int)(difficulty / 4) + 1 + half));
	if ((car_enemies.enemy_x[enemy_] <= -32 - scroll_x) && (car_enemies.enemy_direction[enemy_] == -1))
	{
		car_enemies.enemy_direction[enemy_] = 1;
		NF_SpriteRotScale(0, car_enemies.enemy_id[enemy_], 128, 256, 256); //turn right
		car_enemies.enemy_y[enemy_] -= 7 * 16;
	}
	if ((car_enemies.enemy_x[enemy_] >= 288 + 32 - scroll_x) && (car_enemies.enemy_direction[enemy_] == 1))
	{
		car_enemies.enemy_direction[enemy_] = -1;
		NF_SpriteRotScale(0, car_enemies.enemy_id[enemy_], 384, 256, 256); //turn left
		car_enemies.enemy_y[enemy_] += 7 * 16;
	}

	//animation
	if (car_enemies.anim_delay <= current_msec)
	{
		car_enemies.current_frame++;
		if (car_enemies.current_frame >= 4)
			car_enemies.current_frame = 0;
		car_enemies.anim_delay = current_msec + 142;
	}
	NF_SpriteFrame(0, car_enemies.enemy_id[enemy_], car_enemies.current_frame);

	//move sprite and fill holes
	NF_MoveSprite(0, car_enemies.enemy_id[enemy_], car_enemies.enemy_x[enemy_], car_enemies.enemy_y[enemy_]);
	//make sure that we do stuff inbounds
	if ((car_enemies.enemy_x[enemy_] <= 256 * 2) & (car_enemies.enemy_x[enemy_] >= 0))
	{
		if ((car_enemies.enemy_y[enemy_] <= 192) & (car_enemies.enemy_y[enemy_] >= 0))
		{

			make_16x16_tile(0,0, item_layer, even(car_enemies.enemy_x[enemy_] / 8 + scroll_x / 8 + 1), car_enemies.enemy_y[enemy_] / 8 + 1, 1);
		}
	}
}

void update_snow_ball_enemy(u8 enemy_)
{
	ball_enemies.rotation[enemy_] += (2 + difficulty);
	NF_SpriteRotScale(0, ball_enemies.enemy_id[enemy_], ball_enemies.rotation[enemy_], 256, 256); //spin

	if (rand_(101) >= 50 && ball_enemies.can_spawn[enemy_] == true)
	{
		ball_enemies.enemy_x[enemy_] = inworld(snow_enemies.enemy_x[enemy_] + 8);
		ball_enemies.enemy_y[enemy_] = inworld(snow_enemies.enemy_y[enemy_] + 24);
		ball_enemies.target_x[enemy_] = player.player_x;
		ball_enemies.target_y[enemy_] = player.player_y;
		ball_enemies.can_spawn[enemy_] = false;
		ball_enemies.moves[enemy_] = 0;
	}

	s16 temp_x = ball_enemies.enemy_x[enemy_];
	s16 temp_y = ball_enemies.enemy_y[enemy_];

	if (ball_enemies.can_spawn[enemy_] == false)
	{

		if (ball_enemies.target_x[enemy_] > ball_enemies.enemy_x[enemy_])
		{
			ball_enemies.enemy_x[enemy_] += 1;
		}
		if (ball_enemies.target_x[enemy_] < ball_enemies.enemy_x[enemy_])
		{
			ball_enemies.enemy_x[enemy_] -= 1;
		}
		if (ball_enemies.target_y[enemy_] > ball_enemies.enemy_y[enemy_])
		{
			ball_enemies.enemy_y[enemy_] += 1;
		}
		if (ball_enemies.target_y[enemy_] < ball_enemies.enemy_y[enemy_])
		{
			ball_enemies.enemy_y[enemy_] -= 1;
		}

		if ((temp_x != ball_enemies.enemy_x[enemy_]) || (temp_y != ball_enemies.enemy_y[enemy_]))
		{
			ball_enemies.moves[enemy_] += 1;
			if ((temp_x != ball_enemies.enemy_x[enemy_]) && (temp_y != ball_enemies.enemy_y[enemy_]))
			{
				ball_enemies.moves[enemy_] += 1;
			}
		}
	}
	if (((temp_x == ball_enemies.enemy_x[enemy_]) && (temp_y == ball_enemies.enemy_y[enemy_])) || (ball_enemies.moves[enemy_] >= 72))
	{
		ball_enemies.enemy_x[enemy_] = -220;
		ball_enemies.can_spawn[enemy_] = true;
	}

	ball_enemies.enemy_x[enemy_] = inworld(ball_enemies.enemy_x[enemy_]);
	NF_MoveSprite(0, ball_enemies.enemy_id[enemy_], ball_enemies.enemy_x[enemy_], ball_enemies.enemy_y[enemy_]);
}

void update_snow_enemy(u8 enemy_)
{
	snow_enemies.enemy_x[enemy_] = inworld(snow_enemies.enemy_x[enemy_]);
	NF_SpriteFrame(0, snow_enemies.enemy_id[enemy_], 0);
	if (player.player_x > snow_enemies.enemy_x[enemy_] + 8)
	{
		NF_SpriteFrame(0, snow_enemies.enemy_id[enemy_], 1);
	}
	if (player.player_x < snow_enemies.enemy_x[enemy_] - 8)
	{
		NF_SpriteFrame(0, snow_enemies.enemy_id[enemy_], 2);
	}
	NF_MoveSprite(0, snow_enemies.enemy_id[enemy_], snow_enemies.enemy_x[enemy_], snow_enemies.enemy_y[enemy_]);
	if ((snow_enemies.enemy_x[enemy_] <= 256 * 2) & (even(snow_enemies.enemy_x[enemy_] / 8 + scroll_x / 8 + 1) >= 0))
	{
		make_16x16_tile(0,0, item_layer, even(snow_enemies.enemy_x[enemy_] / 8 + scroll_x / 8 + 1), even(snow_enemies.enemy_y[enemy_] / 8) + 4, 1);
	}
}

void update_shadow_enemy(u8 enemy_)
{

	shadow_enemies.enemy_x[enemy_] = inworld(shadow_enemies.enemy_x[enemy_]);
	NF_SpriteFrame(0, shadow_enemies.enemy_id[enemy_], 0);
	if (frame_in_sec % 2 == 1)
	{
		NF_SpriteFrame(0, shadow_enemies.enemy_id[enemy_], 1);
	}

	if (shadow_enemies.can_spawn[enemy_] == true)
	{
		shadow_enemies.enemy_x[enemy_] = inworld((rand_(39) + 1) * 16 + 8) - (scroll_x % 16);
		shadow_enemies.enemy_y[enemy_] = (rand_(11) + 1) * 16 + 8;
		fire_enemies.enemy_y[enemy_] = shadow_enemies.enemy_y[enemy_] - (48 + rand_(5) * 16);
		shadow_enemies.can_spawn[enemy_] = false;
	}
	NF_MoveSprite(0, shadow_enemies.enemy_id[enemy_], inworld(shadow_enemies.enemy_x[enemy_]), shadow_enemies.enemy_y[enemy_]);
}

void update_fire_enemy(u8 enemy_)
{
	fire_enemies.enemy_x[enemy_] = inworld(shadow_enemies.enemy_x[enemy_]); //!
	if (shadow_enemies.enemy_y[enemy_] - 8 >= fire_enemies.enemy_y[enemy_])
	{
		if (frame_in_sec % 2 ==0)
		{
			fire_enemies.enemy_y[enemy_]+=1+(int)(difficulty/3);
		}
	}
	else if (shadow_enemies.enemy_y[enemy_] - 8 <= fire_enemies.enemy_y[enemy_])
	{
		shadow_enemies.can_spawn[enemy_] = true;
		update_shadow_enemy(enemy_);
	}

	//animation
	if (fire_enemies.anim_delay <= current_msec)
	{
		fire_enemies.current_frame++;
		if (fire_enemies.current_frame >= 2)
			fire_enemies.current_frame = 0;
		fire_enemies.anim_delay = current_msec + 164;
	}

	NF_SpriteFrame(0, fire_enemies.enemy_id[enemy_], fire_enemies.current_frame);

	NF_MoveSprite(0, fire_enemies.enemy_id[enemy_], fire_enemies.enemy_x[enemy_], fire_enemies.enemy_y[enemy_]);
}

bool touch_box(s32 x, s32 y, u16 sizex, u16 sizey)
{
	touchRead(&touchXY);
	if (touchXY.px >= x && touchXY.px <= x + sizex && touchXY.py >= y && touchXY.py <= y + sizey)
	{
		return true;
	}
	return false;
}

bool sprites_collide(s32 x1, s32 y1, s32 x2, s32 y2, s16 size1, s16 size2, s16 offsetx1, s16 offsety1, s16 offsetx2, s16 offsety2)
{
	if ((((x1 + offsetx1 >= x2 + offsetx2) && (x1 + offsetx1 <= x2 + size2 + offsetx2)) || ((x1 + size1 + offsetx1 >= x2 + offsetx2) && (x1 + size1 + offsetx1 <= x2 + size2 + offsetx2))) && (((y1 + offsety1 >= y2 + offsety2) && (y1 + offsety1 <= y2 + size2 + offsety2)) || ((y1 + size1 + offsety1 >= y2 + offsety2) && (y1 + size1 + offsety1 <= y2 + size2 + offsety2))))
	{
		return true;
	}

	return false;
}

void get_enemy_collision()
{
	//update enemies here
	update_enemy_scroll();
	if (car_enemies_used == true)
	{
		for (u8 enemy_index = 1; enemy_index <= car_enemies.group_members; enemy_index++) //start from 1 bc 0 is for player
		{
			update_car_enemy(enemy_index);

			if (sprites_collide(player.player_x, player.player_y, car_enemies.enemy_x[enemy_index], car_enemies.enemy_y[enemy_index], 12, 14, 2, 1, 1, 1) == true)
			{
				player.player_state = 1;
			}
		}
	}
	if (snow_enemies_used == true)
	{
		for (u8 enemy_index = 1; enemy_index <= snow_enemies.group_members; enemy_index++) //start from 1 bc 0 is for player
		{
			update_snow_enemy(enemy_index);

			if (sprites_collide(player.player_x, player.player_y, snow_enemies.enemy_x[enemy_index], snow_enemies.enemy_y[enemy_index], 12, 16, 2, 1, 0, 20) == true)
			{
				player.player_state = 1;
			}

			update_snow_ball_enemy(enemy_index);
			if (sprites_collide(player.player_x, player.player_y, ball_enemies.enemy_x[enemy_index], ball_enemies.enemy_y[enemy_index], 13, 8, 2, 1, 0, 4) == true)
			{
				player.player_state = 1;
			}
		}
	}
	if (fire_enemies_used == true)
	{
		for (u8 enemy_index = 1; enemy_index <= fire_enemies.group_members; enemy_index++)
		{
			update_fire_enemy(enemy_index);
			update_shadow_enemy(enemy_index);

			if (sprites_collide(player.player_x, player.player_y, shadow_enemies.enemy_x[enemy_index], shadow_enemies.enemy_y[enemy_index], 13, 10, 2, 1, 2, 4) == true)
			{
				if (sprites_collide(player.player_x, player.player_y, fire_enemies.enemy_x[enemy_index], fire_enemies.enemy_y[enemy_index], 13, 10, 2, 1, 2, 2) == true)
				{
					player.player_state = 1;
				}
			}
		}
	}
}

bool is_in(u8 input, u8 array[])
{
	for (u16 i = 0; i <= (sizeof &array / sizeof array[0]); i++)
	{
		if (input == array[i])
		{
			return true;
		}
	}
	return false;
}

void spawn_enemies()
{
	u8 rnd_[2];
	bool illegal_position[14][22];
	u8 enemy = {0};
	u8 i = {0};

	if (car_enemies_used == true)
	{
		while ((enemy <= 5 + rand_(4)) & (i <= 255))
		{
			rnd_[2] = rand_(50);
			rnd_[1] = rand_(9) + 10;

			//set y position to one of the four lanes
			if (rnd_[2] <= 8)
			{
				rnd_[0] = 0;
			}
			else if ((rnd_[2] >= 9) & (rnd_[2] <= 13))
			{
				rnd_[0] = 2;
			}
			else if ((rnd_[2] >= 14) & (rnd_[2] <= 19))
			{
				rnd_[0] = 7;
			}
			else if ((rnd_[2] >= 20) & (rnd_[2] <= 26))
			{
				rnd_[0] = 9;
			}
			else
			{
				rnd_[0] = 0;
			}

			//actually spawn the enemy
			if (illegal_position[rnd_[1] + 1][rnd_[0] + 1] == false)
			{
				if (rnd_[0] >= 7)
				{
					spawn_enemy(0, -1, rnd_[1], rnd_[0]);
				}
				else
				{
					spawn_enemy(0, 1, rnd_[1], rnd_[0]);
				}
				//add the used pos. & the ones next to it to illegal positions
				illegal_position[rnd_[1] + 1][rnd_[0] + 1] = true;
				illegal_position[rnd_[1]][rnd_[0] + 1] = true;
				illegal_position[rnd_[1] + 2][rnd_[0] + 1] = true;
				enemy++;
			}
		}
	}
	if (snow_enemies_used == true)
	{

		while (enemy < 4)
		{
			rnd_[1] = rand_(19) + 1; //x
			rnd_[0] = rand_(10) + 1; //y

			if (illegal_position[rnd_[1] + 1][rnd_[0] + 1] == false)
			{
				spawn_enemy(1, 0, rnd_[1], rnd_[0]);

				illegal_position[rnd_[1] + 1][rnd_[0] + 1] = true;
				illegal_position[rnd_[1] - 1 + 1][rnd_[0] + 1] = true;
				illegal_position[rnd_[1] + 1 + 1][rnd_[0] + 1] = true;
				illegal_position[rnd_[1] + 1][rnd_[0] - 1 + 1] = true;
				illegal_position[rnd_[1] + 1][rnd_[0] + 1 + 1] = true;

				enemy++;
			}
		}
	}
	if (fire_enemies_used == true)
	{
		while (enemy < 5)
		{
			rnd_[1] = rand_(39) + 1; //x
			rnd_[0] = rand_(20) + 1; //y

			spawn_enemy(2, 0, rnd_[1], rnd_[0]);
			enemy++;
		}
	}
}

void clear_enemies()
{
	for (u8 enemy = 1; enemy <= total_enemies; enemy++)
	{
		NF_SpriteRotScale(0, enemy, 0, 256, 256);
		NF_DeleteSprite(0, enemy);
	}
	enemies_deleted = true;
}

void clear_map(u8 layer, u16 tile, u8 mode)
{ //mode -> 0 fill map | mode -> 1 switch to empty map | mode -> 2 fill map wth 16x16 tiles
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
	case 2:
		for (u8 x = 0; x < 40; x += 1)
		{
			for (u8 y = 0; y < 24; y += 1)
			{
				make_16x16_tile(0,tile, layer, even(x), even(y), 1);
			}
		}
		break;
	}
}

void add_object(u8 screen, u8 layer_, char *str_)
{
	u8 prop_id = 0;
	if (strcmp(str_, "item") == 0)
	{
		u8 rnd_num = rand_(9);
		if (rnd_num >= 5) //don't forget to make the this "==6" when adding last item
		{
			rnd_num = 4;
		}
		prop_id = 21 + rnd_num * 4;
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
		for (u8 i = 0; i < (rand_(7) + 28); i++)
		{
			make_16x16_tile(screen,2, layer_, even(rand_(80)), even(rand_(48)), 1);
		}
		for (u8 i = 0; i < (rand_(8) + 5); i++)
		{
			make_16x16_tile(screen,6, layer_, even(rand_(80)), even(rand_(48)), 1);
		}
	}
	else if (strcmp(str_, "snow_grass") == 0)
	{
		for (u8 i = 0; i < (rand_(5) + 12); i++)
		{
			make_16x16_tile(screen,6, layer_, even(rand_(80)), even(rand_(48)), 1);
		}
	}
	else if (strcmp(str_, "rocks") == 0)
	{
		clear_map(layer_,1,2);
		for (u8 i = 0; i < (rand_(5) + 12); i++)
		{
			make_16x16_tile(screen,6, layer_, even(rand_(80)), even(rand_(48)), 1);
		}
	}
	else if (strcmp(str_, "item") == 0)
	{
		u16 rnd_x = even(rand_(80));
		u16 rnd_y = even(rand_(48));

		if ((NF_GetTileOfMap(screen, item_layer, rnd_x, rnd_y) == 75) || (NF_GetTileOfMap(screen, item_layer, rnd_x, rnd_y) == 0))
		{
			make_16x16_tile(screen,prop_id, layer_, rnd_x, rnd_y, 1);
		}
	}
}

void gen_map(u8 map_index)
{
	char map_name[10];
	sprintf(map_name, "map%i", level);

	NF_CreateTiledBg(0, map_layer, map_name); //map layer

	if (map_index == 0)
	{
		add_object(0,map_layer, "grass");
	}
	else if (map_index == 2)
	{
		add_object(0,map_layer, "snow_grass");
	}
	else if (map_index == 3)
	{
		add_object(0,map_layer, "rocks");
		u8 rnd_river = (int)(rand_(26));

		if (rnd_river >= 20){
			rnd_river = 9;
		}
		else if (rnd_river >= 16)
		{
			rnd_river = 8;
		}
		else if (rnd_river >= 12)
		{
			rnd_river = 7;
		}
		else if (rnd_river >= 10)
		{
			rnd_river = 6;
		}

		array2river(item_layer, rnd_river, rivers);
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
				make_16x16_tile(0,NF_GetTileOfMap(0, item_layer, x, y) + 4, item_layer, x, y, 1);
			}
		}
	}
}

void reset_enemies()
{
	//delete enemy sprites if there are any
	if ((enemies_deleted == false) & (total_enemies > 0))
	{
		clear_enemies();
	}

	//reset gen enemy data
	enemies_used = false;
	car_enemies_used = false;
	snow_enemies_used = false;
	fire_enemies_used = false;
	total_enemies = 0;
	//clear enemy data
	car_enemies = empty_group;
	snow_enemies = empty_group;
	ball_enemies = empty_group;
	fire_enemies = empty_group;
	shadow_enemies = empty_group;
	//re-SET the gen enemy data
	switch (level)
	{
	case 0:
		enemies_used = false;
		break;
	case 1:
		car_enemies_used = true;
		enemies_used = true;
		break;
	case 2:
		snow_enemies_used = true;
		//|-> refers to snow men & balls
		enemies_used = true;
		break;
	case 3:
		fire_enemies_used = true;
		//|-> refers to targets & fire balls
		enemies_used = true;
		break;
	}
	NF_ResetSpriteBuffers();
}

void enemies_layer(u8 layer){
	for (u8 enemy = 1; enemy <= total_enemies; enemy++)
	{
		NF_SpriteLayer(0, enemy, layer);
	}
}

void set_sprites_out(u8 screen){
	for (u8 sprite = 0; sprite <= total_enemies; sprite++)
	{
		NF_MoveSprite(screen,sprite, 250,200);
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
	switch (level)
	{
	case 0:
		clear_map(map_layer, 1, 0);
		break;
	case 2:
		clear_map(map_layer, 1, 2);
		break;
	case 3:
		clear_map(map_layer, 1, 2);
		break;
	}
	clear_map(item_layer, 0, 0);

	gen_map(level);
	swiWaitForVBlank();
	//-------
}

void difficulty_menu();
bool cursor_menu();

void title_menu(){
	//-title menu-
	//fancy grass stuff
	swiWaitForVBlank();
	NF_CreateTiledBg(0,menu_layer,"clear_screen");

	set_sprites_out(0);

	NF_CreateTiledBg(0,map_layer,"map0");
	NF_CreateTiledBg(1,map_layer,"map0");
	clear_map(item_layer,0,2);
	clear_map(map_layer,1,0);
	add_object(0,map_layer,"grass");

	render();

	NF_CreateTiledBg(1,map_layer,"map0");
	NF_ScrollBg(1,map_layer,64,0);
	render();

	bool menu_choice={cursor_menu("title","title_bottom")};
	
	if(menu_choice==true)
	{
		NF_CreateTiledBg(0,menu_layer,"power_msg");
		render();
		NF_CreateTiledBg(1,menu_layer,"power_msg");
		render();
		u8 i=0;
		while(i==0){};//loop until shut down
	}

	//------------
}

void main_menu(void)
{
	level = 0;
	player.player_state = 1;
	struct Timer input_delay = {current_msec + 100};
	bool selecting = {true};
	char map_name[11];
	//make temp maps
	NF_CreateTiledBg(0, map_layer, "main_menu0");
	NF_CreateTiledBg(0, menu_layer, "main_menu0");
	NF_CreateTiledBg(1, menu_layer, "main_menu_touch");
	while (selecting)
	{
		scanKeys(); //get button input

		if ((keysHeld() > 0) & (input_delay.delay < current_msec))
		{
			if ((keysHeld() & KEY_LEFT) || (touch_box(17, 42, 85, 85) == true))
			{
				level--;
			}
			else if ((keysHeld() & KEY_RIGHT) || (touch_box(156, 42, 85, 85) == true))
			{
				level++;
			}
			if (level > 3)
			{
				level = 0;
			}
			else if (level < 0)
			{
				level = 3;
			}

			sprintf(map_name, "main_menu%i", level);
			NF_CreateTiledBg(0, menu_layer, map_name);

			input_delay.delay = current_msec + 104;
		}

		if ((keysHeld() & KEY_A) || (touch_box(166, 158, 35, 25) == true))
		{
			goto_title=false;
			selecting = false;
		}
		else if ((keysHeld() & KEY_B) || (touch_box(26, 158, 65, 26) == true))
		{
			goto_title=true;
			input_delay.delay = 0;
			difficulty = 3;
			selecting=false;
		}
		swiWaitForVBlank();
		render();
		swiWaitForVBlank();
		update_current_time();
	}

	NF_CreateTiledBg(0, map_layer, "map0");
	clear_map(menu_layer, 0, 1);
	NF_CreateTiledBg(0, menu_layer, "diff_menu");
	swiWaitForVBlank();
}

void difficulty_menu()
{
	difficulty = 3;
	player.player_state = 1;
	struct Timer input_delay = {100};
	bool selecting = true;
	//make a temp maps
	NF_CreateTiledBg(0, map_layer, "map0");
	NF_CreateTiledBg(0, menu_layer, "diff_menu");
	NF_ScrollBg(0, menu_layer, 0, 192 * (difficulty - 1));
	swiWaitForVBlank();
	render();

	while (selecting)
	{
		scanKeys(); //get  button input

		if ((keysHeld() > 0) & (input_delay.delay < current_msec))
		{
			if ((keysHeld() & KEY_LEFT) || (touch_box(17, 42, 85, 85) == true))
			{
				difficulty--;
			}
			else if ((keysHeld() & KEY_RIGHT) || (touch_box(156, 42, 85, 85) == true))
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
			input_delay.delay = current_msec + 104;
		}

		if ((keysHeld() & KEY_A) || (touch_box(166, 158, 35, 25) == true))
		{
			exit_menus=true;
			selecting = false;
		}
		else if ((keysHeld() & KEY_B) || (touch_box(26, 158, 65, 26) == true))
		{
			reset();
			swiWaitForVBlank();
			input_delay.delay = 0;
			difficulty = 3;
			selecting=false;
		}

		NF_ScrollBg(0, menu_layer, 0, 192 * (difficulty - 1));
		swiWaitForVBlank();
		render();
		update_current_time();
	}
	swiWaitForVBlank();
	NF_CreateTiledBg(1, menu_layer, "game_touch");
}

bool cursor_menu(char* menu_img_top ,char* menu_img_bottom)
{
	s8 selected = {0};
	s8 prev_selected = selected;
	bool confirmed=false;

	touch_text_render=false;//make text layer not render on touch screen

	u32 backup_msec = current_msec;
	u32 backup_sec = current_sec;
	u32 backup_frame_in_sec = frame_in_sec;

	struct Timer input_delay = {current_msec + 120};

	NF_ClearTextLayer16(1, text_layer);
	NF_UpdateTextLayers();
	NF_CreateTiledBg(0, menu_layer, menu_img_top);
	NF_CreateTiledBg(1, menu_layer, menu_img_bottom);
	create_sprite(1, 0, 0, 0);
	NF_SpriteLayer(1, 0, 0); //make it so that the sprite displays in front on the menu
	NF_SpriteRotScale(1, 0, 0, 388, 358);

	while(confirmed==false)
	{
		scanKeys();

		if(input_delay.delay<=current_msec)
		{
			//get input:
			//buttons:

			switch(keysHeld())
			{
				case KEY_UP:
					selected--;
				break;
				case KEY_DOWN:
					selected++;
				break;
				case KEY_A:
					confirmed=true;
				break;
			}

			//touch:
			if (touch_box(48,40,160,40))
			{
				if (selected==0)
				{
					confirmed=true;
				}
				else
				{
				selected--;
				}
			}
			else if (touch_box(48,112,160,40))
			{
				if (selected==1)
				{
					confirmed=true;
				}
				else
				{
				selected++;
				}
			}

			if (selected!=prev_selected)
			{
				input_delay.delay=current_msec+120;
			}
		}

		//cap S8 selected of:
		if(selected<=-1)
		{
			selected=1;
		}
		else if(selected>=2)
		{
			selected=0;
		}

		switch(selected)
		{
			case 0:
				NF_MoveSprite(1,0,48+16,40-4);
			break;
			case 1:
				NF_MoveSprite(1,0,48+16,112-4);
			break;
		}

		prev_selected=selected;

		render();
		update_current_time();
	}

	NF_DeleteSprite(1,0);
	NF_CreateTiledBg(0,menu_layer,"clear_screen");
	NF_CreateTiledBg(1,menu_layer,"game_touch");

	touch_text_render=true;

	current_msec = backup_msec;
	current_sec = backup_sec;
	frame_in_sec = backup_frame_in_sec;
	
	return selected;
}

void spawn_hole()
{
	u8 hole_x;
	u8 hole_y;

	hole_x = even(rand_(80));
	hole_y = even(rand_(48));

	if ((NF_GetTileOfMap(0, item_layer, hole_x, hole_y) > 8 && NF_GetTileOfMap(0, item_layer, hole_x, hole_y) < 20) || (NF_GetTileOfMap(0, item_layer, hole_x, hole_y) >= 49))
	{
		update_holes();
		spawn_hole();
	}
	else
	{
		make_16x16_tile(0,1, item_layer, hole_x, hole_y, 1);
	}
}

void replace_item(u8 tile, s8 tile_2)
{
	if (get_player_tile(item_layer) == tile)
	{
		make_16x16_tile(0,tile_2, item_layer, player.collision_x, player.collision_y, 1);
	}
	else if (get_player_tile(item_layer) == tile + 1)
	{
		make_16x16_tile(0,tile_2, item_layer, player.collision_x - 1, player.collision_y, 1);
	}
	else if (get_player_tile(item_layer) == tile + 2)
	{
		make_16x16_tile(0,tile_2, item_layer, player.collision_x, player.collision_y - 1, 1);
	}
	else if (get_player_tile(item_layer) == tile + 3)
	{
		make_16x16_tile(0,tile_2, item_layer, player.collision_x - 1, player.collision_y - 1, 1);
	}
}

void do_physics()
{
	if (get_player_tile(item_layer) > 8)
	{
		if (get_player_tile(item_layer) < 20)
		{
			if ((player.bridges > 0) && (player.player_state != 4))
			{
				replace_item(9, 45);
				replace_item(13, 45);
				replace_item(17, 45);
				player.bridges -= 1;
			}
			else
			{
				if (player.player_state != 4)
				{
					player.player_state = 1;
				}
			}
		}
		if (get_player_tile(item_layer) >= 49 && get_player_tile(item_layer) < 75)
		{
			player.player_state = 1;
		}
		if (player.player_state != 4)
		{
			if (get_player_tile(item_layer) >= 21 && get_player_tile(item_layer) <= 24)
			{
				player.speed = 2;
				player.player_state = 3;
				replace_item(21, 0);
				speed_item.delay = current_sec + 6;
			}
			else if (get_player_tile(item_layer) >= 33 && get_player_tile(item_layer) <= 36)
			{
				player.speed = -1;
				player.player_state = 2;
				replace_item(33, 0);
				invert_item.delay = current_sec + 6;
			}
			else if (get_player_tile(item_layer) >= 29 && get_player_tile(item_layer) <= 32)
			{
				player.bridges += 1;
				replace_item(29, 0);
			}
			else if (get_player_tile(item_layer) >= 25 && get_player_tile(item_layer) <= 28)
			{
				player.score += 10000;
				replace_item(25, 0);
			}
			else if (get_player_tile(item_layer) >= 37 && get_player_tile(item_layer) <= 40)
			{
				player.player_state = 4;
				player.speed = 1;
				player.anim_delay = current_msec + 142;
				replace_item(37, 0);
				roller.delay = current_sec + 6;
				create_sprite(0, 0, 4, 1);
				NF_MoveSprite(0, 0, player.player_x, player.player_y);
			}
		}
	}
	if (enemies_used == true)
	{
		get_enemy_collision();
	}
}

void game_over()
{
	enemies_layer(item_layer);

	NF_CreateTiledBg(0, menu_layer, "game_over");
	NF_CreateTiledBg(1, menu_layer, "game_over_touch");
	render();
	//display score
	NF_ClearTextLayer16(0, text_layer);
	NF_WriteText16(0, text_layer, 10, 6, player.score_str);
	NF_UpdateTextLayers();

	while ((keysHeld() != (KEY_A || KEY_START)) || (touch_box(0, 0, 256, 192) == false))
	{
		scanKeys(); //get input
	}
	clear_map(menu_layer, 0, 1);
	NF_ClearTextLayer16(0, text_layer);
	NF_UpdateTextLayers();

	spawn_player();

	reset();
	reset_enemies();
	if (enemies_used)
	{
		spawn_enemies();
	}
	swiWaitForVBlank();
	NF_CreateTiledBg(1, menu_layer, "game_touch");
}

int main(int argc, char **argv)
{
	init();
	load();
	title_menu();
	while (1)
	{
		scanKeys();
		swiDelay(500000);
		swiWaitForVBlank();
		scanKeys();

		while(exit_menus==false){
		main_menu();

			while(goto_title==true){

				title_menu();

				main_menu();
			}
			goto_title=false;

		difficulty_menu();
		}
		exit_menus=false;

		clear_map(menu_layer, 0, 1);

		spawn_player();

		reset();
		reset_enemies();
		if (enemies_used)
		{
			spawn_enemies();
		};

		while (reset_game == false)
		{
			scanKeys(); //get  button input

			u32 button = keysHeld();
			if ((button == KEY_START) || (button == KEY_LID) || (touch_box(205, 163, 30, 20) && current_msec >= 520))
			{
				enemies_layer(item_layer);
				if (cursor_menu("pause_menu_top","pause_menu") == true)
				{
					reset_game = true;
					
				}
				else
				{
					enemies_layer(text_layer);
				}
				
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
				add_object(0,item_layer, "item");
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
			case 4:
				if (player.player_x <= 256 && player.player_y <= 192)
				{
					make_16x16_tile(0,75, item_layer, player.player_x, player.player_y, 0);
				}

				if (player.anim_delay <= current_msec)
				{
					player.current_frame++;
					if (player.current_frame >= 4)
						player.current_frame = 0;
					player.anim_delay = current_msec + 142;
				}
				NF_SpriteFrame(0, 0, player.current_frame);
				if (roller.delay <= current_sec)
				{
					create_sprite(0, 0, 0, 1);
					NF_MoveSprite(0, 0, player.player_x, player.player_y);
					player.player_state = 0;
				}
			default:
				break;
			}
			//-----------

			do_physics();
			render();
			update_current_time();
		}
		reset_game = false;
	}
	return 0;
}