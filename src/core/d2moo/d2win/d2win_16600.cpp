// 1.13c D2Win+0x16600: click / double-click dispatcher. msg[0] = window (type 7, else fatal 0x154). Window dwords: [0xd] default
// handler, [0x19] child list head, [0x1a] current child. Child (byte offsets): +0x204 handler, +0x208..0x214 L,R,T,B exclusive, +0x220 next.
// A hit on the current child inside 300 ms calls its handler (when set) and returns; otherwise the hit child becomes current.
// Seams: mouse x/y, tick, last-tick global, fatal, call (callback tag/pointer, message).
#include <cstdint>
extern "C" uint32_t d2win_16600(const uint32_t* msg, uint32_t mx, uint32_t my, uint32_t now, uint32_t* lastTick,
                                void (*fatal)(uint32_t), uint32_t (*call)(uint32_t cb, const uint32_t* msg)) {
    uint32_t* w = (uint32_t*)(uintptr_t)msg[0];
    if (w[0] != 7) { fatal(0x154); return 0; }
    for (uint32_t c = w[0x19]; c; c = *(uint32_t*)(uintptr_t)(c + 0x220)) {
        auto f = [&](uint32_t o) { return *(int32_t*)(uintptr_t)(c + o); };
        if (f(0x208) < (int32_t)mx && (int32_t)mx < f(0x20c) && f(0x210) < (int32_t)my && (int32_t)my < f(0x214)) {
            if (c == w[0x1a] && now - *lastTick < 300) {
                if (f(0x204)) return call((uint32_t)f(0x204), msg);
            } else w[0x1a] = c;
        }
    }
    *lastTick = now;
    return w[0xd] ? call(w[0xd], msg) : 0;
}
