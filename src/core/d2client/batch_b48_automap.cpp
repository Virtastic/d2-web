// D2Client batch b48-d2client-k2: automap layout/cell walk, automap cel loading, drawlist-node setup.
// Globals the comments reads are passed in one struct of pointers (each member names its RVA); callees go through D2cHost.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
struct D2cAutomapG {
    i32 *mode /*1CC1B0*/, *root /*1CC1C4*/, *cel[4] /*1CC1C8,1CC1CC,1CC1D0,1CC1D4: act1,act2,act4,extown*/, *celH[4] /*1CC1D8..1CC1E4*/,
        *offX /*1CC1E8*/, *offY /*1CC1EC*/, *shiftX /*1CC1F0*/, *shiftY /*1CC1F4*/, *scrX /*1CC1F8*/, *scrY /*1CC1FC*/,
        *panel /*1CC240*/, *panelPrev /*1CC244*/, *panelSave /*1CC248*/, *oy /*1CC238*/, *ox /*1CC23C*/, *side /*1CC414*/,
        *gW /*1B8BC48*/, *gH /*1B8BC4C*/, *scale /*1A16B0*/, *halfW /*1A9E14*/, *halfH /*1A9E18*/, *posX /*1CC960*/,
        *posY /*1CC95C*/, *player /*1CBBFC*/, *act /*1CC284*/, *unitTbl /*1CC24C*/, *unitCnt /*1CC250*/, *lastIdx /*1A16B4*/;
};
namespace {
inline u32 go(D2cHost h, u32 rva, u32 eax, u32 ecx, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0, u32* hi = nullptr) {
    D2cCall c = {}; c.rva = rva; c.eax = eax; c.ecx = ecx; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3;
    u32 r = h(&c); if (hi) *hi = c.out[0]; return r;
}
inline u32 P(const void* p) { return (u32)(uintptr_t)p; }
inline u32 ord(D2cHost h, u32 n, u32 a = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0) { return go(h, 0x80000000u | n, a, a, a, s1, s2, s3); }
inline i32 rd(const void* p, u32 off) { i32 v; memcpy(&v, (const char*)p + off, 4); return v; }
}  // namespace

