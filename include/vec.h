#ifndef MATH_H
#define MATH_H
#include "std.h"

b32 math_is_pow2(u32 n);
s32 math_floor_log2(u32 n);
s32 math_ceil_log2(u32 n);

r32 math_radians(r32 degrees);

// clang-format off

r32 r32_min(r32 a, r32 b) { return (a < b ? a : b); }
r64 r64_min(r64 a, r64 b) { return (a < b ? a : b); }
s32 s32_min(s32 a, s32 b) { return (a < b ? a : b); }
u32 u32_min(u32 a, u32 b) { return (a < b ? a : b); }

r32 r32_min3(r32 a, r32 b, r32 c) { return (r32_min(r32_min(a, b), c)); }
r64 r64_min3(r64 a, r64 b, r64 c) { return (r64_min(r64_min(a, b), c)); }
s32 s32_min3(s32 a, s32 b, s32 c) { return (s32_min(s32_min(a, b), c)); }
u32 u32_min3(u32 a, u32 b, u32 c) { return (u32_min(u32_min(a, b), c)); }

r32 r32_max(r32 a, r32 b) { return (a > b ? a : b); }
r64 r64_max(r64 a, r64 b) { return (a > b ? a : b); }
s32 s32_max(s32 a, s32 b) { return (a > b ? a : b); }
u32 u32_max(u32 a, u32 b) { return (a > b ? a : b); }

r32 r32_max3(r32 a, r32 b, r32 c) { return (r32_max(r32_max(a, b), c)); }
r64 r64_max3(r64 a, r64 b, r64 c) { return (r64_max(r64_max(a, b), c)); }
s32 s32_max3(s32 a, s32 b, s32 c) { return (s32_max(s32_max(a, b), c)); }
u32 u32_max3(u32 a, u32 b, u32 c) { return (u32_max(u32_max(a, b), c)); }

r32 r32_clamp(r32 x, r32 lower, r32 upper) { return r32_min(r32_max(x, lower), upper); }
r64 r64_clamp(r64 x, r64 lower, r64 upper) { return r64_min(r64_max(x, lower), upper); }
s32 s32_clamp(s32 x, s32 lower, s32 upper) { return s32_min(s32_max(x, lower), upper); }
u32 u32_clamp(u32 x, u32 lower, u32 upper) { return u32_min(u32_max(x, lower), upper); }

r32 r32_abs(r32 a) { return (a > 0 ? a : -a); }
r64 r64_abs(r64 a) { return (a > 0 ? a : -a); }
s32 s32_abs(s32 a) { return (a > 0 ? a : -a); }
u32 u32_abs(u32 a) { return (a > 0 ? a : -a); }

r32 r32_pow2(r32 a) { return (a * a); }
r64 r64_pow2(r64 a) { return (a * a); }
s32 s32_pow2(s32 a) { return (a * a); }
u32 u32_pow2(u32 a) { return (a * a); }

r32 r32_pow3(r32 a) { return (a * a * a); }
r64 r64_pow3(r64 a) { return (a * a * a); }
s32 s32_pow3(s32 a) { return (a * a * a); }
u32 u32_pow3(u32 a) { return (a * a * a); }

static const r32 MATH_EPSILON = 1.19209290e-7f;
static const r32 MATH_ZERO = 0.0f;
static const r32 MATH_ONE = 1.0f;
static const r32 MATH_TWO_THIRDS = 0.666666666666666666666666666666666666667f;

static const r32 MATH_TAU = 6.28318530717958647692528676655900576f;
static const r32 MATH_PI = 3.14159265358979323846264338327950288f;
static const r32 MATH_ONE_OVER_TAU = 0.636619772367581343075535053490057448f;
static const r32 MATH_ONE_OVER_PI = 0.159154943091895335768883763372514362f;

static const r32 MATH_TAU_OVER_2 = 3.14159265358979323846264338327950288f;
static const r32 MATH_TAU_OVER_4 = 1.570796326794896619231321691639751442f;
static const r32 MATH_TAU_OVER_8 = 0.785398163397448309615660845819875721f;

