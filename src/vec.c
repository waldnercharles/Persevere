#include "vec.h"

// clang-format off
#define vec2_op(v, eq, v0, op, v1)                                             \
    v.x eq v0.x op v1.x,                                                       \
    v.y eq v0.y op v1.y

#define vec2_op_scalar(v, eq, v0, op, n)                                       \
    v.x eq v0.x op n,                                                          \
    v.y eq v0.y op n

#define vec3_op(v, eq, v0, op, v1)                                             \
    v.x eq v0.x op v1.x,                                                       \
    v.y eq v0.y op v1.y,                                                       \
    v.z eq v0.z op v1.z

#define vec3_op_scalar(v, eq, v0, op, n)                                       \
    v.x eq v0.x op n,                                                          \
    v.y eq v0.y op n,                                                          \
    v.z eq v0.z op n

#define vec4_op(v, eq, v0, op, v1)                                             \
    v.x eq v0.x op v1.x,                                                       \
    v.y eq v0.y op v1.y,                                                       \
    v.z eq v0.z op v1.z,                                                       \
    v.w eq v0.w op v1.w

#define vec4_op_scalar(v, eq, v0, op, n)                                       \
    v.x eq v0.x op n,                                                          \
    v.y eq v0.y op n,                                                          \
    v.z eq v0.z op n,                                                          \
    v.w eq v0.w op n
// clang-format on

v2
vec2(r32 x, r32 y)
{
    v2 v = { x, y };
    return v;
}

v3
vec3(r32 x, r32 y, r32 z)
{
    v3 v = { x, y, z };
    return v;
}

v4
vec4(r32 x, r32 y, r32 z, r32 w)
{
    v4 v = { x, y, z, w };
    return v;
}

v2
vec2_zero()
{
    return vec2(0, 0);
}

v3
vec3_zero()
{
    return vec3(0, 0, 0);
}

v4
vec4_zero()
{
    return vec4(0, 0, 0, 0);
}

v2
vec2_add(v2 a, v2 b)
{
    v2 r;
    vec2_op(r, =, a, +, b);
    return r;
}

v2
vec2_sub(v2 a, v2 b)
{
    v2 r;
    vec2_op(r, =, a, -, b);
    return r;
}

v2
vec2_mul(v2 v, r32 n)
{
    v2 r;
    vec2_op_scalar(r, =, v, *, n);
    return r;
}

v2
vec2_div(v2 v, r32 n)
{
    v2 r;
    vec2_op_scalar(r, =, v, /, n);
    return r;
}

v3
vec3_add(v3 a, v3 b)
{
    v3 r;
    vec3_op(r, =, a, +, b);
    return r;
};

v3
vec3_sub(v3 a, v3 b)
{
    v3 r;
    vec3_op(r, =, a, -, b);
    return r;
};

v3
vec3_mul(v3 v, r32 n)
{
    v3 r;
    vec3_op_scalar(r, =, v, *, n);
    return r;
};

v3
vec3_div(v3 v, r32 n)
{
    v3 r;
    vec3_op_scalar(r, =, v, /, n);
    return r;
};

v4
vec4_add(v4 a, v4 b)
{
    v4 r;
    vec4_op(r, =, a, +, b);
    return r;
};

v4
vec4_sub(v4 a, v4 b)
{
    v4 r;
    vec4_op(r, =, a, -, b);
    return r;
};

v4
vec4_mul(v4 v, r32 n)
{
    v4 r;
    vec4_op_scalar(r, =, v, *, n);
    return r;
};

v4
vec4_div(v4 v, r32 n)
{
    v4 r;
    vec4_op_scalar(r, =, v, /, n);
    return r;
};

#undef vec2_op
#undef vec3_op
#undef vec4_op

v2
vec2_norm(v2 v)
{
    return vec2_div(v, vec2_mag(v));
}

v3
vec3_norm(v3 v)
{
    return vec3_div(v, vec3_mag(v));
}

