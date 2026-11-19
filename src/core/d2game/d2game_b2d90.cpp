// 1.13c D2Game+0xB2D90 (seam table D2GameBatchBd18Env, see d2game_batch_d1890_env.h). Probe: src/core/fn/d2game_b2d90.cpp.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>
#include "d2game_batch_d1890_env.h"

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }

// Inventory-mode tick: the EBX record's +0x1D28 block must be non-null with bit 0x400 set at its +8.
// Then, when the player exists (0x4AD10(0)), the 0xB23D0 probe runs and a non-zero result reaches
// 0xB2760. The in_EAX unit is required (fatal 0x9F1); its +0x3D4 flags bit 2 routes to
// 0xB2C20(EBX, unit, 0).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_B2D90(uint32_t eaxUnit, void* ebx, const D2GameBatchBd18Env* env)
{
    uint8_t* block = (uint8_t*)(uintptr_t)rd32(ebx, 0x1D28);
    if (block != nullptr && (rd32(block, 0x08) & 0x400) != 0) {
        void* pl = env->sub6AD10x(eaxUnit, 0);
        if (pl != nullptr) {
            if (env->subB23D0x((uint32_t)(uintptr_t)pl) != 0)
                env->subB2760x((uint32_t)(uintptr_t)pl, eaxUnit);
        }
        if (eaxUnit == 0) _exit(-1);   // retail: fatal 0x9F1
        if ((rd8((void*)(uintptr_t)eaxUnit, 0x3D4) & 4) != 0) {
            env->subB2C20((uint32_t)(uintptr_t)ebx, (void*)(uintptr_t)eaxUnit, 0);
        }
    }
}
