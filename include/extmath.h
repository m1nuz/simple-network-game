#pragma once

#include <math.h>

#define ABS(t)          ((t) < 0 ? -(t) : (t))
#define SIGN(t)         ((t) < 0 ? -1 : (t) > 0 ? 1 : 0)
#define FLOOR(t)        ((t) < 0 ? (int) ((t)-1.0) : (int) (t))
#define ROUND(t)        ((t) < 0 ? (int) ((t)-0.5) : (int) ((t)+0.5))

#define CLAMP(t,lo,hi)  ((t) < (lo) ? (lo) : (t) > (hi) ? (hi) : (t))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b)  ((a) > (b) ? (a) : (b))
#define MINMIN(a, b, c) ((a) < (b) ? MIN (a,c) : MIN (b,c))
#define MAXMAX(a, b, c) ((a) > (b) ? MAX (a,c) : MAX (b,c))

#define radians(x) ((x) * (M_PI / 180.0))
#define degrees(x) ((x) * (180.0 / M_PI))

////////////////////////////////////////////////////////////////////////////////

typedef struct vec2
{
    float x, y;
} vec2;

typedef struct ivec2
{
    int x, y;
} ivec2;

typedef struct vec3
{
    float x, y, z;
} vec3;

typedef struct ivec3
{
    int x, y, z;
} ivec3;

typedef struct vec4
{
    float x, y, z, w;
} vec4;

typedef struct ivec4
{
    int x, y, z, w;
} ivec4;

typedef vec3 mat3[3];
typedef vec4 mat4[4];
/*******************************************************************************
 * vec2 set, copy, addition, subtraction, multiplication, division
 */
#define vec2_set(v, a, b)       \
        ((v).x = a,             \
         (v).y = b)

#define vec2_copy(u, v)         \
        ((u).x = (v).x,         \
         (u).y = (v).y)

/* store result in a */
#define vec2_add(a, b)          \
    ((a).x += (b).x,            \
    (a).y += (b).y)

#define vec2_sub(a, b)          \
    ((a).x -= (b).x,            \
    (a).y -= (b).y)

#define vec2_mul(a, b)          \
    ((a).x *= (b).x,            \
    (a).y *= (b).y)

#define vec2_div(a, b)          \
    ((a).x /= (b).x,            \
    (a).y /= (b).y)

/* store result in v */
#define vec2_addv(v, a, b)      \
    ((v).x = (a).x + (b).x,     \
    (v).y = (a).y + (b).y)

#define vec2_subv(v, a, b)      \
    ((v).x = (a).x - (b).x,     \
    (v).y = (a).y - (b).y)

#define vec2_mulv(v, a, b)      \
    ((v).x = (a).x * (b).x,     \
    (v).y = (a).y * (b).y)

#define vec2_divv(v, a, b)      \
    ((v).x = (a).x / (b).x,     \
    (v).y = (a).y / (b).y)

/*
 * vec2 addition, subtraction, multiplication, division with scalar
 */
#define vec2_adds(v, s)         \
    ((v).x += (s),              \
    (v).y += (s))

#define vec2_subs(v, s)         \
    ((v).x -= (s),              \
    (v).y -= (s))

#define vec2_muls(v, s)         \
    ((v).x *= (s),              \
    (v).y *= (s))

#define vec2_divs(v, s)         \
    ((v).x /= (s),              \
    (v).y /= (s))

/*
 * vec2 dot, cross, length, normalize, determinant
 */
#define vec2_dot(u, v)          \
    ((u).x * (v).x +            \
    (u).y * (v).y)

#define vec2_cross(a, b)        \
    ((a).x * (b).y -            \
    (a).y * (b).x)

#define vec2_length(v)          \
    sqrtf(vec2_dot(v, v))

#define vec2_normalize(v) do {          \
    float tmp = vec2_length(v);         \
    tmp = (tmp == 0.0f) ? 1.f : tmp;    \
    vec2_muls(v, 1.f / tmp);            \
    }while(0)