v4
vec4_norm(v4 v)
{
    return vec4_div(v, vec4_mag(v));
}

r32
vec2_dot(v2 a, v2 b)
{
    return a.x * b.x + a.y * b.y;
}

r32
vec3_dot(v3 a, v3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

r32
vec4_dot(v4 a, v4 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

r32
vec2_mag(v2 v)
{
    return sqrtf(vec2_dot(v, v));
}

r32
vec3_mag(v3 v)
{
    return sqrtf(vec3_dot(v, v));
};

r32
vec4_mag(v4 v)
{
    return sqrtf(vec4_dot(v, v));
};

v2
vec2_rotate(v2 v, r32 angle_radians)
{
    v2 r;
    r32 c = cosf(angle_radians);
    r32 s = sinf(angle_radians);

    r.x = v.x * c - v.y * s;
    r.y = v.x * s + v.y * c;

    return r;
}

v2
mat2_mul_vec2(m2 m, v2 v)
{
    v2 r;
    r.x = m.m[0][0] * v.x + m.m[1][0] * v.y;
    r.y = m.m[0][1] * v.x + m.m[1][1] * v.y;
    return r;
}

v3
mat3_mul_vec3(m3 m, v3 v)
{
    v3 r;
    r.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z;
    r.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z;
    r.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z;
    return r;
}

v4
mat4_mul_vec4(m4 m, v4 v)
{
    v4 r;
    r.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0] * v.w;
    r.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1] * v.w;
    r.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2] * v.w;
    r.w = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3] * v.w;
    return r;
}

m2
mat2_zero()
{
    m2 m;
    memset(&m, 0, sizeof(m2));
    return m;
}

m3
mat3_zero()
{
    m3 m;
    memset(&m, 0, sizeof(m3));
    return m;
}

m4
mat4_zero()
{
    m4 m;
    memset(&m, 0, sizeof(m4));
    return m;
}

m2
mat2(r32 d)
{
    m2 m = mat2_zero();
    m.m[0][0] = d;
    m.m[1][1] = d;
    return m;
}

m3
mat3(r32 d)
{
    m3 m = mat3_zero();
    m.m[0][0] = d;
    m.m[1][1] = d;
    m.m[2][2] = d;
    return m;
}

m4
mat4(r32 d)
{
    m4 m = mat4_zero();
    m.m[0][0] = d;
    m.m[1][1] = d;
    m.m[2][2] = d;
    m.m[3][3] = d;
    return m;
}

m2
mat2_transpose(m2 m)
{
    m2 r;
    r.m[0][1] = m.m[1][0];
    r.m[1][0] = m.m[0][1];
    return r;
}

m3
mat3_transpose(m3 m)
{
    m3 r;
    r.m[0][1] = m.m[1][0];
    r.m[0][2] = m.m[2][0];
    r.m[1][0] = m.m[0][1];
    r.m[1][2] = m.m[2][1];
    r.m[2][0] = m.m[0][2];
    r.m[2][1] = m.m[1][2];
    return r;
}

m4
mat4_transpose(m4 m)
{
    m4 r;
    r.m[0][1] = m.m[1][0];
    r.m[0][2] = m.m[2][0];
    r.m[0][3] = m.m[3][0];
    r.m[1][0] = m.m[0][1];
    r.m[1][2] = m.m[2][1];
    r.m[1][3] = m.m[3][1];
    r.m[2][0] = m.m[0][2];
    r.m[2][1] = m.m[1][2];
    r.m[2][3] = m.m[3][2];
    r.m[3][0] = m.m[0][3];
    r.m[3][1] = m.m[1][3];
    r.m[3][2] = m.m[2][3];
    return r;
}