extern "C" {
// 1.13c D2Client+0x608D0  (ECX = force)
EMSCRIPTEN_KEEPALIVE i32 d2c_608d0(D2cHost h, D2cAutomapG* g, i32 force) {
    i32 r = 0;
    if (*g->lastIdx != 0 || force != 0) {
        go(h, 0x5F9F0, 0, 0);
        if (*g->mode - 1 != 0) { *g->offX = 0; *g->offY = 0; return *g->mode - 1; }
        *g->offX = (*g->gW / 3 - *g->ox) - 0x10;
        r = (i32)((int64_t)*g->gH * 0x55555556);   // retail leaves the multiply result in eax
        *g->offY = (*g->gH / 3 - *g->oy) - 0x10;
    }
    return r;
}

// 1.13c D2Client+0x60250  (ECX = cell node, EDX = rect {cel,x0,x1,y0,y1,ox,oy})
EMSCRIPTEN_KEEPALIVE void d2c_60250(D2cHost h, D2cAutomapG* g, const uint8_t* node, i32* rc) {
    while (node) {
        i32 q = *g->gW + ((*g->gW >> 31) & 3), sh = *g->side == 1 ? -(q >> 2) : *g->side == 2 ? (q >> 2) : 0;
        i32 x = (*(const int16_t*)(node + 6) * 10) / *g->scale - *g->scrX;
        i32 y = (*(const int16_t*)(node + 8) * 10) / *g->scale - *g->scrY;
        if (rc[3] <= y) {
            d2c_60250(h, g, (const uint8_t*)(uintptr_t)rd(node, 0xc), rc);
            if (rc[3] <= y) {
                if (rc[4] < y) return;
                if (rc[1] <= x && x <= rc[2]) {
                    i32 rec[18] = {};
                    rec[0] = *(const int16_t*)(node + 4); rec[13] = rc[0];
                    i32 col = 5; u32 hi = 0;
                    u32 k = go(h, 0x5F9C0, P(node), P(node));
                    if (k == 1) {
                        if (rc[0] == *g->cel[0] && *g->mode == 0) {
                            i32 px = rc[5] + x, py = rc[6] + y, cx = *g->halfW / 2, cy = *g->halfH / 2;
                            if (cx - 0x8c + sh <= px && px <= cx + 0x8c + sh && cy - 0x96 <= py && py <= cy + 0x82) {
                                i32 dx = (cx - px) + sh, dy = (cy - 10) - py;
                                if (dx < 0) dx = -dx;
                                if (dy < 0) dy = -dy;
                                i32 d = dy < dx ? dy + dx * 2 : dx + dy * 2;
                                if (d / 2 < 0x96) {
                                    u32 lo = go(h, 0x5F9C0, P(node), P(node), 0, 0, 0, 0, &hi);  // 64-bit result: edx in out[0]
                                    col = (lo != 2) + 1;
                                    if ((i32)hi < 0x64) { col = 1; if ((i32)hi < 0x32) col = 0; }
                                }
                            }
                        }
                    } else if (k == 2) col = 1;
                    else if (k == 3) {
                        uint8_t st = *(const uint8_t*)(uintptr_t)(*g->player + 0x18);
                        if (st == 0 || st == 2) col = *g->mode == 1 ? 1 : 2;
                    }
                    i32 r4[4];
                    if (*g->mode == 1) {
                        if (*g->panel == 0) { r4[0] = *g->gW - 0x119; r4[2] = 0x39; }
                        else { r4[0] = 0; r4[2] = (((*g->act == 2) - 1) & 0x60) - 0x15; }
                        r4[1] = r4[0] + 0x117; r4[3] = r4[2] + 0xe1;
                    } else { r4[0] = 0; r4[2] = 0; r4[1] = *g->gW - 1; r4[3] = *g->gH - 1; }
                    go(h, 0x80000000u | 10079, P(rec), P(rec), (u32)x, (u32)y, P(r4), (u32)col);
                }
            }
        }
        if (rc[4] < y) return;
        node = (const uint8_t*)(uintptr_t)rd(node, 0x10);
    }
}

// 1.13c D2Client+0x60C40
EMSCRIPTEN_KEEPALIVE void d2c_60c40(D2cHost h, D2cAutomapG* g) {
    if (!*g->root) return;
    i32 q = *g->gW + ((*g->gW >> 31) & 3);
    *g->shiftX = 0;
    if (*g->side == 1) {
        if (*g->mode != 1) *g->shiftX = -(q >> 2);
        if (*g->panel == 0) { *g->panelPrev = 0; *g->panel = 1; }
    } else if (*g->side == 2) {
        if (*g->mode != 1) *g->shiftX = q >> 2;
        if (*g->panel != 0) { *g->panelPrev = *g->panel; *g->panel = 0; }
    } else {
        if (*g->panel != *g->panelPrev) *g->panel = *g->panelPrev;
        if (*g->panel != *g->panelSave) *g->panel = *g->panelSave;
    }
    *g->shiftX = (*g->posX / *g->scale - *g->halfW / 2) - *g->shiftX;
    *g->shiftY = *g->posY / *g->scale - *g->halfH / 2;
    *g->scrX = *g->offX + 0x28 + *g->shiftX;
    *g->scrY = *g->offY + 0xf + *g->shiftY;
    i32 rc[7] = {};   // {cel, x0, x1, y0, y1, ox, oy}
    i32 hw = *g->halfW, hh = *g->halfH;
    if (*g->mode == 1) {
        rc[1] = *g->ox - 8; rc[3] = *g->oy - 0x10; rc[2] = hw / 3 + 8 + *g->ox; rc[4] = hh / 3 + 0x10 + *g->oy;
        rc[5] = 4; rc[6] = 8; *g->scale = 0x14;
    } else {
        rc[2] = hw + 0x10; rc[4] = hh + 0x20; rc[1] = -0x10; rc[3] = -0x20; rc[5] = 8; rc[6] = 0x10; *g->scale = 10;
    }
    rc[0] = *g->cel[0];
    const uint8_t* r = (const uint8_t*)(uintptr_t)*g->root;
    d2c_60250(h, g, (const uint8_t*)(uintptr_t)rd(r, 8), rc);
    d2c_60250(h, g, (const uint8_t*)(uintptr_t)rd(r, 0xc), rc);
    d2c_60250(h, g, (const uint8_t*)(uintptr_t)rd(r, 0x10), rc);
    i32 a = rd(r, 4);
    if (*g->mode != 1) {
        switch (a) {
        case 1: rc[2] = hw + 0xa0; rc[1] = -0xa0; rc[3] = -100; rc[4] = hh + 100; rc[5] = 0x50; rc[6] = 0x32; rc[0] = *g->cel[1]; break;
        case 2: rc[2] = hw + 0x88; rc[1] = -0x88; rc[3] = -0x5a; rc[4] = hh + 0x5a; rc[5] = 0x44; rc[6] = 0x2d; rc[0] = *g->cel[2]; break;
        case 3: rc[2] = hw + 0xb4; rc[1] = -0xb4; rc[3] = -0xaa; rc[4] = hh + 0xaa; rc[5] = 0x5a; rc[6] = 0x55; rc[0] = *g->cel[3]; break;
        default: rc[2] = hw + 0x10; rc[1] = -0x10; rc[3] = -0x20; rc[4] = hh + 0x20; rc[5] = 8; rc[6] = 0x10; rc[0] = *g->cel[0]; break;
        }
        *g->scale = 10;
    } else {
        static const i32 W[4] = {8, 0xa0, 0x88, 0xb4}, H[4] = {0x10, 100, 0x5a, 0xaa}, A[4] = {4, 0x50, 0x44, 0x5a}, B[4] = {8, 0x32, 0x2d, 0x55};
        i32 n = (a >= 1 && a <= 3) ? a : 0;
        rc[0] = *g->cel[n]; rc[1] = *g->ox - W[n]; rc[3] = *g->oy - H[n];
        rc[2] = hw / 3 + W[n] + *g->ox; rc[4] = hh / 3 + H[n] + *g->oy; rc[5] = A[n]; rc[6] = B[n];
        *g->scale = 0x14;
    }
    d2c_60250(h, g, (const uint8_t*)(uintptr_t)rd(r, 0x14), rc);
}

// 1.13c D2Client+0x5FB50
EMSCRIPTEN_KEEPALIVE void d2c_5fb50(D2cHost h, D2cAutomapG* g) {
    char p[260];
    static const char* const nm[4] = {"MaxiMap", "Act2Map", "Act4Map", "ExTnMap"};
    go(h, 0x5F180, 0, 0);
    bool small = *g->mode == 1;
    for (int i = 0; i < 4; i++) {
        if (i == 3 && !small && ord(h, 10227) == 0) { *g->cel[3] = 0; *g->celH[3] = 0; return; }
        snprintf(p, sizeof p, "%s\\UI\\AutoMap\\%s%s", "DATA\\GLOBAL", nm[i], small ? "S" : "");
        *g->cel[i] = (i32)go(h, 0x2B420, P(p), P(p), P(p), 0);
        *g->celH[i] = (i32)ord(h, 10086, (u32)*g->cel[i]);
    }
}
}
