// Callee seams of D2Common 0x84220 (see d2common_84220.cpp).
#pragma once
#include <cstdint>
struct D2c84220Env {
	void (*f67fd0)(uint32_t ecx, uint32_t edi, uint32_t esi);                                   // 0x67FD0, regs only
	uint32_t (*f68b90)(uint32_t eax, uint32_t ecx, int n);                                      // 0x68B90, eax + ecx + one stack word (ret 4)
	void (*f68cd0)(uint32_t ecx, int edx, int z, int y, uint32_t v);                           // 0x68CD0, ecx, edx + three stack words (ret 0xC)
	void (*f83630)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2);   // 0x83630, eax ecx edx + three stack words (ret 0xC)
	void (*store)(uint32_t dst, uint32_t val);                                                  // PTR_FUN_6FDEB250 = 0x36C20: *dst = val (ecx dst, edx val)
};
extern "C" void d2c_84220_run(uint32_t p1, int p2, int p3, uint32_t p4, uint32_t p5, uint32_t p6, int p7, const D2c84220Env* env);
