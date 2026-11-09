// 1.13c D2Game+0xED250: player save header reader. EAX = end, ECX = game, EBX = client; stack (ret 8): &cursor, &outPlayer.
// 0xEB5D0 (unpack a skill word triple) and 0x494E0 (store the hotkey triple in the client) are plain leaf code, inlined.
#include "d2game_seam4e.h"
static void d2g_ed250_unpack(uint32_t p, uint32_t& skill, uint32_t& flag, uint32_t& val)   // 0xEB5D0
{
    const uint32_t w = d2rd16(p);
    if (w == 0xffff) { skill = 0xffffffffu; flag = 0; val = 0xffffffffu; return; }
    skill = w & 0xfff;
    flag = w >> 15;
    val = d2rd16(p + 2);
    if (val == 0) val = 0xffffffffu;
}
extern "C" uint32_t d2g_ed250(uint32_t end, uint32_t game, uint32_t cl, uint32_t pCur, uint32_t pOut, const D2Seam* e)
{
    const uint32_t b = d2rd32(pCur);
    d2wr32(pOut, 0);
    d2wr16(cl + 0xa, 0);
    const uint32_t next = b + 0x14f;
    if (next > end) return 4;
    const uint32_t len = end - b;
    const uint32_t saved = d2rd32(b + 0xc);
    d2wr32(b + 0xc, 0);
    if (e->call(0xA2B0, 0, 0, 0, 0, 0, b, len) != saved) return 6;
    if (len != d2rd32(b + 8)) return 5;
    const uint32_t ver = d2rd32(b + 4);
    if (ver <= 0x5b || ver > 0x60) return 7;
    d2wr8(b + 0x23, 0);
    const uint32_t name = e->call(0x49A20, cl);
    if (!name) return 3;
    if (e->call(0x2813, 0, 0, 0, 0, 0, name, b + 0x14) != 0) return 7;
    const uint32_t r = e->call(0xEBAF0, d2rd16(b + 0x24), 0, 0, 0, game, cl);
    if (r) return r;
    const uint32_t cls = d2rd8(b + 0x28);
    if (cls > 7) return 4;
    d2wr8(cl + 8, cls);
    uint32_t fl = d2rd32(b + 0x24);
    if (fl & 1) {
        fl &= 0xfffffffeu;
        d2wr32(b + 0x24, fl);
        d2wr16(cl + 0xa, fl);
        d2wr32(pCur, next);
        return 2;
    }
    d2wr8(cl + 0x45c, d2rd8(b + 0x10) & 1);
    d2wr16(cl + 0xa, d2rd16(b + 0x24));
    e->call(0x49AE0, cl, 0, 0, 0, 0, d2rd32(b + 0x2c));
    const uint32_t d = d2rd8(b + d2rd8(game + 0x6d) + 0xa8);
    uint32_t a = d & 0x7f;
    if (a >= 5) a = 0;
    e->call(0x49D70, cl, a);
    if (d2rd8(game + 0x6a) == 3 && d2rd32(game + 0x84) == 0 && (d & 0x80)) d2wr32(game + 0x7c, d2rd32(b + 0xab));
    const uint32_t unit = e->call(0xE1D90, 0, 0, cls, 0, 0, 0, 0, game, 0, 0, 0, 0);
    d2wr32(pOut, unit);
    e->call(0xEBA40, cl, unit, 0, 0, 0, game);
    const uint32_t v = e->call(0x49A80, cl);
    e->call(0x7B0A0, 0, 0, 0, 0, 0, v, d2rd32(pOut), cl);
    for (uint32_t i = 0; i < 0x10; ++i) {
        uint32_t skill, flag, val;
        d2g_ed250_unpack(b + 0x38 + 4 * i, skill, flag, val);
        if (cl) {   // 0x494E0
            d2wr16(cl + i * 8 + 0x3dc, skill);
            d2wr8(cl + i * 8 + 0x3de, flag);
            d2wr32(cl + i * 8 + 0x3e0, val);
        }
    }
    const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, d2rd32(pOut));
    if (!pd) return 4;
    static const uint32_t off[4] = {0x78, 0x7c, 0x80, 0x84}, dS[4] = {0x74, 0x70, 0x84, 0x80}, dV[4] = {0x7c, 0x78, 0x8c, 0x88};
    for (int k = 0; k < 4; ++k) {
        uint32_t skill, flag, val;
        d2g_ed250_unpack(b + off[k], skill, flag, val);
        d2wr32(pd + dS[k], skill);
        d2wr32(pd + dV[k], val);
    }
    d2wr8(cl + 0x480, d2rd8(b + 0xcf));
    d2wr8(cl + 0x481, 0);
    d2wr8(cl + 0x482, 0);
    d2wr32(pCur, next);
    return 0;
}
