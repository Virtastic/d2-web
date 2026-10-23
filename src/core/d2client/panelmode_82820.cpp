// D2Client rva 0x82820: set the panel/UI mode on a state block.
// EAX = new mode, ESI = state block, stack arg = redraw flag. Callees are Ordinal_* imports, injected.
#include <cstddef>
#include <cstdint>

struct D2Client82820State {
    uint8_t p0[0xc];
    uint32_t c;       // +0x0c passed to Ordinal_11013
    uint32_t mode;    // +0x10
    uint8_t p1[0x4c];
    uint32_t f60;     // +0x60
    uint8_t p2[0x60];
    uint32_t flags;   // +0xc4
};
static_assert(offsetof(D2Client82820State, mode) == 0x10 && offsetof(D2Client82820State, f60) == 0x60 &&
              offsetof(D2Client82820State, flags) == 0xc4, "D2Client 0x82820 state");

struct D2Client82820Callees {
    void (*o10408)(); void (*o10650)(); void (*o10574)(); void (*o10032)(); void (*o10857)();
    void (*o10595)(); void (*c88A70)(uint32_t, uint32_t, uint32_t, uint32_t); void (*c827C0)();
    void (*o10977)(); void (*o11166)(); void (*o11013)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    void (*o10807)(); uint32_t (*o10757)(); void (*o10387)(uint32_t);
};

// 1.13c D2Client+0x82820
extern "C" void d2_client_82820(int32_t mode, uint32_t redraw, D2Client82820State* s, const D2Client82820Callees& k) {
    if (mode >= 7) return;
    if ((s->mode == 5 || s->mode == 3) && mode != 5 && mode != 3) k.o10408();
    s->mode = (uint32_t)mode;
    k.o10650();
    switch (mode) {
    case 3: k.o10574(); k.o10032(); k.o10857(); s->flags |= 2; k.o10595(); break;
    case 4: k.c88A70(0, 0, 0, 0); s->flags &= ~2u; break;
    case 0: case 1: case 2: case 6: s->flags &= ~2u; break;
    case 5: k.o10574(); k.o10032(); k.o10857(); s->flags &= ~2u; k.o10595(); k.c827C0(); break;
    default: return;   // negative mode: stored, no flag fixup
    }
    s->flags = (s->flags & 0xfffffff3u) | 0x20;
    if (redraw) {
        k.o10977(); k.o11166(); k.o11013(0, 0x40, 0, 4, s->c); k.o10807();
        if (k.o10757() != 0 && s->f60 == 0) k.o10387(0);
    }
}