/*******************************************************************************
 * vec3 set, copy, addition, subtraction, multiplication, division
 */
#define vec3_set(v, a, b, c)    \
    ((v).x = (a),               \
    (v).y = (b),                \
    (v).z = (c))

#define vec3_copy(u, v)         \
    ((u).x = (v).x,             \
    (u).y = (v).y,              \
    (u).z = (v).z)

/* store result in a */
#define vec3_add(a, b)          \
    ((a).x += (b).x,            \
    (a).y += (b).y,             \
    (a).z += (b).z)

#define vec3_sub(a, b)          \
    ((a).x -= (b).x,            \
    (a).y -= (b).y,             \
    (a).z -= (b).z)

#define vec3_mul(a, b)          \
    ((a).x *= (b).x,            \
    (a).y *= (b).y,             \
    (a).z *= (b).z)

#define vec3_div(a, b)          \
    ((a).x /= (b).x,            \
    (a).y /= (b).y,             \
    (a).z /= (b).z)

/* store result in v */
#define vec3_addv(v, a, b)      \
    ((v).x = (a).x + (b).x,     \
    (v).y = (a).y + (b).y,      \
    (v).z = (a).z + (b).z)

#define vec3_subv(v, a, b)      \
    ((v).x = (a).x - (b).x,     \
    (v).y = (a).y - (b).y,      \
    (v).z = (a).z - (b).z)

#define vec3_mulv(v, a, b)      \
    ((v).x = (a).x * (b).x,     \
    (v).y = (a).y * (b).y,      \
    (v).z = (a).z * (b).z)

#define vec3_divv(v, a, b)      \
    ((v).x = (a).x / (b).x,     \
    (v).y = (a).y / (b).y,      \
    (v).z = (a).z / (b).z)

/*
 * vec3 addition, subtraction, multiplication, division with scalar
 */
#define vec3_adds(v, s)         \
    ((v).x += (s),              \
    (v).y += (s),               \
    (v).z += (s))

#define vec3_subs(v, s)         \
    ((v).x -= (s),              \
    (v).y -= (s),               \
    (v).z -= (s))

#define vec3_muls(v, s)         \
    ((v).x *= (s),              \
    (v).y *= (s),               \
    (v).z *= (s))

#define vec3_divs(v, s)         \
    ((v).x /= (s),              \
    (v).y /= (s),               \
    (v).z /= (s))
/*
 * vec3 dot, cross, length, normalize, determinant
 */
#define vec3_dot(u, v)          \
    ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)

#define vec3_cross(v, a, b)                     \
    ((v).x = (a).y * (b).z - (a).z * (b).y,     \
    (v).y = (a).z * (b).x - (a).x * (b).z,      \
    (v).z = (a).x * (b).y - (a).y * (b).x)

#define vec3_length(v)          \
    sqrtf(vec3_dot(v, v))

#define vec3_normalize(v) do {          \
    float tmp = vec3_length(v);         \
    tmp = (tmp == 0.0f) ? 1.f : tmp;    \
    vec3_muls(v, 1.f / tmp);            \
    }while(0)

#define vec3_delta(a, b, c)                     \
        ((a).x * ((b).y * (c).z - (b).z * (c).y)\
        +(a).y * ((b).z * (c).x - (b).x * (c).z)\
        +(a).z * ((b).x * (c).y - (b).y * (c).x))

/*******************************************************************************
 * vec4 set, copy
 */
#define vec4_set(v, a, b, c, d) \
    ((v).x = (a),               \
    (v).y = (b),                \
    (v).z = (c),                \
    (v).w = (d))

#define vec4_copy(v, u) \
    ((v).x = (u).x,     \
    (v).y = (u).y,      \
    (v).z = (u).z,      \
    (v).w = (u).w)

/*
 * vec4 addition, subtraction, multiplication, division with scalar
 */
#define vec4_adds(v, s) \
    ((v).x += (s),      \
    (v).y += (s),       \
    (v).z += (s),       \
    (v).w += (s))
