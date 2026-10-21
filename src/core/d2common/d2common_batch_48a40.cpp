// D2Common 1.13c batch. Callees are passed in an env struct (46E50 / 475E0 / 1180).
#include <cstdint>
#include <emscripten.h>

struct D2Batch48A40Env {
	void (*check46e50)(const int32_t* unit);                                                  // 0x46E50
	int32_t (*fn475e0)(void* p2, const int32_t* unit, int32_t p5, int16_t p6, int32_t zero,
	                   int32_t v, int32_t p9, uint32_t p10);                                  // 0x475E0
	int32_t (*roll1180)(const uint32_t* seed, int32_t nMin, int32_t nMax);                    // 0x1180
};

extern "C" {
// 1.13c D2Common+0x48A40
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_48a40(void* p2, const int32_t* u, const int32_t* p4, int32_t p5, int16_t p6,
                                                  int32_t p9, uint32_t p10, const D2Batch48A40Env* e) {
	if (!p4) return 0;
	if (u && u[0] == 4) e->check46e50(u);
	int32_t v = p4[2];
	if (p4[3] != v) {
		const uint32_t* seed;
		if (!u || u[0] != 4) seed = reinterpret_cast<const uint32_t*>(u + 8);
		else if (u[5] == 0) seed = nullptr;
		else seed = reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(u[5]) + 4);
		v = e->roll1180(seed, v, p4[3]);
	}
	return e->fn475e0(p2, u, p5, p6, 0, v, p9, p10);
}
}
