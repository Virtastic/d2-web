// Shared vector file format, linked by BOTH the retail capture tool (Win32, retail DLLs) and the wasm
// engine so the two can never drift. Deliberately dumb: fixed little-endian header, then
// `count` records of `recordSize` bytes. Record layouts live here too.
//
//   +0  "D2VEC\0"            magic (6)
//   +6  u16 version          format version, 1
//   +8  u32 kind             D2VecKind
//   +12 u32 recordSize
//   +16 u32 count
//   +20 u32 reserved
//   +24 records...
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#pragma pack(push, 1)
struct D2VecHeader {
    char     magic[6];
    uint16_t version;
    uint32_t kind, recordSize, count, reserved;
};

// PRNG: one record = one seeded run. The retail seed struct is {lowSeed, highSeed}; `count`
// rolls are made (RollRandomNumber when nMax == 0, RollLimitedRandomNumber(nMax) otherwise),
// the final seed state is recorded, and `hash` is FNV-1a over the u32 result of every roll in
// order. A mismatch localises by re-running with smaller counts; storing every draw would put
// tens of MB in git for no extra evidence.
struct D2VecPrng {
    uint32_t lowIn, highIn, nMax, count;
    uint32_t lowOut, highOut, hash;
};

// DRLG: one record = one generated level. `hash` is FNV-1a over the level's subtile collision
// grid as the retail engine builds it (u16 per subtile, row-major, width*height cells, cells no
// room covers = 0xFFFF), the grid d2mapapi/libd2 compare cell-for-cell. Grids are ~11M cells
// per seed, so the vector keeps the hash; the retail capture tool writes the full grids to a scratch dir
// on request (D2CAPTURE_DRLG_FULL=<dir>) for diagnosis.
struct D2VecDrlg {
    uint32_t seed;
    uint8_t  difficulty, act;
    uint16_t level;
    int32_t  originX, originY;      // level position in subtiles (dwPosX*5, dwPosY*5)
    uint32_t width, height;         // in subtiles (dwSizeX*5, dwSizeY*5)
    uint32_t hash;
    uint32_t tombArea;              // ActMisc.RealTombArea for act 2 (which tomb is the real one), else 0
};
// ITEMGEN: one record = one D2GAME_CreateItemEx call on a headless 1.13c D2Game (the retail capture tool
// boots the server DLL the way d2-dedicated-server's d2host does). Inputs are the D2ItemDropStrc
// fields the call reads; outputs are what the item unit holds afterwards and
// its ITEMS_Serialize bitstream (D2Common #10987), which is the wire/save encoding of the whole
// item -- quality, affixes, sockets, stats. Byte-equal bitstreams = identical items.
struct D2VecItem {
    uint32_t itemId, ilvl, qualityIn, seed, itemSeed, itemIndex, flags1, extraFlags;
    uint32_t classId, qualityOut, ilvlOut, uniqueId, seedLoOut, seedHiOut, itemFlags, bitBytes;   // bitBytes 0 = the call returned no item
    uint32_t itemSeedLoOut, itemSeedHiOut;   // the item's own (mod) seed after generation: how many rolls the affix path took
    uint8_t  bits[120];                      // bitBytes > 120: truncated, compare the first 120
};
#pragma pack(pop)
// kind 4 MONSPAWN: one record per (seed, difficulty, level) after every
// room of the level was activated in Room2-list order by the 1.13c D2Game: how many units the
// rooms hold, how many are monsters, and FNV-1a over the D2VecMonUnit records in room order.
#pragma pack(push, 1)
struct D2VecMonLevel {
    uint32_t seed; uint8_t difficulty, act; uint16_t level;
    uint32_t units, monsters, hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecMonLevel) == 20, "D2VecMonLevel layout");

