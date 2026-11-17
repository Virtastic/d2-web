// Draw-call sink: runs the ported D2Client HUD draw functions against a recording dispatcher instead of a renderer. Every D2Gfx
// draw primitive a port emits (cel, cel-with-rows) is appended, in call order, to a per-frame list the browser replays onto its
// canvas; the queries a port makes (smoothed stat, maximum, state test) are answered from the values the caller supplied.
#include <cstdint>
#include <emscripten.h>
#include "d2client_callee.h"

extern "C" void d2c_27bd0(const D2cSvc&, uint32_t, int32_t, int32_t, uint32_t, uint32_t);
extern "C" void d2c_27a90(const D2cSvc&, uint32_t, int32_t, int32_t, uint32_t, uint32_t);

namespace {
// One record: callee RVA, cell handle (blk[13]), frame (blk[0]), x, y (bottom row), a, rows, mode.
constexpr int kFields = 8, kMax = 256;
int32_t g_list[kMax * kFields];
int g_count = 0;

struct Model { int32_t cur[2], max[2]; };   // [0] life, [1] mana
Model g_model;
uint32_t g_unit[8];   // the unit words the ports read: type at +0, mode at +0x10

void push(uint32_t rva, const uint32_t* blk, const uint32_t* s, int firstArg) {
    if (g_count >= kMax) return;
    int32_t* r = g_list + g_count++ * kFields;
    r[0] = (int32_t)rva; r[1] = (int32_t)blk[13]; r[2] = (int32_t)blk[0];
    r[3] = (int32_t)s[firstArg]; r[4] = (int32_t)s[firstArg + 1];
    r[5] = (int32_t)s[firstArg + 2]; r[6] = (int32_t)s[firstArg + 3]; r[7] = (int32_t)s[firstArg + 4];
}

uint32_t callee(void*, uint32_t rva, const uint32_t* r, const uint32_t* s) {
    switch (rva) {
    case 0x26b20: return (uint32_t)g_model.cur[r[0] == 8 ? 1 : 0];     // smoothed stat 6 (life) / 8 (mana)
    case 0xc196: return (uint32_t)g_model.max[0];                      // life maximum
    case 0xcf4c: return (uint32_t)g_model.max[1];                      // mana maximum
    case 0xd096: case 0xd1b6: push(rva, (const uint32_t*)(uintptr_t)s[0], s, 1); return 0;
    default: return 0;                                                  // state tests, resolution query: nothing set
    }
}
}  // namespace

extern "C" {
// Runs the life then the mana orb draw (+0x27BD0, +0x27A90) at the 800x600 base. Values are raw 1/256, already smoothed.
// cellFluid / cellOverlap are the caller's handles for the fluid and overlap cels. `out` receives the records (8 words each, kFields); returns the count.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_draw_orbs(int32_t life, int32_t lifeMax, int32_t mana, int32_t manaMax, int32_t alive, uint32_t cellFluid, uint32_t cellOverlap, int32_t* out) {
    g_count = 0;
    g_model = { { life, mana }, { lifeMax, manaMax } };
    g_unit[0] = 0; g_unit[4] = alive ? 1 : 0x11;
    const D2cSvc v = { nullptr, callee };
    const uint32_t unit = (uint32_t)(uintptr_t)g_unit;
    d2c_27bd0(v, unit, 800, 600, cellFluid, cellOverlap);
    d2c_27a90(v, unit, 800, 600, cellFluid, cellOverlap);
    for (int i = 0; i < g_count * kFields; i++) out[i] = g_list[i];
    return g_count;
}
}