m4
mat4_mul(m4 a, m4 b)
{
    m4 m;
    u32 col, row;
    for (col = 0; col < 4; ++col)
    {
        for (row = 0; row < 4; ++row)
        {
            m.m[col][row] =
                a.m[0][row] * b.m[col][0] + a.m[1][row] * b.m[col][1] +
                a.m[2][row] * b.m[col][2] + a.m[3][row] * b.m[col][3];
        }
    }
    return m;
}

m4
mat4_translate(v3 v)
{
    m4 m = mat4(1.0f);
    m.col[3].xyz = vec3_add(m.col[3].xyz, v);
    return m;
}

m4
mat4_scale(v3 v)
{
    m4 m = mat4(1.0f);

    m.m[0][0] = v.x;
    m.m[1][1] = v.y;
    m.m[2][2] = v.z;

    return m;
}

m4
mat4_rotate(v3 axis, r32 angle_radians)
{
    m4 m = mat4(1.0f);

    r32 s = sinf(angle_radians);
    r32 c = cosf(angle_radians);
    r32 n = 1.0f - c;

    axis = vec3_norm(axis);

    m.m[0][0] = (axis.x * axis.x * n) + c;
    m.m[0][1] = (axis.x * axis.y * n) + (axis.z * s);
    m.m[0][2] = (axis.x * axis.z * n) - (axis.y * s);

    m.m[1][0] = (axis.y * axis.x * n) - (axis.z * s);
    m.m[1][1] = (axis.y * axis.y * n) + c;
    m.m[1][2] = (axis.y * axis.z * n) + (axis.x * s);

    m.m[2][0] = (axis.z * axis.x * n) + (axis.y * s);
    m.m[2][1] = (axis.z * axis.y * n) - (axis.x * s);
    m.m[2][2] = (axis.z * axis.z * n) + c;

    return m;
}

m4
mat4_ortho(r32 left, r32 right, r32 bottom, r32 top, r32 near, r32 far)
{
    m4 m = mat4(1.0f);

    m.m[0][0] = 2.0f / (right - left);
    m.m[1][1] = 2.0f / (top - bottom);
    m.m[2][2] = 2.0f / (near - far);

    m.m[3][0] = (left + right) / (left - right);
    m.m[3][1] = (bottom + top) / (bottom - top);
    m.m[3][2] = (far + near) / (near - far);

    return m;
}

m4
mat4_perspective(r32 fov_radians, r32 aspect, r32 near, r32 far)
{
    m4 m = mat4_zero();

    r32 tan_half_fov = tanh(fov_radians / 2.0f);

    m.m[0][0] = 1.0f / (aspect * tan_half_fov);
    m.m[1][1] = 1.0f / (tan_half_fov);
    m.m[2][2] = -(far + near) / (far - near);
    m.m[2][3] = -1.0f;
    m.m[3][2] = -(2.0f * far * near) / (far - near);

    return m;
}

r32
math_radians(r32 degrees)
{
    return degrees * (math_pi / 180.0f);
}

bool
math_is_pow2(u32 n)
{
    return n && !(n & (n - 1));
}

s32
math_floor_log2(u32 n)
{
    static s8 log2_4[16] = { -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3 };

    if (n < (1U << 14))
    {
        if (n < (1U << 4))
        {
            return 0 + log2_4[n];
        }
        else if (n < (1U << 9))
        {
            return 5 + log2_4[n >> 5];
        }
        else
        {
            return 10 + log2_4[n >> 10];
        }
    }
    if (n < (1U << 24))
    {
        if (n < (1U << 19))
        {
            return 15 + log2_4[n >> 15];
        }
        else
        {
            return 20 + log2_4[n >> 20];
        }
    }
    if (n < (1U << 29))
    {
        return 25 + log2_4[n >> 25];
    }
    else
    {
        return 30 + log2_4[n >> 30];
    }
}

s32
math_ceil_log2(u32 n)
{
    if (math_is_pow2(n))
    {
        return math_floor_log2(n);
    }
    else
    {
        return math_floor_log2(n) + 1;
    }
}
