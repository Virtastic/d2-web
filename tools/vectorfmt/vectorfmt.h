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
// AITRACE (kind 27): one aitarget unit record per monster per snapshot, prefixed by the frame offset
// the snapshot was taken at (every D2CAPTURE_AI_EVERY frames); the player's state per snapshot is a
// D2VecCombatResult (its `frame` = the same offset) in aitrace-player.bin.
#pragma pack(push, 1)
struct D2VecAiTrace { uint32_t frame; D2VecAiTargetUnit unit; };
#pragma pack(pop)
static_assert(sizeof(D2VecAiTrace) == 136, "D2VecAiTrace layout");
// AIPATH (kind 28, aitrace-path.bin beside kind 27): each traced monster's D2DynamicPathStrc per
// snapshot, pointer fields dropped, every scalar at its 1.13c offset (named in the comments), the first
// 16 path points and the saved steps. Retail reads the offsets; the engine copies the fields.
#pragma pack(push, 1)
struct D2VecAiPath {
    uint32_t frame, unitId;
    uint32_t precX, precY;              // 0x00, 0x04 (16.16 subtiles)
    int32_t  clientX, clientY;          // 0x08, 0x0C
    uint32_t target, prevTarget, finalTarget; // 0x10, 0x14, 0x18 (x | y << 16)
    int32_t  curIdx, points;            // 0x24, 0x28
    uint32_t flags, unk38, pathType, prevPathType, unitSize, collisionPattern, footprintMask, moveTestMask; // 0x34..0x50
    uint16_t collidedWith, pad56;       // 0x54
    uint32_t targetType, targetId;      // 0x5C, 0x60
    uint8_t  direction, newDirection, diffDirection, unk67, speed, unk69; // 0x64..0x69
    int32_t  dirVecX, dirVecY, velVecX, velVecY; // 0x6A, 0x72 (unaligned in retail)
    int32_t  velocity, prevVelocity, maxVelocity, acceleration, accelCounter; // 0x7C..0x8C
    uint8_t  dist, distMax, idaF, stepNum, distance, pad95[3]; // 0x90..0x97
    int32_t  dirOffset;                 // 0x98
    uint32_t pathPoints[16];            // 0x9C
    int32_t  savedSteps;                // 0x1D4
    uint32_t saved[10];                 // 0x1D8
};
#pragma pack(pop)
static_assert(sizeof(D2VecAiPath) == 250, "D2VecAiPath layout");
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

// A1Q6: the game-global transition after retail's live Andariel killed callback.  This first
// slice deliberately has no player reward flags: it proves the boss/timer state machine before
// the separate player-record attachment path is added.
#pragma pack(push, 1)
struct D2VecA1Q6 {
    uint32_t seed;
    uint8_t difficulty, phase;
    uint16_t level;
    uint32_t andarielId;
    int32_t andarielGuid;
    uint16_t timerInvocations;
    uint8_t andarielKilled, state;
    uint8_t monsterKilledCallback, timerPresent;
    uint32_t hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecA1Q6) == 26, "D2VecA1Q6 layout");

// A1Q6 player slice: post-kill player quest-record state through the live retail callback.
#pragma pack(push, 1)
struct D2VecA1Q6Player {
    uint32_t seed;
    uint8_t difficulty, phase;
    uint16_t level;
    uint32_t playerId, playerFlags;
    uint8_t primaryGoalDone, rewardPending, rewardGranted, completedNow;
    uint32_t hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecA1Q6Player) == 24, "D2VecA1Q6Player layout");

// A1Q6 timer slice: the scheduled retail callback after Andariel's kill transition.  The
// callback is replayed through invocation 10, where it opens the return portal in Catacombs 4.
#pragma pack(push, 1)
struct D2VecA1Q6Timer {
    uint32_t seed;
    uint8_t difficulty, phase;
    uint16_t level;
    uint32_t andarielId;
    uint16_t timerInvocations;
    uint8_t timerStillScheduled, portalCount;
    uint32_t portalId, portalClass, hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecA1Q6Timer) == 28, "D2VecA1Q6Timer layout");

