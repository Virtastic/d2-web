// 1.13c D2Client+0x19830: unit state-packet dispatcher.
// Every callee goes through `cb` (one event per call: rva, eax ecx edx ebx esi, four stack words); its result is the callee's eax.
#include <cstdint>
struct D2C19830Ev { uint32_t rva, a, c, d, b, s, w[4]; };
typedef uint32_t (*D2C19830Cb)(void*, const D2C19830Ev*, uint32_t* out);
struct D2C19830G { uint32_t sel, f46c4, f46c8, c2f8, c2f4; };   // 0x11BBFC, 0xF46C4, 0xF46C8, 0x11C2F8, 0x11C2F4
static_assert(sizeof(D2C19830G) == 20, "g");
static inline uint32_t rd(uint32_t p, uint32_t o) { return *(const uint32_t*)(uintptr_t)(p + o); }
static inline uint32_t rh(uint32_t p, uint32_t o) { return *(const uint16_t*)(uintptr_t)(p + o); }
static inline void wr(uint32_t p, uint32_t o, uint32_t v) { *(uint32_t*)(uintptr_t)(p + o) = v; }
// type = ECX, U = EDX (the unit), pkt/flag = stack. Returns eax (1 handled, 0 fell through to 0x19710); ~0u = the _exit(-1) default.
extern "C" uint32_t d2_client_19830(uint32_t type, uint32_t U, uint32_t pkt, uint32_t flag, D2C19830G* g, D2C19830Cb cb, void* cx) {
    auto call = [&](uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t s,
                    uint32_t w0 = 0, uint32_t w1 = 0, uint32_t w2 = 0, uint32_t w3 = 0, uint32_t* out = nullptr) {
        D2C19830Ev e = { rva, a, c, d, b, s, { w0, w1, w2, w3 } }; return cb(cx, &e, out); };
    uint32_t r = call(0xC5C2, 0, 0, 0, 0, 0, U, 0);
    if (r) call(0xA8630, 0, 0, 0, 0, r);
    if (type != 0x13) { call(0x197D0, U, 0, 0, 0, 0, U == g->sel); call(0xC778, 0, 0, 0, 0, 0, U, 0); }
    uint32_t esi = rd(U, 0x2C);
    if (rd(U, 0x10) == 0x13) { if (!flag) return 1; call(0xCD2A, 0, 0, 0, 0, 0, esi); }
    call(0xCD78, 0, 0, 0, 0, 0, esi);
    uint32_t edi = call(0xC208, 0, 0, 0, 0, 0, call(0xC1F0, 0, 0, 0, 0, 0, U)) ? 5 : 1;   // ebp = 6 / 2 is invisible to the hooks
    auto a1b0 = [&](uint32_t ax, uint32_t w0, uint32_t w1) { call(0x2A1B0, ax, 0, 0, 0, 0, w0, w1); };
    switch (type) {
    case 0: case 0x18: r = call(0x2A070, 0, rd(pkt, 4), rd(pkt, 0), 0, 0); goto L49;
    case 1: case 0x17: r = call(0x2A040, rd(pkt, 4), rd(pkt, 0), 0, 0, U);
    L49: if (!r) goto fail; call(0x2A6C0, 0, 0, 0, 0, U); return 1;
    case 2: call(0x2AC10, 0, rh(pkt, 0), rd(pkt, 4), 0, 0); return 1;
    case 6: wr(U, 0xB0, rd(pkt, 8)); call(0x2A6C0, 0, 0, 0, 0, U); a1b0(rh(pkt, 0), rh(pkt, 4), 1); return 1;
    case 7: {
        uint32_t bp = call(0xA5440, U, 0, 0, 0, 0); call(0xA6210, 0, 0, 0, 0, U);
        wr(U, 0xC4, (rd(U, 0xC4) & ~0x40u) | 2);
        if (call(0xC43C, 0, 0, 0, 0, 0, U, edi)) { call(0x6A220, U, 0, 0, 0, 0, 0); call(0x2A590, 0, edi, U, 0, 0); call(0x24490, U, 0, 0, 0, 0, edi, 0); }
        if (pkt) call(0x2A460, U, 0, 0, 0, 0, rd(pkt, 0), rd(pkt, 4), bp);
        return 1; }
    case 8: {
        if (g->sel == U) call(0xBE350, 0, 0, 0, 0, 0, 1, 0);
        wr(U, 0xB0, rd(pkt, 8));
        uint32_t ax = call(0x51A80, 0, 0, 0, 0, 0);
        if (ax == U) {
            ax = (ax & ~0xFFu) | 8;   // mov al,8: the stale high bytes reach 0x2A1B0's eax
            if ((g->f46c4 & 8) || (g->f46c8 & 8)) { ax = call(0x42AD0, 0, 0, 0, 0, 0); g->c2f8 = 1; } else g->c2f8 = 0;
            g->c2f4 = 0;
        }
        a1b0((ax & 0xFFFF0000u) | rh(pkt, 0), rh(pkt, 4), 0);
        call(0x2A6C0, 0, 0, 0, 0, U); call(0x18B20, 0, 0, 0, 0, U); call(0x16A0, U, 0, 0, 0, 0); return 1; }
    case 9: call(0x2A6C0, 0, 0, 0, 0, U); call(0x18B20, 0, 0, 0, 0, U); return 1;
    case 0x12: {
        wr(U, 0xB0, rd(pkt, 8));
        r = call(0xC274, 0, 0, 0, 0, 0, rd(U, 0x60), 0);
        uint32_t out = pkt;   // the out-block is the arg1 stack slot: an untouched callee leaves the packet pointer there
        if (!r) call(0xC2AA, 0, 0, 0, 0, 0, U, rd(U, 0x60), 0xB3000000u, 0xFFFFFFFFu, &out);
        if (r || out == 0xD) call(0x2A6C0, 0, 0, 0, 0, U);
        a1b0(rh(pkt, 0), rh(pkt, 4), 1); return 1; }
    case 0x13: {
        uint32_t w = rd(pkt, 8); wr(U, 0xB0, w);
        if (rd(U, 0) == 0 && (w & 0xF0) && (w & 0xF)) {
            uint32_t m = 0;
            switch (w & 0xF0) { case 0x20: m = 0x51; break; case 0x30: m = 0x24; break; case 0x40: m = 1; break; case 0x10: m = 0x36; break;
                                case 0x70: m = 0x93; break; case 0x80: m = 0x15; break; case 0x90: m = 0x71; break; }
            if (m) call(0x6B300, m, 0, 0, 0, 0, U, 2, 0, 0);
        }
        uint32_t ax = call(0x24490, U, 0, 0, 0, 0, call(0xCFBE, 0, 0, 0, 0, 0, U) ? 3 : 0x13, 1);
        a1b0((ax & 0xFFFF0000u) | rh(pkt, 0), rh(pkt, 4), 1); return 1; }
    case 0x14:
        wr(U, 0xB0, rd(pkt, 8));
        call(0xCD2A, 0, 0, 0, 0, 0, esi); call(0xC38E, 0, 0, 0, 0, 0, esi, 0xB); call(0xCD36, 0, 0, 0, 0, 0, esi, 5);
        call(0xC3EE, 0, 0, 0, 0, 0, esi, rd(pkt, 0), rd(pkt, 4)); call(0xC376, 0, 0, 0, 0, 0, esi, U, 0);
        call(0x2A6C0, 0, 0, 0, 0, U); return 1;
    case 0x15: if (call(0x19030, U, 0, 0, U, pkt, 0) && call(0xA22B0, U, 0, 0, 0, 0, pkt)) return 1; goto fail;
    case 0x16: if (call(0x19030, U, 0, 0, U, pkt, 0) && call(0xA30C0, 0, U, pkt, 0, 0)) return 1; goto fail;
    case 0x19:
        call(0x2A6C0, 0, 0, 0, 0, U);
        a1b0((pkt & 0xFFFF0000u) | rh(pkt, 0), rh(pkt, 4), 0); return 1;   // eax keeps the packet pointer's high half
    default: return ~0u;   // bf92/bf80 error report then _exit(-1)
    }
fail:
    call(0x19710, edi, U, 0, 0, 0, 1);
    return 0;
}
