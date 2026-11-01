// 1.13c D2Game+0x35E90: quest trigger-FX setter. EAX = game, CL = value; game+0x10F4 (quest control)+0x20 := CL, then the
// FX callee 0xDFCC0 runs with ECX = the full register and one stack zero. Port seam: r[0]=game, r[1]=ECX, r[5]=D2GameBatch20Env.
#include <cstdint>
#include <cstring>
#include "d2game_batch20env.h"

extern "C" void D2Game_35e90(uint32_t* r) {
    const D2GameBatch20Env& env = *(const D2GameBatch20Env*)(uintptr_t)r[5];
    uint32_t ctl; std::memcpy(&ctl, (const void*)(uintptr_t)(r[0] + 0x10F4), 4);
    *(uint8_t*)(uintptr_t)(ctl + 0x20) = (uint8_t)r[1];
    env.triggerFx(r[1]);
    r[4] = 0;
}
