#pragma once
#include <cstdint>
#include "storm_big_types.h"

// Retail 1.13c Storm2fVector (x86-32 layout): capacity@0, count@4, data@8 (4 bytes), growth@12, offset@16.
// A probe receives the retail struct address; this copies it into the native wasm64 layout before the call.
inline Storm2fVector Storm2fVectorFromRetail(uint32_t addr) {
    const uint32_t* w = (const uint32_t*)(uintptr_t)addr;
    Storm2fVector v{};
    v.capacity = w[0];
    v.count = w[1];
    v.data = (uint32_t*)(uintptr_t)w[2];
    v.growth = w[3];
    v.offset = w[4];
    return v;
}