// A1Q6 completion slice: the same scheduled callback taken through invocation 12, where retail
// closes the status cycle (lastState 3, flag byte cleared, timer released) and composes the
// per-player quest-status packet 0x5D from QUESTS_RefreshStatus.  The packet bytes are read
// back from the client's queued packet-data tail, exactly as retail composed them.
#pragma pack(push, 1)
struct D2VecA1Q6Complete {
    uint32_t seed;
    uint8_t difficulty, phase;
    uint16_t level;
    uint16_t timerInvocations;
    uint8_t timerReleased, lastState;
    uint8_t state, questFlagsLow;
    uint16_t playerFlags;
    uint8_t packetHeader, packetQuestId, packetFlags, packetState;
    uint16_t packetField4, packetBytes;
    uint32_t hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecA1Q6Complete) == 28, "D2VecA1Q6Complete layout");

// A1Q6 quest-drop slice: the player-attached kill callback rolls two chipped gems and one
// normal gem off the global quest seed and drops them at Andariel through
// D2GAME_DropItemAtUnit.  One record per item unit found in the level afterwards, in Room2-list
// then unit-list order; the ITEMS_Serialize bitstream is the whole item (kind 3's standard).
#pragma pack(push, 1)
struct D2VecA1Q6Drop {
    uint32_t seed;
    uint8_t difficulty, phase;
    uint16_t level;
    uint32_t itemId, classId, quality, ilvl;
    uint16_t x, y, index, count;
    uint32_t seedLo, seedHi, bitBytes;
    uint8_t bits[32];
    uint32_t hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecA1Q6Drop) == 80, "D2VecA1Q6Drop layout");

// A1Q6 act travel: with the reward granted, client packet 0x38 to Warriv (class 155) takes
// retail's NPC_HandleDialogMessage -> PlayerChangeAct -> LEVEL_WarpUnit -> LEVEL_ChangeAct into
// Act II.  Recorded after the handler returns: where the player stands, which act the client
// and unit are in, the loaded-act mask, and what was queued to the client.
#pragma pack(push, 1)
struct D2VecA1Q6Travel {
    uint32_t seed;
    uint8_t difficulty, phase;
    uint16_t level;                       // level the player was placed in before the packet
    uint32_t warrivId, playerId;
    uint8_t action, handlerResult, clientAct, unitAct;
    uint16_t destLevel, x, y, roomsLoaded;   // destLevel: level of the player's room afterwards
    uint8_t actsLoaded, packetsFirst;         // pAct[] mask; first packet header appended
    uint16_t packetBytes;
    uint32_t hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecA1Q6Travel) == 36, "D2VecA1Q6Travel layout");