#define vec4_subs(v, s) \
    ((v).x -= (s),      \
    (v).y -= (s),       \
    (v).z -= (s),       \
    (v).w -= (s))
#define vec4_muls(v, s) \
    ((v).x *= (s),      \
    (v).y *= (s),       \
    (v).z *= (s),       \
    (v).w *= (s))
#define vec4_divs(v, s) \
    ((v).x /= (s),      \
    (v).y /= (s),       \
    (v).z /= (s),       \
    (v).w /= (s))

#define vec4_dot(u, v)\
        ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z + (u).w * (v).w)

#define vec4_length(v)          \
    sqrtf(vec4_dot(v, v))

#define vec4_normalize(v) do {          \
    float tmp = vec3_length(v);         \
    tmp = (tmp == 0.0f) ? 1.f : tmp;    \
    vec4_muls(v, 1.f / tmp);            \
    }while(0)
/*******************************************************************************
 * mat3
 */
#define IDENTITY_MAT3   \
    {{1.0, 0.0, 0.0},   \
    {0.0, 1.0, 0.0},    \
    {0.0, 0.0, 1.0}}

#define mat3_set(m, a, b, c)    \
    (vec3_set((m)[0], a),       \
    vec3_set((m)[1], b),        \
    vec3_set((m)[2], c))

#define mat3_copy(m, n)         \
    (vec3_copy((m)[0], (n)[0]), \
    vec3_copy((m)[1], (n)[1]),  \
    vec3_copy((m)[2], (n)[2]))

/*
 * mat3 addition, subtraction, multiplication, division (by a scalar element)
 */

#define mat3_adds(m, s)     \
    (vec3_adds((m)[0], s),  \
    vec3_adds((m)[1], s),   \
    vec3_adds((m)[2], s))

#define mat3_subs(m, s)     \
    (vec3_subs((m)[0], s),  \
    vec3_subs((m)[1], s),   \
    vec3_subs((m)[2], s))

#define mat3_muls(m, s)     \
    (vec3_muls((m)[0], s),  \
    vec3_muls((m)[1], s),   \
    vec3_muls((m)[2], s))

#define mat3_divs(m, s)     \
    (vec3_divs((m)[0], s),  \
    vec3_divs((m)[1], s),   \
    vec3_divs((m)[2], s))
/*
 * mat3 multiply
 */
#define mat3_row_vec3(v, n, m)\
    ((v).x * (m)[0].n + (v).y * (m)[1].n + (v).z * (m)[2].n)

#define mat3_multiply(m, a, b) \
    ((m)[0].x = mat3_row_vec3((a)[0], x, (b)),\
    (m)[0].y = mat3_row_vec3((a)[0], y, (b)),\
    (m)[0].z = mat3_row_vec3((a)[0], z, (b)),\
    (m)[1].x = mat3_row_vec3((a)[1], x, (b)),\
    (m)[1].y = mat3_row_vec3((a)[1], y, (b)),\
    (m)[1].z = mat3_row_vec3((a)[1], z, (b)),\
    (m)[2].x = mat3_row_vec3((a)[2], x, (b)),\
    (m)[2].y = mat3_row_vec3((a)[2], y, (b)),\
    (m)[2].z = mat3_row_vec3((a)[2], z, (b)))
/*
 * mat3 determinant, transposition, adjunction, inversion
 */
/*#define mat3_delta(m)           \
    (vec3_delta((m)[0], (m)[1], (m)[2]))*/

#define mat3_transpose(m, a)\
    ((m)[0].x = (a)[0].x, (m)[0].y = (a)[1].x, (m)[0].z = (a)[2].x,\
    (m)[1].x = (a)[0].y, (m)[1].y = (a)[1].y, (m)[1].z = (a)[2].y,\
    (m)[2].x = (a)[0].z, (m)[2].y = (a)[1].z, (m)[2].z = (a)[2].z)

