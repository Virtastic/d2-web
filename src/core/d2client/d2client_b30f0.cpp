// 1.13c D2Client+0xB30F0: create a client missile unit from a request block.
// fastcall, ECX = the request: +0 flags, +4 owner, +8 source unit, +0xC target, +0x10 missile row, +0x14/+0x18 x/y,
// +0x1C/+0x20 target offset or position, +0x24, +0x28 velocity, +0x2C, +0x30 level, +0x34, +0x40, +0x44, +0x4C, +0x50 byte,
// +0x54 callback (ECX = unit, EDX = +0x58). Returns the unit or 0. Written from the disassembly.
// Module globals: 0x11BBFC player unit, 0x11C3B8 (handed to the room test), 0xDD854 (sound switch); *sgptDataTables missile
// rows +0xB64 (stride 0x1A4, count +0xB6C); gdwBitMasks byte +0xC.
#include <cstdint>
#include "cl_env.h"

namespace {
constexpr uint32_t kBase = 0x6FAB0000u;
constexpr uint32_t G(uint32_t rva) { return kBase + rva; }
int32_t sdiv(int32_t a, int32_t b) { return (int32_t)((int64_t)a / b); }
uint32_t iabs(uint32_t v) { return (int32_t)v < 0 ? 0u - v : v; }   // retail: neg (0x80000000 stays negative)
}

