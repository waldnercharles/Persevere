#include "nerd_math.h"

bool math_is_pow2(uint n)
{
    return n && !(n & (n - 1));
}

int math_floor_log2(uint n)
{
    static char log2_4[16] = { -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3 };
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

int math_ceil_log2(uint n)
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
