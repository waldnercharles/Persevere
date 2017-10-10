#pragma once

#include "nerd.h"
#include "nerd_typedefs.h"

bool math_is_pow2(uint n);
int math_floor_log2(uint n);
int math_ceil_log2(uint n);

float math_radians(float degrees);

#define math_min(a, b) ((a) < (b) ? (a) : (b))
#define math_max(a, b) ((a) > (b) ? (a) : (b))
#define math_min3(a, b, c) math_min(math_min(a, b), c)
#define math_max3(a, b, c) math_max(math_max(a, b), c)
#define math_clamp(x, lower, upper) (math_min(math_max(x, (lower)), (upper)))
#define math_abs(a) ((a) > 0 ? (a) : -(a))
#define math_pow2(a) ((a) * (a))
#define math_pow3(a) ((a) * (a) * (a))

#define math_epsilon 1.19209290e-7f
#define math_zero 0.0f
#define math_one 1.0f
#define math_two_thirds 0.666666666666666666666666666666666666667f

#define math_tau 6.28318530717958647692528676655900576f
#define math_pi 3.14159265358979323846264338327950288f
#define math_one_over_tau 0.636619772367581343075535053490057448f
#define math_one_over_pi 0.159154943091895335768883763372514362f

#define math_tau_over_2 3.14159265358979323846264338327950288f
#define math_tau_over_4 1.570796326794896619231321691639751442f
#define math_tau_over_8 0.785398163397448309615660845819875721f

#define math_e 2.71828182845904523536f
#define math_sqrt_two 1.41421356237309504880168872420969808f
#define math_sqrt_three 1.73205080756887729352744634150587236f
#define math_sqrt_five 2.23606797749978969640917366873127623f

#define math_log_two 0.693147180559945309417232121458176568f
#define math_log_ten 2.30258509299404568401799145468436421f

// clang-format off
union vec2
{
    struct { float x, y; };
    float e[2];
};

union vec3
{
    struct { float x, y, z; };
    struct { float r, g, b; };
    union vec2 xy;
    float e[3];
};

union vec4
{
    struct { float x, y, z, w; };
    struct { float r, g, b, a; };
    struct { union vec2 xy, zw; };
    union vec3 xyz;
    union vec3 rgb;
    float e[4];
};
// clang-format on

union vec2 vec2(float x, float y);
union vec3 vec3(float x, float y, float z);
union vec4 vec4(float x, float y, float z, float w);

union vec2 vec2_zero();
union vec3 vec3_zero();
union vec4 vec4_zero();

union vec2 vec2_add(union vec2 a, union vec2 b);
union vec2 vec2_sub(union vec2 a, union vec2 b);
union vec2 vec2_mul(union vec2 v, float n);
union vec2 vec2_div(union vec2 v, float n);

union vec3 vec3_add(union vec3 a, union vec3 b);
union vec3 vec3_sub(union vec3 a, union vec3 b);
union vec3 vec3_mul(union vec3 v, float n);
union vec3 vec3_div(union vec3 v, float n);

union vec4 vec4_add(union vec4 a, union vec4 b);
union vec4 vec4_sub(union vec4 a, union vec4 b);
union vec4 vec4_mul(union vec4 v, float n);
union vec4 vec4_div(union vec4 v, float n);

union vec2 vec2_norm(union vec2 v);
union vec3 vec3_norm(union vec3 v);
union vec4 vec4_norm(union vec4 v);

float vec2_dot(union vec2 a, union vec2 b);
float vec3_dot(union vec3 a, union vec3 b);
float vec4_dot(union vec4 a, union vec4 b);

float vec2_mag(union vec2 v);
float vec3_mag(union vec3 v);
float vec4_mag(union vec4 v);

// clang-format off
union mat2
{
    struct { union vec2 x, y; };
    union vec2 col[2];
    float m[2][2];
    float e[4];

};

union mat3
{
    struct { union vec3 x, y, z; };
    union vec3 col[3];
    float m[3][3];
    float e[9];

};

union mat4
{
    struct { union vec4 x, y, z, w };
    union vec4 col[4];
    float m[4][4];
    float e[16];
};

// clang-fromat on

union vec2 mat2_mul_vec2(union mat2 m, union vec2 v);
union vec3 mat3_mul_vec3(union mat3 m, union vec3 v);
union vec4 mat4_mul_vec4(union mat4 m, union vec4 v);

union mat2 mat2_zero();
union mat3 mat3_zero();
union mat4 mat4_zero();

union mat2 mat2(float d);
union mat3 mat3(float d);
union mat4 mat4(float d);

union mat2 mat2_transpose(union mat2 m);
union mat3 mat3_transpose(union mat3 m);
union mat4 mat4_transpose(union mat4 m);

union mat4 mat4_mul(union mat4 a, union mat4 b);

union mat4 mat4_translate(union vec3 v);
union mat4 mat4_scale(union vec3 v);
union mat4 mat4_rotate(union vec3 axis, float angle_radians);

union mat4 mat4_ortho(float left,
                      float right,
                      float bottom,
                      float top,
                      float near,
                      float far);

union mat4 mat4_perspective(float fovy, float aspect, float near, float far);
