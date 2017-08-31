#pragma once

#include "nerd.h"
#include "nerd_typedefs.h"

bool math_is_pow2(uint n);
int math_floor_log2(uint n);
int math_ceil_log2(uint n);

#define math_clamp(x, a, b) ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))
#define math_min(a, b) ((a) < (b) ? (a) : (b))
#define math_max(a, b) ((a) > (b) ? (a) : (b))