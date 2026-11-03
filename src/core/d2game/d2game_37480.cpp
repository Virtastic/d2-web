// D2Game+0x37480 (ACT5Q5_OnPortalOpened): EAX = object, stack word 0 = value (ret 4). EAX == 0 is fatal 0x23F; rec = Ordinal 10920(EAX) (thunk 0xA4C0);
// a null rec is fatal 0x242; else rec[0x48] = value. The fatal path formats via Ordinal 10265, reports via 10024 and _exit(-1).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game37480Env {
    uint32_t (*getRec)(uint32_t obj);   // Ordinal 10920
    void     (*fatal)(uint32_t line);   // never returns
};

// 1.13c D2Game+0x37480
extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_37480(uint32_t obj, uint32_t value, const D2Game37480Env* e)
{
    if (!obj) e->fatal(0x23f);
    const uint32_t rec = e->getRec(obj);
    if (!rec) e->fatal(0x242);
    std::memcpy((void*)(uintptr_t)(rec + 0x48), &value, 4);
}
