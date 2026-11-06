// Callee seams of D2Common 0x84AD0 (see d2common_84ad0.cpp).
#pragma once
#include <cstdint>
struct D2c84AD0Env {
	void* (*entries)(int32_t idx);                // 0x6C4D0, EDX = idx
	void (*prep)(void* ctx, void* ent);           // 0x83CA0, ECX = ctx, stack ent (ret 4)
	void (*flat)(void* self, void* ent);          // 0x84500, stack self, ent (ret 8)
	void (*scatter)(void* self, void* ent);       // 0x84890, stack self, ent (ret 8)
};
extern "C" void d2c_84ad0_run(int32_t* self, const D2c84AD0Env* e);
