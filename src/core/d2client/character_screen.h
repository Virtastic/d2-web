#pragma once
// The character screen's computed numbers for one player unit. Single-player and a joined realm game
// both read the panel through this one function; only the unit and the game mode differ.
// `expansion` is 1 (Lord of Destruction), 0 (classic) or -1 (no game: no difficulty penalty).
#include <algorithm>
#include <cstdint>
#include <D2DataTbls.h>
#include <D2Inventory.h>
#include <D2Items.h>
#include <D2StatList.h>
#include <Units/Units.h>

inline int d2CharacterScreen(D2UnitStrc* p, int expansion, int difficulty, int32_t* out) {
    if (!out || !p) return 0;
    out[0] = UNITS_GetDefense(p); out[1] = UNITS_GetAttackRate(p);
    const int str = STATLIST_UnitGetStatValue(p, STAT_STRENGTH, 0), dex = STATLIST_UnitGetStatValue(p, STAT_DEXTERITY, 0);
    D2UnitStrc* w = p->pInventory ? INVENTORY_GetItemFromBodyLoc(p->pInventory, BODYLOC_RARM) : nullptr;
    const int bonus = w ? str * ITEMS_GetStrengthBonus(w) / 100 + dex * ITEMS_GetDexBonus(w) / 100 : 0;
    out[2] = STATLIST_GetMinDamageFromUnit(p, 0) * (100 + bonus) / 100; out[3] = STATLIST_GetMaxDamageFromUnit(p, 0) * (100 + bonus) / 100;
    if (!w) {
        // 1.13c D2Client+0x31220 unarmed branch: base (stat 21 + 1, stat 22 + 2), percent = stat 25 + strength
        // (floored at -90), min = (stat 18 + pct + 100) * min / 100, max = (stat 17 + pct + 100) * max / 100; 0x30E30 then forces min >= 1, max >= min + 1.
        int pct = STATLIST_UnitGetStatValue(p, STAT_DAMAGEPERCENT, 0) + str;
        if (pct < -90) pct = -90;
        out[2] = (STATLIST_UnitGetStatValue(p, STAT_ITEM_MINDAMAGE_PERCENT, 0) + pct + 100) * (STATLIST_UnitGetStatValue(p, STAT_MINDAMAGE, 0) + 1) / 100;
        out[3] = (STATLIST_UnitGetStatValue(p, STAT_ITEM_MAXDAMAGE_PERCENT, 0) + pct + 100) * (STATLIST_UnitGetStatValue(p, STAT_MAXDAMAGE, 0) + 2) / 100;
        if (out[3] > 0) { if (out[2] < 1) out[2] = 1; if (out[3] < out[2] + 1) out[3] = out[2] + 1; }
    }
    out[4] = STATLIST_UnitGetStatValue(p, STAT_STAMINA, 0) >> 8; out[5] = STATLIST_UnitGetStatValue(p, STAT_MAXSTAMINA, 0) >> 8;
    const int ids[4] = { STAT_FIRERESIST, STAT_COLDRESIST, STAT_LIGHTRESIST, STAT_POISONRESIST };
    const int maxIds[4] = { STAT_MAXFIRERESIST, STAT_MAXCOLDRESIST, STAT_MAXLIGHTRESIST, STAT_MAXPOISONRESIST };
    for (int i = 0; i < 4; i++) {
        int resistance = STATLIST_UnitGetStatValue(p, ids[i], 0);
        // D2Client's character panel applies the difficulty penalty before its -100 floor
        // and maximum-resistance ceiling. Lord of Destruction reads ResistPenalty from the
        // active DifficultyLevels row; classic uses the original -20/-50 penalties.
        if (expansion > 0) {
            if (D2DifficultyLevelsTxt* row = DATATBLS_GetDifficultyLevelsTxtRecord(difficulty))
                resistance += static_cast<int32_t>(row->dwResistPenalty);
        } else if (expansion == 0 && difficulty == DIFFMODE_NIGHTMARE) {
            resistance -= 20;
        } else if (expansion == 0 && difficulty == DIFFMODE_HELL) {
            resistance -= 50;
        }
        resistance = std::max(resistance, -100);
        const int maximum = std::min(95, 75 + STATLIST_UnitGetStatValue(p, maxIds[i], 0));
        out[6 + i] = std::min(resistance, maximum);
    }
    // out[10] the Next Level figure (the experience table's threshold for the next level, 500 at 1),
    // out[11..16] life, max life, mana, max mana (whole points), class id.
    const int level = STATLIST_UnitGetStatValue(p, STAT_LEVEL, 0);
    out[10] = level < DATATBLS_GetMaxLevel((int)p->dwClassId) ? (int32_t)DATATBLS_GetLevelThreshold((int)p->dwClassId, (uint32_t)level) : 0;
    out[11] = STATLIST_UnitGetStatValue(p, STAT_HITPOINTS, 0) >> 8; out[12] = STATLIST_UnitGetStatValue(p, STAT_MAXHP, 0) >> 8;
    out[13] = STATLIST_UnitGetStatValue(p, STAT_MANA, 0) >> 8; out[14] = STATLIST_UnitGetStatValue(p, STAT_MAXMANA, 0) >> 8;
    out[15] = (int32_t)p->dwClassId;
    return 1;
}
