// 1.13c D2Client+0x619E0 automap unit marker (EAX = unit or 0). Every callee is a callback and every module global is a parameter.
// Callee conventions are the ones the retail call sites use (a hidden register or stack word is a plain parameter here).
#include <cstdint>

namespace d2client_619e0 {
struct Globals {  // module words, one field each
    int32_t div;        // 0xF16B0 (divisor of the world-to-cell conversion; retail idiv, never zero)
    int32_t flag_b8;    // 0xF16B8
    int32_t flag_bc;    // 0xF16BC
    int32_t org_x;      // 0x11C1F8
    int32_t org_y;      // 0x11C1FC
    uint8_t col_c213;   // 0x11C213
    int32_t x_lo, y_lo, x_hi, y_hi;   // 0x11C228, 0x11C22C, 0x11C230, 0x11C234
    uint32_t local_unit;  // 0x11BBFC
};
struct Env {
    int32_t (*gate)(const uint8_t* unit, int32_t sel);                 // 0xC1FC (unit, 7)
    int32_t (*colour)(const uint8_t* unit, uint32_t* out);             // 0x61490 (EAX unit, EBX &colour)
    int32_t (*px)(const uint8_t* unit);                                // 0xC1D2
    int32_t (*py)(const uint8_t* unit);                                // 0xC1F6
    void (*mark)(int32_t y, int32_t x, uint32_t colour);               // 0x5F1C0 (EAX y, ECX x, stack colour)
    const uint8_t* (*nameOf)(const void* unit);                        // 0xC7DE
    void (*label)(const uint8_t* name, int32_t x, int32_t y, int32_t mode);  // 0x601A0 (EBX name, stack x y mode)
    uint32_t (*cls_test)(int32_t cls);                                 // 0x1760 (EAX class or -1, EDX 9)
    void* (*ctx)();                                                    // 0xD3A8 (ECX 6)
    void (*ctx_back)(void* c);                                         // 0xD3A8 (ECX saved)
    void* (*cell)(const uint8_t* unit);                                // 0xA5D90 (EAX unit)
    int32_t (*width)(void* h, int32_t y, int32_t a, int32_t b);        // 0xD35A (ECX h, stack y a b)
    void (*place)(void* h, int32_t x);                                 // 0xD37E (ECX h, EDX x)
    void* (*text)(int32_t id);                                         // 0xD1EC (ECX id)
    void (*query)(const uint8_t* unit, uint32_t* q);                   // 0xC658 (unit, &q[0] kind, &q[2] class, &q[1] mode, file, line); q = {kind, mode, class}
    int32_t (*check)(const uint8_t* unit, int32_t sel);                // 0xCBDA (unit, 0x25)
    const int32_t* (*rec)(const uint8_t* unit);                        // 0xA60D0 (EAX unit)
    int32_t (*idx)(const void* unit);                                  // 0xA5B60 (EAX unit)
};

static inline int32_t rd(const uint8_t* p, uint32_t o) { return *(const int32_t*)(p + o); }

static void mid(const Env& e, const Globals& g, const uint8_t* u, int32_t x, int32_t y8, uint32_t col, int32_t kind) {
    (void)kind;
    if (!g.flag_bc || !u || rd(u, 4) != 0x10b) { e.mark(y8, x, col); return; }
    void* c = e.ctx();
    void* t = e.text(0xcf3);
    int32_t w = e.width(t, y8 - 10, (int32_t)(col & 0xFF), 0);
    e.place(t, x - ((w - (w >> 31)) >> 1));
    e.ctx_back(c);
}

// 1.13c D2Client+0x619E0
void sub_619E0(const Env& e, const Globals& g, const uint8_t* u) {
    int32_t kind = u ? rd(u, 0) : 6;
    if (kind == 0) {
        bool call = (rd(u, 0xc4) & 0x10000) != 0;
        if (!call) { int32_t m = rd(u, 0x10); call = (m == 0 || m == 0x11); }
        if (call && !e.gate(u, 7)) return;
    }
    uint32_t col = 0;
    if (!e.colour(u, &col)) return;
    int32_t px = e.px(u), py = e.py(u);
    int32_t x = px / g.div - g.org_x + 8;
    int32_t y8 = py / g.div - g.org_y - 8;
    if (x < g.x_lo || x > g.x_hi || y8 < g.y_lo || y8 > g.y_hi) return;
    if (kind == 0) {
        if (g.flag_b8 || (uint8_t)col != g.col_c213) e.mark(y8, x, col);
        if (g.flag_bc && g.flag_b8 && (uint32_t)(uintptr_t)u != g.local_unit)
            e.label(e.nameOf(u), x, y8, (uint8_t)col == g.col_c213 ? 2 : 1);
    } else if (kind == 1) {
        e.mark(y8, x, col);
        if (!g.flag_bc) return;
        if (e.cls_test(u ? rd(u, 4) : -1)) {
            void* c = e.ctx();
            void* h = e.cell(u);
            int32_t w = e.width(h, y8 - 10, 4, 0);
            e.place(h, x - ((w - (w >> 31)) >> 1));
            e.ctx_back(c);
        }
        if (!g.flag_bc || !g.flag_b8) return;
        uint32_t q[3] = { (uint32_t)kind, (uint32_t)rd(u, 0x10), (uint32_t)rd(u, 4) };
        e.query(u, q);
        if (q[0] != 0) return;
        if (!e.check(u, 0x25)) return;
        const int32_t* p = e.rec(u);
        if (!p || *p != 0) return;
        int32_t a = (int16_t)e.idx((const void*)(uintptr_t)g.local_unit);
        if (a != -1) { int32_t b = (int16_t)e.idx(u); if ((int16_t)a == (int16_t)b) return; }
        e.label(e.nameOf(p), x, y8, 1);
    } else if (kind == 2) {
        mid(e, g, u, x, y8, col, kind);
    }
}
}
