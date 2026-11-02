// d2game+0xad860 sub_AD860: probe. thiscall r[0] = this, r[1] = unit, r[2] = source blob (flags at +0xC4), r[3] = unused, r[5] = packed skill word.
// Only the guard paths are vectored (the passing path needs a live unit and the 0xDE0C0 / 0xDCF20 callees), so the env seam is never reached and is null.
#include <emscripten.h>
#include <cstdint>
struct D2GameBatchAD860Env;
extern "C" uint32_t d2_fn_d2game_AD860(void* self, void* unit, uint32_t p2, uint32_t p3, uint32_t p4, const D2GameBatchAD860Env* env);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ad860(uint32_t* r) {
    r[4] = d2_fn_d2game_AD860((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], r[2], r[3], r[5], nullptr);
}
