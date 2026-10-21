// D2Common 1.13c +0x810A0: room tile fetch (same 4-byte-cell style as d2common_batch_69170.cpp).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x810A0: (ecx=y, edx=room, a3=x). Room+0x64 is the collision grid (null = fatal 0x29c,
// _exit(-1), via fatal seam). If its flags bit0 is set the grid is one shared cell at +0x30; else
// cell = cells(+0x1c)[ rowOffsets(+0x20)[y - room.y(+0x38)] - room.x(+0x34) + x ], 4 bytes each.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_810a0(int32_t y, uintptr_t room, int32_t x, void (*fatal)()) {
	auto w = [](uintptr_t a, int o) { return *reinterpret_cast<uint32_t*>(a + o); };
	uintptr_t g = w(room, 0x64);
	if (!g) { fatal(); return 0; }
	if (*reinterpret_cast<uint8_t*>(g) & 1) return w(g, 0x30);
	uint32_t row = w(w(g, 0x20), (uint32_t)(y - (int32_t)w(room, 0x38)) * 4);
	return w(w(g, 0x1c), ((int32_t)(row - w(room, 0x34)) + x) * 4);
}
}
