// Callee seams of D2Common 0x83E20 (see d2common_83e20.cpp).
#pragma once
#include <cstdint>
struct D2c83E20Env {
	int (*f83b30)(uint32_t esi, int x, int y, uint32_t p3, uint32_t p2);                                   // 0x83B30, ESI = p1, four stack words (ret 0x10)
	void (*f83990)(uint32_t ecx, uint32_t edx, uint32_t eax, int x, int y, uint32_t p1, uint32_t p3, uint32_t p2);   // 0x83990, ecx = r+1, edx = (w+1)*(r+1), eax = x, five stack words
};
extern "C" uint32_t d2c_83e20_run(uint32_t p1, uint32_t p2, uint32_t p3, const D2c83E20Env* env);
