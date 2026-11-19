// 1.13c D2Client+0x29920 seam for the platform layer: the belt-use gate on an Items.txt row's unique (+0x129), quest (+0x12A) and
// useable (+0x11D) bytes. 1 = the item may be used; 0 = refused (unique/quest rows open the reading panel, a non-useable row plays
// the refusal voice; neither effect is wired for the joined game, the use is simply not sent).
#include <cstdint>
#include <emscripten.h>
#include "d2client_callee.h"
extern "C" uint32_t d2c_29920(const D2cSvc& v, const uint8_t* rec, uint32_t s0, uint32_t s1);

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_use_row_gate(uint32_t useable, uint32_t unique, uint32_t quest) {
    uint8_t rec[0x130] = {};
    rec[0x11d] = (uint8_t)useable; rec[0x129] = (uint8_t)unique; rec[0x12a] = (uint8_t)quest;
    const D2cSvc v = { nullptr, [](void*, uint32_t, const uint32_t*, const uint32_t*) -> uint32_t { return 0; } };
    return (int32_t)d2c_29920(v, rec, 0, 0);
}
