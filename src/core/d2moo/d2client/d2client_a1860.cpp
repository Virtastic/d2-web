// 1.13c D2Client+0xA1860: walk the rooms around `unit` and call the callback on each candidate in range that passes the 0xA1620 filter (stdcall, 8 stack words, ret 0x20).
// Env-seam style: every callee takes the arguments the call site loads into registers or pushes.
#include <cstdint>
#include <cstring>
struct D2cA1860Env {
    void* c;
    uint32_t (*c1f0)(void* c, uint32_t unit);                                            // Ordinal 10331: room of unit
    uint32_t (*p1630)(void* c, uint32_t ecx);                                            // D2Client+0x1630 (ECX = unit)
    uint32_t (*p1660)(void* c, uint32_t ecx);                                            // D2Client+0x1660 (ECX = unit)
    void (*c3dc)(void* c, uint32_t room, uint32_t* list, uint32_t* count);              // Ordinal 10383: (room, &list, &count)
    uint32_t (*c208)(void* c, uint32_t room);                                            // Ordinal 10057
    uint32_t (*a0750)(void* c, uint32_t ecx, uint32_t edx, uint32_t eax);               // D2Client+0xA0750: ECX = room, EDX = x, EAX = y
    uint32_t (*c2ec)(void* c, uint32_t id);                                              // Ordinal 10750
    uint32_t (*c3be)(void* c, uint32_t id);                                              // Ordinal 10867
    int32_t (*c6fa)(void* c, uint32_t x, uint32_t y, uint32_t w, uint32_t h);           // Ordinal 10769
    uint32_t (*a1620)(void* c, uint32_t esi, uint32_t ebx, uint32_t unit);              // D2Client+0xA1620: ESI = candidate, EBX = flags, stack unit
    uint32_t (*p1760)(void* c, uint32_t eax, uint32_t edx);                              // D2Client+0x1760: EAX = [cand+4], EDX = 0x1b
    uint32_t (*cb)(void* c, uint32_t fn, uint32_t* ctx, uint32_t cand);                  // fastcall callback: ECX = &ctx, EDX = candidate
};
static inline uint32_t a1860_rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
// 1.13c D2Client+0xA1860
extern "C" void d2c_a1860(const D2cA1860Env& e, uint32_t unit, uint32_t x, uint32_t y, uint32_t radius, uint32_t flags, uint32_t cb, uint32_t value, uint32_t flag) {
    if (!unit || !cb || !radius) return;
    const uint32_t room = e.c1f0(e.c, unit);
    if (!room) return;
    if (!x || !y) { x = e.p1630(e.c, unit); y = e.p1660(e.c, unit); }
    if (!flags) flags = 0x583;
    uint32_t ctx[3] = { unit, 0, value };
    const int32_t d2 = (int32_t)(radius * radius);
    uint32_t list = 0, count = 0;
    e.c3dc(e.c, room, &list, &count);
    for (uint32_t i = 0; i < count; i++) {
        const uint32_t rm = a1860_rd(list, 4 * i);
        if ((flags & 0x2000) && e.c208(e.c, rm)) continue;
        if (!e.a0750(e.c, rm, x, y)) continue;
        for (uint32_t cand = a1860_rd(rm, 0x74); cand;) {
            const uint32_t next = a1860_rd(cand, 0xE8);
            if (cand != unit) {
                const int32_t t = (int32_t)a1860_rd(cand, 0);
                uint32_t h, w;
                if (t == 2 || (t > 3 && t <= 5)) { const uint32_t d = a1860_rd(cand, 0x2C); h = a1860_rd(d, 0x10); w = a1860_rd(d, 0xC); }
                else {
                    const uint32_t id = a1860_rd(cand, 0x2C);
                    h = id ? e.c2ec(e.c, id) : 0;
                    w = id ? e.c3be(e.c, id) : 0;
                }
                const int32_t dist = e.c6fa(e.c, x, y, w, h);
                if (!(d2 < dist) && e.a1620(e.c, cand, flags, unit)) {
                    if (!(flag && a1860_rd(cand, 0) == 1 && e.p1760(e.c, a1860_rd(cand, 4), 0x1B))) {
                        if (e.cb(e.c, cb, ctx, cand)) ctx[1]++;
                    }
                }
            }
            cand = next;
        }
    }
}
