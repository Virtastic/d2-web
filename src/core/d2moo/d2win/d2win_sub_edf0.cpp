#include <cstdint>

struct D2WinEDF0State {
    int32_t active;        // DAT_6f8ffd44
    int32_t timed;         // DAT_6f9a9e28
    uint32_t lastTick;     // DAT_6f9a9e2c
    int32_t counter;       // DAT_6f921630
};

// 1.13c D2Win+0x0EDF0 d2win_sub_edf0.cpp:d2win_sub_edf0
void d2win_sub_edf0(D2WinEDF0State& s, uint32_t (*getTick)(), void (*ord10061)()) {
    if (!s.active) return;
    if (s.timed) {
        if (getTick() - s.lastTick < 0x7d1) return;
        s.lastTick = getTick();
        if (--s.counter > 0) { ord10061(); return; }
        s.timed = 0;
    }
    s.active = 0;
    ord10061();                                           // Ordinal_10061(&DAT_6f922338)
}
