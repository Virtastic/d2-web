// 1.13c D2Client+0x8F710: one stack byte, the wanted tab (above 4 it becomes 0). With the busy word D2Client+0x11BC4C clear and the
// player-valid word +0x119721 set, the player is looked up by its id (+0x11971D) in the 128-bucket chain table at +0x10A808 (id at node +0xC,
// next at +0xE4); a missing node returns, a node whose kind is not 1 is fatal. Its inventory (node +0x60) is walked through D2Common #10460
// (thunk 0xC4F0) / #10464 (0xC568); each item's class is #10020(#11147(item)) (thunks 0xC92E, 0xC526) and classes below 5 are counted.
// Then the current-tab byte +0x11BC48 takes the wanted tab, every tab record (0x12 bytes each from +0xE1182, count at +0xE11C0) gets its
// selected dword cleared and its populated dword (+4) set to "that class has items"; when the wanted tab is empty the next populated one
// is searched (at most four steps, wrapping to 0 past the byte +0x11BC50 or at 0xFF) and stored, and the chosen record is marked selected.
#include "cl_env.h"
namespace {
const uint32_t B = 0x6FAB0000u;
void wr8(const cl::Env& e, uint32_t a, uint32_t v) { e.wr(e.ctx, a, (e.R(a) & 0xffffff00u) | (v & 0xff)); }
}
extern "C" void d2c_8f710(uint32_t tab, const cl::Env& e) {
    uint32_t t = tab & 0xff;
    if (t > 4) t = 0;
    if (e.R(B + 0x11BC4Cu) != 0 || e.R(B + 0x119721u) == 0) return;
    const uint32_t id = e.R(B + 0x11971Du);
    uint32_t n = e.R(B + 0x10A808u + (id & 0x7f) * 4);
    if (!n) return;
    while (e.R(n + 0xc) != id) { n = e.R(n + 0xe4); if (!n) return; }
    if (e.R(n) != 1) return;   // ponytail: retail reports and exits the process here; no caller survives it
    const uint32_t inv = e.R(n + 0x60);
    if (!inv) return;
    uint32_t cnt[256] = {};    // retail keeps five counters on the stack; an index past 4 reads the words above them
    for (uint32_t it = e.h(e.ctx, 0xC4F0, 0, 0, 0, 0, 0, inv, 0, 0, 0, 0); it; it = e.h(e.ctx, 0xC568, 0, 0, 0, 0, 0, it, 0, 0, 0, 0)) {
        const uint32_t rec = e.h(e.ctx, 0xC526, 0, 0, 0, 0, 0, it, 0, 0, 0, 0);
        const uint32_t k = e.h(e.ctx, 0xC92E, 0, 0, 0, 0, 0, rec, 0, 0, 0, 0) & 0xff;
        if (k < 5) cnt[k]++;
    }
    wr8(e, B + 0x11BC48u, t);
    const int32_t tabs = (int32_t)e.R(B + 0xE11C0u);
    for (int32_t i = 0; i < tabs; ++i) {
        e.wr(e.ctx, B + 0xE1182u + (uint32_t)i * 0x12, 0);
        e.wr(e.ctx, B + 0xE1186u + (uint32_t)i * 0x12, cnt[i & 0xff] != 0);
    }
    if (cnt[t] == 0) {
        int32_t steps = 0;
        do {
            t = (t + 1) & 0xff; ++steps;
            if (e.R8(B + 0x11BC50u) < t || t == 0xff) t = 0;
        } while (cnt[t] == 0 && steps < 4);
        wr8(e, B + 0x11BC48u, t);
    }
    e.wr(e.ctx, B + 0xE1182u + t * 0x12, 1);
}