// Monster drop (kind 29): on a fresh level with the player fixture attached, every monster in
// Room2-list then unit-list order is handed to D2Game's monster-death drop routine (1.13c
// 0x6FC95680, D2MOO sub_6FC631B0: TC column by monster type, quest TC, item level) with the
// player as the mode change's target. One D2VecMonDrop per monster: the inputs the routine gave
// the treasure-class walk (D2Game 0x6FC32380 / D2MOO D2GAME_DropTC_6FC51360: TC row, quality and
// level arguments, number of walks) and the monster seed before and after it; one
// D2VecMonDropItem per item unit the call added to the level (Room2-list then unit-list order).
#pragma pack(push, 1)
struct D2VecMonDrop {
    uint32_t seed;
    uint8_t difficulty, act;
    uint16_t level;
    uint16_t room, ordinal;
    uint32_t monClass;
    uint32_t seedLo, seedHi;              // monster seed before the drop
    uint32_t tcId;                        // pTreasureClassEx row handed to the walk (0xFFFFFFFF: none)
    uint32_t walks;                       // DropTC calls the routine made
    int32_t quality, itemLevel;           // the walk's nQuality / nLvl arguments
    uint32_t seedLoAfter, seedHiAfter;
    uint32_t items, itemHash;             // FNV-1a over the D2VecMonDropItem records from classId on
};
#pragma pack(pop)
static_assert(sizeof(D2VecMonDrop) == 56, "D2VecMonDrop layout");
#pragma pack(push, 1)
struct D2VecMonDropItem {
    uint16_t monster, index;              // monster record index within the level, item index within its drop
    uint32_t classId, quality, ilvl, seedLo, seedHi;
    uint16_t x, y;
};
#pragma pack(pop)
static_assert(sizeof(D2VecMonDropItem) == 28, "D2VecMonDropItem layout");
static inline uint32_t d2vec_mondrop_item_hash(uint32_t h, const D2VecMonDropItem* it) {
    for (const uint8_t* p = (const uint8_t*)&it->classId; p < (const uint8_t*)(it + 1); ++p) { h ^= *p; h *= 16777619u; }
    return h;
}

// NPC dialogue: client packet 0x13 (interact with entity) from a player standing within 6
// subtiles of a town NPC takes retail's immediate activation path (0x6FC828D0 -> 0x6FC61C70 in
// D2MOO's map): QUESTS_NPCActivate composes the message list and the 40-byte 0x27 packet,
// then the global quest flags (0x29, 97 bytes) and the player's quest record (0x28, 103 bytes)
// are queued to the client.  One record per (game, NPC): every byte the handler queued, in
// order, as retail composed it.
#pragma pack(push, 1)
struct D2VecNpcDialog {
    uint32_t seed;
    uint8_t difficulty, phase;
    uint16_t level;
    uint32_t npcId, npcClass;
    uint16_t playerX, playerY;
    uint8_t handlerResult, packetCount;   // packetCount: whole packets of known size in the stream
    uint16_t packetBytes;                 // bytes the handler appended to the client queue
    uint8_t stream[512];                  // the appended bytes (one retail packet-data block)
    uint32_t hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecNpcDialog) == 540, "D2VecNpcDialog layout");

// NPC response: after the 0x13 activation, client packet 0x31 {npcGUID, messageId} for one of
// the message ids the 0x27 list offered (Rcv0x31 -> QUESTS_NPCMessage -> the quest callbacks).
// One record per (game, NPC, message): the game's global quest record before the message (an
// input -- earlier responses in the same game set global bits, e.g. Jerhyn's 253 marks A2Q0's
// primary goal, and packet 0x29 echoes them), everything the handler queued, and the player's
// whole quest record afterwards (the 96 bytes packet 0x28 carries).
#pragma pack(push, 1)
struct D2VecNpcResponse {
    uint32_t seed;
    uint8_t difficulty, phase;
    uint16_t level;
    uint32_t npcId, npcClass;
    uint16_t messageId;
    uint8_t handlerResult, packetCount;
    uint16_t packetBytes;
    uint8_t globalRecordBefore[96];
    uint8_t stream[512];
    uint8_t questRecord[96];
    uint32_t hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecNpcResponse) == 730, "D2VecNpcResponse layout");

// Vendor stock: after the 0x13 activation, client packet 0x38 {1, npcGUID, -1} (trade) makes
// retail build the NPC's vendor cache (D2GAME_STORES_CreateVendorCache -> FillStoreInventory).
// One record per item in the NPC's inventory afterwards, in list order, with the item's
// ITEMS_Serialize bitstream (kind 3's standard, first 80 bytes).  `phase` is the 0x38 action.
#pragma pack(push, 1)
struct D2VecVendorItem {
    uint32_t seed;
    uint8_t difficulty, phase;
    uint16_t level;
    uint32_t npcId, npcClass;
    uint16_t index, count;
    uint32_t itemId, classId, quality, ilvl;
    uint32_t seedLo, seedHi, bitBytes;
    uint8_t bits[80];
    uint32_t hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecVendorItem) == 132, "D2VecVendorItem layout");

