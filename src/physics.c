#include "physics.h"

extern void
interpolate(const struct STATE *previous, const struct STATE *current, float alpha, struct STATE *state)
{
    state->position.x = current->position.x * alpha + previous->position.x * (1 - alpha);
    state->position.y = current->position.y * alpha + previous->position.y * (1 - alpha);

    state->velocity.x = current->velocity.x * alpha + previous->velocity.x * (1 - alpha);
    state->velocity.y = current->velocity.y * alpha + previous->velocity.y * (1 - alpha);
}

static vec2
accelerate(const struct STATE *state)
{
    vec2 result;

    result.x = state->velocity.x * state->acceleration;
    result.y = state->velocity.y * state->acceleration;

    return result;
}

// t
static void
evaluate0(const struct PLAYER_INPUT *input, const struct STATE *initial, struct DERIVATIVE *d)
{
    d->position.x = initial->velocity.x;
    d->position.y = initial->velocity.y;

    vec2 a = accelerate(initial);

    d->velocity.x = a.x;
    d->velocity.y = a.y;
}
// t + dt
static void
evaluate1(const struct PLAYER_INPUT *input, const struct STATE *initial, float dt, const struct DERIVATIVE *d, struct DERIVATIVE *result)
{
    struct STATE state;

    state.position.x = initial->position.x + d->position.x * dt;
    state.position.y = initial->position.y + d->position.y * dt;

    state.velocity.x = initial->velocity.x + d->velocity.x * dt;
    state.velocity.y = initial->velocity.y + d->velocity.y * dt;

    result->position.x = state.velocity.x;
    result->position.y = state.velocity.y;

    vec2 a = accelerate(initial);

    result->velocity.x = a.x;
    result->velocity.y = a.y;
}

extern void
integrate(const struct PLAYER_INPUT *input, struct STATE *state, float dt)
{
    struct DERIVATIVE a;
    struct DERIVATIVE b;
    struct DERIVATIVE c;
    struct DERIVATIVE d;

    evaluate0(input, state, &a);
    evaluate1(input, state, dt + .5f, &a, &b);
    evaluate1(input, state, dt + .5f, &b, &c);
    evaluate1(input, state, dt, &c, &d);

    const float dxdt = 1.f / 6.f * (a.position.x + 2.f * (b.position.x + c.position.x) + d.position.x);
    const float dydt = 1.f / 6.f * (a.position.y + 2.f * (b.position.y + c.position.y) + d.position.y);

    const float dvdt = 1.f / 6.f * (a.velocity.x + 2.f * (b.velocity.x + c.velocity.x) + d.velocity.x);
    const float dwdt = 1.f / 6.f * (a.velocity.y + 2.f * (b.velocity.y + c.velocity.y) + d.velocity.y);

    state->position.x = state->position.x + dxdt * dt;
    state->position.y = state->position.y + dydt * dt;

    state->velocity.x = state->velocity.x + dvdt * dt;
    state->velocity.y = state->velocity.y + dwdt * dt;
}

extern void
add_impulse(struct STATE *state, float x, float y)
{
    state->velocity.x += x;
    state->velocity.y += y;
}
