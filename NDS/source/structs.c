#include <nds.h>
#include <stdio.h>

struct Player
{
    u64 score;
    s16 player_x;
    s16 player_y;
    u8 player_state;
    u8 bridges;
    u8 speed;
};
