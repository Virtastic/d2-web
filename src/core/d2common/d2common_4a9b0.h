// D2Common 0x4A9B0: outdoor room tile setup. Callees arrive through the Env seams.
#pragma once
#include <cstdint>
struct D2Common4a9b0Env {
	uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line);        // 0x9222 (Fog 10045), ECX pool, EDX size
	void (*countWarp)(uint32_t room, uint32_t wall, uint32_t type, uint32_t edx, uint32_t s0);   // 0x685D0
	uint32_t (*countTypes)(uint32_t eax, uint32_t grid);                    // 0x68660 (the three flags are 0); its EAX feeds the next call
	void (*allocData)(uint32_t eax);                                        // 0x68040: EAX = the second 0x68660's return
	void (*loadTiles)(uint32_t room, uint32_t grid, uint32_t type);         // 0x69B50 (the three flags are 0)
	void (*coordLists)(uint32_t room);                                      // 0x81100
};
extern "C" void D2Common_4A9B0_With(uint32_t room, const D2Common4a9b0Env* e);
