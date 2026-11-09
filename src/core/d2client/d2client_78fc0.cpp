// 1.13c D2Client+0x78FC0: the mouse-up handler of the control bar (stdcall, one argument: the window message, x word +0xC,
// y word +0xE, result +0x18/+0x1C). With the bar armed it runs the pressed button (left: the mini-panel toggle and the
// screen shift; right: the same with its own hit box; the third: one fixed click); otherwise it resolves a click on a belt
// slot (right half of the screen only) and uses the potion there. Ends by handing the message on.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd(uint32_t a, uint32_t o = 0) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
inline int32_t rs(uint32_t a, uint32_t o = 0) { return (int32_t)rd(a, o); }
inline void wr(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
inline uint8_t rb(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
inline int32_t q4(int32_t v) { return v / 4; }   // cdq / and 3 / add / sar 2: truncates toward zero
}

// Addresses of the module globals (retail RVAs in the comments).
struct D2cGlobals78FC0 {
    uint32_t player;                    // 0x11BBFC
    uint32_t armed, belt, ready;        // 0x11BFB4 (cleared), 0x11BFB8 (cleared on use), 0x11BFC8
    uint32_t c3c4, c3dc, c3e0, c3f8, c3fc, shiftDir, shiftX;   // 0x11C3C4; 0x11C3DC/3E0/3F8/3FC; 0x11C414; 0x11C418
    uint32_t act, button;               // 0xE24F8 (belt row compared with the slot's byte +2), 0xE25F0 (1, 2, 3)
    uint32_t viewX, viewY;              // 0x11B9A0 / 0x11B9A4
    uint32_t scrW, scrH;                // 0xDBC48 / 0xDBC4C
    uint32_t panels;                    // 0xFAD80: 38 dwords; +0x8 +0x10 +0x3C +0x40 +0x50 +0x58 +0x90 +0x94 read alone
    uint32_t kinds, kindCount;          // 0xDD538: 38 pointers (kind at +0x10); 0xDD5D0 count (asserted 0x26)
    uint32_t block;                     // 0x113A68
    uint32_t beltArg, beltOn;           // 0x11971D / 0x119721 (unaligned)
    uint32_t a9e14, a9e18;              // 0xF9E14 / 0xF9E18
    uint32_t slotB, slotD;              // 0x113B38 / 0x113D38: per-slot state, cleared
};

struct D2cSeam78FC0 {
    void (*notify)(uint32_t msg);                          // 0x16300 (stdcall 1)
    uint32_t (*busy)(uint32_t eaxMsg);                     // 0x26320
    void (*pass)(uint32_t msg);                            // 0xBE84 (stdcall 1, also the tail jump)
    uint32_t (*hit)(int32_t eaxX, int32_t edxY);           // 0x779F0 (ESI = 0x22, stack 0x22)
    void (*bf8c)();                                        // 0xBF8C
    uint32_t (*assertFile)();                              // 0xBF92
    void (*assertMsg)(uint32_t fmtRva, uint32_t file, uint32_t line);   // 0xBF80 (cdecl)
    void (*exitp)(int32_t code);                           // 0x3664
    void (*beltUse)(uint32_t ediArg);                      // 0x47B20
    void (*click)(uint32_t ecx, uint32_t edx, uint32_t z); // 0xC2790 (stack 1)
    uint32_t (*a5440)(uint32_t* edx);                      // 0xA5440 (EAX = player; EDX is live across the call: *edx is its value afterwards)
    uint32_t (*str)(uint32_t moduleRva);                   // 0xD0B4 (stdcall 1: a module string)
    void (*show)(uint32_t eax, uint32_t s);                // 0x3CCC0
    void (*hide)(uint32_t eax, uint32_t s);                // 0x3CC60
    void (*be2e0)();                                       // 0xBE2E0
    void (*f77da0)();                                      // 0x77DA0
    void (*setShift)(int32_t ecx);                         // 0xD1B0
    void (*view)(uint32_t eax, int32_t ecx, uint32_t edx, uint32_t esi, int32_t s0);   // 0x8A4D0 (stack 1)
    int32_t (*maxCount)(uint32_t unit, uint32_t a, uint32_t b);   // 0xC250 (stdcall 3)
    int32_t (*count)(uint32_t inv);                        // 0xCB38 (stdcall 1)
    uint32_t (*item)(uint32_t inv, uint32_t i);            // 0xCB3E (stdcall 2)
    uint32_t (*txt)(uint32_t item);                        // 0x15B0 (EAX = ESI = item)
    uint32_t (*slot)(uint32_t item);                       // 0x17F0 (EAX)
    int32_t (*slotY)(uint8_t al, uint8_t cl);              // 0x77CA0
    int32_t (*slotX)(uint8_t al, uint8_t cl);              // 0x77C50
    uint32_t (*c4ea)(uint32_t ecxUnit, uint32_t edxItem, int32_t s0);   // 0xC4EA (stdcall 1)
    int32_t (*c718)(uint32_t unit, uint32_t r, uint32_t z);       // 0xC718 (stdcall 3)
    int32_t (*c316)(uint32_t unit, uint32_t v, uint32_t item, int32_t n);   // 0xC316 (stdcall 4)
    int32_t (*f17b0)(int32_t eax);                         // 0x17B0
    uint32_t (*f78220)();                                  // 0x78220
    void (*use)(uint32_t eax, uint32_t ecxItem);           // 0x14A10 (AL = 0x3B)
};

extern "C" void d2c_78fc0(const D2cGlobals78FC0* g, const D2cSeam78FC0* s, uint32_t msg) {
    const uint32_t p = rd(g->player);
    const int32_t x = *(const uint16_t*)(uintptr_t)(msg + 0xC), y = *(const uint16_t*)(uintptr_t)(msg + 0xE);
    if (!p) return;
    const uint32_t inv = rd(p, 4);
    s->notify(msg);
    if (s->busy(msg)) return;
    auto clear4 = [&] { wr(g->c3dc, 0); wr(g->c3e0, 0); wr(g->c3f8, 0); wr(g->c3fc, 0); };
    auto finish = [&] { wr(msg + 0x18, 1); wr(msg + 0x1C, 0); s->pass(msg); };
    if (!rd(g->ready)) { clear4(); finish(); return; }
    if (rd(g->armed)) {
        const uint32_t b = rd(g->button);
        wr(g->armed, 0);
        if (b == 1 || b == 2) {
            const int32_t hx = rs(g->scrW) - rs(g->viewX) - (b == 1 ? 0x95 : 0xDC);
            const int32_t hy = rs(g->viewY) + rs(g->scrH) - 0x3F;
            if (s->hit(hx, hy)) {
                const uint32_t was = rd(g->panels, 0x10);
                s->bf8c();
                if (rd(g->kindCount) != 0x26) { s->assertMsg(0xD359F, s->assertFile(), 0x139B); s->exitp(-1); }
                uint32_t edx = rd(g->panels, 0x10);
                bool out = false;
                if (!edx) {
                    if (rd(g->block)) { finish(); return; }
                    for (uint32_t i = 0; (int32_t)i < rs(g->kindCount); i++) {
                        if (rd(g->panels, 4 * i) != 1) continue;
                        const uint32_t k = rd(rd(g->kinds, 4 * i), 0x10);
                        if (k > 4 || k == 2 || k == 3) { out = true; break; }
                        if (k == 1) { s->click(i, 1, 0); edx = rd(g->panels, 0x10); }
                        else if (k == 4) { if (!rd(g->beltOn)) continue; s->beltUse(rd(g->beltArg)); edx = rd(g->panels, 0x10); }
                    }
                }
                if (out) { finish(); return; }
                const uint32_t pl = rd(g->player);
                if (!pl || (!rd(pl) && !s->a5440(&edx))) { edx = edx == 0; wr(g->panels + 0x10, edx); }
                if (!was) { if (edx) { const uint32_t t = s->str(0xD0604); s->show(2, t); } }
                else if (!edx) { const uint32_t t = s->str(0xD0604); s->hide(2, t); s->be2e0(); s->f77da0(); }
                const bool any = rd(g->panels, 0x8) || rd(g->panels, 0x50) || rd(g->panels, 0x3C) || rd(g->panels, 0x40) ||
                                 rd(g->panels, 0x90) || rd(g->panels, 0x94) || rd(g->panels, 0x58);
                if (any ? !edx : edx != 0) {
                    const int32_t q = any ? q4(rs(g->scrW)) : -q4(rs(g->scrW));
                    wr(g->shiftX, (uint32_t)q); wr(g->shiftDir, any ? 2 : 1);
                    s->setShift(q);
                    const int32_t c = rs(g->a9e14), r = q4(c);
                    s->view(rd(g->a9e18), any ? c + r : c - r, 0, rd(g->c3c4), any ? r : -r);
                } else {
                    wr(g->shiftX, 0); wr(g->shiftDir, any ? 3 : 0);
                    s->setShift(0);
                    s->view(rd(g->a9e18), rs(g->a9e14), 0, rd(g->c3c4), 0);
                    if (any) clear4();
                }
            }
        } else if (b == 3) {
            const int32_t hx = rs(g->scrW) - rs(g->viewX) - 0x131, hy = rs(g->viewY) + rs(g->scrH) - 0x3F;
            if (s->hit(hx, hy)) s->click(4, 2, 0);
        }
        finish();
        return;
    }
    if (x <= rs(g->scrW) / 2) { wr(msg + 0x18, 0); return; }
    const int32_t cap = s->maxCount(p, 5, 0);
    if (cap > 0 && s->count(inv) > 0) {
        for (uint32_t i = 0;;) {
            const uint32_t it = s->item(inv, i);
            const uint32_t t = s->txt(it);
            if (t) {
                const uint32_t d = s->slot(it);
                if (d) {
                    bool on = false;
                    if (rs(g->act) == (int32_t)(int8_t)rb(d, 2)) {
                        const int32_t a = s->slotY(rb(d, 3), rb(d, 2));
                        if (y > a - 0x30 && y < a) {
                            const int32_t bx = s->slotX(rb(d, 4), (uint8_t)y);
                            on = x > bx && x < bx + 0x30;
                        }
                    }
                    if (on && rs(g->slotD, 4 * i) == 1) {
                        bool ok = true;
                        if (rs(t, 0x170) != -1) {
                            const uint32_t r = s->c4ea(p, it, -1);
                            const int32_t n = r ? s->c718(p, r, 0) : 0;
                            ok = s->c316(p, rd(t, 0x170), it, n) <= cap;
                        }
                        if (ok) {
                            const uint32_t r = s->c4ea(p, it, -1);
                            const int32_t n = r ? s->c718(p, r, 0) : 0;
                            if (n <= s->f17b0(*(const int16_t*)(uintptr_t)t) && !s->f78220()) { s->use(0x3B, it); wr(g->belt, 0); }
                        }
                        wr(g->slotD + 4 * i, 0); wr(g->slotB + 4 * i, 0);
                        finish();
                        return;
                    }
                    if (rs(g->slotD, 4 * i) != -1) { wr(g->slotD + 4 * i, 0); wr(g->slotB + 4 * i, 0); }
                }
            }
            i++;
            if ((int32_t)i >= s->count(inv)) break;
        }
    }
    clear4();
    finish();
}
