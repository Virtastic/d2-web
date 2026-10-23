// D2Client rva 0x8CFB0: stat-point / skill-point button + count text.
// Globals in a flat state struct, D2Win/D2Common ordinals injected.
#include <cstdint>

struct D2Client8CFB0State {
    uint32_t statOwner;   // 0x1CBBFC (unit)
    uint32_t wide;        // 0x1C9854
    uint32_t a9a4, a9a0;  // 0x1CB9A4, 0x1CB9A0
    uint32_t bc4c, bc48;  // 0x18BC4C, 0x18BC48
    uint32_t mouseX, mouseY; // 0x1CB824, 0x1CB828
    uint32_t be80, be7c;  // 0x1CBE80, 0x1CBE7C
    uint32_t bc7c;        // 0x1CBC7C
};

struct D2Client8CFB0Callees {
    uint32_t (*stat)(uint32_t unit, uint32_t id, uint32_t layer); // D2Common #10973
    void (*ord10003)(int32_t a, int32_t b, int32_t c);
    void (*ord10085)();
    void (*ord10028)();
    void (*text)(int32_t x, int32_t y, int32_t z);                // #10150, after win2Unicode of "%d"
    void (*fmt)(uint32_t value);                                  // #578 + Unicode::win2Unicode
    void (*ord10041)(uint32_t flag, uint32_t extra, int32_t x, int32_t y, int32_t c, int32_t d, int32_t e);
};

// 1.13c D2Client+0x8CFB0
extern "C" void d2_client_8cfb0(const D2Client8CFB0State& s, const D2Client8CFB0Callees& k, int32_t mode) {
    int32_t x = 0, y = 0, tx = 0;
    uint32_t flag = 0, v;
    const int32_t base = (int32_t)(s.a9a4 + s.bc4c);
    if (mode == 0) {
        v = k.stat(s.statOwner, 0xf, 0);
        x = base - (s.wide ? 0x1b8 : 0xf4);
        y = base - (s.wide ? 0x1b6 : 0xf2);
        tx = (int32_t)s.a9a0 + 0x49;
        flag = s.be80 != 0;
        const int32_t lo = base - (s.wide ? 0x1c8 : 0x104), hi = base - (s.wide ? 0x1b6 : 0xf2);
        if (tx <= (int32_t)s.mouseY && (int32_t)s.mouseY <= (int32_t)s.a9a0 + 0x5d &&
            lo <= (int32_t)s.mouseX && (int32_t)s.mouseX <= hi) {
            k.ord10003(base - (s.wide ? 0x1ca : 0x106), 0, 0);
            k.ord10085();
        }
    } else if (mode == 1) {
        v = k.stat(s.statOwner, 0xe, 0);
        x = base - 0x48;
        tx = (int32_t)(s.bc48 - s.a9a0) - 0xed;
        y = base - 0x45;
        flag = s.be7c != 0;
    } else {
        v = k.stat(s.statOwner, 0xf, 0);
        k.ord10003(0, 0, 0);
        k.ord10028();
        x = base - 0x6a;
    }
    if (mode == 3) { k.ord10003(x, 0, 0); k.text(0, 0, 0); return; }
    k.fmt(v);
    if (mode == 2) k.ord10028();
    k.text(x, 0, 0);
    if (mode == 0) { if (s.be80) y++; tx += 2; }
    else if (mode == 1) { if (s.be7c) y++; tx += 1; }
    else { k.ord10003(x, 0, 0); k.text(0, 0, 0); return; }
    k.ord10041(flag, s.bc7c, tx, y - 2, -1, 5, 0);
}