// Vendor purchase: with the trade open and the fixture given `goldBefore`, client packet 0x32
// {npcGUID, itemGUID, type 0, itemMode, cost} for the vendor's first item, `cost` being what
// ITEMS_GetTransactionCost (D2Common #10107 on 1.13c) returns for it.  Records the handler
// result, the gold afterwards, the bought item (the first item of the player's inventory) with
// its bitstream and inventory position, the vendor's remaining stock count, and the stream.
#pragma pack(push, 1)
struct D2VecVendorBuy {
    uint32_t seed;
    uint8_t difficulty, phase;
    uint16_t level;
    uint32_t npcId, npcClass, itemId;
    int32_t cost, goldBefore, goldAfter;
    uint8_t handlerResult, playerItems;
    uint16_t vendorItemsAfter;
    uint32_t boughtId, boughtClass, boughtSeedLo, boughtSeedHi, boughtBits;
    uint8_t bits[80];
    uint16_t packetBytes, packetCount;
    uint8_t stream[512];
    uint32_t hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecVendorBuy) == 656, "D2VecVendorBuy layout");

// Vendor sale: right after the purchase, client packet 0x33 {npcGUID, boughtGUID, itemMode,
// 0, price} sells the bought item back at ITEMS_GetTransactionCost's sell price.  Records the
// result, gold before/after, the player's remaining item count, the vendor's stock afterwards
// and its last item (the duplicate the sale placed with it), and the queued reply.
#pragma pack(push, 1)
struct D2VecVendorSell {
    uint32_t seed;
    uint8_t difficulty, phase;
    uint16_t level;
    uint32_t npcId, npcClass, itemId;
    int32_t price, goldBefore, goldAfter;
    uint8_t handlerResult, playerItems;
    uint16_t vendorItemsAfter;
    uint32_t dupeId, dupeClass, dupeSeedLo, dupeSeedHi, dupeBits;
    uint8_t bits[80];
    uint16_t packetBytes, packetCount;
    uint8_t stream[512];
    uint32_t hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecVendorSell) == 656, "D2VecVendorSell layout");

// Vendor multibuy: like the buy, but the client packet 0x32 carries the multibuy flag (bit 15 of
// nItemMode, i.e. `mode | 0x8000`) on a STACKABLE stock item (a tome, key or quiver -- selected
// as the first stock item ITEMS_CheckIfStackable accepts).  Retail's multibuy branch fills the
// bought stack to its max (or as far as gold allows), so this additionally records the bought
// item's STAT_QUANTITY afterwards -- the observable the stubbed 1.10f path never produced.
#pragma pack(push, 1)
struct D2VecVendorMultibuy {
    uint32_t seed;
    uint8_t difficulty, phase;
    uint16_t level;
    uint32_t npcId, npcClass, itemId;
    int32_t cost, goldBefore, goldAfter;
    uint8_t handlerResult, playerItems;
    uint16_t vendorItemsAfter;
    uint32_t boughtId, boughtClass, boughtSeedLo, boughtSeedHi, boughtBits;
    uint8_t bits[80];
    uint16_t packetBytes, packetCount;
    uint8_t stream[512];
    uint32_t boughtQuantity;
    uint32_t hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecVendorMultibuy) == 660, "D2VecVendorMultibuy layout");

