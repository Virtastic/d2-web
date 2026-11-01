// D2Game death gold penalty in the retail 32-bit layout (addresses are the low 32 bits of a pointer).
#include <cstdint>
#include <cstring>

static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static uint8_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }

struct D2Game38180Env {
    uint32_t (*getStat)(uint32_t unit, uint32_t stat, uint32_t layer);              // 0xA42A thunk (stdcall 3)
    void (*setStat)(uint32_t unit, uint32_t stat, uint32_t value, uint32_t layer);  // 0xA472 thunk (stdcall 4)
    uint32_t (*bankMax)(uint32_t unit);                                             // 0xA4A8 thunk (stdcall 1)
    uint32_t (*goldMax)(uint32_t unit);                                             // 0xA45A thunk (stdcall 1)
    uint32_t (*minionOwner)(uint32_t unit);                                         // 0x21260: ECX = unit
    void (*dropGold)(uint32_t game, uint32_t id, uint32_t unit, uint32_t amount);    // 0x37F80: EAX = game, ECX = id; stack unit, amount (ret 8)
};

// Stat value clamped the way retail inlines it: negative gives 0; a player over the cap gives 0.
static uint32_t clampFor(uint32_t unit, int32_t v, bool bank, const D2Game38180Env& env)
{
    if (v < 0) return 0;
    if (rd32(unit) == 0 && v > (int32_t)(bank ? env.bankMax(unit) : env.goldMax(unit))) return 0;
    return (uint32_t)v;
}

// 1.13c D2Game+0x38180
// ESI = unit; stack game, killer; ret 8. loss = min(level, 20) * (gold + bank) / 100. A game whose byte +0x6A is 3
// keeps at least 500 * level and never takes more than the carried gold. A player killer, or a monster whose
// minion owner (0x21260) is a player, makes it a PvP death: the excess over carried gold comes out of the bank and
// the carried gold stat becomes the loss, then 0x37F80 drops the loss. Otherwise 0x37F80 drops what is left of the
// carried gold (or the bank pays the excess), and carried gold becomes 0. Stat 0xAF records the loss.
void PLAYER_GoldDeathPenalty_38180(uint32_t unit, uint32_t game, uint32_t killer, const D2Game38180Env& env)
{
    const int32_t level = (int32_t)env.getStat(unit, 0xC, 0);
    const int32_t gold = (int32_t)env.getStat(unit, 0xE, 0);
    const int32_t bank = (int32_t)env.getStat(unit, 0xF, 0);
    const int32_t total = gold + bank;
    const int32_t m = level < 0x14 ? level : 0x14;
    int32_t loss = m * total / 100;
    const int32_t keep = total - loss;
    bool pvp = false;
    if (killer != 0 && killer != unit) {
        const uint32_t t = rd32(killer);
        if (t == 0) pvp = true;
        else if (t == 1) { const uint32_t o = env.minionOwner(killer); if (o != 0 && rd32(o) == 0) pvp = true; }
    }
    const uint8_t mode = rd8(game + 0x6A);
    if (mode == 3) {
        const int32_t floor = level * 0x1F4;
        if (keep < floor) { loss = total - floor; if (loss < 0) loss = 0; }
        if (loss > gold) loss = gold;
    }
    if (pvp) {
        if (loss > gold) {
            const int32_t rest = bank - loss + gold;
            if (unit != 0) {
                env.setStat(unit, 0xF, clampFor(unit, rest, true, env), 0);
                env.setStat(unit, 0xE, clampFor(unit, loss, false, env), 0);
            }
        }
        env.dropGold(game, unit ? rd32(unit + 0xC) : 0xFFFFFFFFu, unit, (uint32_t)loss);
    } else {
        if (loss <= gold) {
            env.dropGold(game, unit ? rd32(unit + 0xC) : 0xFFFFFFFFu, unit, (uint32_t)(gold - loss));
        } else if (mode != 3) {
            if (unit == 0) return;
            env.setStat(unit, 0xF, clampFor(unit, bank - loss + gold, true, env), 0);
        }
        if (unit == 0) return;
        if (rd32(unit) == 0) env.goldMax(unit);
        env.setStat(unit, 0xE, 0, 0);
    }
    if (unit == 0) return;
    env.setStat(unit, 0xAF, loss < 0 ? 0 : (uint32_t)loss, 0);
}
