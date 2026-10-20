// Prior art used: RE map; D2MOO has no CRT heap_init implementation.
// Cross-checked the equivalent VC7.1 startup in d2gfx_heap_init_3377.cpp against the cached
// D2Game 1.13c decompile. Windows heap calls and the SBH initializer are native host seams.
#include "d2game_heap_init.h"
#include <emscripten.h>

extern "C" int32_t d2_fn_d2game_6730(uint32_t os_platform, uint32_t os_major);

// 1.13c D2Game+0x0674A (__heap_init): publish the newly created heap before selecting its mode.
// A failed create leaves the old mode unchanged. Failed SBH setup destroys the heap but retail
// retains both global values, including the now-destroyed handle, for the caller's failure path.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_heap_init_674a(int32_t mtflag,
                                                          D2GameHeapInitEnv* env) {
    env->heap = env->heap_create(mtflag == 0 ? 1u : 0u, 0x1000u, 0u);
    if (env->heap == nullptr) return 0;
    env->heap_mode = static_cast<uint32_t>(d2_fn_d2game_6730(env->os_platform, env->os_major));
    if (env->heap_mode == 3 &&
        (env->sbh_heap_init ? env->sbh_heap_init(0x3f8u) :
         d2game_sbh_heap_init_8bd5(0x3f8u, env->heap, &env->sbh, env->heap_alloc)) == 0) {
        env->heap_destroy(env->heap);
        return 0;
    }
    return 1;
}
