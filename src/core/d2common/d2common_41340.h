// 1.13c D2Common 0x41340 (see d2common_41340.cpp).
#pragma once
#include <cstdint>
// 0x42A70 tile stamp: ECX = level, EDX = x, stack y, preset, -1, 0 (ret 0x10).
struct D2Common41340Env { void (*stamp)(uint32_t pLevel, int32_t x, int32_t y, int32_t preset, int32_t a, int32_t b); };
extern "C" void D2Common_41340_Port(uint32_t pLevel, const D2Common41340Env* env);