// Hiring: after 0x13 and 0x38 {3, hire}, client packet 0x36 {npcGUID, mercName} for the first
// hireling the 0x4F/0x4E list offered, from a fixture at level 20 with 10,000 gold.  Records
// the result, gold, the hireling unit the hire spawned (first new monster unit: GUID, class,
// seeds, level/life/strength/dexterity stats) and the queued reply.
#pragma pack(push, 1)
struct D2VecHire {
    uint32_t seed;
    uint8_t difficulty, phase;
    uint16_t level;
    uint32_t npcId, npcClass;
    uint16_t mercName, playerLevel;
    int32_t goldBefore, goldAfter;
    uint8_t handlerResult, newMonsters;
    uint16_t packetBytes;
    uint32_t mercId, mercClass, mercSeedLo, mercSeedHi;
    int32_t mercLevel, mercLife, mercMaxLife, mercStr, mercDex, mercExp;
    uint8_t stream[512];
    uint32_t hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecHire) == 588, "D2VecHire layout");

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
// TILE: one record = one tile the 1.13c DRLG placed in a room, read out of retail's own memory
// after AddRoomData. This is the terrain oracle: the browser engine builds its draw list from the
// same DRLG, so a record-for-record diff turns rendering from an assertion ("tiles resolved", "the
// canvas is painted") into a comparison with retail -- the standard every other subsystem is held
// to and the one rendering was missing when every floor tile in the game drew the wrong art.
//
// 1.13c offsets, derived by scanning under Wine (D2CAPTURE_SCANROOM) because D2MOO's headers are
// the 1.10f decompile and differ here: Room1 records are 0x80 bytes; Room1+0x08 -> tile struct;
// that struct holds (array pointer, count) at +0x08/+0x0C; records are 0x30 bytes and BEGIN with
// screen X and Y -- not with a pointer to the D2CMP entry, which is the 1.10f shape.
// Identified by the first record reading (-720, 93840), the exact screen position the browser
// engine independently computes for the town's first floor tile.
struct D2VecTile {
    uint32_t seed;
    uint8_t  difficulty, act;
    uint16_t level;
    uint32_t roomOrdinal, tileIndex;
    int32_t  screenX, screenY;
    uint8_t  layer;              // which (array,count) pair in the tile struct: 0 @+0x00, 1 @+0x08, 2 @+0x10
    uint8_t  pad[3];
    int32_t  tileX, tileY;       // record +0x08 / +0x0C
    // The art identity is not in the 0x30 record: record +0x18 points at the D2CMP library entry,
    // whose layout is the DT1 block header. These offsets are into that entry, located by s141.
    int32_t  orientation;        // entry +0x14
    int32_t  mainIndex;          // entry +0x18
    int32_t  subIndex;           // entry +0x1C
    int32_t  frameIndex;         // entry +0x20 (the rarity/animation-frame selector)
    uint16_t material;           // entry +0x06
    uint16_t pad2;
    int32_t  width;              // entry +0x0C
    int32_t  height;             // entry +0x08 (negative: tiles hang above their anchor)
    // The tile RECORD's own words at +0x10 and +0x14. +0x14 reads like flags (0x00046001 on the
    // first town floor). Needed to settle whether retail's ninth-row copy is flagged differently
    // from the neighbour's interior copy -- adding that record contributes MAPTILE_UNWALKABLE and
    // MAPTILE_FILL_LOS in our engine and moved monster spawn.
    uint32_t word10, flags14;
};

// Kind 26 (s175): the per-lookup tile pick stream (OBJECTIVE v5 B; libd2's probe_seq is the prior
// art). One record per D2CMP GetTiles call D2Common makes (#10059 in 1.13c, #10088 in 1.10f/D2MOO) --
// GetTileCache's candidate set -- with what the GetTileRarity calls (#10027; 1.10f #10081) that follow it reveal: the first `entries` calls sum the
// weights, the rest are the rarity ladder, and the pick is the last entry the ladder consulted
// (entry 0 when there is no ladder). The seed roll itself is inline in D2Common, so the ladder
// length is how it shows. `room` is the room ordinal of the AddRoomData in progress, -1 outside one.
struct D2VecTilePick {
    uint32_t seed;
    uint8_t  difficulty, act;
    uint16_t level;
    int32_t  room;
    uint32_t call;               // index within (seed, difficulty, act, level)
    int32_t  type, style, sequence;
    int32_t  entries;            // GetTiles' return value
    int32_t  weightSum;          // sum of the first `entries` GetTileRarity results
    int32_t  ladder;             // GetTileRarity calls after the sum
    int32_t  chosenFrame;        // +0x20 of the entry picked; -1 if GetTiles returned none
};
static_assert(sizeof(D2VecTilePick) == 44, "tile pick record");