// The per-unit record the hash is over (and what D2CAPTURE_MONS_FULL / D2_MONS_DUMP write for
// diagnosis, never committed). Rooms are activated in Room2-list order; `room` is that index.
#pragma pack(push, 1)
struct D2VecMonUnit {
    uint32_t seed; uint8_t difficulty, act; uint16_t level, room;
    uint8_t  unitType;      // 1 monster, 2 object, 4 item (D2C_UnitTypes)
    uint32_t classId;       // monstats / objects / items row
    uint16_t x, y;          // D2DynamicPathStrc pos
    uint8_t  mode;
    uint32_t seedLo, seedHi;
    uint8_t  monFlags;      // MonsterData +0x16: normal/champ/boss/minion bits
    uint8_t  enchants[9];   // MonsterData +0x1C
    uint16_t uniqueNo;      // MonsterData +0x26 (superunique index)
    uint32_t hp, maxHp, lvl;   // stats 6, 7, 12 (server view, with the value shift)
};
#pragma pack(pop)
static_assert(sizeof(D2VecMonUnit) == 52, "D2VecMonUnit layout");
static inline uint32_t d2vec_mon_hash(uint32_t h, const D2VecMonUnit* u) {   // the hashed part starts at `room`
    const uint8_t* p = (const uint8_t*)&u->room; for (size_t i = 0; i < sizeof(D2VecMonUnit) - 8; i++) { h ^= p[i]; h *= 16777619u; }
    return h;
}

// AISPAWN: pointer-free AI state immediately after room activation.  This is the common
// starting point for the active-AI replay: it proves that retail x86 and wasm64 created the
// same controller, command, ownership/minion relationship and first scheduled think frame.
#pragma pack(push, 1)
struct D2VecAiLevel {
    uint32_t seed; uint8_t difficulty, act; uint16_t level;
    uint32_t monsters, hash, reserved;
};
struct D2VecAiUnit {
    uint16_t room, ordinal;
    uint32_t classId, unitId;
    uint16_t x, y;
    int32_t  specialState;
    uint16_t flags, reserved;
    int32_t  ownerGuid;
    uint32_t ownerType;
    int32_t  params[3];
    uint32_t hasCurrentCmd;
    int32_t  currentCmdParams[5];
    uint32_t minionCount, minionHash;
    int32_t  thinkFrame;
};
#pragma pack(pop)
static_assert(sizeof(D2VecAiLevel) == 20, "D2VecAiLevel layout");
static_assert(sizeof(D2VecAiUnit) == 80, "D2VecAiUnit layout");
static inline uint32_t d2vec_ai_hash(uint32_t h, const D2VecAiUnit* u) {
    const uint8_t* p = (const uint8_t*)u;
    for (size_t i = 0; i < sizeof(*u); i++) { h ^= p[i]; h *= 16777619u; }
    return h;
}

// AITARGET: the same active-AI state with a live player attached to the first populated room.
// The suffix records mutable unit/path/skill state that the initial no-player checkpoint omits.
#pragma pack(push, 1)
struct D2VecAiTargetUnit {
    D2VecAiUnit ai;
    uint32_t mode, seedLo, seedHi;
    int32_t hp, maxHp;
    uint32_t targetType, targetId;
    uint16_t targetX, targetY;
    int32_t usedSkillId;
    int32_t modeEventFrame;
    int32_t framePrecise, frameCountPrecise;
    int16_t animSpeed;
    uint8_t actionFrame, pad;
};
#pragma pack(pop)
static_assert(sizeof(D2VecAiTargetUnit) == 132, "D2VecAiTargetUnit layout");
static inline uint32_t d2vec_aitarget_hash(uint32_t h, const D2VecAiTargetUnit* u) {
    const uint8_t* p = (const uint8_t*)u;
    for (size_t i = 0; i < sizeof(*u); i++) { h ^= p[i]; h *= 16777619u; }
    return h;
}

// COMBAT: D2DamageStrc immediately after SUNITDMG_CalculateTotalDamage in ordinary/skill
// combat. The surrounding identity and seed fields make each trace event pointer-free and
// associate it with the deterministic spawned-game fixture that produced it.
#pragma pack(push, 1)
struct D2VecCombat {
    uint32_t seed;
    uint8_t difficulty, act;
    uint16_t level;
    uint32_t frame;
    uint32_t attackerType, attackerClass, attackerId;
    uint32_t defenderType, defenderClass, defenderId;
    uint32_t seedLo, seedHi;
    uint8_t damage[112];
};
#pragma pack(pop)
static_assert(sizeof(D2VecCombat) == 156, "D2VecCombat layout");

