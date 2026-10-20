#pragma once
#include <cstdint>
#include <cstddef>

// Live bigint storage shared by the Storm SBig entry points and limb helpers.
// Retail offsets 0/4/8/12/16 describe x86 fields; the data pointer widens natively.
struct Storm2fVector {
    uint32_t capacity, count;
    uint32_t* data;
    uint32_t growth, offset;
};
struct Storm2fScratch { Storm2fVector slots[16]; uint32_t used; };
struct Storm2fBytes { uint32_t capacity, count; uint8_t* data; uint32_t growth; };
struct Storm2fBig { Storm2fVector value; Storm2fScratch scratch; Storm2fBytes bytes; };

// Scalar retail fields retain their widths; only live pointers widen.
static_assert(sizeof(uint32_t) == 4);
static_assert(offsetof(Storm2fVector, capacity) == 0);
static_assert(offsetof(Storm2fVector, count) == 4);
static_assert(offsetof(Storm2fVector, data) == 8);
static_assert(offsetof(Storm2fVector, growth) == 8 + sizeof(uint32_t*));
static_assert(offsetof(Storm2fVector, offset) == 12 + sizeof(uint32_t*));

uint32_t Storm_2AD10(const Storm2fVector*);
uint32_t Storm_2ADC0(const Storm2fVector*);
int32_t Storm_2AE30(const Storm2fVector*, const Storm2fVector*);
extern "C" void Storm_Sub25F30(uint32_t, void*, uint32_t, const uint32_t*);
