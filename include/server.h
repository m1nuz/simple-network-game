#pragma once

#include <stdbool.h>
#include <enet/enet.h>

#include "main.h"
#include "physics.h"
#include "collision.h"

struct PLAYER
{
    ENetPeer                *peer;

    int                     id;
    char                    name[MAX_PLAYER_NAME];

    struct PLAYER_INPUT     input;

    struct STATE            previous;
    struct STATE            current;

    vec3                    color;
    AABB                    aabb;
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

struct BULLET
{
    struct STATE            previous;
    struct STATE            current;

    int                     alive;
    int                     used;
};
