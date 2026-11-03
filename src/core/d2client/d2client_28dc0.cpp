// 1.13c D2Client+0x28DC0: belt panel. Every callee goes through the injected dispatcher keyed by RVA;
// the Unicode imports are keyed by their IAT slot RVA (0xCEB00 ctor, 0xCEAB0 strlen, 0xCEAB8 strcpy). Module string pointers are 0xB2000000|rva.
#include "d2client_callee.h"
#include <cstring>
struct D2c28dc0G {   // the module globals the function reads and writes, in the order the FN descriptor pins them
    int32_t w, h;                                    // 0xDBC48, 0xDBC4C
    uint32_t state, adfc, ada4, cell, g99c;          // 0xF5384 (rw), 0xFADFC, 0xFADA4, 0x11A774, 0x11B99C
    uint32_t en, g46c, g470, g474;                   // 0x11C468, 0x11C46C (rw), 0x11C470, 0x11C474 (w)
    uint32_t hot[4];                                 // 0xF9F74
    uint8_t col[4];                                  // 0x11A7D0..3 (w)
};
static_assert(sizeof(D2c28dc0G) == 64, "layout");
static inline uint32_t rd32(const uint8_t* p) { uint32_t x; std::memcpy(&x, p, 4); return x; }
// 1.13c D2Client+0x28DC0
extern "C" void d2c_28dc0(const D2cSvc& v, D2c28dc0G* g, uint32_t player, uint32_t item, const uint8_t* tbl) {
    d2c_call(v, 0xd3a8, 0, 1, 0, 0, 0);
    const uint32_t inv = *(const uint32_t*)(uintptr_t)(player + 0x60);
    const uint32_t belt = d2c_call(v, 0xc262, 0, 0, 0, 0, 0, inv, 8);
    if (!belt) g->state = 2;
    else if (d2c_call(v, 0xc220, 0, 0, 0, 0, 0, belt) == 0x13) {
        const int32_t r = (int32_t)d2c_call(v, 0xcf1c, 0, 0, 0, 0, 0, belt);
        if (r >= 0) g->state = (uint32_t)r;
        else {
            const uint32_t m = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, 0xb07);
            d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xB2000000u | 0xd359f, m);
            d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);   // does not return in retail
            g->state = 2;
        }
    }
    if (!g->adfc && !g->g470) g->g46c = 0;
    if (g->ada4) g->g46c = 0;
    g->g474 = 0;
    if (g->g46c) {
        uint8_t b[3] = { 1, 1, 1 };
        switch (g->state) {
            case 0: case 5: b[2] = 0; g->g474 = 1; break;
            case 1: case 4: b[1] = b[2] = 0; g->g474 = 1; break;
            case 2: b[0] = b[1] = b[2] = 0; break;
            default: g->g474 = 1; break;
        }
        uint32_t blk[18] = {}; blk[13] = g->cell;
        for (int i = 0; i < 3; ++i) if (b[i]) {
            const int32_t y = g->h - 0x20 * i - 0x29;
            const int32_t x = ((g->w - (y >> 31)) >> 1) + 0x15;
            d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, (uint32_t)x, (uint32_t)y, 0xffffffffu, 5, 0);
        }
    }
    uint8_t cnt[16] = {};
    d2c_call(v, 0xcf22, 0, 0, 0, 0, 0, g->state, g->g99c, (uint32_t)(uintptr_t)cnt);
    const uint8_t n = cnt[4];
    if (n) for (int32_t ebp = 0;;) {
        if (ebp > 3 && !g->g474) break;
        const uint32_t esi = d2c_call(v, 0xcf28, 0, 0, 0, 0, 0, inv, (uint32_t)ebp);
        if (esi) {
            const uint32_t ty = *(const uint32_t*)(uintptr_t)esi, aux = *(const uint32_t*)(uintptr_t)(esi + 0x2c);
            uint32_t edi;
            if (ty == 2 || (ty > 3 && ty <= 5)) edi = *(const uint32_t*)(uintptr_t)(aux + 0xc);
            else edi = aux ? d2c_call(v, 0xc3be, 0, 0, 0, 0, 0, aux) : 0;
            uint32_t o[4];
            auto cf46 = [&](uint32_t* p) { d2c_call(v, 0xcf46, 0, 0, 0, 0, 0, g->state, g->g99c, (uint32_t)(uintptr_t)p, edi); };
            auto box = [&](const uint32_t* p, uint32_t col) {
                d2c_call(v, 0xd0ba, 0, 0, 0, 0, 0, p[0], p[2], p[0] + 0x1d, p[2] + 0x1d, col, 0);
            };
            const bool dis = d2c_call(v, 0xc1a8, 0, 0, 0, 0, 0, esi, 4, 0x5d5, 0xB2000000u | 0xd6b38) ||
                             !d2c_call(v, 0xc814, 0, 0, 0, 0, 0, esi) || d2c_call(v, 0xca12, 0, 0, 0, 0, 0, esi, 2) ||
                             (player && d2c_call(v, 0xc1fc, 0, 0, 0, 0, 0, player, 0x36));
            if (!dis) {
                d2c_call(v, 0x265e0, 0, 0, 0, 0, 0);
                if (esi == item && g->en) { cf46(o); box(o, g->col[1]); }
            } else {
                for (int k = 0; k < 4; ++k) {
                    static const uint8_t rgb[4][3] = { { 0x80, 0, 0 }, { 0, 0x80, 0 }, { 0, 0, 0x80 }, { 0x80, 0x80, 0 } };
                    g->col[k] = (uint8_t)d2c_call(v, 0xd390, 0, 0, 0, 0, 0, rgb[k][0], rgb[k][1], rgb[k][2]);
                }
                cf46(o); box(o, g->col[0]);
            }
            cf46(o);
            d2c_call(v, 0x6b5a0, 0, 0, 0, esi, 0, o[0], o[2]);
            if (ebp < 4 && d2c_call(v, 0xc814, 0, 0, 0, 0, 0, esi)) {
                int hit = -1, ecx = 0;
                for (int i = 0; i < 114 && hit < 0; ++i) if (rd32(tbl + 10 * i) == g->hot[ebp] && rd32(tbl + 10 * i + 6) == 1) hit = i;
                if (hit >= 0 && (tbl[10 * hit + 4] | tbl[10 * hit + 5] << 8) != 0xffff) ecx = (int)(uintptr_t)(tbl + 10 * hit + 6);
                else {
                    hit = -1;
                    for (int i = 0; i < 114 && hit < 0; ++i) if (rd32(tbl + 10 * i) == g->hot[ebp] && rd32(tbl + 10 * i + 6) == 0) hit = i;
                    if (hit >= 0 && (tbl[10 * hit + 4] | tbl[10 * hit + 5] << 8) != 0xffff) ecx = hit * 5; else hit = -1;
                }
                if (hit >= 0) {
                    uint16_t buf[100];
                    for (int k = 0; k < 100; ++k) d2c_call(v, 0xceb00, 0, (uint32_t)(uintptr_t)&buf[k], 0, 0, 0);
                    const uint32_t src = d2c_call(v, 0x39f30, 0, (uint32_t)ecx, 1, 0, 0);
                    const uint32_t bp = (uint32_t)(uintptr_t)buf;
                    d2c_call(v, 0xceab8, 0, bp, src, 0, 0);
                    int32_t p = (int32_t)d2c_call(v, 0xceab0, 0, bp, 0, 0, 0);
                    int32_t w = (int32_t)d2c_call(v, 0xd35a, 0, bp, 0, 0, 0);
                    if (w > 0x1c) do { if ((uint32_t)p < 100) buf[p] = 0; --p; w = (int32_t)d2c_call(v, 0xd35a, 0, bp, 0, 0, 0); } while (w > 0x1c);
                    d2c_call(v, 0xd37e, 0, bp, o[0] + 2, 0, 0, o[3] - 2, 4, 0);
                }
            }
        }
        ++ebp;
        if (!(ebp < (int32_t)n)) break;
    }
    if (g->state == 2 || g->adfc || g->g46c) d2c_call(v, 0x277e0, 0, 0, 0, 0, 0);
}
