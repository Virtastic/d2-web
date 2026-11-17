// The HUD's orb and stamina values as 1.13c D2Client draws them: every reader (0x27BD0 life, 0x27A90 mana, 0x278B0 stamina)
// takes its current value through 0x26B20 (orb_smooth_26b20.cpp), which keeps the three 16-byte slots at 0x11A794 and reads
// the client tick at 0x1197F8. This file holds those slots for the browser; the caller supplies the unit's values and the tick.
#include <cstdint>
#include <emscripten.h>
namespace d2client_orb {
struct Slot { int32_t shown, tick, prev, prevTick; };
struct Env { uint32_t frame; Slot slot[3]; int32_t (*current)(); int32_t (*max[3])(); };
int32_t sub_26B20(Env&, int32_t stat);
}
namespace {
d2client_orb::Env g_orbs = {};     // D2Client+0x11A794: static, never cleared; 0x26B20 re-seeds a slot while the tick is below 2
int32_t g_cur = 0, g_max = 0;      // the values ord 10973 and the stat's max getter answer for this call
}
// stat 6 life, 8 mana, 10 stamina; cur/max raw 1/256 stat values as D2Common answers them; frame = D2Client+0x1197F8.
// Returns the value 0x26B20 hands its reader (0 for any other stat or a zero max).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_orb_smooth(int32_t stat, int32_t cur, int32_t max, uint32_t frame) {
    g_cur = cur; g_max = max;
    g_orbs.frame = frame;
    g_orbs.current = [] { return g_cur; };
    for (auto& m : g_orbs.max) m = [] { return g_max; };
    return d2client_orb::sub_26B20(g_orbs, stat);
}
