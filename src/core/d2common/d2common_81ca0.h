// 1.13c D2Common+0x81CA0: DRLG logic-grid builder for one level. Retail 32-bit layout; host addresses ride as uint32_t.
#pragma once
#include <cstdint>

struct D2c81CA0Env {
	// Fog alloc thunk 0x9222: EDX = size, ECX = ctx (the caller's scratch word), stack file/line/0. The file name is a fixed module string.
	uint32_t (*alloc)(uint32_t ctx, uint32_t size, uint32_t line);
	void (*c81650)(uint32_t level, uint32_t p4);                          // EAX = level, stack p4
	void (*c36950)(uint32_t ctx, uint32_t blk8, int32_t w, int32_t h);    // ECX = ctx, EBX = block + 8, stack w + 1, h + 1
	void (*c814c0)(uint32_t level, const uint32_t* grid);                  // EDI = level, stack &grid (5 dwords)
	void (*c81bc0)(uint32_t* req);                                         // ESI = request (8 dwords), req[6] updated by the callee
	void (*c81200)(uint32_t level, uint32_t block);
	void (*c81a20)(uint32_t level);
	void (*c81ad0)(uint32_t level);                                        // ESI = level
};
extern "C" void d2c_81ca0_run(uint32_t level, uint32_t p2, uint32_t p3, uint32_t p4, const D2c81CA0Env* env);
