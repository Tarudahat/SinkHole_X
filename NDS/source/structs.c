#include <nds.h>
#include <stdio.h>

struct Player
{
    u64 score;
    s16 player_x;
    s16 player_y;
    u8 player_state;
    u8 bridges;
    s8 speed;
    u8 collision_x; //only needed for systems with 8x8 tiles
    u8 collision_y; //only needed for systems with 8x8 tiles
};

struct Timer
{
    u16 delay;
    bool do_action;
};