static const r32 MATH_E = 2.71828182845904523536f;
static const r32 MATH_SQRT_TWO = 1.41421356237309504880168872420969808f;
static const r32 MATH_SQRT_THREE = 1.73205080756887729352744634150587236f;
static const r32 MATH_SQRT_FIVE = 2.23606797749978969640917366873127623f;

static const r32 MATH_LOG_TWO = 0.693147180559945309417232121458176568f;
static const r32 MATH_LOG_TEN = 2.30258509299404568401799145468436421f;

union vec2
{
    struct { r32 x, y; };
    struct { r32 u, v; };
    struct { r32 width, height; };
    r32 e[2];
};
typedef union vec2 v2;

union vec3
{
    struct { r32 x, y, z; };
    struct { r32 r, g, b; };
    struct { r32 width, height, depth; };
    v2 xy;
    r32 e[3];
};
typedef union vec3 v3;

union vec4
{
    struct { r32 x, y, z, w; };
    struct { r32 r, g, b, a; };
    struct { r32 top, right, bottom, left; };
    struct { v2 xy, zw; };
    v3 xyz;
    v3 rgb;
    r32 e[4];
};
typedef union vec4 v4;

// clang-format on

v2 vec2(r32 x, r32 y);
v3 vec3(r32 x, r32 y, r32 z);
v4 vec4(r32 x, r32 y, r32 z, r32 w);

v2 vec2_zero();
v3 vec3_zero();
v4 vec4_zero();

v2 vec2_add(v2 a, v2 b);
v2 vec2_sub(v2 a, v2 b);
v2 vec2_mul(v2 v, r32 n);
v2 vec2_div(v2 v, r32 n);

v3 vec3_add(v3 a, v3 b);
v3 vec3_sub(v3 a, v3 b);
v3 vec3_mul(v3 v, r32 n);
v3 vec3_div(v3 v, r32 n);

v4 vec4_add(v4 a, v4 b);
v4 vec4_sub(v4 a, v4 b);
v4 vec4_mul(v4 v, r32 n);
v4 vec4_div(v4 v, r32 n);

v2 vec2_norm(v2 v);
v3 vec3_norm(v3 v);
v4 vec4_norm(v4 v);

v2 vec2_rotate(v2 v, r32 theta);

r32 vec2_dot(v2 a, v2 b);
r32 vec3_dot(v3 a, v3 b);
r32 vec4_dot(v4 a, v4 b);

r32 vec2_mag(v2 v);
r32 vec3_mag(v3 v);
r32 vec4_mag(v4 v);

// clang-format off
union mat2
{
    struct { v2 x, y; };
    v2 col[2];
    r32 m[2][2];
    r32 e[4];

};
typedef union mat2 m2;

union mat3
{
    struct { v3 x, y, z; };
    v3 col[3];
    r32 m[3][3];
    r32 e[9];

};
typedef union mat3 m3;

union mat4
{
    struct { v4 x, y, z, w; };
    v4 col[4];
    r32 m[4][4];
    r32 e[16];
};
typedef union mat4 m4;

// clang-format on

v2 mat2_mul_vec2(m2 m, v2 v);
v3 mat3_mul_vec3(m3 m, v3 v);
v4 mat4_mul_vec4(m4 m, v4 v);

m2 mat2_zero();
m3 mat3_zero();
m4 mat4_zero();

m2 mat2(r32 d);
m3 mat3(r32 d);
m4 mat4(r32 d);

m2 mat2_transpose(m2 m);
m3 mat3_transpose(m3 m);
m4 mat4_transpose(m4 m);

m4 mat4_mul(m4 a, m4 b);

m4 mat4_translate(v3 v);
m4 mat4_scale(v3 v);
m4 mat4_rotate(v3 axis, r32 angle_radians);

m4 mat4_ortho(r32 left, r32 right, r32 bottom, r32 top, r32 near, r32 far);

m4 mat4_perspective(r32 fovy, r32 aspect, r32 near, r32 far);

#endif