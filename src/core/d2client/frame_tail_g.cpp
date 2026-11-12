// 1.13c D2Client+0x44230 after the room walk (the walk ends at D2Gfx 10054), as its D2Gfx calls (G-trace seam, beside
// frame_44230_g.cpp). Order and gates are the decompile's, the screen 640x480 (the G capture rig's DAT_6FB8BC48/4C):
//   8B110 tail: 805A0 -> 7FE80 rain (weather_sim drawG); 739E0 head 10031; 29250's HUD chain: 10078(1), 10031 x2, 27290 (the
//   five control-panel cels), 27BD0, 27A90 (10078 0xA, 10074, cel), 26970 (none), 28320, 278B0 (the 10014 line), 26880,
//   28DC0 (belt slots 0..3: 6B5A0 cel + 10013), 28CE0 (two skill buttons, 28BC0 -> 10019), 289A0/267B0 (none, panels shut);
//   739E0 tail: 10031(4, 0x30000) x2, 10043, 10080, 10085.
// Register leftovers (ecx/edx a prior call left) are taken from what the code path computes where the disassembly shows it
// (a cel draw's x or y), otherwise they are arguments.
#include <cstdint>
#include <cstdio>
#include <string>
#include <initializer_list>
#include "weather_sim.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" uint32_t d2_session_belt_index();
extern "C" uint32_t d2_session_belt_boxes(int beltIndex, int higherRes, int32_t* out, uint32_t cap);
extern "C" uint32_t d2_session_belt_slot_mask();

namespace {
std::string out;
constexpr uint32_t P = 0x100000u;   // a pointer argument (cel, palette): the trace masks it
void rec(int ord, uint32_t ecx, uint32_t edx, std::initializer_list<uint32_t> a = {}) {
    char b[24];
    std::snprintf(b, sizeof b, "%d %x,%x", ord, ecx, edx);
    out += b;
    for (uint32_t v : a) {
        if (v >= P) out += ",*";
        else { std::snprintf(b, sizeof b, ",%x", v); out += b; }
    }
    out += '\n';
}
// D2Gfx 10041 DrawCelContext(cel, x, y, palette, mode 5, 0)
void cel(uint32_t ecx, uint32_t edx, int32_t x, int32_t y) { rec(10041, ecx, edx, {P, uint32_t(x), uint32_t(y), P, 5, 0}); }
}

// first: the G trace's first frame (no rain yet; 805A0's place holds a 10045 whose source is not recovered, its leftovers
// the retail values). tick: the trace tick (the frame counter the outer 10040 carries). lead: the ecx 739E0's head 10031 and
// its 10043 inherit (the rain loop's last register; retail's own value on tick 2, 0x221, and 0 after).
extern "C" EMSCRIPTEN_KEEPALIVE const char* d2_client_frame_tail_g(int32_t first, int32_t tick, uint32_t lead) {
    out.clear();
    const int32_t W = 640, H = 480;
    if (first) rec(10045, lead, 0x6b, {0, 0, uint32_t(W), uint32_t(H - 0x2f), 0, 0x6b});
    else d2client::weather::drawG(out, W, H, 0);
    rec(10031, lead, 0);                                         // 739E0 head
    rec(10078, 1, 1); rec(10031, 1, 1); rec(10031, 0, 1);        // 29250
    cel(H, 0, 0, H);                                             // 27290: the control panel's five cels along y = H
    cel(0, 0, W / 2 - 0x9b, H);
    cel(0, 0, W / 2 - 0x1b, H);
    cel(H, 0, W / 2 + 0x65, H);
    cel(W - 0x75, 0, W - 0x75, H);
    cel(0, 0, 0x1c, H - 5);                                      // 27BD0
    rec(10078, 0xa, 0);                                          // 27A90
    rec(10074, W - 0x6f, 0, {P, uint32_t(W - 0x6f), uint32_t(H - 0xd), 0, 0, 5});
    cel(H - 9, W - 0x6e, W - 0x6e, H - 9);
    cel(H - 10, 0, W / 2 - 0x91, H - 10);                        // 28320
    rec(10014, H - 0x1b, W / 2 - 0x7f, {uint32_t(W / 2 - 0x7f), uint32_t(H - 0x1b), uint32_t(W / 2 - 0x7f), uint32_t(H - 9), 0x62, 2});   // 278B0
    cel(H - 0x10, 0, W / 2 - 8, H - 0x10);                       // 26880
    {   // 28DC0: belt slots 0..3 (the popup is shut), each through 6B5A0 at its Belts.txt box (640x480 row) then 10013
        const uint32_t mask = d2_session_belt_slot_mask();
        int32_t box[1 + 16 * 4] = {};
        const uint32_t n = mask ? d2_session_belt_boxes(int(d2_session_belt_index()), 0, box, 1 + 16 * 4) : 0;
        for (uint32_t i = 0; i < 4 && i < n; ++i)
            if (mask & (1u << i)) { cel(0, 0, box[1 + i * 4 + 0], box[1 + i * 4 + 3] - 1); rec(10013, 0, 0); }
    }
    rec(10019, 0, 0, {P, 0x75, uint32_t(H), P, 5, 1});            // 28CE0 -> 28BC0, left and right skill buttons
    rec(10019, 0, 0, {P, uint32_t(W - 0xa5), uint32_t(H), P, 5, 1});
    rec(10031, 4, 0x30000); rec(10031, 4, 0x30000);              // 739E0 tail
    rec(10043, lead, 0);
    rec(10080, 0, 0, {P, P});
    rec(10085, 0, 0, {0, 0, 0, 0});
    (void)tick;
    return out.c_str();
}

// After 44230 returns: the outer loop's 10040 with its frame phase (1, 2, 3, then a host-time value retail reads off the
// clock: not reproducible, 0 here); the first frame adds 10021(1) before it.
extern "C" EMSCRIPTEN_KEEPALIVE const char* d2_client_frame_outer_g(int32_t first, int32_t tick) {
    out.clear();
    if (first) rec(10021, 1, 0, {0});
    const int32_t c = (tick - 1) % 4;
    rec(10040, c < 3 ? uint32_t(c + 1) : 0u, 0);
    return out.c_str();
}
