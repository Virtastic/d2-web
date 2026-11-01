// 1.13c D2Game+0x48BA0 -- level a hireling to a level and rebuild its stats and skills from its hireling record (stdcall: game, player, merc, level; ret 0x10).
// Written from the 1.13c disassembly as a rule over the seam environment (d2game_gp.h); T = the sgptDataTables block.
// No merc: found as in 0x48950 (0xA4C0(player) +0x44 node list, game +0x1320 hash chain); none: return. 0xA42A(player, 0xC, 0); v = 0xA42A(merc, 0xC, 0);
// level 0: level = v + 1. client = 0xA4C0(player) +0x9C for a player unit, else 0. A 0x28-byte packet {0x27, 1, guid (merc +0xC), 1, ?, 3, ?, short 0xD7C, ...}
// is copied and sent: 0x8A3E0(EAX = client; copy, 0x28). 0xA472(merc, 0xC, level, 0). r = 0x96570(EAX = player, EBX = guid);
// h = 0xA3BE(game +0x70, r +8, level); none: return. d = level - h +0x1C. Experience: 0x1E = level >= 0xA48A(0) - 1 ? 0 : 0xA3A6(level + 1, h +0x20);
// e = 0xA3A6(level, h +0x20) raises 0xD when 0xA42A(merc, 0xD, 0) < e. Stats (signed, /8 and /4 truncating): 0 = max(10, h38*d/8 + h34), 2 = max(10, h40*d/8 + h3C),
// 7 and 6 = max(0x2800, (h28<<8)*d + (h24<<8)), 0x1F = max(0, h30*d + h2C), 0x17 = max(0, h58*d/8 + h50), 0x18 = max(1, h58*d/8 + h54), 0x13 = max(0, h48*d + h44),
// 0x27/0x29/0x2B/0x2D = max(0, h60*d/4 + h5C), 0x4A = max(0, 0xA3E2(merc, 7, 0) / 2000). Skills i = 0..5: sid = h +0x78 + 4i; sid <= 0, sid >= the skill count
// (T +0xBA0) or signed byte h +0xC0 + i >= 0x10 ends the function; sk = 0x1420(EAX = sid); with sk and level >= short sk +0x174:
// lv = clamp((sbyte h +0xCC + i * d) >> 5 + sbyte h +0xC6 + i, 0, 0x20); lv > 0: 0x9F9A0(EAX = sid, ESI = merc; lv, 0).
#include "../fn/d2game_gp.h"
#include "../fn/d2game_hk2.h"
namespace d2g {
static uint32_t bp48ba0(GE& e, uint32_t v) { return (v >> 24) == 0xB1 ? e.B((int)(v >> 16 & 0xFF)) + (v & 0xFFFF) : v; }
void f48ba0(GE& e, HkWlog& w, uint32_t T, uint32_t game, uint32_t player, uint32_t merc, int32_t level)
{
    if (!merc) {
        const uint32_t pd = bp48ba0(e, e.h(0xA4C0, 0, 0, 0, 0, 0, player));
        if (!pd || (int32_t)e.rd32(T + 0xBF0) <= 7) return;
        const uint32_t p = e.rd32(pd + 0x44);
        if (!p) return;
        uint32_t node = e.rd32(e.rd32(p) + 0x54);
        if (!node) return;
        while (e.rd8(node) & 1) { node = e.rd32(node + 0x14); if (!node) return; }
        const uint32_t guid = e.rd32(node + 4);
        uint32_t m = e.rd32(game + 0x1320 + (guid & 0x7F) * 4);
        while (m && e.rd32(m + 0xC) != guid) m = e.rd32(m + 0xE4);
        if (!m) return;
        merc = m;
    }
    e.h(0xA42A, 0, 0, 0, 0, 0, player, 0xC, 0);
    const uint32_t v = e.h(0xA42A, 0, 0, 0, 0, 0, merc, 0xC, 0);
    if (!level) level = (int32_t)v + 1;
    uint32_t client = 0;
    if (e.rd32(player) == 0) client = e.rd32(bp48ba0(e, e.h(0xA4C0, 0, 0, 0, 0, 0, player)) + 0x9C);
    uint8_t pk[0x28] = {};
    pk[0] = 0x27; pk[1] = 1; const uint32_t g = e.rd32(merc + 0xC); std::memcpy(pk + 2, &g, 4); pk[6] = 1; pk[8] = 3; pk[10] = 0x7C; pk[11] = 0x0D;
    hk_send(e, w, (const void*)(uintptr_t)client, pk, 0x28, 0xD7Full);
    e.h(0xA472, 0, 0, 0, 0, 0, merc, 0xC, (uint32_t)level, 0);
    const uint32_t r = bp48ba0(e, e.h(0x96570, player, 0, 0, e.rd32(merc + 0xC), 0));
    const uint32_t h = bp48ba0(e, e.h(0xA3BE, 0, 0, 0, 0, 0, e.rd32(game + 0x70), e.rd32(r + 8), (uint32_t)level));
    if (!h) return;
    auto H = [&](uint32_t o) { return (int32_t)e.rd32(h + o); };
    auto set = [&](uint32_t st, int32_t val) { e.h(0xA472, 0, 0, 0, 0, 0, merc, st, (uint32_t)val, 0); };
    const int32_t d = level - H(0x1C);
    const int32_t mx = (int32_t)e.h(0xA48A, 0, 0, 0, 0, 0, 0);
    set(0x1E, level >= mx - 1 ? 0 : (int32_t)e.h(0xA3A6, 0, 0, 0, 0, 0, (uint32_t)(level + 1), (uint32_t)H(0x20)));
    const int32_t ex = (int32_t)e.h(0xA3A6, 0, 0, 0, 0, 0, (uint32_t)level, (uint32_t)H(0x20));
    if ((int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, merc, 0xD, 0) < ex) set(0xD, ex);
    auto mx0 = [](int32_t a, int32_t lo) { return a < lo ? lo : a; };
    set(0, mx0(H(0x38) * d / 8 + H(0x34), 10));
    set(2, mx0(H(0x40) * d / 8 + H(0x3C), 10));
    const int32_t hp = mx0((int32_t)((uint32_t)H(0x28) << 8) * d + (int32_t)((uint32_t)H(0x24) << 8), 0x2800);
    set(7, hp); set(6, hp);
    set(0x1F, mx0(H(0x30) * d + H(0x2C), 0));
    set(0x17, mx0(H(0x58) * d / 8 + H(0x50), 0));
    set(0x18, mx0(H(0x58) * d / 8 + H(0x54), 1));
    set(0x13, mx0(H(0x48) * d + H(0x44), 0));
    const int32_t res = mx0(H(0x60) * d / 4 + H(0x5C), 0);
    set(0x27, res); set(0x29, res); set(0x2B, res); set(0x2D, res);
    set(0x4A, mx0((int32_t)e.h(0xA3E2, 0, 0, 0, 0, 0, merc, 7, 0) / 2000, 0));
    for (int i = 0; i < 6; i++) {
        const int32_t sid = H(0x78 + 4 * i);
        if (sid <= 0 || sid >= (int32_t)e.rd32(T + 0xBA0) || (int8_t)e.rd8(h + 0xC0 + i) >= 0x10) return;
        const uint32_t sk = bp48ba0(e, e.h(0x1420, (uint32_t)sid, 0, 0, 0, 0));
        if (!sk || level < (int16_t)e.rd16s(sk + 0x174)) continue;
        int32_t lv = (((int8_t)e.rd8(h + 0xCC + i) * d) >> 5) + (int8_t)e.rd8(h + 0xC6 + i);
        if (lv < 0) lv = 0;
        if (lv > 0x20) lv = 0x20;
        if (lv > 0) e.h(0x9F9A0, (uint32_t)sid, 0, 0, 0, merc, (uint32_t)lv, 0);
    }
}
}
