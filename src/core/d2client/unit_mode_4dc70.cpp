// 1.13c D2Client+0x4DC70: unit mode-change handler. fastcall(param_1 = mode, unit, p3 = packet words).
// Every callee goes through D2cHost in retail order; ordinals are 0x10000+ordinal. Memory the retail code reaches through
// globals or foreign pointers comes in via Env (pre-resolved) -- see the spec for each field.
#include "client_host.h"
#include <emscripten.h>
#include <stddef.h>
struct D2c4dc70Env {
    uint32_t dtCount;          // *(sgptDataTables+0xa80)
    uint32_t dtBase;           // *(sgptDataTables+0xa78)
    uint32_t rec4, rec5, rec6; // bytes +4,+5,+6 of the FUN_1260 record
    uint32_t bm0c, bm18;       // gdwBitMasks_exref words 0xc, 0x18
    uint32_t dat46c4, dat46c8; // DAT_6fba46c4 / c8
    uint32_t dbbfc;            // DAT_6fbcbbfc
    uint32_t* e1230;           // FUN_1230 result as words (null = none); byte +0x1a4 in b1a4, short +0x26 in s26
    uint32_t b1a4, s26;
    uint32_t* p30;             // *(unit[0x15]+0x30) as words (null = none or unit[0x15]==0)
    uint32_t out_f8, out_f4;   // DAT_6fbcc2f8 / f4 written by case 8
    uint32_t tokExit;          // address token for the "..\Source\D2Client\UNIT\Monster.cpp" / error strings
};
static const uint32_t O = 0x10000;
static uint32_t cl(D2cHost h, uint32_t r, uint32_t e = 0, uint32_t d = 0, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d2 = 0, uint32_t ebx = 0) {
    D2cCall k = { r, 0, e, d, ebx, 0, { a, b, c, d2 } };
    return h(&k);
}
static void fatal(D2cHost h, uint32_t line, uint32_t tok) {
    uint32_t s = cl(h, O + 10265, 0, 0, line);
    cl(h, O + 10024, 0, 0, tok, s);
    cl(h, 0x3664, 0, 0, 0xFFFFFFFFu);   // _exit(-1)
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_4dc70(D2cHost h, D2c4dc70Env* E, uint32_t p1, uint32_t* u, uint32_t* p3) {
    const uint32_t mon = 0x6FB8359Fu;                  // error-string token (DAT_6fb8359f)
    uint32_t iVar1 = u[0xb];
    if (u[4] == 0xd) { cl(h, O + 10770, iVar1); cl(h, O + 10498, iVar1); }
    bool skip = false;
    if (u[0x31] & 0x10000) skip = true;
    else {
        uint32_t t = u[4];
        if (u[0] == 0) { if (t == 0 || t == 0x11) skip = true; }
        else if (u[0] == 1 && (t == 0 || t == 0xc)) skip = true;
        if (!skip) cl(h, O + 11011, (uint32_t)(uintptr_t)u);
    }
    if (skip && p1 != 8 && p1 != 9) {
        cl(h, O + 10938, 0, 0, (uint32_t)(uintptr_t)u, 0);
        cl(h, 0x4C150);
        u[0x32] &= 0xfffbffffu;
    }
    bool bVar4 = false;
    if (p1 != 0x13 && p1 != 0x16 && p1 != 0x15 && p3) {
        bVar4 = true;
        if ((int32_t)p3[3] > 0) {
            cl(h, O + 10647, 0, 0, iVar1, p3[3]);
            if (p1 == 0x14) { cl(h, O + 10168, 0, 0, iVar1, p3[5] & 0xff); cl(h, O + 11005, 0, 0, iVar1, p3[2] & 0xff); }
        }
        cl(h, O + 11103, 0, 0, (uint32_t)(uintptr_t)u, 0);
        cl(h, 0x212F0, 0, 0, (uint32_t)(uintptr_t)u, 0, 0);   // FUN_6fad12f0(u,0,0): cdecl, all stack
    }
    int32_t t1 = (int32_t)u[1];
    if (t1 < 0) return;
    int32_t cnt = (int32_t)E->dtCount;
    if (cnt <= t1) return;
    if ((uint32_t)(t1 * 0x1a8 + E->dtBase) == 0) return;
    if (!cl(h, 0x1260)) return;
    int32_t iVar8 = (int32_t)cl(h, 0x1700);
    if (iVar8 < 0 || cnt <= iVar8) iVar8 = -1;
    const uint32_t U = (uint32_t)(uintptr_t)u;
    auto tail = [&]() { cl(h, 0x4DC30); };                        // FUN_6fafdc30
    auto walk = [&]() { cl(h, 0x2A6C0); };                        // FUN_6fada6c0 (state tail)
    auto fin = [&]() {
        if (bVar4 && p3 && p3[4]) {
            uint32_t v = cl(h, O + 10973, 0, 0, U, 0x43, 0);
            if (v != p3[4]) {
                cl(h, O + 10196, 0, 0, U);
                cl(h, O + 10551, 0, 0, U, 0x43, p3[4] - v, 0);
                cl(h, O + 10819, 0, 0, U, mon, 0x945);
            }
        }
        cl(h, 0x212F0, 0, 0, U, 0, 1);                         // FUN_6fad12f0(u,0,1)
        if (u[0] == 1 && u[5] && (*(uint8_t*)(uintptr_t)(u[5] + 0x28) & 1)) {
            if (u[4] != 1) tail();
            if (iVar1) cl(h, O + 10159, 0, 0, iVar1);
        }
    };
    bool tailOnly = false;   // goto LAB_6fafeb11
    switch (p1) {
    case 0: case 0x18: {
        if (!p3) { fatal(h, 0x6f6, mon); return; }
        cl(h, O + 10054, 0, 0, U, 1);
        uint32_t a = p3[0];
        cl(h, 0xA6040, 0, 0, p3[1], a, U);
        uint32_t r = cl(h, 0x1E40, 0, 0, 1) & 0xffff;
        if (r == 0 && cl(h, 0x2A070, 0, 0, p3[1], a)) walk(); else tail();
        break;
    }
    case 1: case 0x17: {
        if (!p3) { fatal(h, 0x6e1, mon); return; }
        cl(h, O + 10054, 0, 0, U, 1);
        uint32_t r = cl(h, 0x1E40, 0, 0, 1) & 0xffff;
        if (r == 0 && cl(h, 0x2A040, 0, 0, p3[0])) { walk(); break; }
        tailOnly = true; break;
    }
    default:
        cl(h, O + 10054, 0, 0, U, 0); tailOnly = true; break;
    case 4: case 0xb: case 0xc: case 0xe: case 0x11: case 0x1a: case 0x1c:
        if (!p3) { fatal(h, 0x740, mon); return; }
        cl(h, O + 10054, 0, 0, U, 0);
        cl(h, O + 11129, 0, 0, U, 0);
        if ((p3[0] || p3[1]) && (p3[0] != cl(h, 0x1630, U) || p3[1] != cl(h, 0x1660, U)))
            cl(h, O + 10434, 0, 0, U, p3[0], p3[1]);
        walk();
        if (cl(h, 0x4BFA0, 0, 0, U)) cl(h, 0x2A040, 0, 0, p3[0]);
        if (iVar8 == 0x6e) cl(h, 0x4CAD0, 0, 0, U);
        break;
    case 5: case 10: case 0xd: case 0xf: case 0x10: case 0x1b: case 0x1d: {
        if (!p3) { fatal(h, 0x70e, mon); return; }
        cl(h, O + 10054, 0, 0, U, 0);
        uint32_t a = p3[0];
        if (iVar8 == 0xe7) break;
        cl(h, 0xA6040, 0, 0, p3[1], a, U);
        if (iVar8 == 0x130 && p1 == 10) {
            uint32_t pp = cl(h, 0xA5F70);
            if (pp) cl(h, 0x6B300, pp, 2, 0, 0, 0);
        }
        cl(h, 0xA5FB0);
        if (iVar8 == 0x6e) cl(h, 0x4CAD0, 0, 0, U);
        else if (iVar8 == 0x9a && p1 == 0xd) cl(h, O + 10425, 0, 0, u[0xb], 0x38);
        if (u[1] == 0x195 && p1 == 0xd) cl(h, O + 10425, 0, 0, u[0xb], 0x34);
        if (u[1] == 0x195 && p1 == 0xf) cl(h, O + 10425, 0, 0, u[0xb], 0x30);
        if (iVar8 == 0x9b) { if (p1 == 0xd) cl(h, O + 10425, 0, 0, u[0xb], 0x34); }
        else if (iVar8 == 0xb2 && p1 == 0xd) cl(h, O + 10425, 0, 0, u[0xb], 4);
        walk();
        if (cl(h, 0x4BFA0, 0, 0, U)) cl(h, 0x2A070, 0, 0, p3[1], a);
        break;
    }
    case 6:
        if (p3) {
            cl(h, 0x1FCA0, 0, 0, p3[2] & 0x80);
            uint32_t v = p3[2] & 0x7f; p3[2] = v;
            if (v > 1) p3[2] = v + 1;
            bool b3 = true;
            if (p3[2]) {
                int32_t r = (int32_t)cl(h, O + 10973, 0, 0, U, 6, 0);
                if ((r >> 8) == (int32_t)p3[2]) b3 = false;
                else cl(h, O + 10887, 0, 0, U, 6, p3[2] << 8, 0);
            }
            cl(h, O + 10054, 0, 0, U, 0);
            u[0x2c] = p3[6];
            cl(h, O + 10770, 0, 0, 6);
            walk();
            if (b3) cl(h, 0x4C500, 0, 0, U);
            cl(h, 0xA4B60);
            cl(h, 0x212F0, 0, 0, U, 0, 3);
        }
        break;
    case 7:
        cl(h, O + 10054, 0, 0, U, 0);
        if (p3 && p3[2]) cl(h, O + 10887, 0, 0, U, 6, p3[2] << 8, 0);
        if (((u[0x31] >> 9) & 1) == 0) cl(h, 0x13D0, 2, E->rec4 & E->bm0c);
        else u[0x31] |= 2;
        cl(h, 0x13D0, 0x20, (E->rec5 & E->bm18) == 0);
        cl(h, 0xA6210);
        if (p3) {
            cl(h, 0x2A1B0, p3[1] & 0xffff, 0, 0);
            int32_t d = (int32_t)cl(h, 0x1630, U) - (int32_t)p3[0]; if (d < 0) d = -d;
            if (d < 2) {
                d = (int32_t)cl(h, 0x1660, U) - (int32_t)p3[1]; if (d < 0) d = -d;
                if (d < 2) { tailOnly = true; break; }
            }
            uint32_t r = cl(h, 0x1E40, 0, 0, 1) & 0xffff;
            if (r == 0 && cl(h, 0x2A040, 0, 0, p3[0]) && cl(h, O + 10494, 0, 0, U, 0x8f) == 0 && cl(h, 0x1C10, 0, 2)) {
                walk(); break;
            }
        }
        tailOnly = true; break;
    case 8: {
        cl(h, O + 10054, 0, 0, U, 0);
        u[0x2c] = p3[6];
        if (cl(h, 0x51A80) == U) {
            if ((E->dat46c4 & 8) == 0 && (E->dat46c8 & 8) == 0) E->out_f8 = 0;
            else { cl(h, 0x42AD0); E->out_f8 = 1; }
            E->out_f4 = 0;
        }
        cl(h, O + 10887, 0, 0, U, 6, 0, 0);
        if (cl(h, O + 10341, 0, 0, U)) { u[0x32] |= 0x40000; u[0x31] = (u[0x31] & ~2u) | 0x20; }
        if (cl(h, O + 11092, 0, 0, U)) { cl(h, 0x4BEC0); u[0x2c] = (u[0x2c] & 0xf) | 0xa0; }
        if ((E->bm0c & E->rec6 & 0xff) == 0) cl(h, O + 10084, 0, 0, U, 0);
        uint32_t t = u[1];
        if (!cl(h, 0x1230)) return;
        if (E->b1a4 == 1 && (int32_t)(int16_t)E->s26 >= 0 && (int32_t)(int16_t)E->s26 < cnt) {
            uint32_t y = cl(h, 0x1660, U), x = cl(h, 0x1630, U);
            uint32_t w = cl(h, 0xA7BF0, 0, 0, t, x, y, 1);
            if (w) { uint32_t* ww = (uint32_t*)(uintptr_t)w; ww[0xc4 / 4] &= ~2u; ww[0xb0 / 4] = u[0x2c]; }   // retail writes +0xb0 unguarded
            walk();
            cl(h, 0x4D610 /*FUN_6fafd610 = rva 0x4D610*/, 0, 0, (uint32_t)(int16_t)E->s26);   // FUN_6fafd610
        }
        bool toTail = false, done = false;   // done = LAB_6fafe781, toTail = LAB_6fafe77c
        auto x77c = [&]() { walk(); done = true; };
        auto x766 = [&]() { cl(h, 0x6B300, U, 2, 0, 0, 0); };
        auto sixtyFive = [&](uint32_t id) { uint32_t y2 = cl(h, 0x1660, U), x2 = cl(h, 0x1630, U); cl(h, 0xAFF10, 0, U, id, x2, y2, 0, 1); };
        (void)toTail;
        if (iVar8 < 0x165) {
            if (iVar8 == 0x164) { cl(h, 0xA15C0); x77c(); }
            else switch (iVar8) {
            case 0x6e:
                cl(h, 0x6B300, U, 2, 0, 0, 0); walk();
                if (cl(h, 0xC7F20)) {
                    cl(h, 0xC7BF0);
                    uint32_t* p = E->p30;
                    if (p) p[0] &= ~0x20u;
                    if (p) { p[4] = 0; p[5] = 0; p[6] = 0xffff8000u; }
                    if (p) { p[10] = 0; p[11] = 0; p[12] = 0; }
                }
                done = true; break;
            case 0x76: case 0x7f: case 0xee: case 0xf7: case 0x102: case 0x105: case 0x136: case 0x137: case 0x138:
                x766(); x77c(); break;
            case 0xbe:
                if (u[4] == 0xe) { cl(h, 0x2A180); u[0x11] = 0xe00; done = true; }
                else x77c();
                break;
            case 0xd4: x766(); x77c(); break;
            case 0x11c: {
                uint32_t a[7] = { 0xd8, 0, 0, 0, 1, 0, 0 };   // aiStack_1c[0..6]; the callee reads them as a packet
                D2c4dc70Env E2 = *E;
                d2_client_4dc70(h, &E2, 0x15, u, a);
                done = true; break;
            }
            case 0x15f: case 0x160: case 0x161:
                cl(h, 0x69610); cl(h, 0xA6F60, u[1] - 0xad); x77c(); break;
            default: x77c(); break;
            }
        } else {
            switch (iVar8) {
            case 0x168: case 0x22e: case 0x1a1: case 0x1a2: case 0x1d5: case 0x1da: x766(); x77c(); break;
            case 0x1a9: case 0x1aa: case 0x1ab:
                sixtyFive(0x1d6); walk(); cl(h, 0x88A70, U, 0, 0, 0); done = true; break;
            case 0x1b9: {
                uint32_t v = cl(h, O + 10042, iVar1);
                cl(h, O + 10425, 0, 0, iVar1, (((v & 0xff) + 0x20) & 0x8000003fu));
                x77c(); break;
            }
            case 0x1c5: {
                uint32_t v = cl(h, O + 10663, U);
                uint32_t r = cl(h, O + 10266, 0, 0, (0x6FB82E68u + p1 * 8 & ~0xffu) | (v & 0xff));
                cl(h, 0x88A70, U, 0, 0, 0);   // all four arms call FUN_6fb38a70(u,0,0,0)
                (void)r; x77c(); break;
            }
            case 0x1cd:
                cl(h, 0x6B300, U, 2, 0, 0, 0); cl(h, 0x88A70, U, 0, 0, 0); walk(); done = true; break;
            case 0x1f1: {
                uint32_t t2 = u[1];
                if (t2 == 0x1f1 || t2 == 0x1f2 || t2 == 499) x766();
                x77c(); break;
            }
            case 0x22f: u[0x32] |= 0x40000; u[0x31] |= 0x20; x77c(); break;
            case 0x23a:
                sixtyFive(0x266); u[0x32] |= 0x40000; u[0x31] |= 0x20; x77c(); break;
            default: x77c(); break;
            }
        }
        (void)done;
        u[0x31] &= ~2u;
        cl(h, 0xA5420);
        break;
    }
    case 9: {
        uint32_t i7 = 0, i12;
        cl(h, O + 10054, 0, 0, U);
        uint32_t d = E->dbbfc;
        if ((E->bm0c & *(uint8_t*)(uintptr_t)(iVar1 + 6)) == 0) cl(h, O + 10084, 0, 0, U, 0);
        cl(h, O + 10887, 0, 0, U, 6, 0, 0);
        if (cl(h, 0xA5F70) == U) cl(h, O + 11129, 0, 0, d, 0);
        if (iVar8 == 0x11c && u[4] == 0xe) {
            uint32_t v = cl(h, O + 10630);
            i12 = i7; cl(h, O + 11103, 0, 0, U, v); i7 = 0x11c;
        } else { walk(); u[0x31] |= 0x10000; i12 = i7; }
        if (u[1] != 0xd3 && i7 != 0x1f1 && i7 != 0x1a1 && i7 != 0x1a2) { i7 = i12; cl(h, 0x69610); }
        if (i7 == 0x1b3) {
            uint32_t y = cl(h, 0x1660, U), x = cl(h, 0x1630, U);
            uint32_t w = cl(h, 0xAFF10, 0, U, 0x1a8, x, y, 0, 1);
            if (w) ((uint32_t*)(uintptr_t)w)[0xc4 / 4] |= 0x10000;
        }
        if (i7 == 0x158 || i7 == 0x22f || i7 == 0x23a) u[0x32] |= 0x40000;
        u[0x31] = (u[0x31] & ~2u) | 0x20;
        if (cl(h, O + 10341, 0, 0, U)) { u[0x32] |= 0x40000; u[0x31] = (u[0x31] & ~4u) | 0x20; }
        cl(h, 0xA5420);
        break;
    }
    case 0x12:
        if (!p3) { fatal(h, 0x7af, mon); return; }
        cl(h, O + 10054, 0, 0, U, 0);
        cl(h, O + 10770, 0, 0, 0x12);
        cl(h, 0xA6040, 0, 0, p3[1], p3[0], U);
        cl(h, 0xA5FB0); walk();
        break;
    case 0x13: {
        if (!p3) { fatal(h, 0x780, mon); return; }
        cl(h, 0x1FCA0, 0, 0, p3[1] & 0x80);
        uint32_t v = p3[1] & 0x7f; p3[1] = v;
        if (v > 1) p3[1] = v + 1;
        if (p3[1]) cl(h, O + 10887, 0, 0, U, 6, p3[1] << 8, 0);
        u[0x2c] = p3[0];
        cl(h, 0x4C500, 0, 0, U);
        cl(h, 0xA4B60);
        cl(h, 0x2A590, 0, 0, 0xd, U);
        cl(h, 0x24490, 0, 0, 0xd, 1);
        cl(h, 0x212F0, 0, 0, U, 0, 3);
        break;
    }
    case 0x14:
        if (!p3) { fatal(h, 0x799, mon); return; }
        if (p3[5]) cl(h, O + 10887, 0, 0, U, 6, p3[5] << 8, 0);
        u[0x2c] = p3[6];
        cl(h, O + 10054, 0, 0, U, 0);
        cl(h, O + 10770, 0, 0, 0x14);
        cl(h, O + 10647, 0, 0, 0x14, 0xb);
        cl(h, O + 10527, 0, 0, 0x14, p3[2] & 0xff);
        cl(h, O + 10286, 0, 0, 0x14, p3[0] & 0xffff, p3[1] & 0xffff);
        cl(h, O + 10334, 0, 0, 0x14, U, 0);
        walk();
        break;
    case 0x15: case 0x16: {
        uint32_t ln = p1 == 0x15 ? 0x923 : 0x910, ln2 = p1 == 0x15 ? 0x928 : 0x913;
        if (!p3) { fatal(h, ln, mon); return; }
        cl(h, O + 10054, 0, 0, U, 0);
        if (p1 == 0x15) cl(h, O + 11129, 0, 0, U, 0);
        cl(h, O + 10302, 0, 0, U, p3[0], p3[4], 0);   // + (file token, ln2) on the stack; host ignores extras
        uint32_t v = cl(h, O + 10630);
        cl(h, O + 11103, 0, 0, U, v);
        if (cl(h, O + 10952, 0, 0, U, v) == 0xe) cl(h, 0x4D5B0);
        p3[1] = 0xffffffffu;
        walk();
        if (p1 == 0x15) cl(h, 0xA22B0, 0, 0, (uint32_t)(uintptr_t)p3); else cl(h, 0xA30C0, 0, 0, U, (uint32_t)(uintptr_t)p3);
        (void)ln2;
        break;
    }
    }
    if (tailOnly) tail();
    fin();
}