// ROOMSIGHT: what the retail CLIENT does to the world as the player walks.
// One record per event, in the order retail made it:
//   op 1  DUNGEON_SetClientIsInSight      (level, x, y) = the point the client marked in sight
//   op 0  DUNGEON_UnsetClientIsInSight    (level, x, y) = the point it released
//   op 2  a capture MARK                  level = the mark's 1-based ordinal, x = y = 0
//   op 4  a room lost its Room1           (level, x, y) = the room's subtile origin, FREED
//   op 3  a room gained its Room1         (level, x, y) = the room's subtile origin, ALLOCATED
// x and y are SUBTILES for every op, the units D2DrlgRoomStrc.nTileXPos * 5 and the units our
// d2_session_room_create_log stores, so an op-3 stream and that log are the same shape and compare
// directly. `tick` is GetTickCount() at the event.
struct D2VecRoomSight {
    uint32_t tick;
    uint32_t op;
    int32_t  level;      // op 3/4: the level id; op 1/0: the level the sight point is in; op 2: mark ordinal
    int32_t  x, y;       // subtiles
};
static_assert(sizeof(D2VecRoomSight) == 20, "room sight record");

// TOWNPORTAL: state emitted by a retail Town Portal scroll use.
// Kind 30 is now the distinct room-sight protocol; keep this older capture's 39-byte
// record on its own kind so the two schemas cannot be confused by a reader.
#pragma pack(push, 1)
struct D2VecTownPortal {
    uint32_t seed;
    uint8_t difficulty;
    uint16_t level;
    uint32_t playerId, itemId;
    uint8_t liftResult, insertResult, handlerResult;
    uint16_t playerX, playerY;
    uint8_t portalFound;
    uint32_t portalId, portalClass;
    uint16_t portalX, portalY;
    uint32_t hash;
};
#pragma pack(pop)
static_assert(sizeof(D2VecTownPortal) == 39, "town portal record");

enum D2VecKind : uint32_t { D2VEC_PRNG = 1, D2VEC_DRLG = 2, D2VEC_ITEMGEN = 3, D2VEC_MONSPAWN = 4, D2VEC_TABLE = 5, D2VEC_COMBAT = 6, D2VEC_AISPAWN = 7, D2VEC_AIFRAME = 8, D2VEC_AITARGET = 9, D2VEC_COMBATRESULT = 10, D2VEC_DAMAGECALC = 11, D2VEC_A1Q6 = 12, D2VEC_A1Q6PLAYER = 13, D2VEC_A1Q6TIMER = 14, D2VEC_A1Q6COMPLETE = 15, D2VEC_A1Q6DROP = 16, D2VEC_A1Q6TRAVEL = 17, D2VEC_NPCDIALOG = 18, D2VEC_NPCRESPONSE = 19, D2VEC_VENDORITEM = 20, D2VEC_VENDORBUY = 21, D2VEC_VENDORSELL = 22, D2VEC_HIRE = 23, D2VEC_VENDORMULTIBUY = 24, D2VEC_TILE = 25, D2VEC_TILEPICK = 26, D2VEC_AITRACE = 27, D2VEC_AIPATH = 28, D2VEC_MONDROP = 29, D2VEC_ROOMSIGHT = 30, D2VEC_QUEUEDUNIT = 31, D2VEC_ROOMCALLER = 32, D2VEC_TOWNPORTAL = 34 };

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
