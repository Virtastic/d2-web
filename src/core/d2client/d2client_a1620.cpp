// 1.13c D2Client+0xA1620: does candidate unit ESI pass the target filter EBX for the origin unit (stack word, ret 4)? Env-seam style: callbacks take the
// arguments the call sites push or load into registers.
#include <cstdint>
#include <cstring>
struct D2cA1620Env {
    void* c;
    const uint8_t* bm;                                               // *(D2Client+0xCEE50): the bitmask table
    uint32_t (*c1d8)(void* c, uint32_t u);                           // Ordinal 10239
    uint32_t (*c790)(void* c, uint32_t a, uint32_t u);               // Ordinal 10064 (0, unit)
    uint32_t (*c77e)(void* c, uint32_t u);                           // Ordinal 10278
    uint32_t (*p1580)(void* c, uint32_t eax);                        // D2Client+0x1580, EAX = [unit+4]
    uint32_t (*p1230)(void* c, uint32_t eax);                        // D2Client+0x1230, EAX = [unit+4]
    uint32_t (*c1f0)(void* c, uint32_t u);                           // Ordinal 10331
    uint32_t (*c208)(void* c, uint32_t room);                        // Ordinal 10057
    uint32_t (*a0ee0)(void* c, uint32_t eax, uint32_t ecx);          // D2Client+0xA0EE0, EAX = candidate, ECX = origin
    uint32_t (*a62a0)(void* c, uint32_t eax, uint32_t stk);          // D2Client+0xA62A0, EAX = candidate, stack origin
    uint32_t (*c6e2)(void* c, uint32_t a, uint32_t b, uint32_t d);   // Ordinal 11138 (origin, candidate, 0)
    uint32_t (*c1fc)(void* c, uint32_t u, uint32_t v);               // Ordinal 10494 (candidate, 0x56)
    uint32_t (*p1630)(void* c, uint32_t ecx);                        // D2Client+0x1630 (ECX = unit)
    uint32_t (*p1660)(void* c, uint32_t ecx);                        // D2Client+0x1660 (ECX = unit)
    uint32_t (*c436)(void* c, uint32_t room, const uint32_t* p, const uint32_t* q, uint32_t n);   // Ordinal 11076
};
static inline uint32_t a1620_rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
// 1.13c D2Client+0xA1620
extern "C" uint32_t d2c_a1620(const D2cA1620Env& e, uint32_t flags, uint32_t esi, uint32_t origin) {
    if (!esi) return 0;
    const uint32_t type = a1620_rd(esi, 0);
    if (type > 4) return 0;
    const uint32_t sub = a1620_rd(esi, 0x10);
    switch (type) {
    case 0:
        if (!(flags & 1)) return 0;
        if (flags & 0x1000) { if (sub != 0x11) return 0; }
        else if (sub == 0x11 || sub == 0) return 0;
        break;
    case 1:
        if (!(flags & 2)) return 0;
        if (flags & 0x1000) { if (sub != 0xC) return 0; }
        else if (sub == 0xC || sub == 0) return 0;
        if ((flags & 4) && !e.c1d8(e.c, esi)) return 0;
        if ((flags & 0x4000) && e.c790(e.c, 0, esi)) return 0;
        if ((flags & 0x40000) && e.c77e(e.c, esi)) return 0;
        break;
    case 2: if (!(flags & 0x10)) return 0; break;
    case 3: {
        if (!(flags & 8)) return 0;
        const uint32_t p = e.p1580(e.c, a1620_rd(esi, 4));
        if (!p) return 0;
        if (e.bm[4] & *(const uint8_t*)(uintptr_t)(p + 4)) return 0;
        break; }
    case 4: if (!(flags & 0x20)) return 0; break;
    }
    if ((int8_t)flags < 0 && !((a1620_rd(esi, 0xC4) >> 2) & 1)) return 0;
    if ((flags & 0x400) && !((a1620_rd(esi, 0xC4) >> 3) & 1)) return 0;
    if ((flags & 0x100) && e.c208(e.c, e.c1f0(e.c, esi))) return 0;
    if (flags & 0x10000) {
        if (!e.a0ee0(e.c, esi, origin)) return 0;
        if (a1620_rd(esi, 0) == 1) {
            const uint32_t p = e.p1230(e.c, a1620_rd(esi, 4));
            if (!p) return 0;
            if (e.bm[0] & *(const uint8_t*)(uintptr_t)(p + 0xD)) return 0;
        }
    }
    if ((flags & 0x8000) && !e.a62a0(e.c, esi, origin)) return 0;
    if ((flags & 0x20000) && !e.c6e2(e.c, origin, esi, 0)) return 0;
    if ((flags & 0x80000) && e.c1fc(e.c, esi, 0x56)) return 0;
    if (flags & 0x200) {
        const uint32_t room = e.c1f0(e.c, origin);
        if (!room) return 0;
        if (!e.c1f0(e.c, esi)) return 0;
        uint32_t pq[4];   // retail's stack block: q = (esi x, y) at +0, p = (origin x, y) at +8, as the two pointers passed to Ordinal 11076
        pq[2] = e.p1630(e.c, origin);
        pq[3] = e.p1660(e.c, origin);
        pq[0] = e.p1630(e.c, esi);
        pq[1] = e.p1660(e.c, esi);
        if (e.c436(e.c, room, pq + 2, pq, 4)) return 0;
    }
    return 1;
}
