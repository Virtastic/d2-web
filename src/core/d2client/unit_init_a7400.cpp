// 1.13c D2Client+0xA7400 (ret 0x14): EAX = a, ECX = unit record, EDX = d, stack w0, w1, kind, w3, w4. Fills a freshly allocated client unit: the +0xC8 bit
// 0x2000000 follows the dword at 0x119854; +0 := kind, +4 := w0, +0xC := d, +0xD4 and +8 := 0; when a or w1 is set the room 0xA5A60 (EAX = w1, ECX = a)
// yields +0x18 (Ordinal_10218 of Ordinal_10206 of the room) and +0x1C := the dword at 0x11C3B8; Ordinal_10036 (ECX = +0x20) clears the sub-record; kind 4 runs
// Ordinal_10012(0, unit) and Ordinal_10084(unit); a room advances its 64-bit seed (+0x6C, +0x70 times 0x6AC690C5 plus the high word) into +0x28 and
// Ordinal_10162 (ECX = +0x20, EDX = seed). The kind then selects a creator (0: 0x191E0, 2: 0x25490, 3: 0xB0170, 4: 0x82820, 5: 0xA5A30; 1 is fatal 0x202; 0 and 1
// fall to the free 0xC5C8 when the creator path ran the default arm) and 0xA7360 (EAX = unit) yields the result, which for kind 2 runs 0x25B70 (ECX = result).
#include <cstdint>
#include <emscripten.h>

struct D2ClientA7400Seams {
    uint32_t (*a5a60)(uint32_t w1, uint32_t a);
    uint32_t (*c1de)(uint32_t room);
    uint32_t (*c1ea)(uint32_t x);
    void (*c634)(uintptr_t sub);
    void (*c61c)(uint32_t zero, uintptr_t unit);
    void (*c664)(uintptr_t unit);
    void (*c3b2)(uintptr_t sub, uint32_t seed);
    void (*f191e0)(uint32_t room, uintptr_t unit, uint32_t a, uint32_t w1);
    void (*fatal)(uint32_t code);
    void (*c6b8)(uintptr_t unit);
    void (*f25490)(uint32_t w1, uintptr_t unit, uint32_t al, uint32_t room, uint32_t a, uint32_t w0);
    void (*fb0170)(uint32_t room, uint32_t a, uint32_t w1);
    void (*f82820)(uint32_t w3, uint32_t a, uint32_t w1, uint32_t one);
    void (*fa5a30)(uint32_t ebx, uint32_t a);
    void (*f16950)();
    void (*c5c8)(uintptr_t unit, uint32_t file, uint32_t line);
    uintptr_t (*a7360)(uintptr_t unit);
    void (*f25b70)(uintptr_t res);
};
static inline uint32_t R(uintptr_t p) { return *(const uint32_t*)p; }
static inline void W(uintptr_t p, uint32_t v) { *(uint32_t*)p = v; }

// 1.13c D2Client+0xA7400. mode = the dword at 0x119854, c3b8 = the dword at 0x11C3B8.
extern "C" EMSCRIPTEN_KEEPALIVE uintptr_t d2_client_a7400(uint32_t a, uintptr_t unit, uint32_t d, uint32_t w0, uint32_t w1, uint32_t kind, uint32_t w3,
                                                           uint32_t w4, uint32_t mode, uint32_t c3b8, const D2ClientA7400Seams* s) {
    uintptr_t room = 0;
    if (unit != 0) {
        uint32_t f = R(unit + 0xc8);
        W(unit + 0xc8, mode != 0 ? (f | 0x2000000u) : (f & 0xfdffffffu));
    }
    W(unit, kind); W(unit + 4, w0); W(unit + 0xc, d); W(unit + 0xd4, 0); W(unit + 8, 0);
    if (a != 0 || w1 != 0) {
        room = s->a5a60(w1, a);
        *(uint8_t*)(unit + 0x18) = (uint8_t)s->c1ea(s->c1de((uint32_t)room));
        W(unit + 0x1c, c3b8);
    }
    s->c634(unit + 0x20);
    if (kind == 4) { s->c61c(0, unit); s->c664(unit); }
    if (room != 0) {
        uint64_t m = (uint64_t)R(room + 0x6c) * 0x6ac690c5u + R(room + 0x70);
        W(room + 0x70, (uint32_t)(m >> 32)); W(room + 0x6c, (uint32_t)m); W(unit + 0x28, (uint32_t)m);
        s->c3b2(unit + 0x20, (uint32_t)m);
    }
    bool dflt = true;
    switch (kind <= 5 ? kind : 6) {
        case 0: s->f191e0((uint32_t)room, unit, a, w1); break;
        case 1: s->fatal(0x202); return 0;
        case 2: s->c6b8(unit); s->f25490(w1, unit, w4 & 0xff, (uint32_t)room, a, w0); break;
        case 3: s->fb0170((uint32_t)room, a, w1); break;
        case 4: s->c6b8(unit); s->f82820(w3, a, w1, 1); break;
        case 5: s->c6b8(unit); s->fa5a30(w1, a); dflt = false; break;
        default: break;
    }
    if (dflt) {
        s->f16950();
        if (kind == 0 || kind == 1) s->c5c8(unit, 0x6fb84144u, 0x21d);
    }
    uintptr_t res = s->a7360(unit);
    if (res != 0 && kind == 2) s->f25b70(res);
    return res;
}
