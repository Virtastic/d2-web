// Callee seams of D2Common 0x84890 (see d2common_84890.cpp).
#pragma once
#include <cstdint>
struct D2c84890Env {
	int (*match1)(int x, int y, void* ctx, void* ent, void* param2);                // 0x834D0, five stack words (ret 0x14)
	int (*match2)(int y, int x, void* ctx, void* ent, void* param2);                // 0x837F0, EAX = y, EDI = param2, stack x, ctx, ent (ret 0xC)
	void (*place)(uint32_t v, int x, int y, void* ctx, void* ent, void* param2, int extra);   // 0x84220, seven stack words (ret 0x1C)
};
extern "C" void d2c_84890_run(void* ctx, void* param2, const D2c84890Env* env);
