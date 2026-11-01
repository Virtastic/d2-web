// 1.13c D2Common 0x17180 (see d2common_17180.cpp).
#pragma once
#include <cstdint>
// 0x42A70 tile stamp: ECX = level, EDX = x, stack y, tile, -1, 0 (ret 0x10).
struct D2Common17180Env { void (*stamp)(uint32_t pLevel, int32_t x, int32_t y, int32_t tile, int32_t a, int32_t b); };
extern "C" void D2Common_17180_DrawOutdoorBorder(uint32_t pLevel, const D2Common17180Env* env);
