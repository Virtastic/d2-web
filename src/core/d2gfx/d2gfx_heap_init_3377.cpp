// D2Gfx 1.13c VC7.1 CRT heap startup. The host callbacks model the Windows heap APIs and
// the two retail helpers; all decisions and argument values follow the retail entry point.
#include <cstdint>
#include <emscripten.h>

extern "C" uint32_t d2gfx_heap_select(uint32_t platform, uint32_t version);

struct D2GfxHeapInitEnv {
    void* (*heap_create)(uint32_t options, uint32_t initial_commit, uint32_t maximum_size);
    int32_t (*heap_destroy)(void* heap);
    int32_t (*sbh_heap_init)(uint32_t threshold);
    uint32_t os_platform;
    uint32_t os_version;
    void* heap;
    uint32_t heap_mode;
};

// 1.13c D2Gfx+0x03377 (__heap_init): create the process heap, select the OS or small-block heap,
// and initialize the small-block regions when required. Failure destroys the just-created heap.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2gfx_heap_init_3377(int32_t mtflag,
                                                              D2GfxHeapInitEnv* env) {
    env->heap = env->heap_create(mtflag == 0 ? 1u : 0u, 0x1000, 0);
    if (env->heap == nullptr) return 0;
    env->heap_mode = d2gfx_heap_select(env->os_platform, env->os_version);
    if (env->heap_mode == 3 && env->sbh_heap_init(0x3f8) == 0) {
        env->heap_destroy(env->heap);
        return 0;
    }
    return 1;
}