#define mat3_delta(m) \
    (+ (m)[0].x * ((m)[1].y * (m)[2].z - (m)[1].z * (m)[2].y) \
    - (m)[0].y * ((m)[1].x * (m)[2].z - (m)[1].z * (m)[2].x) \
    + (m)[0].z * ((m)[1].x * (m)[2].y - (m)[1].y * (m)[2].x))

#define mat3_adj(m, a)\
    ((m)[0].x = (a)[1].y * (a)[2].z - (a)[1].z * (a)[2].y,\
    (m)[1].x = (a)[2].y * (a)[0].z - (a)[2].z * (a)[0].y,\
    (m)[2].x = (a)[0].y * (a)[1].z - (a)[0].z * (a)[1].y,\
    (m)[0].y = (a)[1].z * (a)[2].x - (a)[1].x * (a)[2].z,\
    (m)[1].y = (a)[2].z * (a)[0].x - (a)[2].x * (a)[0].z,\
    (m)[2].y = (a)[0].z * (a)[1].x - (a)[0].x * (a)[1].z,\
    (m)[0].z = (a)[1].x * (a)[2].y - (a)[1].y * (a)[2].x,\
    (m)[1].z = (a)[2].x * (a)[0].y - (a)[2].y * (a)[0].x,\
    (m)[2].z = (a)[0].x * (a)[1].y - (a)[0].y * (a)[1].x)

#define mat3_inverse(m, a) do { \
    (m)[0].x = + ((a)[1].y * (a)[2].z - (a)[2].y * (a)[1].z); \
    (m)[1].x = - ((a)[1].x * (a)[2].z - (a)[2].x * (a)[1].z); \
    (m)[2].x = + ((a)[1].x * (a)[2].y - (a)[2].x * (a)[1].y);\
    (m)[0].y = - ((a)[0].y * (a)[2].z - (a)[2].y * (a)[0].z); \
    (m)[1].y = + ((a)[0].x * (a)[2].z - (a)[2].x * (a)[0].z); \
    (m)[2].y = - ((a)[0].x * (a)[2].y - (a)[2].x * (a)[0].y); \
    (m)[0].z = + ((a)[0].y * (a)[1].z - (a)[1].y * (a)[0].z); \
    (m)[1].z = - ((a)[0].x * (a)[1].z - (a)[1].x * (a)[0].z); \
    (m)[2].z = + ((a)[0].x * (a)[1].y - (a)[1].x * (a)[0].y); \
    float det = mat3_delta(a); \
    mat3_divs(m, det); \
    }while(0)

/*******************************************************************************
 * mat4
 */
#define IDENTITY_MAT4       \
    {{1.0, 0.0, 0.0, 0.0},  \
    {0.0, 1.0, 0.0, 0.0},   \
    {0.0, 0.0, 1.0, 0.0},   \
    {0.0, 0.0, 0.0, 1.0}}

#define mat4_copy(m, n)         \
    (vec4_copy((m)[0], (n)[0]), \
    vec4_copy((m)[1], (n)[1]),  \
    vec4_copy((m)[2], (n)[2]),  \
    vec4_copy((m)[3], (n)[3]))

#define mat4_adds(m, s)     \
    (vec4_adds((m)[0], s),  \
    vec4_adds((m)[1], s),   \
    vec4_adds((m)[2], s),   \
    vec4_adds((m)[3], s))

#define mat4_subs(m, s)     \
    (vec4_subs((m)[0], s),  \
    vec4_subs((m)[1], s),   \
    vec4_subs((m)[2], s),   \
    vec4_subs((m)[3], s))

#define mat4_muls(m, s)     \
    (vec4_muls((m)[0], s),  \
    vec4_muls((m)[1], s),   \
    vec4_muls((m)[2], s),   \
    vec4_muls((m)[3], s))

#define mat4_divs(m, s)     \
    (vec4_divs((m)[0],s),   \
    vec4_divs((m)[1], s),   \
    vec4_divs((m)[2], s),   \
    vec4_divs((m)[3], s))

