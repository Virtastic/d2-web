// SUNITINACTIVE_RestoreSpecialMonsterParameters: copy the saved command parameters of an inactive-monster node back onto the unit.
// Register/stack args: EDI = unit, stack = game, node (ret 8). Retail-layout (32-bit offsets) rule; engine callees and module globals are Env members.
// Node: +0x18 type flags (0x800 = super unique), +0x24 map-AI pointer, +0x3C boss index (short), +0x4C / +0x50 saved command params.
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G1F2D0Env {
    uint32_t dataTables;        // *sgptDataTables (slot 0xF829C): +0xA78 monstats records (0x1A8 each), +0xA80 count
    uint32_t bitMasks;          // gdwBitMasks (slot 0xF8BE0)
    uint32_t savedFlagsMarker;  // DAT_6FD2E5FC; noXpBit DAT_6FD2E600; noTcBit DAT_6FD2E604
    uint32_t noXpBit, noTcBit;
    void (*copyAiCommand)(uint32_t game, uint32_t unit, const uint32_t* cmd);   // 0x21290: EAX game, ECX unit, stack cmd (words +8..+0x18 are the command)
    void (*changeMapAi)(uint32_t ai, uint32_t nodeMapAiSlot, uint32_t aiMapAiSlot);   // thunk 0xA562 (D2Common #10171): EAX ai, ECX &node+0x24, EDX ai+0x38
};

// 1.13c D2Game+0x1F2D0
void SUNITINACTIVE_RestoreSpecialMonsterParameters(uint32_t unit, uint32_t game, uint32_t node, const D2G1F2D0Env& env)
{
    const int32_t cls = (int32_t)rd32(unit + 4);
    const int32_t count = (int32_t)rd32(env.dataTables + 0xA80);
    const bool isMonster = rd32(unit) == 1;
    const uint32_t pData = isMonster ? rd32(unit + 0x14) : 0;
    const uint32_t ai = pData ? rd32(pData + 0x28) : 0;
    // the inlined AIGENERAL_SetCurrentAiCommand lookup: the command of type 10 in the AI control's list (0 when none)
    auto find = [&]() -> uint32_t {
        if (!ai) return 0;
        const uint32_t list = rd32(ai + 0x20);
        if (!list) return 0;
        uint32_t n = rd32(list);
        while (rd32(n + 8) != 10 && n != list) n = rd32(n);
        return rd32(n + 8) == 10 ? n : 0;
    };
    auto setCmd = [&](bool clearThird) {
        if (!find()) { const uint32_t cmd[7] = { 0, 0, 10, 0, 0, 0, 0 }; env.copyAiCommand(game, unit, cmd); }
        if (const uint32_t c = find()) {
            wr32(c + 0xC, rd32(node + 0x4C)); wr32(c + 0x10, rd32(node + 0x50));
            if (clearThird) wr32(c + 0x14, 0);
        }
    };
    const uint32_t rec = (cls < 0 || cls >= count) ? 0 : rd32(env.dataTables + 0xA78) + (uint32_t)cls * 0x1A8;
    const uint32_t mf = rd8(rec + 0xD);
    if ((rd32(env.bitMasks + 4) & mf) || (rd32(env.bitMasks) & mf) || (rd32(env.bitMasks + 8) & mf)) {
        setCmd(true);
        if (rd32(node + 0x24)) env.changeMapAi(ai, node + 0x24, ai + 0x38);
    }
    if (cls == 0x10B || cls == 0xF3 || ((rd32(node + 0x18) & 0x800) && (int16_t)rd16(node + 0x3C) == 6)) {
        if (rd32(node + 0x4C) || rd32(node + 0x50)) setCmd(false);
    }
    if (isMonster && cls >= 0) {
        int32_t base = cls;
        if (cls < count) { const uint32_t r = rd32(env.dataTables + 0xA78) + (uint32_t)cls * 0x1A8; if (r) base = (int16_t)rd16(r + 2); }
        if (base >= 0 && base < count) {
            if (base == 0x44) { if (ai) wr32(ai + 0x1C, rd32(node + 0x4C)); }
            else if (base == 0x21F) { if (ai) wr32(ai + 0x14, rd32(node + 0x4C)); }
        }
    }
    const uint8_t p1b = (uint8_t)rd32(node + 0x4C);
    if (cls == 0x138) {
        if (isMonster) { if (pData) wr8(pData + 0xE, p1b); if (pData) wr8(pData + 0xF, p1b); }
    } else if (cls == 0x137) {
        if (isMonster && pData) wr8(pData + 0xE, p1b);
    }
    if (rd32(node + 0x4C) == env.savedFlagsMarker) {
        if (rd32(node + 0x50) & env.noXpBit) wr32(unit + 0xC4, rd32(unit + 0xC4) | 0x4000000);
        if (rd32(node + 0x50) & env.noTcBit) wr32(unit + 0xC4, rd32(unit + 0xC4) | 0x20000);
    }
}
