// 1.13c D2Client+0xBFEC0 -- an item's name line in its colour: EAX = the item unit, stack (dst, cap), callee pops 8.
// gl block: +0x00 [0xDBC54] the text x, +0x04 [0xDBC58] the text y (lowered by 0x10 on every entry, even for a null unit).
// colour: #10106 type (0xC19C) 4..9 maps to 3, 2, 9, 4, 8, 10; any other type is 5 when either unit flag test 0xC1A8(unit, 0x800,
// 0x5AA, file) / 0xC1A8(unit, 0x400000, 0x5AE, file) answers, else 0. The item record (0xC1BA(class at unit+4)) with byte +0x12A set
// and code +0x80 other than "leg " makes it 4; a code among ceh/bet/tes/fed/toa/dhn/bey/mbr/pk1/pk2/pk3, or 0xC1CC(unit, 0x4A), makes it 8.
// Quality (0xC220) 4: "<stat 0xE>" through sprintf (IAT 0xCF114, "%i"), widened (IAT 0xCEAC0, 9), then string 0xF9B and the 0xA5D90
// name appended (strcpy / strcat); otherwise 0x914F0(unit, buf, 0x100) fills the line. No dst: 0xD384 draws it (ECX line, EDX x;
// y, colour, 1). With dst: 0xBF5F0(EDI = line, colour) prefixes the colour, strlen (IAT 0xCEAB0) >= cap is Fog #10265(0x5E0 / 0x5EB),
// #10024, exit(-1) (the hooked exit returns and retail copies on), then strcpy(dst, line).
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t kMod = 0xB2000000u;
bool code8(uint32_t c) {
    static const uint32_t k[] = {0x20686563u, 0x20746562u, 0x20736574u, 0x20646566u, 0x20616f74u, 0x206e6864u,
                                 0x20796562u, 0x2072626du, 0x20316b70u, 0x20326b70u, 0x20336b70u};
    for (uint32_t v : k) if (v == c) return true;
    return false;
}
void fatal(const cl::Env& e, uint32_t line) {
    uint32_t m = e.H(0xbf92, line);
    e.H(0xbf80, kMod | 0xd359fu, m);
    e.H(0x3664, 0xffffffffu);
}
}  // namespace
extern "C" void d2c_bfec0(uint32_t unit, uint32_t dst, uint32_t cap, const cl::Env& e) {
    const uint32_t g = e.gl;
    e.W(g + 4, e.R(g + 4) - 0x10);
    if (!unit) return;
    uint32_t name = e.H(0xa5d90);
    uint32_t col = 0;
    uint32_t ty = e.H(0xc19c, unit);
    static const uint32_t tbl[6] = {3, 2, 9, 4, 8, 10};
    if (ty - 4 <= 5) col = tbl[ty - 4];
    else if (e.H(0xc1a8, unit, 0x800, 0x5aa, kMod | 0xd34c8u) || e.H(0xc1a8, unit, 0x400000, 0x5ae, kMod | 0xd34c8u)) col = 5;
    uint32_t rec = e.H(0xc1ba, e.R(unit + 4));
    uint32_t code = e.R(rec + 0x80);
    if (e.R8(rec + 0x12a) && code != 0x2067656cu) col = 4;
    if (code8(code) || e.H(0xc1cc, unit, 0x4a)) col = 8;
    uint16_t lineA[0x80] = {};
    uint16_t lineC[0x80] = {};
    uint16_t num[10] = {};
    char nb[12] = {};
    const uint32_t pA = (uint32_t)(uintptr_t)lineA, pC = (uint32_t)(uintptr_t)lineC;
    uint32_t line, fl;
    if (e.H(0xc220, unit) == 4) {
        uint32_t v = e.H(0xc1c6, unit, 0xe, 0);
        e.H(0xcf114, (uint32_t)(uintptr_t)nb, kMod | 0xd34c4u, v);
        e.H(0xceac0, 9, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)num, (uint32_t)(uintptr_t)nb);
        e.H(0xceab8, 0, 0, 0, 0, 0, 0, pA, (uint32_t)(uintptr_t)num);
        uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xf9b);
        e.H(0xceabc, 0, 0, 0, 0, 0, 0, pA, s);
        e.H(0xceabc, 0, 0, 0, 0, 0, 0, pA, name);
        line = pA; fl = 0x5e0;
    } else {
        e.H(0x914f0, unit, pC, 0x100);
        line = pC; fl = 0x5eb;
    }
    if (!dst) { e.H(0xd384, e.R(g + 4), col, 1, 0, 0, 0, line, e.R(g)); return; }
    e.H(0xbf5f0, col);
    if ((int32_t)e.H(0xceab0, 0, 0, 0, 0, 0, 0, line) >= (int32_t)cap) {
        fatal(e, fl);
        e.H(0xceab8, 0, 0, 0, 0, 0, 0, dst, pC);
        return;
    }
    e.H(0xceab8, 0, 0, 0, 0, 0, 0, dst, line);
}