// COMBATRESULT: defender state after the fixture has executed all requested frames. This is a
// separate record from D2VecCombat because the latter is captured before the queued combat is
// applied, while this proves the resulting unit/stat mutation.
#pragma pack(push, 1)
struct D2VecCombatResult {
    uint32_t seed;
    uint8_t difficulty, act;
    uint16_t level;
    uint32_t frame;
    uint32_t unitType, unitClass, unitId, mode;
    uint32_t seedLo, seedHi;
    int32_t hp, maxHp, mana, maxMana, stamina, maxStamina;
    uint32_t lastHitClass, flags, flagsEx;
};
#pragma pack(pop)
static_assert(sizeof(D2VecCombatResult) == 72, "D2VecCombatResult layout");

// DAMAGECALC: a direct retail/wasm replay of SUNITDMG_CalculateTotalDamage. The defender's
// relevant reduction/resistance stats and the complete before/after damage structures are
// explicit, allowing elemental, poison, conversion, pierce and absorb cases to grow in-place.
#pragma pack(push, 1)
struct D2VecDamageCalc {
    uint32_t seed;
    uint8_t difficulty, act;
    uint16_t level;
    uint32_t caseId;
    uint32_t seedLoIn, seedHiIn, seedLoOut, seedHiOut;
    int32_t defenderStats[8]; // normal/magic flat reduction; physical/magic/fire/light/cold/poison resistance
    uint8_t damageIn[112], damageOut[112];
};
#pragma pack(pop)
static_assert(sizeof(D2VecDamageCalc) == 284, "D2VecDamageCalc layout");
static_assert(sizeof(D2VecItem) == 192, "itemgen record is 184 bytes on every platform");
static_assert(sizeof(D2VecHeader) == 24, "header is 24 bytes on every platform");
static_assert(sizeof(D2VecDrlg) == 32, "drlg record is 32 bytes on every platform");
static_assert(sizeof(D2VecPrng) == 28, "prng record is 28 bytes on every platform");

// TABLE: the in-memory excel table exactly as retail holds it after DATATBLS_LoadAllTxts --
// `count` records of `recordSize` bytes copied verbatim from sgptDataTables. One file per table
//; recordSize is sizeof the retail record struct, so a layout
// mismatch is refused by d2vec_read_header rather than compared byte-by-byte as garbage.
enum D2VecKind : uint32_t { D2VEC_PRNG = 1, D2VEC_DRLG = 2, D2VEC_ITEMGEN = 3, D2VEC_MONSPAWN = 4, D2VEC_TABLE = 5, D2VEC_COMBAT = 6, D2VEC_AISPAWN = 7, D2VEC_AIFRAME = 8, D2VEC_AITARGET = 9, D2VEC_COMBATRESULT = 10, D2VEC_DAMAGECALC = 11 };

static inline uint32_t d2vec_fnv1a_step(uint32_t h, uint32_t v) {
    for (int i = 0; i < 4; i++) { h ^= (v >> (8 * i)) & 0xFF; h *= 16777619u; }
    return h;
}
#define D2VEC_FNV_INIT 2166136261u

// Returns bytes written (0 on failure). Records are written verbatim; keep them packed and
// free of pointers.
static inline size_t d2vec_write(FILE* f, uint32_t kind, uint32_t recordSize, uint32_t count, const void* records) {
    D2VecHeader h = { {'D','2','V','E','C',0}, 1, kind, recordSize, count, 0 };
    if (fwrite(&h, sizeof h, 1, f) != 1) return 0;
    size_t n = (size_t)recordSize * count;
    if (n && fwrite(records, 1, n, f) != n) return 0;
    return sizeof h + n;
}

// Validates the header; returns false on a mismatch (wrong kind or record size is a versioning
// error, not a parity diff -- refuse rather than compare garbage).
static inline bool d2vec_read_header(FILE* f, uint32_t kind, uint32_t recordSize, D2VecHeader* h) {
    if (fread(h, sizeof *h, 1, f) != 1) return false;
    return memcmp(h->magic, "D2VEC", 6) == 0 && h->version == 1 && h->kind == kind && h->recordSize == recordSize;
}
