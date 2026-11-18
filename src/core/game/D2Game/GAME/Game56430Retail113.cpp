// 1.13c D2Game+0x56430: load the player unit for a joining client. Picks the loader by game mode / global flag,
// on failure frees what the loader left in *ppUnit. Callee ids (w[0] = the register argument, then the stack words): 1 561A0, 2 563C0, 3 55840, 4 AEB6 (Ordinal 10418), 5 96FA0.
#include <cstdint>
typedef uint32_t W;
struct D2G56430Seam { W (*call)(void* ctx, W id, const W* w); W (*rd)(W addr); void* ctx; };
W GAME_56430(const D2G56430Seam& s, W a4, W a5, W client, W game, W p4, W flag, W ppUnit, W p7, W gvar)
{
    auto C = [&](W id, W a = 0, W b = 0, W c = 0, W d = 0, W e = 0, W f = 0, W g = 0) { const W w[7] = { a, b, c, d, e, f, g }; return s.call(s.ctx, id, w); };
    const W mode = (s.rd(game + 0x68) >> 16) & 0xFF;   // byte at game +0x6A
    W r;
    if (mode == 1 || mode == 2 || gvar) r = flag ? C(1, ppUnit, game, client, p7, a4, a5) : C(3, client, ppUnit);
    else r = flag ? C(2, game, client, p4, ppUnit, p7, a4, a5) : C(3, client, ppUnit);
    if (r) return r;
    const W unit = s.rd(ppUnit);
    if (!unit) return 0xE;
    C(4, unit);
    if (s.rd(unit) == 0) C(5, s.rd(unit + 0x80), unit);
    return 0;
}