/*
 * mat4 mat4_multiply
 */
#define mat4_row_vec4(m, n, v)  \
    ((m)[0].n * (v).x + (m)[1].n * (v).y + (m)[2].n * (v).z + (m)[3].n * (v).w)

#define mat4_multiply(m, a, b)                  \
    ((m)[0].x = mat4_row_vec4((a), x, (b)[0]),  \
    (m)[0].y = mat4_row_vec4((a), y, (b)[0]),   \
    (m)[0].z = mat4_row_vec4((a), z, (b)[0]),   \
    (m)[0].w = mat4_row_vec4((a), w, (b)[0]),   \
    (m)[1].x = mat4_row_vec4((a), x, (b)[1]),   \
    (m)[1].y = mat4_row_vec4((a), y, (b)[1]),   \
    (m)[1].z = mat4_row_vec4((a), z, (b)[1]),   \
    (m)[1].w = mat4_row_vec4((a), w, (b)[1]),   \
    (m)[2].x = mat4_row_vec4((a), x, (b)[2]),   \
    (m)[2].y = mat4_row_vec4((a), y, (b)[2]),   \
    (m)[2].z = mat4_row_vec4((a), z, (b)[2]),   \
    (m)[2].w = mat4_row_vec4((a), w, (b)[2]),   \
    (m)[3].x = mat4_row_vec4((a), x, (b)[3]),   \
    (m)[3].y = mat4_row_vec4((a), y, (b)[3]),   \
    (m)[3].z = mat4_row_vec4((a), z, (b)[3]),   \
    (m)[3].w = mat4_row_vec4((a), w, (b)[3]))

#define mat4_multiply2(m, n) do {   \
    mat4 temp;                      \
    mat4_multiply(temp, m, n);      \
    mat4_copy(m, temp);             \
    }while(0)

#define mat4_lmul_vec4(v, a, m)             \
        ((v).x = mat4_row_vec4((m), x, (a)),\
         (v).y = mat4_row_vec4((m), y, (a)),\
         (v).z = mat4_row_vec4((m), z, (a)),\
         (v).w = mat4_row_vec4((m), w, (a)))

#define mat4_rmul_vec4(v, m, a)             \
        ((v).x = vec4_dot((a), (m)[0]),     \
         (v).y = vec4_dot((a), (m)[1]),     \
         (v).z = vec4_dot((a), (m)[2]),     \
         (v).w = vec4_dot((a), (m)[3]))
/*
 * mat4
 */
//#define mat4_delta(m)

