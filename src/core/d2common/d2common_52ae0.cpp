// 1.13c D2Common+0x52AE0 (#10418): reapply every passive skill to a unit. Retail-layout (32-bit) reader with the three callees as
// seams (0x4E480 highest-level skill: eax=unit esi=id; 0x37DB0 state bit test: eax=unit ecx=state; 0x52480 refresh: unit, id).
// The loop bound is read once; ids, skills rows and the state count are re-read per step (retail reloads the table after a refresh).
#include <emscripten.h>
#include <cstdint>
#include <cstring>

struct D2Common52AE0Env {
    uint32_t (*findSkill)(uint32_t unit, int32_t id);
    uint32_t (*stateSet)(uint32_t unit, int32_t state);
    void (*refresh)(uint32_t unit, int32_t id);
};

namespace {
inline uint32_t Rd32(const uint8_t* p, uint32_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
inline const uint8_t* PA(uint32_t a) { return (const uint8_t*)(uintptr_t)a; }
}

extern "C" void D2Common_52AE0_Raw(const uint8_t* dt, uint32_t unit, const D2Common52AE0Env* env)
{
    const int32_t n = (int32_t)Rd32(dt, 0xBB0);
    for (int32_t i = 0; i < n; ++i) {
        int32_t id = -1;
        if (i >= 0 && i < (int32_t)Rd32(dt, 0xBB0)) { int16_t s; memcpy(&s, PA(Rd32(dt, 0xBB4)) + i * 2, 2); id = s; }
        if (id < 0 || id >= (int32_t)Rd32(dt, 0xBA0)) continue;
        const uint32_t row = (uint32_t)id * 0x23C + Rd32(dt, 0xB98);
        if (row == 0) continue;
        int16_t ps; memcpy(&ps, PA(row) + 0x94, 2);
        if (ps <= 0) continue;
        if (env->findSkill(unit, id) == 0) continue;
        if (unit == 0) continue;
        const int32_t type = (int32_t)Rd32(PA(unit), 0);
        if (type < 0 || (type > 1 && type != 3)) continue;
        if ((int32_t)ps >= (int32_t)Rd32(dt, 0xC4)) continue;
        if (env->stateSet(unit, ps) == 0) continue;
        env->refresh(unit, id);
    }
}
