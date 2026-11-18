// 1.13c D2Client+0x20F20 -- run the per-letter handlers for a player's name-ish byte string: EBX = unit (no stack).
// A type-1 unit with a data block (+0x14) whose byte string at +0x1C is non-empty: flag = bit 3 of block byte +0x16;
// the handler table (module global 0xFA340, 256 function pointers) is called with ECX = unit, stack flag (stdcall 1)
// first for the fixed indices 1..4 (.rdata 0xD6D2C), then for each of the 9 bytes at block+0x1C; null slots skip.
// e.gl = the table; a slot holds the handler's retail address, logged as its RVA (module base 0x6FAB0000).
#include "cl_env.h"
extern "C" void d2c_20f20(uint32_t u, const cl::Env& e) {
    if (!u || e.R(u) != 1) return;
    uint32_t p = e.R(u + 0x14);
    if (!p || !e.R8(p + 0x1c)) return;
    uint32_t fl = (e.R8(p + 0x16) >> 3) & 1;
    auto call = [&](uint32_t i) { uint32_t f = e.R(e.gl + i * 4); if (f) e.H(f - 0x6fab0000u, fl, 0, 0, 0, 0, 0, u); };
    for (uint32_t i = 1; i <= 4; ++i) call(i);
    for (uint32_t i = 0; i < 9; ++i) call(e.R8(p + 0x1c + i));
}