#define mat4_inverse(m, a) do { \
    const float subfactor00 = (a)[2].z * (a)[3].w - (a)[3].z * (a)[2].w;\
    const float subfactor01 = (a)[2].y * (a)[3].w - (a)[3].y * (a)[2].w;\
    const float subfactor02 = (a)[2].y * (a)[3].z - (a)[3].y * (a)[2].z;\
    const float subfactor03 = (a)[2].x * (a)[3].w - (a)[3].x * (a)[2].w;\
    const float subfactor04 = (a)[2].x * (a)[3].z - (a)[3].x * (a)[2].z;\
    const float subfactor05 = (a)[2].x * (a)[3].y - (a)[3].x * (a)[2].y;\
    const float subfactor06 = (a)[1].z * (a)[3].w - (a)[3].z * (a)[1].w;\
    const float subfactor07 = (a)[1].y * (a)[3].w - (a)[3].y * (a)[1].w;\
    const float subfactor08 = (a)[1].y * (a)[3].z - (a)[3].y * (a)[1].z;\
    const float subfactor09 = (a)[1].x * (a)[3].w - (a)[3].x * (a)[1].w;\
    const float subfactor10 = (a)[1].x * (a)[3].z - (a)[3].x * (a)[1].z;\
    const float subfactor11 = (a)[1].y * (a)[3].w - (a)[3].y * (a)[1].w;\
    const float subfactor12 = (a)[1].x * (a)[3].y - (a)[3].x * (a)[1].y;\
    const float subfactor13 = (a)[1].z * (a)[2].w - (a)[2].z * (a)[1].w;\
    const float subfactor14 = (a)[1].y * (a)[2].w - (a)[2].y * (a)[1].w;\
    const float subfactor15 = (a)[1].y * (a)[2].z - (a)[2].y * (a)[1].z;\
    const float subfactor16 = (a)[1].x * (a)[2].w - (a)[2].x * (a)[1].w;\
    const float subfactor17 = (a)[1].x * (a)[2].z - (a)[2].x * (a)[1].z;\
    const float subfactor18 = (a)[1].x * (a)[2].y - (a)[2].x * (a)[1].y;\
    (m)[0].x = + ((a)[1].y * subfactor00 - (a)[1].z * subfactor01 + (a)[1].w * subfactor02);\
    (m)[1].x = - ((a)[1].x * subfactor00 - (a)[1].z * subfactor03 + (a)[1].w * subfactor04);\
    (m)[2].x = + ((a)[1].x * subfactor01 - (a)[1].y * subfactor03 + (a)[1].w * subfactor05);\
    (m)[3].x = - ((a)[1].x * subfactor02 - (a)[1].y * subfactor04 + (a)[1].z * subfactor05);\
    (m)[0].y = - ((a)[0].y * subfactor00 - (a)[0].z * subfactor01 + (a)[0].w * subfactor02);\
    (m)[1].y = + ((a)[0].x * subfactor00 - (a)[0].z * subfactor03 + (a)[0].w * subfactor04);\
    (m)[2].y = - ((a)[0].x * subfactor01 - (a)[0].y * subfactor03 + (a)[0].w * subfactor05);\
    (m)[3].y = + ((a)[0].x * subfactor02 - (a)[0].y * subfactor04 + (a)[0].z * subfactor05);\
    (m)[0].z = + ((a)[0].y * subfactor06 - (a)[0].z * subfactor07 + (a)[0].w * subfactor08);\
    (m)[1].z = - ((a)[0].x * subfactor06 - (a)[0].z * subfactor09 + (a)[0].w * subfactor10);\
    (m)[2].z = + ((a)[0].x * subfactor11 - (a)[0].y * subfactor09 + (a)[0].w * subfactor12);\
    (m)[3].z = - ((a)[0].x * subfactor08 - (a)[0].y * subfactor10 + (a)[0].z * subfactor12);\
    (m)[0].w = - ((a)[0].y * subfactor13 - (a)[0].z * subfactor14 + (a)[0].w * subfactor15);\
    (m)[1].w = + ((a)[0].x * subfactor13 - (a)[0].z * subfactor16 + (a)[0].w * subfactor17);\
    (m)[2].w = - ((a)[0].x * subfactor14 - (a)[0].y * subfactor16 + (a)[0].w * subfactor18);\
    (m)[3].w = + ((a)[0].x * subfactor15 - (a)[0].y * subfactor17 + (a)[0].w * subfactor18);\
    float det = (+ (a)[0].x * m[0].x  \
    + (a)[0].y * m[1].x               \
    + (a)[0].z * m[2].x               \
    + (a)[0].w * m[3].x);             \
    mat4_divs(m, det);                \
    }while(0)

#define mat4_transpose(v, m)    \
    ((v)[0].x = (m)[0].x,       \
    (v)[0].y = (m)[1].x,        \
    (v)[0].z = (m)[2].x,        \
    (v)[0].w = (m)[3].x,        \
    (v)[1].x = (m)[0].y,        \
    (v)[1].y = (m)[1].y,        \
    (v)[1].z = (m)[2].y,        \
    (v)[1].w = (m)[3].y,        \
    (v)[2].x = (m)[0].z,        \
    (v)[2].y = (m)[1].z,        \
    (v)[2].z = (m)[2].z,        \
    (v)[2].w = (m)[3].z,        \
    (v)[3].x = (m)[0].w,        \
    (v)[3].y = (m)[1].w,        \
    (v)[3].z = (m)[2].w,        \
    (v)[3].w = (m)[3].w)

