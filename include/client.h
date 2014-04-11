#pragma once

#include "main.h"
#include "physics.h"
#include "collision.h"

struct PLAYER
{
    int                     id;
    char                    name[MAX_PLAYER_NAME];

    struct PLAYER_INPUT     input;

    struct STATE            previous;
    struct STATE            current;

    AABB                    aabb;
    vec3                    color;
    int                     hit;

    int                     weapon_delay;
};

struct PLAYER_SNAP
{
    int                     id;
    struct PLAYER_INPUT     input;
    struct STATE            previous;
    struct STATE            current;
    int                     weapon_delay;
};

struct PLAYER_MOVE
{
    unsigned int time;
    struct PLAYER_INPUT     input;
    struct STATE            state;
};

struct BULLET
{
    struct STATE            previous;
    struct STATE            current;

    int                     alive;
    int                     used;
};

void update_physics(unsigned int t, float dt, struct PLAYER_INPUT *input);
void update(unsigned int t, float dt, struct PLAYER_INPUT *input);

extern unsigned int         timesteps;
extern struct PLAYER_INPUT  player_input;
extern int                  replaying;
