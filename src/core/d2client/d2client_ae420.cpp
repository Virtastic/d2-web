// 1.13c D2Client+0xAE420: read an item packet's bit stream and create the client item it describes.
// fastcall, ECX = the packet: +1 dword id, +5 word item index, +7/+9 words, +0xB byte, +0xC byte length; the bit stream starts at +0xD.
// Written from the disassembly. Fog bit reader: 10126 open (0xBF9E), 10130 unsigned read (0xBFB0), 10129 signed read (0xBFAA).
// Module globals: 0x11BBFC player unit (+0xC key), 0x11C4D4 client list head (node +4 type, +8 id, +0xC key, +0x30 next);
// *sgptDataTables stat rows +0xBCC (stride 0x144; +4 mask byte, +8 bits, +9 parameter bits), count +0xBD4; gdwBitMasks byte +4.
#include <cstdint>
#include "cl_env.h"

namespace {
constexpr uint32_t kBase = 0x6FAB0000u;
constexpr uint32_t G(uint32_t rva) { return kBase + rva; }
}

extern "C" void d2c_ae420(uint32_t pkt, const cl::Env& e) {
    // retail's frame (esp after the prologue): +0x30 name (9 bytes zeroed; the copy loop runs on past it into the rect at +0x3C and the reader at +0x4C), +0x3C rect bytes (16), +0x4C bit reader, +0x60 the item spec (7 dwords)
    alignas(4) volatile uint8_t fr[0x80] = {};
    const auto A = [&](uint32_t o) { return (uint32_t)(uintptr_t)(fr + o); };
    const uint32_t br = A(0x4C);
    const uint32_t id = e.R(pkt + 1);
    const uint32_t idx = (uint32_t)e.R16s(pkt + 5);
    const uint32_t w7 = e.R(pkt + 7) & 0xFFFF, w9 = e.R(pkt + 9) & 0xFFFF;
    const auto U = [&](uint32_t n) { return e.H(0xBFB0, br, n); };
    e.H(0xBF9E, br, pkt + 0xD, e.R8(pkt + 0xC) - 0xD);
    const uint32_t b4 = U(4) & 0xFF;
    uint32_t rect = 0;
    if (U(1)) {
        const uint32_t row = e.H(0x1260, 0, 0, 0, 0, 0, idx);
        if (!row) return;
        rect = A(0x3C);
        for (uint32_t i = 0; i < 16; ++i) {
            const uint32_t b = e.R8(row + 0x15 + i);
            const uint32_t n = b < 3 ? 1u : 32u - (uint32_t)__builtin_clz(b - 1);
            fr[0x3C + i] = (uint8_t)U(n);
        }
    }
    uint32_t flags = 0, w26 = 0, name = 0, w14 = 0, x = 0xFFFFFFFFu;
    if (U(1)) {
        flags = U(1) ? 4u : 0u;
        if (U(1)) flags |= 8;
        if (U(1)) flags |= 2;
        if (U(1)) flags |= 0x10;
        if (U(1)) flags |= 0x40;
        if (flags & 2) w26 = e.H(0xBFAA, br, 0x10);
        for (uint32_t i = 0; i < 9; ++i) fr[0x30 + i] = 0;
        name = A(0x30);
        uint32_t i = 0;
        for (uint8_t ch; (ch = (uint8_t)U(8)) != 0;) if (i < 0x50) fr[0x30 + i++] = ch;   // retail writes on without a bound
        w14 = U(0x10) & 0xFF;   // retail: movzx si, al (the upper half of ESI is left over from the name pointer)
        if (U(1)) x = U(0x20);
    }
    volatile uint32_t* p = (volatile uint32_t*)(fr + 0x60);
    p[0] = id; p[1] = idx; p[2] = w7; p[3] = w9; p[4] = flags & 0xFFFF; p[5] = x; p[6] = b4;
    e.H(0x53C30, 0, 0, 0, 0, 0, 0, idx);
    e.Xs(0);
    const uint32_t u = e.H(0xA7780, A(0x60), rect, name, w14, w26);
    if (!u) return;
    e.H(0x6CB30, 0, 0, 0, 0, 0, 0, 0, 0, 0, u);
    bool mine = false;   // the packet's id is the local player's type-7 list node id
    if (e.H(0xC54A, u)) {
        uint32_t want = 0xFFFFFFFFu;
        if (const uint32_t pl = e.R(G(0x11BBFC))) {
            const uint32_t key = e.R(pl + 0xC);
            for (uint32_t n = e.R(G(0x11C4D4)); n; n = e.R(n + 0x30))
                if (e.R(n + 4) == 7 && e.R(n + 0xC) == key) { want = e.R(n + 8); break; }
        }
        mine = id == want;
    }
    if (!mine) {
        e.H(0xC40C, u, 7, 0x8000, 0);
        e.H(0xC40C, u, 6, e.R8(pkt + 0xB) << 8, 0);
    }
    if (e.H(0xC54A, u) && b4 == 1) e.W(u + 0xC8, e.R(u + 0xC8) & 0xFFFBFFFFu);
    e.H(0xC40C, u, 0x148, (w7 + w9) & 0xFFFF, 0);
    if (U(1)) { const uint32_t v = U(0x1F); e.H(0xC532, u, 0, v); }
    if (!U(1)) return;
    uint32_t list = 0;
    for (int32_t s = (int32_t)U(9); s < 0x1FF; s = (int32_t)U(9)) {
        if (s < 0 || s >= (int32_t)e.R(e.dt + 0xBD4)) return;
        const uint32_t row = (uint32_t)s * 0x144 + e.R(e.dt + 0xBCC);
        if (!row) return;
        const uint32_t b8 = e.R8(row + 8);
        if (!b8) return;
        uint32_t prm = 0;
        if (const uint32_t b9 = e.R8(row + 9)) prm = e.H(0xBFAA, br, b9);
        const uint32_t v = (b8 < 0x20 && (e.R8(e.bm + 4) & e.R8(row + 4))) ? e.H(0xBFAA, br, b8) : U(b8);
        if (!list) {
            list = e.H(0xC4DE, u, 0, 0x40);
            if (!list) {
                list = e.H(0xC442, 0, 0x40, 0, e.R(u), e.R(u + 0xC));
                e.H(0xC2F2, list, 0);
                e.H(0xC36A, u, list, 1);
                if (!list) continue;
            }
        }
        e.H(0xC562, list, (uint32_t)s, v, prm);
    }
}
