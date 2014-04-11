#include "collision.h"

int
test_aabb_vs_aabb(AABB a, AABB b)
{
    if((a.max.x < b.min.x) || (a.min.x > b.max.x)) return 0;
    if((a.max.y < b.min.y) || (a.min.y > b.max.y)) return 0;

    return 1;
}

int
test_aabb_vs_point(AABB a, vec2 p)
{
    if((p.x > a.min.x) && (p.x < a.max.x) && (p.y > a.min.y) && (p.y < a.max.y))
        return 1;

    return 0;
}

AABB
make_aabb(vec2 position, vec2 size)
{
    AABB bbox;

    vec2_addv(bbox.max, position, size);
    vec2_subv(bbox.min, position, size);

    return bbox;
}

int
is_inside_aabb(AABB a, vec2 p)
{
    if((p.x > a.min.x) && (p.x < a.max.x) && (p.y > a.min.y) && (p.y < a.max.y))
        return 1;

    return 0;
}
