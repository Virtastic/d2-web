// 1.13c D2Client+0x20620, written from the disassembly: the hover label of a world unit. fastcall(ECX = unit, EDX = id, [esp+4] = show),
// ret 4. The text goes to D2Win #10971 (thunk 0xC22C, ECX = unit, EDX = text). Strings come from D2Lang #10003 (thunk 0xD1EC, id in CX:
// the upper half of ECX is whatever the register held, so only CX is part of the call). The 300-entry Unicode buffer is built by a
// no-op ctor; 0x90CB0 (the wide sprintf, cdecl) fills it. The module global 0x1223D0 receives u" " from Unicode::toUnicode first.
// e.gl = the pinned global 0x1223D0 (4 bytes). e.dt = *sgptDataTables: the 0x1A8-stride table at dt+0xA78 (count dt+0xA80), word +6.
#include <cstdint>
#include "cl_env.h"
namespace {
using W = char16_t;
constexpr uint32_t kD1EC = 0xd1ec, kC22C = 0xc22c, kC3B2 = 0xc3b2, kC604 = 0xc604, kCF7C = 0xcf7c, kCF82 = 0xcf82, kCF88 = 0xcf88;
// rdata 0xD6D64: (id, string) pairs; a miss leaves the index at 4, which reads the word after the table (0xD6D88 = 0x2B4F)
constexpr uint32_t kPair[5][2] = { { 0x10, 0xc94 }, { 0x24, 0x2b4c }, { 0x25, 0x2b4d }, { 0x26, 0x2b4e }, { 0x27, 0x2b4f } };
uint32_t A(const void* p) { return (uint32_t)(uintptr_t)p; }
// the id table of the 0x9C..0x14D and 0x220, 0x2C0..0x2C5 jump tables; 0 = the default sprintf path
uint32_t label(int32_t id) {
    switch (id) {
    case 0x9c: return 0xbcd;
    case 0xd3: case 0x2c4: return 0xbee;
    case 0xf2: case 0x2c0: return 0xbf6;
    case 0xf3: case 0x14d: case 0x2c1: return 0xbf4;
    case 0x100: case 0x2c2: return 0x3f6;
    case 0x220: case 0x2c5: return 0xbf5;
    case 0x2c3: return 0x2b92;
    default: return 0;
    }
}
}  // namespace
// 1.13c D2Client+0x20620
extern "C" void d2c_20620(const cl::Env& e, uint32_t unit, uint32_t id, uint32_t show) {
    if (!show) return;
    W buf[300];                                           // retail: 300 no-op Unicode ctors, never cleared
    e.W(e.gl, 0x20);                                      // toUnicode(0x1223D0, " ", 2)
    const auto text = [&](uint32_t cx) { return e.H(kD1EC, 0, 0, 0, 0, 0, 0, cx & 0xffff); };
    const auto draw = [&](uint32_t s) { e.H(kC22C, 0, 0, 0, 0, 0, 0, unit, s); };
    const uint32_t seed = unit + 0x20;
    uint32_t dx = 0;
    if (unit && e.R(unit) == 1 && e.R(unit + 0x14)) dx = e.R(e.R(unit + 0x14) + 0x14) & 0xffff;
    int32_t cls = -1;
    if (unit) {
        const uint32_t ty = e.R(unit), d = e.R(unit + 0x14);
        if (ty == 1 && d && (e.R8(d + 0x16) & 2)) {
            const uint32_t rec = e.H(kC604, (uint32_t)e.R16s(d + 0x26));
            if (!rec) return;
            const uint32_t w = e.R(rec + 2) & 0xffff;
            draw(text(w ? w : 0x1506));
            return;
        }
        cls = (int32_t)e.R(unit + 4);
        if (ty == 1 && d && (e.R8(d + 0x16) & 4)) {
            uint32_t k = 0;
            while (k < 4 && kPair[k][0] != id) ++k;
            uint32_t row = 0;
            if (cls >= 0 && cls < (int32_t)e.R(e.dt + 0xa80)) row = e.R(e.dt + 0xa78) + (uint32_t)cls * 0x1a8;
            const uint32_t name = text(e.R(row + 6));
            const uint32_t tag = text(kPair[k][1]);
            e.H(0x90cb0, A(buf), text(0x2b40), tag, name, 0, 0, 0);
            draw(A(buf));
            return;
        }
    }
    e.H(kC3B2, 0, 0, 0, 0, 0, 0, seed, dx);
    if (id == 0xc) { draw(text(0xc27)); return; }
    if (const uint32_t s = label(cls)) { draw(text(s)); return; }
    {
        const uint32_t b = e.H(kCF82, 0, 0, 0, 0, 0, 0, unit);
        const uint32_t a = e.H(kCF88, 0, 0, 0, 0, 0, 0, unit);
        e.H(0x90cb0, A(buf), text(0x6b9), a, b, 0, 0, 0);
    }
    if ((int32_t)e.H(0x1330, 0, 0, 0, 0, 0, 0x64, seed) < 0x32) {
        const uint32_t c = e.H(kCF7C, 0, 0, 0, 0, 0, 0, unit);
        const uint32_t b = e.H(kCF82, 0, 0, 0, 0, 0, 0, unit);
        const uint32_t a = e.H(kCF88, 0, 0, 0, 0, 0, 0, unit);
        e.H(0x90cb0, A(buf), text(0x6ba), a, b, c, 0, 0);
    }
    draw(A(buf));
}
