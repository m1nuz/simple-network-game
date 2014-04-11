#pragma once

#include <stdbool.h>

#include "extmath.h"
// space simulation

struct PLAYER_INPUT
{
    bool            left;
    bool            right;
    bool            forward;
    bool            back;
    bool            shoot;

    float           target_x;
    float           target_y;
};

struct STATE
{
    vec2 position;
    vec2 velocity;

    float acceleration;
};

struct DERIVATIVE
{
    vec2 position;
    vec2 velocity;
};

void interpolate(const struct STATE *previous, const struct STATE *current, float alpha, struct STATE *state);
void integrate(const struct PLAYER_INPUT *input,struct STATE *state, float dt);
void add_impulse(struct STATE *state, float x, float y);
