#pragma once

#include "extmath.h"

typedef struct AABB
{
    vec2 min;
    vec2 max;
} AABB;

int test_aabb_vs_aabb(AABB a, AABB b);
int test_aabb_vs_point(AABB a, vec2 p);

AABB make_aabb(vec2 position, vec2 size);
int is_inside_aabb(AABB a, vec2 p);
