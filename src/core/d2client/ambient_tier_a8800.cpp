// 1.13c D2Client+0xA8800 (120 B, plain ret, no inputs; ESI is saved and restored): picks the ambient tier from Ordinal_10047 (thunk 0xD0C6),
// the flag 0xDD854, the level 0x119808 and the held tier 0x12341C, and latches a changed tier into 0x12341C with the GetTickCount stamp 0x123418
// once more than 0x7D0 ms have passed since the stamp. Returns EAX: the new tier, the held tier when the change is too soon, or the held tier
// without a tick read when the level is 13..15 and a tier >= 1 is held.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0xA8800
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_a8800(uint32_t* stamp, int32_t* state, int32_t level, int32_t flag, int32_t (*ord10047)(), uint32_t (*tick)()) {
    int32_t tier = 2;
    int32_t held;
    bool on = ord10047() != 0;
    held = *state;
    if (on && flag == 0) {
        tier = 0;
    } else {
        if (on) tier = 1;
        if (level <= 9) tier = 0;
        else if (level > 12) {
            if (level <= 15 && held >= 1) return held;
            if (level <= 15) tier = 1;
        } else tier = 1;
    }
    if (tier == held) return tier;
    uint32_t t = tick();
    if (t - *stamp <= 0x7D0u) return *state;
    *stamp = t;
    *state = tier;
    return tier;
}

// The tier for a caller that holds the client's state as plain values (the headless rig: level 0, flag 1, held 0):
// the D2Gfx query and the tick read are the given values.
static int32_t g_tierOrd10047, g_tierNow;
static int32_t tierOrd10047() { return g_tierOrd10047; }
static uint32_t tierNow() { return (uint32_t)g_tierNow; }
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_light_tier(int32_t level, int32_t flag, int32_t ord10047, int32_t held, int32_t stamp, int32_t now) {
    g_tierOrd10047 = ord10047; g_tierNow = now;
    uint32_t s = (uint32_t)stamp;
    return d2_client_a8800(&s, &held, level, flag, tierOrd10047, tierNow);
}
