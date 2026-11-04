// D2Game+0xDDCB0 -- experience bonus for a hired mercenary. EAX = bonus, stack a0 game, a1 owner (player), a2 hireling, a3 hireling level (ret 0x10).
// Written from the 1.13c disassembly; callees are the D2Seam. bonus <= 0 returns. guid = hireling ? [hireling + 0xC] : -1; pet = 0x96570 (EAX = owner,
// EBX = guid), 0: return; rec = 0xA3BE([game + 0x70], [pet + 8], level), 0: return; level >= 0xA42A(owner, 0xC, 0) returns; exp = 0xA3E2(hireling, 0xD, 0);
// newExp = exp + 2 * bonus; lim = 0xA48A(0) - 1; level >= lim returns. 0xA472(hireling, 0xD, newExp, 0); client = owner type 0 ? [0xA4C0(owner) + 0x9C] : 0;
// 0x8BB10 (EAX = client, EDX = newExp, EDI = exp, EBX = 0xD, stack hireling). Walk lv from level while lv + 1 < lim and 0xA3A6(lv + 1, [rec + 0x20]) <= newExp
// (unsigned); lv > level: 0x48BA0(game, owner, hireling, lv), 0x48950 (EAX = owner, ECX = game), 0xDFFD0 (ESI = hireling, 0x5B, owner),
// 0x37030 (EBX = hireling, game, 0xC, 0, 0).
#include "d2game_seam4e.h"
// 1.13c D2Game+0xDDCB0
extern "C" void d2game_ddcb0(uint32_t bonus, uint32_t game, uint32_t owner, uint32_t hireling, uint32_t level, const D2Seam* e)
{
    if ((int32_t)bonus <= 0) return;
    const uint32_t guid = hireling ? d2rd32(hireling + 0xC) : 0xFFFFFFFFu;
    const uint32_t pet = e->call(0x96570, owner, 0, 0, guid);
    if (!pet) return;
    const uint32_t rec = e->call(0xA3BE, 0, 0, 0, 0, 0, d2rd32(game + 0x70), d2rd32(pet + 8), level);
    if (!rec) return;
    if ((int32_t)level >= (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, owner, 0xC, 0)) return;
    const uint32_t exp = e->call(0xA3E2, 0, 0, 0, 0, 0, hireling, 0xD, 0);
    const uint32_t newExp = exp + bonus * 2;
    const int32_t lim = (int32_t)e->call(0xA48A, 0, 0, 0, 0, 0, 0) - 1;
    if ((int32_t)level >= lim) return;
    e->call(0xA472, 0, 0, 0, 0, 0, hireling, 0xD, newExp, 0);
    uint32_t client = 0;
    if (owner && d2rd32(owner) == 0) client = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, owner) + 0x9C);
    e->call(0x8BB10, client, 0, newExp, 0xD, 0, hireling);
    e->edi(0x8BB10, exp);
    uint32_t lv = level;
    do {
        const uint32_t next = lv + 1;
        if (e->call(0xA3A6, 0, 0, 0, 0, 0, next, d2rd32(rec + 0x20)) > newExp) break;
        lv = next;
    } while ((int32_t)lv < lim);
    if ((int32_t)lv <= (int32_t)level) return;
    e->call(0x48BA0, 0, 0, 0, 0, 0, game, owner, hireling, lv);
    e->call(0x48950, owner, game);
    e->call(0xDFFD0, 0, 0, 0, 0, hireling, 0x5B, owner);
    e->call(0x37030, 0, 0, 0, hireling, 0, game, 0xC, 0, 0);
}