extern "C" uint32_t d2c_b30f0(uint32_t R, const cl::Env& e) {
    const uint32_t row = e.Row((int32_t)e.R(R + 0x10));
    if (!row) return 0;
    const uint32_t flags = e.R(R);
    // retail's frame: +4 TY, +8 TX (both handed to 0xA0B30 by address)
    alignas(4) volatile uint32_t fr[4] = {};
    volatile uint32_t& TY = fr[1];
    volatile uint32_t& TX = fr[2];
    uint32_t X, Y;
    if (flags & 1) { X = e.R(R + 0x14); Y = e.R(R + 0x18); }
    else {
        const uint32_t src = e.R(R + 8);
        if (!src) return 0;
        X = e.H(0x1630, 0, 0, 0, 0, 0, 0, src);
        Y = e.H(0x1660, 0, 0, 0, 0, 0, 0, e.R(R + 8));
    }
    const uint32_t owner = e.R(R + 4);
    const uint32_t who = owner ? owner : e.R(G(0x11BBFC));
    if (who) {
        const uint32_t v = e.H(0xC1F0, who);
        const uint32_t in = e.H(0x1400, 0, 0, 0, 0, 0, v, 0, 0, X);
        e.Edj(0x1400, Y);
        if (!in && !e.H(0xC2E0, e.R(G(0x11C3B8)), X, Y)) return 0;
    }
    if (flags & 2) { TX = e.R(R + 0x1C) + X; TY = e.R(R + 0x20) + Y; }
    else if (flags & 0x20) { TX = e.R(R + 0x1C); TY = e.R(R + 0x20); }
    else { TX = X; TY = Y; }
    uint32_t vel;
    if (flags & 4) { vel = e.R(R + 0x28); if (!(flags & 0x10)) vel <<= 8; }
    else vel = (uint32_t)(sdiv((int32_t)(e.R8(row + 0x9B) * e.R(R + 0x30)), 8) + (int32_t)e.R8(row + 0x9A)) << 8;
    uint32_t pct = 100;
    if (owner && (e.R8(row + 4) & e.R8(e.bm + 0xC)) && e.H(0xC1FC, owner, 0x57)) {
        if (const uint32_t st = e.H(0xC1C0, e.R(R + 4), 0x57)) {
            pct = e.H(0xC4B4, st, 0xA1, 0);
            vel = e.H(0x14A0, 0, 0, 0, 0, 0, pct, 100, vel);
        }
    }
    if (vel) {
        if ((int32_t)vel > 0x100000) vel = (uint32_t)(sdiv((int32_t)vel, 100) * 75);   // the _allmul/_alldiv branch (masked < 0x640) is unreachable here
        else vel = (uint32_t)sdiv((int32_t)(vel * 75u), 100);
    }
    uint32_t tgt = e.R(R + 0xC);
    if (vel) {
        const uint32_t ow = e.R(R + 4);
        const uint32_t pTX = (uint32_t)(uintptr_t)&TX, pTY = (uint32_t)(uintptr_t)&TY;
        if (ow && !tgt) {
            if (TX == X && TY == Y) e.H(0xA0B30, pTX, pTY, 0, 0, 0, ow);
        } else if (tgt && ow && tgt != ow) {
            const uint32_t ox = e.H(0x1630, 0, 0, 0, 0, 0, 0, ow);
            if (e.H(0x1630, 0, 0, 0, 0, 0, 0, tgt) == ox) {
                const uint32_t ty = e.H(0x1660, 0, 0, 0, 0, 0, 0, tgt);
                if (ty == e.H(0x1660, 0, 0, 0, 0, 0, 0, e.R(R + 4))) {
                    e.H(0xA0B30, pTX, pTY, 0, 0, 0, e.R(R + 4));
                    tgt = 0;
                }
            }
        }
        uint32_t cx = TX, cy = TY;
        if (tgt) { cx = e.H(0x1630, 0, 0, 0, 0, 0, 0, tgt); cy = e.H(0x1660, 0, 0, 0, 0, 0, 0, tgt); }
        if ((int32_t)iabs(cx - X) >= 100) return 0;
        if ((int32_t)iabs(cy - Y) >= 100) return 0;
    }
    const uint32_t u = e.H(0xA7BF0, Y, 3, 0, 0, 0, 0, e.R(R + 0x10), X);
    if (!u) return 0;
    e.H(0xC472, u);
    e.H(0xC490, u, 0, 0, 0);
    e.W(u + 0x48, e.R8(row + 0x178) << 8);
    e.H(0xC382, u, e.R8(row + 0x1A1) << 4);
    uint32_t b;
    if (flags & 0x8000) b = e.R(R + 0x4C);
    else {
        b = (uint32_t)e.R16s(row + 0x98) * e.R(R + 0x30) + (uint32_t)e.R16s(row + 0x96);
        if ((flags & 8) && e.R8(row + 0x180)) b += (e.R8(row + 0x182) - e.R8(row + 0x181)) * e.R(R + 0x34);
        if (pct && pct != 100) b += e.H(0x14A0, 0, 0, 0, 0, 0, 100, pct, b);
    }
    if (flags & 0x200) { const uint32_t v = e.R(R + 0x40); b -= v; e.W(u + 0x44, v << 8); }
    e.H(0xC4A2, u, b);
    e.H(0xC2C8, u, b);
    b -= (flags & 0x800) ? e.R(R + 0x44) : e.R8(row + 0x136);
    e.H(0xC466, u, b);
    const uint32_t pth = e.R(u + 0x2C);
    if (!pth) {
        const uint32_t ty = e.R(u), id = e.R(u + 0xC);
        e.H(0xA6EF0, id, ty, 0, 0, 0, 0, (ty << 9) + G(0x109A08), id & 0x7F);
        return 0;
    }
    e.H(0xC328, pth, 0, G(0xD4010), 0x1EF4);
    if (tgt) e.H(0xC364, pth, tgt);
    else e.H(0xC3EE, pth, TX, TY);
    if (vel) {
        e.H(0xC328, pth, vel, G(0xD4010), 0x1EFF);
        e.H(0xC376, pth, u, 0);
    }
    if (e.R8(row + 0x135)) e.W(u + 0xC8, e.R(u + 0xC8) | 0x40000);
    e.H(0xC3A0, u, e.R(R + 4));
    e.H(0xC4CC, pth, (uint32_t)e.R16s(row + 0x9E));
    e.H(0xC32E, pth, e.R8(row + 0x9C) << 8);
    if (flags & 0x2000) {
        const uint32_t v = e.H(0x1330, 0, 0, 0, 0, 0, 0x40, u + 0x20);
        e.H(0xC3C4, e.R(u + 0x2C), v);
    }
    e.H(0xC7B00, 0, 0, 0, 0, 0, u);
    e.H(0xC7BF0, 0, 0, 0, 0, 0, u);
    if (flags & 0x400) {
        uint32_t r = e.H(0xC4D8, u, TX, TY);
        if (!r) r = 1;
        const uint32_t v16 = vel << 4;
        if (v16) {
            const uint32_t q = (r << 16) / v16;
            e.H(0xC2C8, u, q);
            e.H(0xC4A2, u, q);
        }
    }
    if (flags & 0x100) {
        const uint32_t c = e.H(0xC430, u);
        e.H(0xC7C80, e.R(R + 0x24), 0, 0, 0, 0, u, c);
        e.H(0xC7FB0, u);
        const uint32_t ty = TY;
        const uint32_t v = ((ty & ~0xFFu) | ((ty + TX) & 0xFF)) & 0xFFFFFF3Fu;
        e.H(0xC3C4, pth, v);
        e.H(0xC3E8, pth, 0);
    }
    e.H(0xC48A, u, e.R(R + 0x2C));
    e.H(0xC460, u, e.R(R + 0x30));
    if (const uint32_t ow = e.R(R + 4)) {
        e.H(0xC358, u, ow);
        e.H(0xB0A60, u, 0, 0, 0, 0, e.R(R + 4));
    }
    e.W(u + 0xC4, e.R(u + 0xC4) & ~8u);
    if (e.R(G(0xDD854)) && !(flags & 0x4000)) {
        if (e.R8(row + 0x130) || e.R8(R + 0x50)) {
            e.H(0xA5420); e.Edj(0xA5420, u);
            e.Xs(0);
            const uint32_t s = e.H(0xA8C20, e.R8(row + 0x130), 0xFF, e.R8(row + 0x132), e.R8(row + 0x133), e.R8(row + 0x134), 0, 0, 0, 1);
            e.Edj(0xA8C20, u);
            e.W(u + 0x64, s);
        }
    } else {
        e.H(0xA5420); e.Edj(0xA5420, u);
    }
    if (const uint32_t fp = e.R(R + 0x54)) e.H(fp & 0xFFFFFFu, 0, 0, 0, 0, 0, 0, u, e.R(R + 0x58));
    if (const int32_t sx = e.R16s(row + 0x12)) {
        e.H(0x88A70, u, 0, 0, 0, 0, 0, 0, 0, (uint32_t)sx);
        if (const uint32_t ow = e.R(R + 4))
            if (const uint32_t r = e.H(0x225D0, 0, 0, 0, 0, 0, 0x13A, ow))
                e.H(0x88BF0, 0, 0, 0, 0, 0, 0, 0, r, (uint32_t)sx);
    }
    if (const uint32_t ow = e.R(R + 4)) e.H(0x213A0, 0, 0, 0, 0, 0, ow, u);
    return u;
}
