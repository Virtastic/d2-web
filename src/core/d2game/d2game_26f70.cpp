// D2Game+0x26F70: pick the potion code a Find Potion corpse yields. EAX = unit, stack: game, skill id (ret 8).
// The unit's room gives the act (D2Common 10331 -> 10826 level id -> 10459 act, low byte); act >= 5 yields 0.
// row = act + game difficulty (+0x6D) * 5. One roll of the unit seed (+0x20 lo, +0x24 hi) mod 100 picks the
// column: below Param3 (0x1BF0) mana, below Param3 + Param4 (0x1C20, read only when needed) rejuvenation,
// else healing. Rows past the table count (0x10E49C, 15) yield 0; row 15 itself reads the count and the two
// dwords after it, exactly as retail's `<=` bound does.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game26F70Env {
    uint32_t (*getRoom)(uint32_t unit);          // thunk 0xA478 (D2Common 10331)
    uint32_t (*levelIdFromRoom)(uint32_t room);  // thunk 0xA556 (D2Common 10826)
    uint32_t (*actFromLevel)(uint32_t level);    // thunk 0xA56E (D2Common 10459)
    int32_t (*skillParam3)(int32_t skill);       // D2Game 0x1BF0 (EAX = skill)
    int32_t (*skillParam4)(int32_t skill);       // D2Game 0x1C20 (EAX = skill)
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }

// D2Game .data 0x10E3E8: 15 rows of {healing, mana, rejuvenation} item codes, then the row count and two zero dwords.
static const uint32_t kPotionCodes[16 * 3] = {
    0x20327068,0x2032706d,0x20737672, 0x20337068,0x2033706d,0x20737672, 0x20337068,0x2033706d,0x20737672,
    0x20347068,0x2034706d,0x206c7672, 0x20347068,0x2034706d,0x206c7672, 0x20347068,0x2034706d,0x206c7672,
    0x20347068,0x2035706d,0x206c7672, 0x20357068,0x2035706d,0x206c7672, 0x20357068,0x2035706d,0x206c7672,
    0x20357068,0x2035706d,0x206c7672, 0x20357068,0x2035706d,0x206c7672, 0x20357068,0x2035706d,0x206c7672,
    0x20357068,0x2035706d,0x206c7672, 0x20357068,0x2035706d,0x206c7672, 0x20357068,0x2035706d,0x206c7672,
    15, 0, 0
};

// 1.13c D2Game+0x26F70
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_26f70(uint32_t unit, uint32_t game, int32_t skillId, const D2Game26F70Env* env)
{
    const uint32_t room = env->getRoom(unit);
    if (!room) return 0;
    const uint32_t act = env->actFromLevel(env->levelIdFromRoom(room)) & 0xff;
    if (act >= 5) return 0;
    const int32_t row = (int32_t)(act + (uint32_t)*(const uint8_t*)(uintptr_t)(game + 0x6d) * 5);
    const uint64_t seed = (uint64_t)rd32(unit, 0x20) * 0x6ac690c5u + rd32(unit, 0x24);
    wr32(unit, 0x24, (uint32_t)(seed >> 32));
    wr32(unit, 0x20, (uint32_t)seed);
    const int32_t roll = (int32_t)((uint32_t)seed % 100);
    uint32_t column = 0;
    const int32_t p3 = env->skillParam3(skillId);
    if (roll < p3) column = 1;
    else if (roll < (int32_t)((uint32_t)env->skillParam4(skillId) + (uint32_t)p3)) column = 2;
    if (row > 15) return 0;
    return kPotionCodes[row * 3 + column];
}
