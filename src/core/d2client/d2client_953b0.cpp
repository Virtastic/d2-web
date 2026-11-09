// 1.13c D2Client+0x953b0 -- equipped body-slot painter (no arguments, plain ret). For each body slot 0..10 of the player's inventory it
// centres the item's cell box in the slot rectangle of the table at D2Client+0x11BCA0 (stride 0x14: x0 +0, y0 +8, width byte +0x10,
// height byte +0x11), draws the slot backdrop through 0x68970 (ECX = x0, EAX = y0, EDX = 0; width, height, colour byte) when the slot
// origin lies inside the view (0xF9E14 / 0xF9E18), and the item through 0x6B5A0 (EBX = item; x, y). Slots 4/5 (the two hands) also
// paint the other hand's box when the player is two-handed (0xC89E == 2) and neither item answers 0xC928. The held item
// (0x11BC38 with 0x11BC2C set) gets colour 0x11B330/0x11B331 by 0x11B864 == 8 and the socket overlay 0x95140; other items run the gate
// chain 0xC772 / 0xC928 / 0xC898 / 0xCA12 / 0xC1FC(player, 0x36) / 0xC724 (colour 0x11B330) or 0xC1A8(item, 0x10) == 0 (colour 0x11B334).
// The tail is 0x8E2F0 with EAX = the 0x11B3E0 block and EBX = the inventory.
// 0xC7F6 fills the item's cell width/height bytes through pointers to two adjacent bytes of the retail frame (esp+0x12, esp+0x13);
// the port keeps the same frame layout (the x/y adjustment dwords at +0x14 / +0x18) and reads every local back from it.
#include <cstring>
#include "cl_env.h"
// ga = the 0x11B330 pin (0xC8), gb = 0x11B864, gc = the 0x11BC2C pin (0x150), gd = 0x119854, ge = 0xF9E14 (8)
extern "C" void d2c_953b0(uint32_t ga, uint32_t gb, uint32_t gc, uint32_t gd, uint32_t ge, const cl::Env& e) {
    constexpr uint32_t kInvCpp = 0xB2000000u | 0xD476Cu, kCmnItems = 0xB2000000u | 0xD478Cu, kBlk = 0xB2000000u | 0x11B3E0u;
    auto A = [&](uint32_t rva) -> uint32_t {
        if (rva >= 0x11BC2C) return gc + (rva - 0x11BC2C);
        if (rva >= 0x11B864) return gb;
        if (rva >= 0x11B330) return ga + (rva - 0x11B330);
        if (rva >= 0x119854) return gd;
        return ge + (rva - 0xF9E14);
    };
    uint8_t fr[0x30];
    memset(fr, 0, sizeof fr);
    const uint32_t F = (uint32_t)(uintptr_t)fr;
    const uint32_t tick = e.R(A(0x119854));
    if (tick && e.R(A(0x11BC94))) { e.W(F + 0x14, 4); e.W(F + 0x18, 0xfffffffcu); }
    const uint32_t pl = e.self, inv = e.R(pl + 0x60);
    e.H(0xc8a4, pl, 0, tick);
    e.H(0x908c0);
    const uint32_t tbl = A(0x11BCA0);
    const int32_t vw = (int32_t)e.R(A(0xF9E14)), vh = (int32_t)e.R(A(0xF9E18));
    auto box = [&](uint32_t slot, uint32_t col) {            // the bounded backdrop draw of one slot rectangle
        const uint32_t r = tbl + 0x14 * slot; const int32_t x0 = (int32_t)e.R(r), y0 = (int32_t)e.R(r + 8);
        if (x0 < 0 || x0 >= vw || y0 < 0 || y0 >= vh) return;
        e.H(0x68970, e.R8(r + 0x10), e.R8(r + 0x11), col, 0, 0, (uint32_t)y0, (uint32_t)x0, 0);
    };
    for (uint32_t s = 0; s < 11; s++) {
        uint32_t other = 0;
        const uint32_t it = e.H(0xc262, inv, s);
        if (!it) continue;
        e.H(0xc7f6, it, F + 0x12, F + 0x13, kCmnItems, 0x6da);
        const uint32_t two = e.H(0xc89e, pl, inv) == 2;
        const uint32_t me = tbl + 0x14 * s;
        if (s == 4 || s == 5) {
            const uint32_t os = 9 - s, ot = tbl + 0x14 * os;
            other = e.H(0xc262, inv, os);
            if (two && !e.H(0xc928, other) && !e.H(0xc928, it)) {
                const uint32_t c = e.R(A(0x11B3F4)), w = fr[0x12], h = fr[0x13];
                const int32_t y = ((int32_t)(e.R8(me + 0x11) - (c >> 8 & 0xff) * h) >> 1) + (int32_t)e.R(ot + 8) + (int32_t)e.R(F + 0x18);
                const int32_t x = ((int32_t)(e.R8(me + 0x10) - (c & 0xff) * w) >> 1) + (int32_t)e.R(ot) + (int32_t)e.R(F + 0x14);
                e.H(0x6b5a0, (uint32_t)x, (uint32_t)y, 0, 0, 0, 0, 0, 0, it);
                box(os, e.R8(A(0x11B330)));
            }
        }
        const uint32_t c = e.R(A(0x11B3F4)), w = fr[0x12], h = fr[0x13];
        int32_t x = ((int32_t)(e.R8(me + 0x10) - (c & 0xff) * w) >> 1) + (int32_t)e.R(me);
        int32_t y = ((int32_t)(e.R8(me + 0x11) - (c >> 8 & 0xff) * h) >> 1) + (int32_t)e.R(me + 8);
        switch (s) {
        case 2: case 6: case 7: case 10: y -= 2; break;
        case 3: x += 3; break;
        case 8: x += 2; y -= 2; break;
        case 9: x += 2; y -= 4; break;
        case 4: case 5: x += (int32_t)e.R(F + 0x14); y += (int32_t)e.R(F + 0x18); break;
        default: break;
        }
        if (it == e.R(A(0x11BC38)) && e.R(A(0x11BC2C))) {
            box(s, e.R8(A(e.R(gb) == 8 ? 0x11B330 : 0x11B331)));
            e.H(0x6b5a0, (uint32_t)x, (uint32_t)y, 0, 0, 0, 0, 0, 0, it);
            if (e.H(0xc90a, it) && (e.H(0xc97c, it) & 0xff) && e.H(0xc1a8, it, 0x800, 0x673, kInvCpp))
                e.H(0x95140, it, kBlk, (uint32_t)x, (uint32_t)y);
            continue;
        }
        const bool g = (!other && (e.H(0xc772, it) || e.H(0xc928, it))) || !e.H(0xc898, it, pl, 0, 0, 0) || e.H(0xca12, it, 2) ||
                       (e.self && e.H(0xc1fc, e.self, 0x36)) || !e.H(0xc724, it);
        if (g) box(s, e.R8(A(0x11B330)));
        else if (!e.H(0xc1a8, it, 0x10, 0x682, kInvCpp)) box(s, e.R8(A(0x11B334)));
        e.H(0x6b5a0, (uint32_t)x, (uint32_t)y, 0, 0, 0, 0, 0, 0, it);
    }
    e.H(0x8e2f0, 0, 0, 0, 0, 0, kBlk, 0, 0, inv);
}
