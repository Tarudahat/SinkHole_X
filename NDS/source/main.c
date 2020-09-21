
#include <nds.h>
#include <stdio.h>

#include <nf_lib.h>
#include "structs.c"

struct Player player = {0, 16, 16, 0, 0, 1};
struct PrintConsole topScreen;
struct PrintConsole bottomScreen;

void init(void)
{
	NF_Set2D(0, 0);
	consoleDemoInit();

	consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
	consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
	consoleSelect(&bottomScreen);

	swiWaitForVBlank(); //vsync
	NF_SetRootFolder("NITROFS");
	NF_Set2D(0, 0);
	NF_InitSpriteBuffers();
	NF_InitSpriteSys(0);
	NF_InitSpriteSys(1);
	//load assets
	//					name     , id  , w, h
	NF_LoadSpriteGfx("sprites/car", 0, 16, 16);
	NF_LoadSpritePal("sprites/car", 0);

	//NF_LoadSpriteGfx("sprites/bola", 1, 32, 32); // Bola azul
	//NF_LoadSpritePal("sprites/bola", 1);

	//in which vram? screen,vram,ram,animframes?
	NF_VramSpriteGfx(0, 0, 0, false);
	NF_VramSpritePal(0, 0, 0);

	//NF_VramSpriteGfx(0, 1, 1, false);
	//NF_VramSpritePal(0, 1, 1);
}

void render(void)
{
	// Update the OAM array
	NF_SpriteOamSet(0);
	NF_SpriteOamSet(1);

	iprintf("\x1b[1;1H Score:%lli", player.score);
	swiWaitForVBlank(); // Wait for vertical sync
	// Update the OAM
	oamUpdate(&oamMain);
	oamUpdate(&oamSub);
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
	}
	if (player.player_x > 240)
	{
		player.player_x = -16;
	}

	if (player.player_y < -8)
	{
		player.player_y = 192 - 16;
	}
	if (player.player_y > 192 - 16)
	{
		player.player_y = -8;
	}
	NF_MoveSprite(0, 0, player.player_x, player.player_y);
}

int main(void)
{
	init();

	//creat player sprite and enable rot.
	NF_CreateSprite(0, 0, 0, 0, 0, 0);
	NF_EnableSpriteRotScale(0, 0, 0, true);

	//NF_CreateSprite(0, 1, 1, 1, 0, 0);

	touchPosition touchXY;

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
