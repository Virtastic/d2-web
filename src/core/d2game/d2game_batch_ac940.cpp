// src/core/d2game/d2game_batch_ac940.cpp -- one 1.13c D2Game.dll function. Pure arithmetic over a caller-built
// array: no callees, no globals, no register inputs.

#include <cstdint>

// 1.13c D2Game+0xAC940  (ecx=array a, edx=count n; returns the tiered weighted total)
// Tiers split at n = 15, 30, 45, 60 (signed compares, as the decompile). Arithmetic wraps as 32-bit retail does.
// Spec deltas: none.
int32_t D2GAME_AC940(const int32_t* a, int32_t n)
{
    if (n < 2 || a == nullptr) return 0;
    uint32_t r;
    if (n > 60)
        r = (uint32_t)(a[3] + a[2] + a[1]) * 15u + (uint32_t)(n - 60) * (uint32_t)a[4] + (uint32_t)a[0] * 14u;
    else if (n > 45)
        r = (uint32_t)(a[2] + a[1]) * 15u + (uint32_t)(n - 45) * (uint32_t)a[3] + (uint32_t)a[0] * 14u;
    else if (n > 30)
        r = (uint32_t)(n - 30) * (uint32_t)a[2] + (uint32_t)a[1] * 15u + (uint32_t)a[0] * 14u;
    else if (n > 15)
        r = (uint32_t)(n - 15) * (uint32_t)a[1] + (uint32_t)a[0] * 14u;
    else
        r = (uint32_t)(n - 1) * (uint32_t)a[0];
    return (int32_t)r;
}
