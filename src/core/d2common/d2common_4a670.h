// D2Common 0x4A670: EAX = room, no stack, plain ret. Callees arrive through Env with the registers retail loads.
#pragma once
#include <cstdint>
struct D2Common4A670Env {
	uint32_t dataTables;                                        // *0x99E1C (count of level rows at +0xC5C)
	uint32_t levels;                                            // *0xA0B38 (level rows, stride 0x9C)
	void (*grid)(uint32_t ecx, uint32_t ebx, uint32_t w, uint32_t h);   // 0x36950: ECX = pool, EBX = grid, stdcall (w, h)
	void (*set)(uint32_t ecx, uint32_t edx);                    // *0x9B250 (0x36C20): ECX = cell, EDX = value
	void (*orr)(uint32_t ecx, uint32_t edx);                    // *0x9B244 (0x36C10): ECX = cell, EDX = flags
	void (*edge)(uint32_t eax, uint32_t ecx);                   // 0x3DC90: EAX = room, ECX = level
	void (*fill)(uint32_t room);                                // 0x3DB20: stdcall 1
	uint32_t (*alloc)(uint32_t ecx, uint32_t edx, uint32_t file, uint32_t line, uint32_t z);   // 0x9222 (Fog pool alloc)
	void (*tiles)(const uint32_t* w);                           // 0x84AD0: EDI = the 15-dword work block
	uint32_t (*release)(uint32_t esi);                          // 0x36A60: ESI = grid
};
extern "C" uint32_t D2Common_4A670_With(uint32_t room, const D2Common4A670Env* e);
