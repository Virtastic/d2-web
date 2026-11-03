// 1.13c D2Client+0x125A0 -- per-frame draw of the 15-row list panel (rows, scrollbar, tabs, toast).
// Decompile-derived from the 1.13c disassembly.
// Globals are handed in as images of the retail ranges (rva noted per field); callees go through Cb in retail's call order.
#include <cstdint>
#include <cstring>
#include <cstddef>

namespace d2client_125a0 {
using I = int32_t; using U = uint32_t;
// image of 0x11C4FC..0x11C553 (22 dwords)
struct Panel {
    I rows;        // 11C4FC  row count (loop bound)
    I cells;       // 11C500  scrollbar thumb cell count
    I r504, flagDown;   // 11C504, 11C508 (mouse-down token)
    I r50c, r510;
    I up;          // 11C514  up-button pressed
    I down;        // 11C518  down-button pressed
    I r51c, r520, r524, r528, r52c, r530;
    I scroll;      // 11C534  first visible row
    I sel;         // 11C538  selected row
    I region;      // 11C53C  hovered tab
    I active;      // 11C540  selection active
    U cellFile;    // 11C544  scrollbar cel file (word 13 of the cel block)
    I r548;
    U toastId;     // 11C54C  toast string id
    U toastTick;   // 11C550  toast start tick
};
static_assert(sizeof(Panel) == 0x58, "panel image");
static_assert(offsetof(Panel, flagDown) == 0xC && offsetof(Panel, up) == 0x18 && offsetof(Panel, down) == 0x1C, "panel offsets");
static_assert(offsetof(Panel, scroll) == 0x38 && offsetof(Panel, sel) == 0x3C && offsetof(Panel, region) == 0x40, "panel offsets");
static_assert(offsetof(Panel, active) == 0x44 && offsetof(Panel, cellFile) == 0x48 && offsetof(Panel, toastId) == 0x50 && offsetof(Panel, toastTick) == 0x54, "panel offsets");
// image of 0x1236D0..0x1236DF
struct Last { I y; I x; U frame; I pad; };
static_assert(sizeof(Last) == 16, "last image");
struct G {
    I mouseY, mouseX;   // 11B824, 11B828
    I W, H;             // DBC48, F9E18
    I thumbY;           // 1233F4
    const Panel* p;
    Last* last;         // 1236D0 (written)
    const uint8_t* ui;  // F8B98..F8BEB: three tabs, stride 0x1A (word @0, dword @6, dword @0xA); dword @0x50 = mode (F8BE8)
    const uint8_t* rows;   // *(F8B94): stride 10, dword id @0, word cel @4
    const uint8_t* keys;   // 11A2E0..: 114 entries, stride 10 (dword id @0, word kind @4, dword flag @6)
};
struct Cb {
    void (*pre)();                                   // 11410
    void (*mouse)(I x, I y);                         // 11A80, stdcall (mouseX, mouseY)
    U (*font)(U id);                                 // D3A8, returns the previous font
    U (*lang)(U ecx);                                // D1EC, ecx (the id is cx)
    void (*text)(U str, I x, I y, I a, I b);         // D37E, ecx = str, edx = x, stack y a b
    I (*width)(U str);                               // D35A
    void (*fill)(I x0, I y0, I x1, I y1, I c, I f);  // D0BA
    void (*cel)(const U* blk, I x, I y, I m1, I mode, I last);  // D096, blk = 18 dwords
    void (*rect)(const I* r);                        // BE4C0, eax = 4 dwords
    U (*label)(U id, U flag);                        // 391E0, esi = id, edx = flag
    I (*known)(U id, U flag);                        // 398C0, edx = id, esi = flag
    U (*tick)();                                     // GetTickCount
};
static inline I d2(I v) { return (v - (v >> 31)) >> 1; }                       // cdq; sub; sar
static inline I d3(I v) { I h = (I)(((int64_t)v * 0x55555556LL) >> 32); return h + (I)((U)h >> 31); }   // imul 0x55555556; shr 31; add
static inline U r32(const uint8_t* p) { U v; memcpy(&v, p, 4); return v; }
static inline U r16(const uint8_t* p) { uint16_t v; memcpy(&v, p, 2); return v; }

// label colour of a row (retail 0x2919..0x297B, second pass 0x29FE..0x2A58): the key table picks the lookup; the second pass
// runs 0x398C0 with flag 0 on both calls, the first with flag 0 then 1.
static I labelColour(const G& g, const Cb& cb, U id, bool second) {
    I i = 0;
    for (; i < 114; ++i) if (r32(g.keys + 10 * i) == id && r32(g.keys + 10 * i + 6) == 1) break;
    if (i < 114 && r16(g.keys + 10 * i + 4) != 0xFFFF) return cb.known(id, second ? 0 : 1) ? 4 : 5;
    if (!cb.known(id, 0)) return 1;
    return cb.known(id, second ? 0 : 1) ? 4 : 5;
}

void run(const G& g, const Cb& cb) {
    const Panel& p = *g.p;
    const I mx = g.mouseX, my = g.mouseY, W = g.W, H = g.H;
    cb.pre();
    if (g.last->x != mx || g.last->y != my || p.flagDown) { g.last->x = mx; g.last->y = my; cb.mouse(mx, my); }
    const U oldFont = cb.font(0xD);
    const I wx = d2(W - 0x26C), hy = d2(H - 0x1A4);
    cb.fill(wx, hy, W - wx, hy + 0x171, 0, 1);
    { I r[4] = { wx, hy + 3, W - wx - 3, hy + 0x170 }; cb.rect(r); r[1] = hy + 0x143; cb.rect(r); }
    const I q = d3(W - 2 * wx - 0x31);
    cb.text(cb.lang(0xF51), wx + 0x12, hy + 0x1E, 4, 0);
    cb.text(cb.lang(0xF52), wx + q + 0x12, hy + 0x1E, 4, 0);
    cb.text(cb.lang(0xF53), wx + 2 * q + 0x12, hy + 0x1E, 4, 0);
    I rowY = 0;
    const I mode = (I)r32(g.ui + 0x50);
    for (I i = 0; i < 15 && i < p.rows; ++i) {
        const I y = rowY + hy + 0x22, idx = p.scroll + i;
        rowY += 0x12;
        const bool dim = !(p.active && idx == p.sel) || ((g.last->frame & 0xF) > 4);
        const uint8_t* row = g.rows + 10 * idx;
        const U id = r32(row);
        if (id == 0x39) continue;
        const I ty = y + 0x12;
        if (idx == p.sel) {
            const I x1 = wx + 0x12 + 3 * q;
            cb.fill(wx + 0x12, y + 1, x1, y + 0x13, 0, 1);
            cb.text(cb.lang(((U)x1 & 0xFFFF0000u) | r16(row + 4)), wx + 0x1E, ty, p.active ? 3 : 0, 0);   // ecx high half: D0BA leaves ecx = x1 (the capture's `e` hook)
        } else {
            cb.text(cb.lang(r16(row + 4)), wx + 0x1E, ty, 0, 0);   // ecx = i+1 (small) at the load
        }
        if (dim || !mode) {
            const I c = (idx == p.sel && (p.active || mode == 1)) ? 3 : labelColour(g, cb, id, false);
            cb.text(cb.label(id, 1), wx + q + 0x1E, ty, c, 0);
        }
        if (dim || mode) {
            const I c = (idx == p.sel && (p.active || !mode)) ? 3 : labelColour(g, cb, id, true);
            cb.text(cb.label(id, 0), wx + 2 * q + 0x1E, ty, c, 0);
        }
    }
    U blk[18] = {};
    blk[13] = p.cellFile;
    blk[0] = 0xD;
    for (I j = 0; j < 21; ++j) cb.cel(blk, W - wx - 0x1F, hy + 0x3B + 12 * j, -1, 5, 0);
    blk[0] = 0xE;
    for (I j = 0; j < p.cells; ++j) cb.cel(blk, W - wx - 0x20, 12 * j + g.thumbY + 0xC, -1, 5, 0);
    const bool inX = mx >= W - wx - 0x1F && mx <= W - wx - 0x12;
    blk[0] = (p.up && inX && my >= hy + 0x23 && my <= hy + 0x2F) ? 9 : 0xB;
    cb.cel(blk, W - wx - 0x1F, hy + 0x2F, -1, 5, 0);
    blk[0] = (p.down && inX && my >= hy + 0x125 && my <= hy + 0x131) ? 8 : 0xA;
    cb.cel(blk, W - wx - 0x1F, hy + 0x131, -1, 5, 0);
    for (I k = 0; k < 3; ++k) {
        const uint8_t* e = g.ui + 0x1A * k;
        const U color = k == p.region ? r32(e + 6) : 4;
        cb.text(cb.lang((color & 0xFFFF0000u) | r16(e)), (I)r32(e + 0xA) + 0xA, hy + 0x15F, (I)color, 0);
    }
    if (cb.tick() < p.toastTick + 0x7D0u) {
        const I w = cb.width(cb.lang(p.toastId));
        cb.text(cb.lang(p.toastId), d2(W) - d2(w), hy + 0x18F, 1, 1);
    }
    cb.font(oldFont);
    g.last->frame += 1;
}
}  // namespace d2client_125a0

extern "C" void d2_client_125a0(const d2client_125a0::G* g, const d2client_125a0::Cb* cb) { d2client_125a0::run(*g, *cb); }
