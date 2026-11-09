// 1.13c D2Client+0xA7E20 (ret 0xC): EAX = unit, stack P1, P2, P3. Re-seats a unit at room-relative coordinates (P1, P2): r = 0xA5A60 (EAX = P2, ECX = P1)
// resolves the room; the unit id (+0x2C) must be set and r must resolve when both coordinates are; a unit whose +0xC4 carries 0x10000, or whose
// (first dword, kind) is (0,0), (0,0x11), (1,0) or (1,0xC), is left alone. The player (global 0x11BBFC) first zeroes 0x11C2F8 and 0x11C2F4, then updates
// the active room/palette through Ordinal_10331/10310/c640/c66a/c1de/c5f2/d0cc/c214/d39c/c0b8; every unit then runs 0xC6A0 (id, unit, r, P1, P2), with
// 0xC676 over the {P1, P2} record when it answers 0 and a fatal when 0xC688 also answers 0; 0x11C204 := 2; the player runs 0x81720 (ESI = P3); 0xA7D50 (EDI = unit).
// Fatal codes 0x166 (no id), 0x168 (coordinates without a room), 0x17D (P3 with a populated unit), 0x1A9 (0xC688 refused).
#include <cstdint>
#include <emscripten.h>

struct D2ClientA7E20Seams {
    uint32_t (*a5a60)(uint32_t p2, uint32_t p1);
    void (*f81720)(uint32_t p3);
    void (*f10ea0)();
    void (*a7d50)(uintptr_t unit);
    uint32_t (*c1f0)(uintptr_t unit);
    uint32_t (*c310)(uintptr_t unit);
    uint32_t (*c640)(uint32_t m);
    void (*c66a)(uint32_t r, uint32_t p1, uint32_t p2, uint32_t flag);
    uint32_t (*c1de)(uint32_t room);
    uint32_t (*c5f2)(uint32_t x);
    void (*d0cc)(uint32_t flag);
    uint32_t (*c214)(uint32_t x);
    void (*d39c)();
    void (*c0b8)(uint32_t b);
    uint32_t (*c6a0)(uint32_t id, uintptr_t unit, uint32_t r, uint32_t p1, uint32_t p2);
    uint32_t (*c4c0)(uintptr_t unit);
    void (*c676)(uint32_t r, const uint32_t* rec, uint32_t v, uint32_t code, uint32_t one);
    uint32_t (*c688)(uint32_t id, uintptr_t unit, uint32_t r, uint32_t p1, uint32_t p2);
    void (*fatal)(uint32_t code);
};
static inline uint32_t R(uintptr_t p) { return *(const uint32_t*)p; }

// 1.13c D2Client+0xA7E20. g2f4 -> {0x11C2F4, 0x11C2F8}; g204 -> 0x11C204; tbl = the dword at 0xCEE50 (a record whose byte +0x18 is tested).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a7e20(uintptr_t unit, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t player, uint32_t* g2f4,
                                                       uint32_t* g204, uintptr_t tbl, const D2ClientA7E20Seams* s) {
    if ((uint32_t)unit == player) { g2f4[1] = 0; g2f4[0] = 0; }
    uint32_t r = s->a5a60(p2, p1);
    uint32_t id = R(unit + 0x2c);
    if (id == 0) { s->fatal(0x166); return; }
    if (p1 != 0 && p2 != 0 && r == 0) { s->fatal(0x168); return; }
    if (R(unit + 0xc4) & 0x10000) return;
    uint32_t type = R(unit), kind = R(unit + 0x10);
    if (type == 0) { if (kind == 0 || kind == 0x11) return; }
    else if (type == 1) { if (kind == 0 || kind == 0xc) return; }
    if ((uint32_t)unit == player) {
        uint32_t own = s->c1f0(unit);
        if (own != 0) {
            bool skip = false;
            uint32_t m = s->c310(unit);
            if (m != 0) {
                uint32_t p = s->c640(m);
                if (p != 0 && (*(const uint8_t*)(tbl + 0x18) & *(const uint8_t*)(uintptr_t)(p + 8)) != 0) { p3 = 0; skip = true; }
            }
            if (!skip && p3 != 0) {
                if (type != 0) { s->fatal(0x17d); return; }
                s->f10ea0();
            }
        }
        if (r != 0) {
            s->c66a(r, p1, p2, own != r ? 1u : 0u);
            uint32_t y = s->c5f2(s->c1de(r));
            s->d0cc((y & 0xff) == 0 ? 1u : 0u);
            if (own != 0) {
                uint32_t a = s->c214(s->c1de(own));
                uint32_t b = s->c214(s->c1de(r));
                uint8_t ba = *(const uint8_t*)(uintptr_t)(a + 2), bb = *(const uint8_t*)(uintptr_t)(b + 2);
                if (ba != bb) { s->d39c(); s->c0b8(bb); }
            }
        }
    }
    if (s->c6a0(id, unit, r, p1, p2) == 0) {
        uint32_t rec[2] = {p1, p2};
        s->c676(r, rec, s->c4c0(unit), 0x1c09, 1);
        if (s->c688(id, unit, r, p1, p2) == 0) { s->fatal(0x1a9); return; }
    }
    *g204 = 2;
    if ((uint32_t)unit == player) s->f81720(p3);
    s->a7d50(unit);
}
