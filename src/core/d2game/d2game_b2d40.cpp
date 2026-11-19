// 1.13c D2Game+0xB2D40 (seam table D2GameBatchBd18Env, see d2game_batch_d1890_env.h). Probe: src/core/fn/d2game_b2d40.cpp.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>
#include "d2game_batch_d1890_env.h"

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }

// Unit-mode teardown: 0xB2C20(in_EAX, ESI, 1), fatal 0xFB when ESI is null, then 0xB2760 unless the
// mode at ESI+4 is 4 or the ESI+0x174 handle is null. ESI arrives as the unit.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_B2D40(uint32_t eaxVal, void* esi, const D2GameBatchBd18Env* env)
{
    env->subB2C20(eaxVal, esi, 1);
    if (esi == nullptr) _exit(-1);   // retail: fatal 0xFB (only the exit is observable)
    if (rd32(esi, 0x04) != 4 && rd32(esi, 0x174) != 0)
        env->subB2760x(rd32(esi, 0x174), (uint32_t)(uintptr_t)esi);
}
