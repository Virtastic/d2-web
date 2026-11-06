// D2Client+0x5A9A0 -- one frame of the FPS/skip counter (1000 ms window) and, when enabled, the debug text lines.
#include "d2client_5a9a0.h"

// 1.13c D2Client+0x5A9A0
void d2c_5a9a0(const D2Client5a9a0Env& e, D2Client5a9a0State* s) {
    const uint32_t now = e.tick();
    const uint32_t elapsed = now - s->lastTick;
    const uint32_t en = s->enable;
    if (en && !s->flagTick) { s->flagTick = now; s->skip = 0; }
    if (elapsed <= 0x3e8) { s->frames++; s->skip++; }
    else { s->lastTick = now; s->acc = s->frames; s->frames = 0; }
    if (!en) { s->flagTick = 0; return; }
    e.text(0xa, 2, 0xD5844, s->acc, e.g9810());
    e.text(0xa0, 2, 0xD5838, e.g9804(), 0xa);           // fifth word: x of the line above, still on the stack
    uint32_t m[8] = {0x20, 0, 0, 0, 0, 0, 0, 0};
    e.memStatus(m);
    e.text(0xa, 0x10, 0xD582C, ((m[2] - m[3]) >> 20) + 1, (m[2] >> 20) + 1);
    e.text(0xa0, 0x10, 0xD5820, ((m[4] - m[5]) >> 20) + 1, (m[4] >> 20) + 1);
    static const uint32_t names[7] = {0xD5814, 0xD5808, 0xD5800, 0xD57F8, 0xD57EC, 0xD5814, 0xD57E4};
    const uint32_t mode = e.c10e() - 1;
    char buf[0x40];
    e.copy(buf, mode > 6 ? 0xD57DC : names[mode]);
    uint32_t b0; __builtin_memcpy(&b0, buf, 4);          // the local buffer sits right above the 4-word lines: their fifth word
    e.text(0x1f4, 2, 0xD57D0, (U)buf, b0);
    if (s->cacheOff == 0) {
        e.text(0xa, 0x78, 0xD57C0, e.c040(), b0);
        e.text(0xa, 0x84, 0xD57A8, e.c044(), e.c048());
        uint32_t t = e.c13c();
        e.text(0xa, 0x90, 0xD5788, e.c148(t), t);        // c148 is cdecl: its argument stays as the fifth word
        e.text(0xa, 0x9c, 0xD5778, e.c094(), b0);
        t = e.c15a();
        e.text(0xa, 0xa8, 0xD5758, e.c14e(t), t);
        e.cacheOverlay();
    }
}
