// 1.13c D2Client+0x65760, written from the disassembly: EAX = run-the-panel-loop flag, one stack word = screen mode (ret 4).
// Globals go through fx.ga (rva to pinned storage); the table pointers 0x11C05C/0x11C060 are stored as absolute 1.13c addresses (image base 0x6FAB0000 + rva, as retail's capture records them in the pinned globals) and the
// count is read from the table rva directly. Callees through cl::Env::H with the registers of each call site.
#include <cstring>
#include "cl_env.h"
namespace d2c65760 {
struct Fx { void* ctx; uint32_t (*ga)(void*, uint32_t); uint32_t (*tick)(void*); };
}
// 1.13c D2Client+0x65760
extern "C" void d2c_65760(uint32_t doLoop, uint32_t mode, const cl::Env& e, const d2c65760::Fx& fx) {
    const auto P = [&](uint32_t rva) { return (uint8_t*)(uintptr_t)fx.ga(fx.ctx, rva); };
    const auto rd = [&](uint32_t rva) { uint32_t v; memcpy(&v, P(rva), 4); return v; };
    const auto wr = [&](uint32_t rva, uint32_t v) { memcpy(P(rva), &v, 4); };
    const uint32_t ARR = 0xFAD80;
    const auto q4 = [](uint32_t v) { return (uint32_t)(((int32_t)v + (int32_t)((((int32_t)v) >> 31) & 3)) >> 2); };
    if (doLoop) {
        uint32_t i = 0;
        for (uint32_t off = 0; off < 0x1c8; off += 0xc, ++i) {
            wr(0xE2900 + off, 0);
            if (rd(0xE28F8 + off) == 0) continue;
            if (rd(0xE28FC + off) != 0) wr(0xE2900 + off, rd(ARR + 4 * i));
            const uint32_t old = rd(ARR + 4 * i);
            if (old == 0) continue;
            const uint32_t r = e.H(0xbf8c);
            if (r == 0 || rd(0x119854) == 0) {
                if (off == 0x1a4 || off == 0x1b0 || off == 0x1bc) continue;
            }
            if (e.H(0xc1a80, 0, 0, 0, 0, 0, 1) == 0) continue;
            wr(ARR + 4 * i, 0);
            if (old != 0) e.H(0xc21f0, 0, 0, 0, 0, 0, i);
            // 0: skip, 1: case 1 (0x3BBF0 then scroll B), 2: scroll B, 3: scroll C, 4: 0x11F80, 5: flat
            int k = 0;
            switch (i) {
            case 1: k = 1; break;
            case 4: k = 2; break;
            case 2: case 0xf: case 0x10: case 0x14: case 0x16: case 0x24: case 0x25: k = 3; break;
            case 0xb: k = 4; break;
            case 0xc: case 0xe: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x20: k = 5; break;
            default: break;
            }
            if (k == 0) continue;
            if (k == 4) { e.H(0x11f80); continue; }
            if (k == 1) e.H(0x8bbf0);
            if (k == 3 && rd(ARR + 4) == 0 && rd(ARR + 0x10) == 0) k = 5;
            if ((k == 1 || k == 2) && rd(ARR + 8) == 0 && rd(ARR + 0x50) == 0 && rd(ARR + 0x3c) == 0 && rd(ARR + 0x40) == 0 &&
                rd(ARR + 0x90) == 0 && rd(ARR + 0x94) == 0 && rd(ARR + 0x58) == 0) k = 5;
            uint32_t x, y;
            if (k == 5) {
                wr(0x11C418, 0); wr(0x11C414, 0);
                e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
                x = rd(0xF9E14); y = 0;
            } else if (k == 3) {
                const uint32_t v = 0u - q4(rd(0xDBC48));
                wr(0x11C418, v); wr(0x11C414, 1);
                e.H(0xd1b0, 0, 0, 0, 0, 0, 0, v);
                const uint32_t f = rd(0xF9E14), qq = q4(f);
                x = f - qq; y = 0u - qq;
            } else {
                const uint32_t v = q4(rd(0xDBC48));
                wr(0x11C418, v); wr(0x11C414, 2);
                e.H(0xd1b0, 0, 0, 0, 0, 0, 0, v);
                const uint32_t f = rd(0xF9E14), qq = q4(f);
                x = f + qq; y = qq;
            }
            e.H(0x8a4d0, y, 0, 0, 0, 0, rd(0xF9E18), x, 0, 0, rd(0x11C3C4));
        }
    }
    const uint32_t prev = rd(0xFADA4);
    e.H(0xbf8c);
    if (rd(0xDD5D0) != 0x26) { e.H(0xbf80, 0xB20D359Fu, e.H(0xbf92, 0x139b)); e.H(0x3664, 0xffffffffu); return; }
    const uint32_t t = fx.tick(fx.ctx);
    uint32_t pp = rd(0x11BBFC);
    if (!(rd(0xFAD50) > t) && pp != 0) {
        if (e.R(pp) == 0 && e.R(pp + 0x10) == 0x11) {
            e.H(0xa6210, 0, 0, 0, 0, 0, 0, 0, 0, 0, pp);
            e.H(0x14a30, 0, 0, 0, 0, 0, 0x41);
            const uint32_t f = rd(0x11B980) + 0x1eb;
            if (e.R8(f) & 4) {
                e.H(0x14660);
                const uint32_t g = rd(0x11B980) + 0x1eb;
                e.W(g, (e.R(g) & ~0xffu) | ((e.R8(g) | 8) & 0xffu));
                wr(0x11C3A0, 0); wr(0x11C39C, 0); wr(0xF8C9C, 0); wr(0x11C3A4, 1);
            }
        } else if (rd(0x113A68) == 0) {
            uint32_t eax = pp;
            bool bail = false;
            for (int32_t b = 0; b < (int32_t)rd(0xDD5D0); ++b) {
                if (rd(ARR + 4 * (uint32_t)b) != 1) continue;
                const uint32_t kd = e.R(rd(0xDD538 + 4 * (uint32_t)b) + 0x24);
                if (kd != 0 && kd != 1 && kd != 4) { bail = true; break; }   // the switch default (2, 3, >4) jumps past the tail
                if (kd == 1) { e.H(0xc2790, 0, 0, 0, 0, 0, 0, (uint32_t)b, 1); eax = rd(0x11BBFC); }
                else if (kd == 4) {
                    if (rd(0x119721) != 0) { const uint32_t edi = rd(0x11971D); e.H(0x47b20); e.Edj(0x47b20, edi); eax = rd(0x11BBFC); }
                }
            }
            if (!bail) {
                if (eax == 0 || (e.R(eax) == 0 && e.H(0xa5440) == 0)) { wr(0xFADA4, 1); eax = 1; }
                else eax = rd(0xFADA4);
                if (prev == 0) {
                    if (eax != 0) {
                        const uint32_t h = e.H(0xd0b4, 0xB20D0680u);
                        e.H(0x3ccc0, h, 0, 0, 0, 0, 7);
                        wr(0x11C2F8, 1); wr(0x11C2F4, 0);
                    }
                } else if (eax == 0) {
                    const uint32_t h = e.H(0xd0b4, 0xB20D0680u);
                    e.H(0x3cc60, h, 0, 0, 0, 0, 7);
                    wr(0x11C2F8, 0); wr(0x11C2F4, 0);
                }
            }
        }
    }
    static const uint32_t T[5][2][2] = { { { 0xE2AC0, 0xE2AD8 }, { 0, 0 } }, { { 0xE3AC8, 0xE3AE0 }, { 0, 0 } }, { { 0xE5570, 0xE5588 }, { 0, 0 } },
                                          { { 0xE8008, 0xE8020 }, { 0xEA550, 0xEA568 } }, { { 0xECFE8, 0xED000 }, { 0xEEFE0, 0xEEFF8 } } };
    uint32_t cnt;
    if (mode <= 4) {
        const uint32_t kk = (mode >= 3 && e.H(0xbf8c) != 0) ? 1 : 0;
        wr(0x11C05C, 0x6FAB0000u + T[mode][kk][0]); wr(0x11C060, 0x6FAB0000u + T[mode][kk][1]);
        cnt = rd(T[mode][kk][0]);
    } else {
        cnt = e.R(rd(0x11C05C));
    }
    wr(0x11C058, cnt - 1);
    if (mode == 5) wr(0x11C058, 1);
}
