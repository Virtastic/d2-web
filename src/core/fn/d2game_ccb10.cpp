// d2game+0xccb10 sub_CCB10: probe. thiscall this = record blob, then game, param_2. Only the guard paths are vectored (the
// 0xE03A0 lookup needs a live game), so the env seam is never reached and is passed null.
#include <emscripten.h>
#include <cstdint>
struct D2GameBatchCc75Env;
extern "C" uint32_t d2_fn_d2game_CCB10(void* thisp, void* game, const void* param_2, const D2GameBatchCc75Env* env);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ccb10(uint32_t* r) {
    r[4] = d2_fn_d2game_CCB10((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], (const void*)(uintptr_t)r[2], nullptr);
}