/*
 * mat4 translate, rotate, scale
 */
#define mat4_translate(m, v) do {   \
    mat4 n = {{1,0,0,0},            \
    {0,1,0,0},                      \
    {0,0,1,0},                      \
    {(v).x,(v).y,(v).z,1}};         \
    mat4 temp;                      \
    mat4_multiply(temp, m, n);      \
    mat4_copy(m, temp);             \
    }while(0)

#define mat4_rotate_x(m, a) do {        \
    const float c = cosf(radians(a));   \
    const float s = sinf(radians(a));   \
    mat4 n = {{1,0,0,0},                \
    {0,c,-s,0},                         \
    {0,s,c,0},                          \
    {0,0,0,1}};                         \
    mat4 temp;                          \
    mat4_multiply(temp, m, n);          \
    mat4_copy(m, temp);                 \
    }while(0)

#define mat4_rotate_y(m, a) do {        \
    const float c = cosf(radians(a));   \
    const float s = sinf(radians(a));   \
    mat4 n = {{c,0,s,0},                \
    {0,1,0,0},                          \
    {-s,0,c,0},                         \
    {0,0,0,1}};                         \
    mat4 temp;                          \
    mat4_multiply(temp, m, n);          \
    mat4_copy(m, temp);                 \
    }while(0)

#define mat4_rotate_z(m, a) do {        \
    const float c = cosf(radians(a));   \
    const float s = sinf(radians(a));   \
    mat4 n = {{c,-s,0,0},               \
    {s,c,0,0},                          \
    {0,0,1,0},                          \
    {0,0,0,1}};                         \
    mat4 temp;                          \
    mat4_multiply(temp, m, n);          \
    mat4_copy(m, temp);                 \
    }while(0)

#define mat4_scale(m, v) do {       \
    mat4 n = {{(v).x,0,0,0},        \
    {0,(v).y,0,0},                  \
    {0,0,(v).z,0},                  \
    {0,0,0,1}};                     \
    mat4 temp;                      \
    mat4_multiply(temp, m, n);      \
    mat4_copy(m, temp);             \
    }while(0)

/*
 * mat4 perspective
 */
#define mat4_perspective(m, fov, aspect, znear, zfar) do {                              \
    const float range = tanf(fov * M_PI / 360.0) * znear;                               \
    vec4_set((m)[0], (2 * znear) / ((range * aspect) - (-range * aspect)), 0, 0, 0);    \
    vec4_set((m)[1], 0, (2 * znear) / (2 * range), 0, 0);                               \
    vec4_set((m)[2], 0, 0, -(zfar + znear) / (zfar - znear), -1);                       \
    vec4_set((m)[3], 0, 0, -(2 * zfar * znear) / (zfar - znear), 0);                    \
    }while(0)

#define mat4_lookat(m, eye, center, up) do { \
    vec3 f, u, s; \
    vec3_copy(u, up); \
    vec3_subv(f, center, eye); \
    vec3_normalize(f); \
    vec3_normalize(u); \
    vec3_cross(s, f, u); \
    vec3_cross(u, s, f); \
    (m)[0].x = s.x; \
    (m)[1].x = s.y; \
    (m)[2].x = s.z; \
    (m)[0].y = u.x; \
    (m)[1].y = u.y; \
    (m)[2].y = u.z; \
    (m)[0].z = -f.x; \
    (m)[1].z = -f.y; \
    (m)[2].z = -f.z; \
    (m)[3].x = -vec3_dot(s, eye); \
    (m)[3].y = -vec3_dot(u, eye); \
    (m)[3].z = -vec3_dot(f, eye); \
    (m)[0].w = 0; \
    (m)[1].w = 0; \
    (m)[2].w = 0; \
    (m)[3].w = 1; \
    }while(0)
