#include <cstdint>

// Env seam: pal(va) maps a 1.13c D2Win data VA to our address; ord10022 is the D2Gfx(?) import (thunk 0x785E).
struct D2WinEE60Env {
    uint32_t (*pal)(uint32_t va);
    void (*ord10022)(const uint32_t* tbl);
};

// 1.13c D2Win+0x0EE60 (sub_EE60): builds a 72-entry pointer table and passes it to Ordinal_10022.
void d2win_sub_ee60(const D2WinEE60Env& e) {
    uint32_t t[72];
    int n = 0;
    t[n++] = e.pal(0x6f93db60);
    t[n++] = e.pal(0x6f94db60);
    t[n++] = e.pal(0x6f95db60);
    for (uint32_t p = 0x6f932a38; p < 0x6f935b38; p += 0x100) t[n++] = e.pal(p);  // 49
    for (uint32_t p = 0x6f921638; p < 0x6f922338; p += 0x100) t[n++] = e.pal(p);  // 13
    static const uint32_t tail[7] = {0x6f901630, 0x6f911630, 0x6f922738, 0x6f932838, 0x6f935b38, 0x6f932738, 0x6f932938};
    for (uint32_t v : tail) t[n++] = e.pal(v);
    e.ord10022(t);
}
