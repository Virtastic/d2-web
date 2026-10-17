// 1.13c D2Client+0x1CCD0
#include <cstdint>
#include <emscripten.h>
#include "D2QuestRecord.h"

namespace { uint32_t g_clear[12], g_mode, g_a, g_b; }  // 0x123365..0x123395 (clear 48 bytes, mode), 0x1223F5, 0x1223F9

// in = EAX (signed); q = D2Client+0x11973B (quest record). Keeps the highest tier whose quest bit is set, down to 0.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1ccd0(int32_t in, D2BitBufferStrc* q) {
    auto has = [&](int n) { return QUESTRECORD_GetQuestState(q, n, 0) != 0; };
    if (!(4 < in)) {  // retail: cmp/jg (signed), Ghidra "4 < in_EAX"; negatives fall to the switch default and keep their value
        switch (in) {
        case 4: if (!has(0x1a)) in = 3; [[fallthrough]];
        case 3: if (!has(0x17)) in = 2; [[fallthrough]];
        case 2: if (!has(0xf)) in = 1; [[fallthrough]];
        case 1: if (!has(7)) in = 0; break;
        default: break;
        }
    } else in = 0;
    for (auto& w : g_clear) w = 0;
    g_mode = (uint32_t)in; g_a = g_b = 0xffffffffu;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1ccd0_get(uint32_t* out) {  // out[0..12] cleared bytes, out[12] mode, out[13..14] the two -1s
    for (int i = 0; i < 12; ++i) out[i] = g_clear[i];
    out[12] = g_mode; out[13] = g_a; out[14] = g_b;
}
