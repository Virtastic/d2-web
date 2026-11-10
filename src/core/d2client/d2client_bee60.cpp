// 1.13c D2Client+0xBEE60 -- the "you died" overlay: two banner cels, the death line and the corpse hint. No arguments.
// gl block (pinned module globals): +0x00 [0x11BBFC] the player unit, +0x04 [0x11B9D8] the drawn flag, +0x08 [0x11B9DC] the hardcore
// banner cel, +0x0C [0x11B9E0] the second banner cel, +0x10 [0xDBC48] the screen width, +0x14 [0xDBC4C] the screen height,
// +0x18 [0xF9E14] the text centring width, +0x1C [0x11B980] the game info (flag byte +0x1EB), +0x20 the byte [0x11C390].
// No player: return. A player that is not flagged (+0xC4 bit 0x10000) must be dead: type 0 in mode 0 or 0x11, type 1 in mode 0 or 0xC,
// otherwise the drawn flag clears and the function returns. y = height/2 - 0x5E. Language folder by #10004 (0xD1E0, stdcall 2).
// Each banner cel loads once (sprintf 0x3146 "%s\UI\%s\<name>" then 0x2B420(EAX = path, 0)) and draws by 0xD372 (ECX = cel,
// EDX = width/2, y, 1, 5, 1); the second at y+0x30 after the drawn flag is set. The 0x100-character line buffer is built by
// Unicode::strcpy (string 0x13E8 by 0xD1EC) when game-info bit 4 is set, else by Unicode::sprintf(0x100, buf, string 0x13E6, stat)
// with stat = 0xC1C6(player, 0xAF, 0) (zero: no line, and y does not advance); the line draws in font 2 (0xD3A8) centred by 0xD35A at
// y+0x60. With [0x11C390] set and bit 4 clear, string 0x13E7 draws centred one step further down.
#include <cstdint>
#include "cl_env.h"
namespace { constexpr uint32_t kMod = 0xB2000000u; }
// Returns the stale EAX the retail epilogue leaves (the function is void): the type minus one on the rejected path, else the last call result with the low byte reloaded from [0x11C390].
extern "C" uint32_t d2c_bee60_seam(const cl::Env& e) {
    const uint32_t g = e.gl;
    uint32_t pl = e.R(g);
    if (!pl) return 0;
    if (!(e.R(pl + 0xc4) & 0x10000u)) {
        uint32_t t = e.R(pl), m = e.R(pl + 0x10);
        bool ok = t == 0 ? (m == 0 || m == 0x11) : t == 1 ? (m == 0 || m == 0xc) : false;
        if (!ok) { e.W(g + 4, 0); return (t == 0 || t == 1) ? 0u : t - 1; }
    }
    int32_t y = (int32_t)e.R(g + 0x14) / 2 - 0x5e;
    char lang[8] = {};
    char path[0x104] = {};
    uint16_t line[0x100] = {};
    const uint32_t pLang = (uint32_t)(uintptr_t)lang, pPath = (uint32_t)(uintptr_t)path, pLine = (uint32_t)(uintptr_t)line;
    e.H(0xd1e0, pLang, 0);
    const uint32_t names[2] = {kMod | 0xd345cu, kMod | 0xd3444u};
    for (int k = 0; k < 2; k++) {
        uint32_t cell = g + 8 + 4 * (uint32_t)k;
        uint32_t cel = e.R(cell);
        if (!cel) {
            e.H(0x3146, pPath, names[k], kMod | 0xd349cu, pLang);
            cel = e.H(0x2b420, 0, 0, 0, 0, 0, pPath);
            e.W(cell, cel);
        }
        if (cel) e.H(0xd372, (uint32_t)y, 1, 5, 1, 0, 0, cel, (uint32_t)((int32_t)e.R(g + 0x10) / 2));
        if (k == 0) { y += 0x30; e.W(g + 4, 1); }
    }
    const uint32_t gi = e.R(g + 0x1c);
    bool line_ok = true;
    uint32_t eax = 0;
    if (e.R8(gi + 0x1eb) & 4) {
        uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x13e8);
        e.H(0xceab8, 0, 0, 0, 0, 0, 0, pLine, s);
    } else if (!e.H(0xc1c6, pl, 0xaf, 0)) {
        line_ok = false;
    } else {
        uint32_t v = e.H(0xc1c6, pl, 0xaf, 0);
        uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x13e6);
        e.H(0xceab4, 0x100, pLine, s, v);
    }
    if (line_ok) {
        y += 0x30;
        e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 2);
        int32_t wl = (int32_t)e.H(0xd35a, 0, 0, 0, 0, 0, 0, pLine);
        int32_t x = (int32_t)e.R(g + 0x18) / 2 - wl / 2;
        eax = e.H(0xd37e, (uint32_t)y, 1, 1, 0, 0, 0, pLine, (uint32_t)x);
    }
    eax = (eax & ~0xffu) | e.R8(g + 0x20);
    if (e.R8(g + 0x20) && !(e.R8(e.R(g + 0x1c) + 0x1eb) & 4)) {
        e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 2);
        uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x13e7);
        int32_t wl = (int32_t)e.H(0xd35a, 0, 0, 0, 0, 0, 0, s);
        int32_t x = (int32_t)e.R(g + 0x18) / 2 - wl / 2;
        y += 0x30;
        uint32_t s2 = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x13e7);
        eax = e.H(0xd37e, (uint32_t)y, 1, 1, 0, 0, 0, s2, (uint32_t)x);
    }
    return eax;
}
