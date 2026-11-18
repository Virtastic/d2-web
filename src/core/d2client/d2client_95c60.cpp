// 1.13c D2Client+0x95C60, written from the disassembly: the trade panel frame (four background quarters, the hired unit's item
// grid through 0x958A0, the buy/sell button strip and its captions). No arguments, plain ret. e.gl = the module globals:
// +0 0x11BC30, +4 0x11B9A0 (x origin), +8 0x11B9A4 (y origin), +0xC 0xDBC4C (screen height), +0x10 0x1197C0 (the quarter cels),
// +0x14 0x11971C..0x119727 (dword id at 0x11971D, dword gate at 0x119721), +0x20 0x11B9C4 (button cels, loaded once), +0x24 0x11BC6C
// (the caption cels), +0x28 0x1197A8 (captions off), +0x2C 0xE1178..0xE120F (caption records, stride 0x12, then the counts 0xE11C0,
// 0xE11C4, 0xE11C8 and the button x table 0xE11D0), +0xC4 0x10B23C (button frames, stride 0x14), +0x128 0x10A808 (the 128-slot unit hash).
// The 0x48-byte cel record is handed to D2Gfx #10041 (0xD096): +0 frame, +0x34 cel file, the rest zero.
#include <cstring>
#include "cl_env.h"
namespace {
constexpr uint32_t kEmpty = 0xB20D359Fu;   // "" (module string); the cel path is sprintf(0xD0564 "%s\\ui\\panel\\buysellbtn", 0xD357C "DATA\\GLOBAL")
uint32_t G(uint32_t rva) {
    if (rva >= 0x10a808 && rva < 0x10aa08) return 0x128 + (rva - 0x10a808);
    if (rva >= 0x10b23c && rva < 0x10b2a0) return 0xc4 + (rva - 0x10b23c);
    if (rva >= 0xe1178 && rva < 0xe1210) return 0x2c + (rva - 0xe1178);
    switch (rva) {
    case 0x11bc30: return 0; case 0x11b9a0: return 4; case 0x11b9a4: return 8; case 0xdbc4c: return 0xc; case 0x1197c0: return 0x10;
    case 0x11b9c4: return 0x20; case 0x11bc6c: return 0x24; case 0x1197a8: return 0x28;
    default: return 0x14 + (rva - 0x11971c);
    }
}
}  // namespace
// 1.13c D2Client+0x95C60
extern "C" void d2c_95c60(const cl::Env& e) {
    const uint32_t g = e.gl;
    const auto R = [&](uint32_t rva) { return e.R(g + G(rva)); };
    if (R(0x11bc30) == 1 && e.H(0x8d300)) e.W(g + G(0x11bc30), 0);
    uint32_t rec[18] = {};
    const uint32_t p = (uint32_t)(uintptr_t)rec;
    const auto draw = [&](uint32_t x, uint32_t y) { e.H(0xd096, p, x, y, 0xffffffffu, 0, 5, 0); if (e.blk) e.blk(e.ctx, 0xd096, p, 18); };
    rec[13] = R(0x1197c0);
    for (uint32_t q = 0; q < 4; ++q) {
        rec[0] = q;
        draw(R(0x11b9a0) + (q & 1 ? 0x100 : 0), R(0x11b9a4) + R(0xdbc4c) - (q & 2 ? 0x30 : 0xe0));
    }
    if (R(0x119721)) {
        const uint32_t id = R(0x11971d);
        for (uint32_t u = R(0x10a808 + (id & 0x7f) * 4); u; u = e.R(u + 0xe4)) {
            if (e.R(u + 0xc) != id) continue;
            uint32_t a = u;
            if (e.R(u) != 1) {
                const uint32_t m = e.H(0xbf92, 0x5a);
                e.H(0xbf80, kEmpty, m);
                a = e.H(0x3664, 0xffffffffu);
            }
            e.H(0x958a0, a);
            break;
        }
    }
    uint32_t cel = R(0x11b9c4);
    if (!cel) {
        char path[0x100] = "DATA\\GLOBAL\\ui\\panel\\buysellbtn";   // what that sprintf writes
        const uint32_t b = (uint32_t)(uintptr_t)path;
        cel = e.H(0x2b420, 0, 0, 0, 0, 0, b);
        if (e.blk) e.blk(e.ctx, 0x2b420, b, 8);
        e.W(g + G(0x11b9c4), cel);
    }
    rec[13] = cel;
    for (uint32_t i = 0; (int32_t)i < (int32_t)R(0xe11c8); ++i) {
        const uint32_t row = 0x10b23c + i * 0x14;
        rec[0] = R(row) + (R(row + 8) >> 16);   // + the word at +0xA
        const uint32_t x = R(0xe11d0 + (i + R(0xe11c4) * 4) * 4);
        draw(x + R(0x11b9a0) - 1, R(0x11b9a4) + R(0xdbc4c) - 0x3f);
    }
    const uint32_t old = e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 1);
    for (uint32_t i = 0; (int32_t)i < (int32_t)R(0xe11c0); ++i) {
        const uint32_t r = 0xe1178 + i * 0x12;
        const uint32_t col = R(r + 0xa) ? 4 : 0;
        if (!R(r + 0xe)) continue;
        const uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, R(r + 8) & 0xffff);
        const int32_t w = (int32_t)e.H(0xd35a, 0, 0, 0, 0, 0, 0, s);
        rec[13] = R(0x11bc6c);
        rec[0] = col == 4 ? i : i + 4;
        draw(i * 0x50 + R(0x11b9a0), R(0x11b9a4) + R(0xdbc4c) - 0x1c1);
        if (!R(0x1197a8))
            e.H(0xd37e, R(r + 4) + R(0x11b9a4) + R(0xdbc4c) - 0x1e0, col, 0, 0, 0, 0, s, R(r) - (uint32_t)(w / 2) + R(0x11b9a0));
    }
    e.H(0xd3a8, 0, 0, 0, 0, 0, 0, old);
}
