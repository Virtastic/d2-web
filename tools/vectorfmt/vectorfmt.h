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

// SKILLCLASS (s3000): table row index and whether the skills-table pointer is null; outputs are
// the Boolean result and the output class. Null output is fatal in retail and is not replayable.
#pragma pack(push, 1)
struct D2VecSkillClass {
    int32_t skillId;
    uint8_t nullSkillsTable;
    uint8_t result;
    int32_t playerClass;
};
#pragma pack(pop)
static_assert(sizeof(D2VecSkillClass) == 10, "skill-class record");

struct D2VecClientInitFlag {
    uint32_t globalValue;
    uint32_t ignoredArg;
    uint32_t result;
};
static_assert(sizeof(D2VecClientInitFlag) == 12, "client init flag record");

struct D2VecClientByteLookup {
    int32_t index;
    uint32_t ignoredArg;
    uint32_t tableByte;
    uint32_t result;
};
static_assert(sizeof(D2VecClientByteLookup) == 16, "client byte lookup record");

struct D2VecLevelsTxtRecord {
    int32_t id;
    uint32_t count;    // levels record count at sgptDataTables+0xC5C
    int32_t index;     // (returned - base) / 0x220, -1 for NULL
};
static_assert(sizeof(D2VecLevelsTxtRecord) == 12, "levels txt record lookup");

struct D2VecWaypointCopy {   // s3049: D2Common 0x67D00 validate-and-copy of a 16-byte waypoint block
    uint16_t in[8];       // source block before the call (in[0] is 0, 0x101 or 0x102)
    uint16_t srcAfter[8]; // source block after (flags[0] normalised to 0x102)
    uint16_t out[8];      // destination block (pre-filled 0xFFFF)
};
static_assert(sizeof(D2VecWaypointCopy) == 48, "waypoint copy record");

struct D2VecCommon10003 {   // s3002: D2Common 0x2D0B0 = SetBit(*target, mul*16 + add)
    int32_t mul, add;
    uint8_t before[64];   // bit field before the call
    uint8_t after[64];    // bit field after
};
static_assert(sizeof(D2VecCommon10003) == 136, "common 10003 record");

struct D2VecBackpackType {   // s3009: D2Common 0x1FE10 backpack item-type search over a synthetic inventory
    int32_t ownerType, sigOk, itemType;  // owner unit type (0 player, 1 other), signature valid, type selector
    int32_t cont, n, badIdx;             // continuation: -1 none, 0..n-1 chain item, 99 foreign item; chain length; chain index whose unit type is 1 (-1 none)
    int32_t cls[6];                      // item classIds in grid-chain order
    int32_t result;                      // chain index returned, -1 null
};
static_assert(sizeof(D2VecBackpackType) == 52, "backpack record");

struct D2VecRunesTxtRecord {   // s3005: D2Common 0x70FA0 = &pRunesTxt[id] (valid ids only; invalid terminates)
    int32_t id;
    uint32_t count;       // rune record count (D2Common+0x9BBD4)
    uint8_t bytes[0x120]; // the record at the returned pointer
};
static_assert(sizeof(D2VecRunesTxtRecord) == 296, "runes txt record");

struct D2VecMonItemPercent {   // s3049: D2Common 0x53390 = id<count && id!=-1 ? &tbl[id] : null (signed ids, -8..count+3)
    int32_t id;
    uint32_t count;       // record count (D2Common+0xA0B5C)
    int32_t isNull;       // returned null
    int32_t offset;       // returned pointer - table base (0 when null)
    uint8_t bytes[4];     // the 4 percent bytes at the returned pointer when 0<=id<count, else 0
};
static_assert(sizeof(D2VecMonItemPercent) == 20, "monitempercent record");

struct D2VecEarLevelGet {   // s3053: D2Common 0x236B0 ITEMS_GetEarLevel(unit): synthetic unit {type, pItemData?}, data byte +0x48 = 0x37
    uint32_t type;
    uint32_t flags;       // bit0 pItemData non-null, bit1 unit pointer null
    uint32_t level;       // returned byte
};
static_assert(sizeof(D2VecEarLevelGet) == 12, "earlevelget record");

struct D2VecStackable22E20 {   // s3121: D2Common 0x22E20 (ordinal 10048) stackable byte of items.txt record, synthetic unit {type, classId}
    uint32_t type;        // dwUnitType (flag 0x80000000 = null unit pointer)
    uint32_t classId;
    uint32_t result;      // returned byte
};
static_assert(sizeof(D2VecStackable22E20) == 12, "stackable22e20 record");

struct D2VecUnitFinds12720 {   // s3122: D2Common 0x12720 (ordinal 10052) unit-find over a synthetic room graph with a callback test
    int32_t x, y, size;       // search circle
    uint32_t flags;           // pUnitFindData->nFlags (0x2000 = skip town rooms)
    uint32_t maxEntries;      // initial array capacity (growth is +15)
    uint32_t cbMask;          // callback accepts a unit when (unitId & cbMask) != 0
    int32_t room[4][4];       // room 0 = the search room (list head), 1..3 = adjacent; {subtileX, subtileY, width, height}
    uint32_t nAdj;            // adjacent rooms 0..3 (list = room 0 + these, used when the circle leaves room 0)
    uint32_t townMask;        // bit i: room i sits on a town level (1) else level 5
    uint32_t nUnits[4];       // units in each room; unit id = room * 100 + k + 1
    uint32_t nullRoom;        // 1 = pRoom is NULL
    uint32_t result;          // returned count
    uint32_t nIndex;          // struct +0x18 afterwards
    uint32_t maxAfter;        // struct +0x1C afterwards
    uint32_t hash;            // FNV-1a over the unit ids stored in the array [0, nIndex)
};
static_assert(sizeof(D2VecUnitFinds12720) == 4 * (6 + 16 + 2 + 4 + 5), "unitfinds12720 record");

struct D2VecStream3cda0 {   // s3123: D2Common 0x3CDA0 (ordinal 10053) stream the room at tile (x, y) of an act
    uint32_t seed, difficulty, act;   // act 0-based, as LoadAct
    int32_t x, y;                     // probe tile coordinates
    uint32_t found;                   // returned non-NULL
    int32_t level, ordinal;           // level id of the returned room's DrlgRoom and its index in that level's room list
    int32_t rect[4];                  // that DrlgRoom's tile x, y, width, height
};
static_assert(sizeof(D2VecStream3cda0) == 4 * 12, "stream3cda0 record");

struct D2VecDist7f5e0 {   // s3124: D2Common 0x7F5E0 distance from a unit's position to (x, y)
    uint32_t type, nullPath;   // unit type; path pointer NULL
    int32_t px, py;            // path position (static types 2/4/5: dword +0xC/+0x10; others: word +2/+6, zero-extended)
    int32_t x, y;              // target point
    int32_t result;
};
static_assert(sizeof(D2VecDist7f5e0) == 4 * 7, "dist7f5e0 record");

struct D2VecAutomapCell6fac0 {   // s3131: D2Common 0x6FAC0 automap cell lookup (levelType, tileType, style, sequence) with the automap seed
    int32_t levelType, tileType, style, seq;
    uint32_t seedLo, seedHi;           // automap seed before the call
    int32_t result;
    uint32_t seedLoAfter, seedHiAfter; // automap seed after the call
};
static_assert(sizeof(D2VecAutomapCell6fac0) == 36, "automap cell 6fac0 record");

struct D2VecPrestId9c20 {   // s3141: D2Common 0x9C20 level preset id from room (*(room+0x20))
    uint32_t in, out;
};
static_assert(sizeof(D2VecPrestId9c20) == 8, "prestid9c20 record");

struct D2VecRefresh665c0 {   // s3132: D2Common 0x665C0 queue a unit on its room's refresh list
    uint32_t type, roomMode, unitFlags, headPresent, actIn;   // inputs
    uint32_t unitFlagsOut, nextKind, headKind, actOut;        // kinds: 0 null, 1 other unit, 2 self (next) / self (head=1, other=2), 3 unknown
};
static_assert(sizeof(D2VecRefresh665c0) == 36, "refresh665c0 record");

struct D2VecQuestState2d0f0 {   // s3133: D2Common 0x2D0F0 quest-record bit test
    uint32_t buf[4], quest, state, result;
};
static_assert(sizeof(D2VecQuestState2d0f0) == 28, "queststate2d0f0 record");

struct D2VecMoving313f0 {   // s3130: D2Common 0x313F0 UNITS_IsInMovingMode over a synthetic unit
    uint32_t type, flags, classId, mode, seqMode;   // flags: bit0 anim-seq pointer set, bit1 null unit
    int32_t result;
};
static_assert(sizeof(D2VecMoving313f0) == 24, "moving313f0 record");

struct D2VecAnimAdj12d40 {   // s3135: D2Common 0x12D40 DRLGANIM_UpdateFrameInAdjacentRooms over synthetic Room2 graphs
    uint32_t nullMask;          // bit0 room1 null, bit1 room2 null
    uint32_t n1;                // room1 near-room count (0..3)
    int32_t chain1[3];          // per room1 near room: -1 no tile grid, else anim-tile count (0 = grid without pAnimTiles)
    uint32_t frame1[3];         // nCurrentFrame of the head anim tile of that near room
    uint32_t n2;                // room2 near-room count
    int32_t chain2[3];          // same, room2 near rooms
    uint32_t init2[3][3];       // initial nCurrentFrame of each room2 anim tile
    uint32_t out2[3][3];        // nCurrentFrame after the call (0 where no tile)
};
static_assert(sizeof(D2VecAnimAdj12d40) == 120, "animadj12d40 record");

struct D2VecArmorComp437c0 { int32_t comp, result; };   // s3067: D2Common 0x437C0 COMPOSIT_IsArmorComponent
static_assert(sizeof(D2VecArmorComp437c0) == 8, "armorcomp437c0 record");
struct D2VecChain1930 { int32_t id, depth, count, result; };   // s3134: D2Common 0x1930 monster id + chain depth -> class id
static_assert(sizeof(D2VecChain1930) == 16, "chain1930 record");
struct D2VecGameCheckMonType1620 { int32_t type1, type2, result; };   // s3233: D2Game 0x1620 (ecx type1, eax type2) -> eax
static_assert(sizeof(D2VecGameCheckMonType1620) == 12, "checkmontype 1620 record");

struct D2VecGameGetMonStats1540 { int32_t type, hasMon, hasUnit, idx, result; };   // s3234: D2Game 0x1540 (eax unit) -> monstats nId or -1
static_assert(sizeof(D2VecGameGetMonStats1540) == 20, "getmonstats 1540 record");

struct D2VecHoverChat {   // s3137: D2Common 0x181F0/0x18220/0x18240 hover-message accessors (kinds 3137/3138/3139)
    uint8_t in[64], out[64];   // in = hover struct image; out = copy buffer (3138) or lang/timeout in out[0..3]
};
static_assert(sizeof(D2VecHoverChat) == 128, "hoverchat record");

struct D2VecSandLeaper1A990 { uint32_t present, type; int32_t classId, count; uint32_t flag, result; };   // s3140: D2Common 0x1A990 unit, flag, monstats count -> BOOL
static_assert(sizeof(D2VecSandLeaper1A990) == 24, "sandleaper record");

struct D2VecIso4da00 { int32_t a, b, x, y; };   // s3125: D2Common 0x4DA00 (a,b) -> (x,y)
static_assert(sizeof(D2VecIso4da00) == 16, "iso4da00 record");

struct D2VecEarName {   // s3054: D2Common 0x23700 ITEMS_GetEarName(unit): synthetic unit {type, pItemData?}
    uint32_t type;
    uint32_t flags;       // bit0 pItemData non-null, bit1 unit pointer null
    int32_t isNull;       // returned null
    int32_t offset;       // returned pointer - pItemData (0 when null)
};
static_assert(sizeof(D2VecEarName) == 16, "earname record");

struct D2VecEarLevel {   // s3052: D2Common 0x23690 ITEMS_SetEarLevel(unit, level): synthetic unit {type, pItemData?}
    uint32_t type;
    uint32_t flags;       // bit0 pItemData non-null, bit1 unit pointer null
    uint32_t level;       // nLevel argument
    int32_t result;       // byte at pItemData+0x48 after the call (sentinel 0xAA pre-filled)
};
static_assert(sizeof(D2VecEarLevel) == 16, "earlevel record");

struct D2VecClientWindowHandle {
    uint32_t globalValue;
    uint32_t result;
    uint32_t globalAfter;
};
static_assert(sizeof(D2VecClientWindowHandle) == 12, "client window handle record");

struct D2VecGameReturnArgument {
    int32_t input;
    int32_t result;
};
static_assert(sizeof(D2VecGameReturnArgument) == 8, "game return argument record");

struct D2VecRunWalk2FD10 {   // kind 3150
    uint32_t classId;
    uint32_t out1;   // walk (record+0x40)
    uint32_t out2;   // run (record+0x41)
};
static_assert(sizeof(D2VecRunWalk2FD10) == 12, "runwalk record");

struct D2VecNet65D0 {   // kind 3606
    uint32_t len;        // param_2
    int32_t table;       // DAT_6fbfabd8[opcode] (0 when opcode >= 0x71), read before the call
    int32_t outParam;    // *param_3 after the call (sentinel 0x7A7A7A7A pre-call)
    int32_t result;
    uint8_t buf[32];     // packet bytes, zero padded
};
static_assert(sizeof(D2VecNet65D0) == 48, "net 65D0 record");

struct D2VecNetQueue {   // kind 3441: D2Net Fog-queue thunks after 0x6590 init(init0, init1)
    uint32_t fn;                 // D2Net rva
    uint32_t init0, init1;       // args of the 0x6590 init that built the queue
    uint32_t a, b, c;            // thunk args (71E0: a = dest buffer, recorded in buf; 6170: 2nd arg = out byte, recorded in buf; b is then the 3rd arg)
    int32_t ret;
    uint32_t fault;              // 1 if the call faulted
    uint32_t state[3];           // queue+0x86C, +0x87C, +0x884 after the call (0xDEAD: no queue)
    uint8_t buf[16];             // first 16 bytes of the 0xA5-filled buffer after the call
};
static_assert(sizeof(D2VecNetQueue) == 60, "net queue record");

// kind 3442: D2Common 0x241D0 stdcall(unit) over a fixture unit {type +0, id +4, item data +0x14 -> flags +0x18}.
struct D2VecItemPlain241d0 { uint32_t type, id, hasData, flags, ret; };
static_assert(sizeof(D2VecItemPlain241d0) == 20, "241d0 record");
// kind 3443: D2Common 0x33C20 (eax = struct, ecx = mask) and 0x34000 (stdcall(struct), mask = [[0x6FDE9E1C]+0xE0]).
// Fixture struct: +0x5c -> rec {+0x10 flag word, +0x58 -> bits}. nwords = ([[0x6FDE9E1C]+0xC4]+31)/32 (recorded, pinned by the tables).
struct D2VecBitmask33c20 { uint32_t fn, hasStruct, hasRec, recFlag, hasBits, nwords, mask[8], bits[8], ret; };
static_assert(sizeof(D2VecBitmask33c20) == 92, "33c20 record");

struct D2VecGameAccessor { int32_t a, b; uint32_t result; };   // kinds 3240/3241: D2Game accessor (a,b) -> result
static_assert(sizeof(D2VecGameAccessor) == 12, "game accessor record");

struct D2VecGameClearFlag { int32_t before, after; };   // kind 40: D2Game+0x38BB0 sentinel in, global read back
static_assert(sizeof(D2VecGameClearFlag) == 8, "game clear flag record");
struct D2VecGameSetInitSeed {
    int32_t input;
    int32_t globalAfter;   // int32 at D2Game 0x6FD2E354 (rva 0x10E354) read back
};
static_assert(sizeof(D2VecGameSetInitSeed) == 8, "game set init seed record");

struct D2VecGameCacc0 { uint32_t seedLoIn, seedHiIn, difficulty, seedLo, seedHi, dwMonSeed; };   // kind 3260: D2Game+0xCACC0 (ESI=pGame, 0xCAB30 stubbed); seed +0xD0/+0xD4, dwMonSeed +0xEC after
static_assert(sizeof(D2VecGameCacc0) == 24, "game cacc0 record");
struct D2VecNpcRec8FBE0 { int32_t nNPC; uint8_t nAct, bTrader, bLevelRefresh, bInited, bForceVendor, b27, pad[2]; uint32_t dw0C; };   // rec +0,+0x22..+0x27,+0xC (bGambleInit is dw0C: set to 1 by the gamble classes)
static_assert(sizeof(D2VecNpcRec8FBE0) == 16, "npc record");
struct D2VecNpcCtl8FBE0 { uint32_t nInitSeed, preSeedLo, preSeedHi, postSeedLo, postSeedHi, nArraySize, unk10, ctlSeedLo, ctlSeedHi; D2VecNpcRec8FBE0 rec[64]; };   // kind 3261: D2Game+0x8FBE0 stdcall(pGame); control at pGame+0x1D24
static_assert(sizeof(D2VecNpcCtl8FBE0) == 36 + 64 * 16, "npc control record");
struct D2VecGameSeedInit2A900 { uint32_t nInitSeed, dwInitSeed7C, initSeedFlag84, seedLo, seedHi; };   // kind 3250: D2Game+0x2A900 (EDI=zeroed pGame), pGame+0x7C/+0x84/+0xD0..D7 after
static_assert(sizeof(D2VecGameSeedInit2A900) == 20, "game seed init 2A900 record");

struct D2VecQuestInitEntry { int32_t nQuestNo; uint8_t nAct, bNotIntro, bActive, pad; };
struct D2VecQuestInit348A0 {   // kind 3251: D2Game+0x348A0 on a fresh game; chain walked pLastQuest -> prev (+0xF4)
    uint32_t nInitSeed;
    uint32_t preControlNonNull, preSeedLo, preSeedHi;   // pGame+0x10F4 != 0 before the call; pGame+0xD0..D7 before
    uint32_t postSeedLo, postSeedHi;                    // pGame+0xD0..D7 after
    uint32_t info[9];                                   // control block +0..0x23 after, pointers (+0,+0xC,+0x10) reduced to non-null 0/1
    uint32_t nRecords;
    D2VecQuestInitEntry rec[96];
};
struct D2VecEventQueue8E630 {   // kind 3252: D2Game+0x8E630 (EBX=pGame) called twice; pointers reduced to non-null flags
    uint32_t nInitSeed, b8Pre, b8After1, slab1, b8After2, b8Changed2, slab2, queue2Equal1, q1[0x288];   // q1[0x287] = slab-head non-null flag
};
static_assert(sizeof(D2VecEventQueue8E630) == 4 * 8 + 0xA20, "event queue 8E630 record");
struct D2VecFreeQ8E4D0 { uint32_t nInitSeed, kase, chainBefore, queueAfter, poolSame; };   // kind 3255: D2Game+0x8E4D0 (EDI=pGame); kase 0/1/2 = 1 slab / 3 slabs / queue already null
static_assert(sizeof(D2VecFreeQ8E4D0) == 20, "freeq 8E4D0 record");
struct D2VecTimerSlab8E420 { uint32_t nInitSeed, nonNull, nonZeroWords, fnv, headOff, lastNextOff, secondNextOff; };   // kind 3254: D2Game+0x8E420 (EAX=pGame) slab 0x7088 B; pointers -> offsets from base; fnv = FNV-1a over the 0x1C22 normalized dwords
// kind 3253: D2Common+0x3A280 STATLIST_SetStat(list, packedId, value) on a freshly built list; listKind 0 NULL, 1 plain, 2 extended owner type 0, 3 extended owner type 1; preseed set 0/1
struct D2VecStatList3A280 { uint32_t listKind, preset, packedId, value, ret, hasMod, nBase, base[24][2], nMod, mod[24]; };
static_assert(sizeof(D2VecStatList3A280) == 4 * 7 + 192 + 4 + 96, "statlist 3A280 record");
static_assert(sizeof(D2VecQuestInit348A0) == 4 * 15 + 4 + 96 * 8, "quest init 348A0 record");

struct D2VecGfxViewModeSelector { uint32_t input, globalAfter; };   // kind 3715: D2Gfx+0xB190 (ECX in), uint32 at D2Gfx+0x11270 read back
static_assert(sizeof(D2VecGfxViewModeSelector) == 8, "gfx view mode selector record");
struct D2VecLangStrstrB020 { uint16_t hay[16], needle[8]; int32_t offset; uint32_t nullMask; };   // kind 3723: D2Lang+0xB020; offset -1 = NULL; nullMask bit0 hay NULL, bit1 needle NULL
static_assert(sizeof(D2VecLangStrstrB020) == 56, "lang strstr b020 record");
struct D2VecLangStrncpyB0D0 { uint16_t src[16]; uint16_t dst[16]; int32_t count; int32_t retOffset; };   // kind 3726: D2Lang+0xB0D0; dst pre-filled 0xCCCC; retOffset = ret - dst
static_assert(sizeof(D2VecLangStrncpyB0D0) == 72, "lang strncpy b0d0 record");

// Retail NPC interaction snapshot; node identities are list-local, never host addresses.
#pragma pack(push, 1)
struct D2VecInteractNode { uint32_t unitIdentity; int32_t flag; };
struct D2VecInteractCount {
    uint32_t seed, npcClass, npcId, tick;
    uint16_t level, playerX, playerY;
    uint8_t difficulty, phase;
    uint32_t nodeCount;
    int32_t result;
    D2VecInteractNode nodes[16];
};
#pragma pack(pop)
static_assert(sizeof(D2VecInteractCount) == 160, "interaction snapshot layout");

struct D2VecClient1230 { int32_t index; uint32_t count; uint32_t offset; uint32_t isNull; };   // kind 3410
static_assert(sizeof(D2VecClient1230) == 16, "client 1230 record");

struct D2VecClient1260 { int32_t index; uint32_t count; uint32_t count2; int32_t field; uint32_t offset; uint32_t isNull; };   // kind 3411
struct D2VecClient1300 { int32_t index; uint32_t count; uint32_t offset; uint32_t isNull; };   // kind 3412
static_assert(sizeof(D2VecClient1300) == 16, "client 1300 record");
struct D2VecClient1580 { int32_t index; uint32_t count; uint32_t offset; uint32_t isNull; };   // kind 3416
static_assert(sizeof(D2VecClient1580) == 16, "client 1580 record");
struct D2VecClient15b0 { int32_t index; uint32_t count; uint32_t offset; uint32_t isNull; };   // kind 3417
static_assert(sizeof(D2VecClient15b0) == 16, "client 15b0 record");
struct D2VecClient1630 { int32_t type; uint32_t pathPresent; uint32_t staticX; uint32_t dynX; uint32_t result; };   // kind 3418
static_assert(sizeof(D2VecClient1630) == 20, "client 1630 record");
struct D2VecClient1660 { int32_t type; uint32_t pathPresent; uint32_t staticY; uint32_t dynY; uint32_t result; };   // kind 3419
static_assert(sizeof(D2VecClient1660) == 20, "client 1660 record");
struct D2VecClient17B0 { int32_t idx; uint32_t count; uint32_t field; uint32_t inRange; uint32_t result; };   // kind 3420
static_assert(sizeof(D2VecClient17B0) == 20, "client 17B0 record");
struct D2VecClient18D0 { uint32_t seedLo, seedHi; int32_t range, base; uint32_t outLo, outHi, result; };   // kind 3421
static_assert(sizeof(D2VecClient18D0) == 28, "client 18D0 record");
struct D2VecClient2DC0 { uint32_t seedLo, seedHi; int32_t max, min; uint32_t outLo, outHi, result; };   // kind 3424
static_assert(sizeof(D2VecClient2DC0) == 28, "client 2DC0 record");
struct D2VecClient1A60 { uint32_t unitPresent; int32_t type; uint32_t extPresent; uint32_t idx; uint32_t result; };   // kind 3422
static_assert(sizeof(D2VecClient1A60) == 20, "client 1A60 record");
struct D2VecClient2D50 { uint32_t unitPresent, first, innerPresent, a, b, c, o150, o154, o158, o15c; };   // kind 3423
static_assert(sizeof(D2VecClient2D50) == 40, "client 2D50 record");
struct D2VecClient1330 { uint32_t lowIn, highIn; int32_t n; uint32_t lowOut, highOut, result; };   // kind 3413
static_assert(sizeof(D2VecClient1330) == 24, "client 1330 record");
struct D2VecClient1380 { uint32_t lowIn, highIn; int32_t n; uint32_t lowOut, highOut, result; };   // kind 3414
static_assert(sizeof(D2VecClient1380) == 24, "client 1380 record");
struct D2VecClient1400 { int32_t rooms[4][8]; int32_t room, x, y, result; };   // kind 3415; rooms = x0,y0,w,h,n,near[3]
static_assert(sizeof(D2VecClient1400) == 144, "client 1400 record");
static_assert(sizeof(D2VecClient1260) == 24, "client 1260 record");

enum D2VecKind : uint32_t { D2VEC_PRNG = 1, D2VEC_DRLG = 2, D2VEC_ITEMGEN = 3, D2VEC_MONSPAWN = 4, D2VEC_TABLE = 5, D2VEC_COMBAT = 6, D2VEC_AISPAWN = 7, D2VEC_AIFRAME = 8, D2VEC_AITARGET = 9, D2VEC_COMBATRESULT = 10, D2VEC_DAMAGECALC = 11, D2VEC_A1Q6 = 12, D2VEC_A1Q6PLAYER = 13, D2VEC_A1Q6TIMER = 14, D2VEC_A1Q6COMPLETE = 15, D2VEC_A1Q6DROP = 16, D2VEC_A1Q6TRAVEL = 17, D2VEC_NPCDIALOG = 18, D2VEC_NPCRESPONSE = 19, D2VEC_VENDORITEM = 20, D2VEC_VENDORBUY = 21, D2VEC_VENDORSELL = 22, D2VEC_HIRE = 23, D2VEC_VENDORMULTIBUY = 24, D2VEC_TILE = 25, D2VEC_TILEPICK = 26, D2VEC_AITRACE = 27, D2VEC_AIPATH = 28, D2VEC_MONDROP = 29, D2VEC_ROOMSIGHT = 30, D2VEC_QUEUEDUNIT = 31, D2VEC_ROOMCALLER = 32, D2VEC_SKILLCLASS = 33, D2VEC_CLIENT_INIT_FLAG = 33, D2VEC_TOWNPORTAL = 34, D2VEC_CLIENT_BYTE_LOOKUP = 35, D2VEC_CLIENT_WINDOW_HANDLE = 36, D2VEC_GAME_RETURN_ARGUMENT = 37, D2VEC_GAME_SET_INIT_SEED = 38, D2VEC_LEVELS_TXT_RECORD = 39, D2VEC_GAME_CLEAR_FLAG = 40, D2VEC_WAYPOINT_COPY = 41, D2VEC_INTERACTCOUNT = 42, D2VEC_COMMON_10003 = 43, D2VEC_RUNES_TXT_RECORD = 44, D2VEC_BACKPACK_TYPE = 45, D2VEC_MONITEMPERCENT = 46, D2VEC_EARNAME = 47, D2VEC_EARLEVEL = 48, D2VEC_EARLEVELGET = 49, D2VEC_GFX_VIEW_MODE_SELECTOR = 3715, D2VEC_LANG_STRSTR_B020 = 3723, D2VEC_STACKABLE_22E20 = 5022, D2VEC_CLIENT_1230 = 3410, D2VEC_UNITFINDS_12720 = 5023, D2VEC_STREAM_3CDA0 = 3123, D2VEC_CLIENT_1260 = 3411, D2VEC_DIST_7F5E0 = 3124, D2VEC_CLIENT_1300 = 3412, D2VEC_CLIENT_1330 = 3413, D2VEC_CLIENT_1380 = 3414, D2VEC_CLIENT_1400 = 3415, D2VEC_CLIENT_1580 = 3416, D2VEC_CLIENT_15B0 = 3417, D2VEC_CLIENT_1630 = 3418, D2VEC_ISO_4DA00 = 3125, D2VEC_AUTOMAP_CELL_6FAC0 = 3131, D2VEC_REFRESH_665C0 = 3132, D2VEC_QUESTSTATE_2D0F0 = 3133, D2VEC_CLIENT_1660 = 3419, D2VEC_MOVING_313F0 = 3130, D2VEC_ARMORCOMP_437C0 = 3067, D2VEC_CLIENT_17B0 = 3420, D2VEC_CLIENT_18D0 = 3421, D2VEC_CLIENT_1A60 = 3422, D2VEC_CLIENT_2D50 = 3423, D2VEC_CHAIN_1930 = 3141, D2VEC_SANDLEAPER_1A990 = 3140, D2VEC_ANIMADJ_12D40 = 3135, D2VEC_GAME_CHECKMONTYPE_1620 = 3233, D2VEC_GAME_GETMONSTATS_1540 = 3234, D2VEC_HOVER_LANG = 3137, D2VEC_HOVER_COPY = 3138, D2VEC_HOVER_TIMEOUT = 3139, D2VEC_CLIENT_2DC0 = 3424, D2VEC_GAME_MONFLAG_15D0 = 3240, D2VEC_GAME_UNIQDROP_E250 = 3241, D2VEC_PRESTID_9C20 = 3141, D2VEC_NET_65D0 = 3606, D2VEC_RUNWALK_2FD10 = 3150, D2VEC_CLIENT_10DF0 = 3425, D2VEC_CLIENT_10EC0 = 3426, D2VEC_CLIENT_11070 = 3427, D2VEC_CLIENT_110C0 = 3429, D2VEC_CLIENT_11240 = 3430, D2VEC_GAME_SEEDINIT_2A900 = 3250, D2VEC_QUESTINIT_348A0 = 3251, D2VEC_EVENTQUEUE_8E630 = 3252, D2VEC_STATLIST_3A280 = 3253, D2VEC_STATLIST_3A100 = 3262, D2VEC_OBJRGN_B510 = 3263, D2VEC_FN = 3950, D2VEC_CLIENT_11360 = 3431, D2VEC_CLIENT_11410 = 3432, D2VEC_LANG_STRNCPY_B0D0 = 3726, D2VEC_TIMERSLAB_8E420 = 3254, D2VEC_FREEQ_8E4D0 = 3255, D2VEC_CLIENT_114F0 = 3433, D2VEC_CLIENT_119C0 = 3436, D2VEC_GAME_CACC0 = 3260, D2VEC_GAME_2CBE0 = 3247, D2VEC_GAME_GETPLAYER_4AD10 = 3246, D2VEC_NPCCTL_8FBE0 = 3261, D2VEC_CLIENT_11630 = 3434, D2VEC_CLIENT_11A20 = 3437, D2VEC_CLIENT_11F80 = 3438, D2VEC_STATARRAY_38640 = 3439, D2VEC_LANG_USHORT_1010 = 3440, D2VEC_NET_QUEUE = 3441, D2VEC_ITEMPLAIN_241D0 = 3442, D2VEC_BITMASK_33C20 = 3443, D2VEC_CLIENT_148A0 = 3444, D2VEC_CLIENT_148E0 = 3445, D2VEC_GAME_47820 = 3446, D2VEC_GAME_DFC10 = 3447, D2VEC_GAME_AFB90 = 3448, D2VEC_GAME_47A20 = 3450, D2VEC_GAME_46CD0 = 3451, D2VEC_GAME_E1430 = 3452, D2VEC_GAME_4B500 = 3453, D2VEC_GAME_4B1D0 = 3454, D2VEC_GAME_4B3C0 = 3455, D2VEC_GAME_4B400 = 3459, D2VEC_GAME_4BCC0 = 3460, D2VEC_GAME_34860 = 3456, D2VEC_GAME_769A0 = 3457, D2VEC_GAME_8A3E0 = 3463, D2VEC_GAME_8C380 = 3464, D2VEC_GAME_4AB70 = 3465, D2VEC_GAME_8A980 = 3466, D2VEC_GAME_2C830 = 3468, D2VEC_GAME_2D670 = 3469, D2VEC_GAME_2C120 = 3470, D2VEC_GAME_2D9B0 = 3471, D2VEC_GAME_2E050 = 3472, D2VEC_GAME_2CA20 = 3473, D2VEC_GAME_29BE0 = 3474, D2VEC_GAME_2CF30 = 3475, D2VEC_GAME_2C0F0 = 3476, D2VEC_GAME_2EEA0 = 3458, D2VEC_GAME_90320 = 3467, D2VEC_CLIENT_C14F0 = 3480, D2VEC_CLIENT_C1580 = 3480 };
// kind 3950 (checked against retail): ONE record shape for every function captured by the generic FN path (the retail capture tool/fn/,
// the retail captures). args are the inputs as 32-bit words (what they mean is the descriptor's and
// the probe's business), result is the return value. A struct-argument function keeps a bespoke capture block
// but may still write this shape, so its replay stays generic.
struct D2VecFn { uint32_t args[4]; uint32_t result; };
static_assert(sizeof(D2VecFn) == 20, "fn record");
// s3444/s3445: D2Client 0x148A0 (AL,ECX,EDX,[c,d] stack) and 0x148E0 (AL,ECX,EDX,[c]): the packet buffer handed to the
// stubbed sub_143E0 (buf = op + dword args LE) and its EBX (length). Bytes past the length are zero.
struct D2VecClient148a0 { uint32_t op, ecx, edx, c, d, ebx; uint8_t buf[20]; };
static_assert(sizeof(D2VecClient148a0) == 44, "148a0 record");
struct D2VecClient148e0 { uint32_t op, ecx, edx, c, ebx; uint8_t buf[16]; };
static_assert(sizeof(D2VecClient148e0) == 36, "148e0 record");

// s3425: D2Client 0x10DF0 (resolution apply). mode = ESI, cur = pinned D2Gfx ordinal 10031 result; the six
// globals read after.
struct D2VecClient10df0 { uint32_t mode, cur, w, h, wide, e18, bc50, e14; };
static_assert(sizeof(D2VecClient10df0) == 32, "10df0 record");

// s3426: D2Client 0x10EC0 (fade box). Inputs: layout [0x6FBCC414], deadline [0x6FBCC554], pinned GetTickCount
// now, W [0x6FB8BC48], H [0x6FB8BC4C]. drew = D2Gfx 10045 call count; a[] = its six stdcall args in order; the
// deadline global read after.
struct D2VecClient10ec0 { uint32_t layout, deadline, now, w, h, drew, a[6], deadlineAfter; };
static_assert(sizeof(D2VecClient10ec0) == 52, "10ec0 record");

// s3427: D2Client 0x11070 (button hit test): x = ESI, y = EDI, W [0x6FB8BC48], hudH [0x6FBA9E18]; result = EAX.
struct D2VecClient11070 { int32_t x, y; uint32_t w, hudH, result; };
static_assert(sizeof(D2VecClient11070) == 20, "11070 record");
// s3429: D2Client 0x110C0 (sibling hit test): same record shape and inputs as 0x11070.
struct D2VecClient110c0 { int32_t x, y; uint32_t w, hudH, result; };
static_assert(sizeof(D2VecClient110c0) == 20, "110c0 record");
// s3430: D2Client 0x11240 (no args): reads G [0x6FBCC4FC] and hudH [0x6FBA9E18]; result = EAX.
struct D2VecClient11240 { uint32_t g, hudH, result; };
static_assert(sizeof(D2VecClient11240) == 12, "11240 record");
// s3431: D2Client 0x11360 (ECX = n): reads G,D,P [0x6FBCC4FC/500/534] and hudH; writes P and y [0x6FBD33F4].
struct D2VecClient11360 { int32_t n, g, d, p, h, newP, y; };
static_assert(sizeof(D2VecClient11360) == 28, "11360 record");
// s3432: D2Client 0x11410: reads flag [0x6FBC9854]; writes ptr [0x6FBA8B94], G [0x6FBCC4FC], D [0x6FBCC500], X [0x6FBCC504].
struct D2VecClient11410 { uint32_t f, ptr, g, d, x; };
static_assert(sizeof(D2VecClient11410) == 20, "11410 record");
// s3433: D2Client 0x114F0 (stdcall param): which of the 3 panel refreshes (0x6FBA8C50/44/38) and the 0x6FB6E920 reset run.
// sm = GetSystemMetrics(0x4B); c [0x6FBCC3CC], m [0x6FBC9898], s [0x6FBC988C], u [0x6FBC9890]; mask bits 0-2 = calls, 3 = reset call, 4 = [0x6FBCC540] cleared.
struct D2VecClient119c0 { uint32_t idx, entry, param, ret, out, n3a, ecx, seenParam, outBefore, n114, arg114, g54C; };   // kind 3436
struct D2VecGetPlayer4ad10 { uint32_t flags, ign, playerSet, type, id, layout, ret, after; };   // kind 3246; ret/after: 0 null, 1 sentinel pPlayer, 2/3/4 unit A/B/C
static_assert(sizeof(D2VecGetPlayer4ad10) == 32, "4ad10 record");
struct D2VecSub2cbe0 { uint32_t pend0, pend1, n0, in[3][4], a1[4], rec2E, b318, pend0A, pend1A, out[3][3], a1out[3], logN; uint8_t log[48]; };   // kind 3247; room = +0x34,+0x6C,+0x70,+0x78 in; +0x34,+0x6C,+0x70 out; log = stub ids
static_assert(sizeof(D2VecSub2cbe0) == 4 * (3 + 12 + 4 + 2 + 2 + 9 + 3 + 1) + 48, "2cbe0 record");
struct D2VecClient114f0 { uint32_t param, sm, c, m, s, u, mask; };
static_assert(sizeof(D2VecClient114f0) == 28, "114f0 record");
// s3435: D2Client 0x11630 (no args): inputs W [0x6FB8BC48], H [0x6FBA9E18], G, D, P, label WORDs [0x6FBA8B98+0x1A*i], the three widths (D2Win thunk 0xD35A);
// outputs the 12 ints [0x6FBA8BA2/A6/AA/AE +0x1A*i] (o[4*i+k]), y [0x6FBD33F4], the two SetFont args (13, then the previous value).
struct D2VecClient11630 { int32_t W, H, w[3], G, D, P; uint32_t lab[3]; int32_t o[12], y; uint32_t font[2]; };
static_assert(sizeof(D2VecClient11630) == 4 * 8 + 12 + 48 + 12, "11630 record");
static_assert(sizeof(D2VecClient119c0) == 48, "119c0 record");

// kind 3480: D2Client 0xC14F0 (loading-screen release). Inputs h [0x6FBCB9D0], b [..D4], t [0x6FBCC3C0], deadline [0x6FBC981C], tick (GetTickCount stubbed), v9CC [..B9CC], v9B0 [..B9B0] before;
// outputs the four globals after, n callee calls and call[8] = {id, eax, ecx, edx, stack0, stack1} (1 = 0xC18A(h), 2 = Fog free ecx edx 0x7A 0, 3 = D2Win cl (ecx & 0xFF), 4 = D2CMP(b), 5 = 0x5F090, 6 = 0xBE860, 7 = 0x81750 eax=t).
struct D2VecClientC14F0 { uint32_t h, b, t, deadline, tick, v9CC, v9B0, hOut, v9CCOut, v9B0Out, deadlineOut, n, call[8][6]; };
static_assert(sizeof(D2VecClientC14F0) == 48 + 192, "c14f0 record");

// a retail recording batch 9: D2Client 0x11A20 (no args, callees 11630 D2Win thunks stubbed, 2B420 stubbed). Inputs: flag [0x6FBC9854], W [0x6FB8BC48], H [0x6FBA9E18],
// P [0x6FBCC534] before the call, widths w[3], label WORDs lab[3]. Outputs: ptr [0x6FBA8B94], G D X [0x6FBCC4FC/500/504], rects o[12], p534 p538, p544 (2B420 return kept),
// y [0x6FBD33F4], 2B420 call count / stack arg / eax-buffer string (NUL padded to 32), the two SetFont args.
struct D2VecClient11A20 { uint32_t flag; int32_t W, H, P, w[3]; uint32_t lab[3]; uint32_t ptr, G, D, X; int32_t o[12]; uint32_t p534, p538, p544, y, n2b420, arg2b420, font[2]; char str[32]; };
static_assert(sizeof(D2VecClient11A20) == 168, "11a20 record");
// D2Client 0x11F80: inputs gate [0x6FBCC540], flag [0x6FBD349C], seed (src dwords [0x6FBC9E68+4i] = seed + 0x01010101*i); outputs dst hash/first/last/guard (dst 0x6FBCA2E0+0x474),
// [0x6FBCC434], [0x6FBD349C], [0x6FBCC2F8], [0x6FBCC2F4], [0x6FBCC540], then the stubbed callee log: n calls, each {id, eax, [esp+4], [esp+8]}
// (id 1 = 0x114F0 arg0, 2 = 0x3A3A0, 3 = Ordinal_10048 thunk, 4 = 0x2CCC0 eax=4 ptr,h, 5 = 0x42880).
struct D2VecClient11F80 { uint32_t gate, flag, seed, dstHash, dstFirst, dstLast, guard, p434, p349c, p2f8, p2f4, p540, nCalls, call[5][4]; };
static_assert(sizeof(D2VecClient11F80) == 4 * 13 + 80, "11f80 record");
// D2Common 0x38640 StatArray_InsertStat (eax = packed id, esi = array {ptr, count +4, cap +6}, stack arg pool=0): array built from own earlier inserts of ids0[n0],
// probe id; out: slot index (-1 = NULL), count/cap before and after, element ids/values after (<= 10).
struct D2VecStatArray38640 { uint32_t n0, ids0[8], probe; int32_t idx; int32_t cnt0, cap0, cnt1, cap1; uint32_t id[10], val[10]; };
static_assert(sizeof(D2VecStatArray38640) == 4 * 10 + 4 + 16 + 80, "38640 record");
// D2Lang 0x1010 operator unsigned short (ECX = this -> 16-bit unit, plus 2 guard bytes after): in unit, out AX (zero-extended EAX recorded too)
struct D2VecLangUShort1010 { uint32_t unit, ax, eax; };
static_assert(sizeof(D2VecLangUShort1010) == 12, "lang ushort record");

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

struct D2Vec47820 { uint32_t seedLo, seedHi, unk2C4, level, newLo, newHi, entered, ctx; };   // kind 3446: D2Game 0x47820 (thiscall room, stdcall(level, room), EDI = game); ctx bit0 room, bit1 game, bit2 region seen by the stubbed 0x47660
static_assert(sizeof(D2Vec47820) == 32, "47820 record");
struct D2VecDFC10 { uint32_t n, arg; uint8_t bucket[8], state7[8], seq[8]; uint32_t calls, ctxBad; };   // kind 3447: D2Game 0xDFC10; seq = 1-based unit index per callback call, in call order
static_assert(sizeof(D2VecDFC10) == 40, "DFC10 record");
struct D2VecAFB90 { uint32_t type, classId, hasMon, hasInv, hasHover, interact, nCalls; uint32_t call[8][10]; };   // kind 3448: D2Game 0xAFB90 (ESI = unit, stack = pGame); call[i] = {site 1..8, eax, ecx, edx, ebx, esi, edi, arg0, arg1, arg2} at each stubbed callee; fixture pointers are 0xF100000n (unit, game, inv, hover, mon, game+0x1C)
static_assert(sizeof(D2VecAFB90) == 28 + 320, "AFB90 record");
struct D2Vec47A20 { uint32_t seedLo, seedHi, monDen, level, gate, script, nNodes; uint32_t node[3][6]; uint32_t minGrp, maxGrp, sparse;   // kind 3450: D2Game 0x47A20 (stdcall game, room); node = {bNode, nIndex, left, top, right, bottom}
                    uint32_t outLo, outHi, reg8, monDenOut, nCalls, nLog; uint32_t log[48][5]; };   // log[i] = {callee id 1..9, 4 normalized words}; callees scripted by xorshift32(script)
static_assert(sizeof(D2Vec47A20) == 4 * (28 + 6 + 240), "47A20 record");
struct D2Vec46CD0 { uint32_t regNull, act, pre4, pre0C, monDen, lvl1, pop, cnt, lvl2, cleared;   // kind 3451: D2Game 0x46CD0 (EBX = game, EDI = room); inputs = the five stubbed callees' returns + region state
                    uint32_t ret, post4, post0C, nCalls; uint32_t log[5][3]; };   // log[i] = {callee 1 A556 room-level, 2 B204, 3 B18C, 4 A556 again, 5 B8940, arg0, arg1} (ptrs normalized: game 1, room 2, act 4)
static_assert(sizeof(D2Vec46CD0) == 4 * (14 + 15), "46CD0 record");
struct D2VecE1430 { uint32_t gameNull, level, seed, ctr0, n, pre[8][3];   // kind 3452: D2Game 0xE1430 (ECX = game, EAX = room); pre[i] = {type, class, flags}; level = the stubbed A556 return
                    uint32_t ctr1, nSp, nHead, nLev, ctxOk, spawn[16]; };   // out: game+0x1DE4 after, E13B0 calls (nSp) and the node index of each (spawn), A664 / A556 call counts, ebx/edi == game/room at every E13B0
static_assert(sizeof(D2VecE1430) == 4 * (5 + 24 + 5 + 16), "E1430 record");
struct D2Vec4B500 { uint32_t nullMask, roomGiven, x, y, nAct, flags0, golem0, unitType, unitId, nItems, item[3][5], golemChain;   // kind 3453: D2Game 0x4B500; nullMask b0 game b1 unit b2 client; item = {guid, tflag (10209), w (10635), inChain, decoy}; golemChain 0 none 1 present 2 decoy+present
                    uint32_t res10613, res10589, outX, outY, c10, c14, level, res10585, v10630, res10306, a1100, b1130;   // scripted callee returns (10816 out-params outX/outY, 10561 buf+0x10/+0x14, 10826 level)
                    uint32_t flags1, golem1, nCalls, over; uint32_t log[56][8]; };   // out: client +0x3D4 and +0x484 after; log[i] = {callee id 1..28, 7 normalized words}
static_assert(sizeof(D2Vec4B500) == 4 * (38 + 4 + 448), "4B500 record");
struct D2Vec4B1D0 { uint32_t nullMask, oldRoom, newRoom, nA, nB, A[4], B[4], flags0, unit0, v16c, v170, game, rE03A0, rA556, rB1C2, rB1E0;   // kind 3454: D2Game 0x4B1D0 (ESI = client, stack = new room); nullMask b0 client b1 client+0x1A8 (game); A/B = the room lists A55C returns for the new / old room
                    uint32_t room1, unit1, nCalls, over; uint32_t log[32][8]; };   // out: client +0x1B4 and +0x174 after; log[i] = {callee id 1..11, 7 words}
static_assert(sizeof(D2Vec4B1D0) == 4 * (22 + 4 + 256), "4B1D0 record");
struct D2Vec4B3C0 { uint32_t nullClient, edi, old, room1, nCalls, over; uint32_t log[4][8]; };   // kind 3455: D2Game 0x4B3C0 (EAX = client, EDI = room); out: client +0x1B4 after; log = {id 1 4B1D0 / 2 A2DA / 3 A2C2 / 4 _exit, 7 words}
static_assert(sizeof(D2Vec4B3C0) == 4 * (6 + 32), "4B3C0 record");
struct D2Vec34860 { uint32_t eax, ecx, edx, arg, rCallee, result, nCalls, over; uint32_t log[2][8]; };   // kind 3456: D2Game 0x34860 (EAX, ECX, EDX = the event words, stack = arg, ret 4); 33CE0 stubbed; log = {1, eax, 3, arg, ecx, edx, 1, 0}
static_assert(sizeof(D2Vec34860) == 4 * (8 + 16), "34860 record");
struct D2Vec769A0 { int32_t idx; uint32_t unit, a0, a1, a2, r968E, rFFA90, nullMask; uint32_t nCalls, over; uint32_t log[4][8]; };   // kind 3457: D2Game 0x769A0 (ESI = handler index, EDI = unit, stack a0 a1 a2, ret 0xC); nullMask bit i = handler table entry i (0xFBC14) is null; 968E / FFA90 / the handlers stubbed; log = {1 968E(edi, 1) / 2 FFA90(eax) / 3 handler(idx, ecx a0, edx unit, a1, a2), words}
static_assert(sizeof(D2Vec769A0) == 4 * (8 + 2 + 32), "769A0 record");
struct D2Vec4B400 { uint32_t nullMask, arg, oldRoom, flag, rUnit, rA478a, rA478b, rA556a, rA556b, cnt0, cnt1, nCalls, over; uint32_t log[16][8]; };   // kind 3459: D2Game 0x4B400 (EAX = client, stack = room); nullMask b0 client b1 arg; rUnit = 4AD10 result (non-zero: a unit); flag = unit+0xC8 bit 21; cnt = client +0x1B0 before/after; log = {id, 7 words}
struct D2Vec4BCC0 { uint32_t a2, a3, a4, a5, cflags, f3d4, g70, err, rUnit, utype, uid, uflags, rB23, r49, rB1C, nSk, rA4C0, w70, w74, w78, w7C, slotA[16], slotD[16], ret, c174, f3d4o, uflags1, nCalls, over, log[32][8]; };   // kind 3460: D2Game 0x4BCC0 (EAX = client, stack = game a2 a3 a4 a5); cflags client+0xA, f3d4 client+0x3D4, g70 game+0x70, err 56430 result, rB23 B23A0 result, slotA = short | byte<<16 of client+0x3DC+8i, slotD its dword; w* = A4C0 record +0x70/74/78/7C; log = {id, 7 words}
static_assert(sizeof(D2Vec4B400) == 4 * (13 + 128), "4B400 record");
struct D2Vec90320 { uint32_t sel, mode, tick, n, nu, uid[4], uf4[4], rMask, rOrd, ent[3][17], entO[3][17], nCalls, over, log[12][8]; };   // kind 3467: D2Game 0x90320 (EBX = game, stack = char sel, int mode); in: tick = GetTickCount stub, n entries of 0x44 bytes (ent), nu units (id +0xC, ref +4) chained by id&0x7F, rMask bit k = 0x48390 result of its k-th call, rOrd = base of the A448 results; out: entries after, call log {id, 7 words}
struct D2Vec8A3E0 { uint32_t nullClient, count, hasTail, tailUsed, nFree, seed; uint32_t head, tail, freeH, nAlloc, aLine, aEdx, used[4], next[4], hash[4]; };   // kind 3463: D2Game 0x8A3E0 (EAX = client, stack = src, count); in: tail block used count (hasTail), free-list length, seed of the source bytes; out: head/tail/free as pool index (0xFF none), allocator calls + line + size, per pool block used / next idx / FNV of its 0x82 dwords
static_assert(sizeof(D2Vec8A3E0) == 4 * 24, "8A3E0 record");
struct D2VecPkt3 { uint32_t a, b, c, d, e, nCalls, client, count; uint8_t bytes[8]; };   // kinds 3464-3466: D2Game 0x8C380 / 0x4AB70 / 0x8A980, the packet handed to 0x8A3E0. In: a = EAX (8C380, 8A980) / unit is null (4AB70), b = ECX / unit +0, c = EDX (8A980), d = stack flag (8A980) / unit +0xC (4AB70), e = client null; out: calls to 0x8A3E0, client (0 null, 1 the pinned buffer), count, first 8 packet bytes
static_assert(sizeof(D2VecPkt3) == 40, "pkt3 record");
// kinds 3468-3470: D2Game 0x2C830 / 0x2D670 / 0x2C120 (client-list walkers). ONE record shape = SwRec in src/core/d2game/clientsweep.h (words: 8 header, 4x10 client, 4x3 unit, 4x8 scripts, then outputs: f174 x4, outFlag, exited, nCalls, over, log[48][8]).
struct D2VecSweep { uint32_t hdr[8], cl[4][10], un[4][3], s[4][8], cf174o[4], outFlag, exited, nCalls, over, log[48][8]; };
static_assert(sizeof(D2VecSweep) == 1936, "sweep record");
// kind 3471: D2Game 0x2D9B0 (per-frame client state machine). ONE record shape = GfRec in src/core/d2game/gameframe.h (inputs 8 header + 4x7 client + 4 unit + 5x16 scripts words, then outputs: client states x4, exited, nCalls, over, log[64][8]).
struct D2VecGameFrame { uint32_t hdr[8], cl[4][7], un[4], s[5][16], st[4], exited, nCalls, over, log[64][8]; };
static_assert(sizeof(D2VecGameFrame) == 4 * (120 + 519), "game frame record");
// kind 3472: D2Game 0x2E050 (frame sequencer). ONE record shape = UpRec in src/core/d2game/gameupdate.h (8 input words, then frame after, exited, nCalls, over, log[20][5]).
struct D2VecGameUpdate { uint32_t in[8], out[104]; };
// kinds 3473-3476: D2Game 0x2CA20 / 0x29BE0 / 0x2CF30 / 0x2C0F0. ONE record shape = GmRec in src/core/d2game/gamemisc.h (64 input words, then nCalls, over, 8 extra outputs, log[64][5]).
struct D2VecGameMisc { uint32_t in[64], out[2 + 8 + 64 * 5]; };
static_assert(sizeof(D2VecGameMisc) == 4 * (64 + 2 + 8 + 64 * 5), "game misc record");
static_assert(sizeof(D2VecGameUpdate) == 4 * (8 + 104), "game update record");
struct D2Vec2EEA0 { uint32_t unit, thisB, thisCnt, thisV8, gArr, g210, cnt, r2D6, rB204, rB2E2, rA808, rB18C, r2EAD0; uint32_t result, cnt1, v81, nCalls, over; uint32_t log[6][8]; };   // kind 3458: D2Game 0x2EEA0 (EBX = unit, stack game + 1 unused, ret 8); cnt = [[0x6FD1829C]] + 0xC5C (read, not scripted); rXXX = scripted callee returns; out: result, this +4 and +8 after, log = {1 B2D6(ebx) / 2 B204(ebx) / 3 B2E2(ebx) / 4 A808(ebx) / 5 B18C(gArr, skill) / 6 2EAD0(this code, game code, ebx, g210), words}
static_assert(sizeof(D2Vec2EEA0) == 4 * (13 + 5 + 48), "2EEA0 record");
struct D2Vec3A100 { uint32_t mode, tExt, lExt, eq, flag40, set, preset, value, lFlagsIn, tFlags, ret, lFlagsOut, nBase, base[16][2], nFull, full[16][2], nMod, mod[16]; };   // kind 3262: D2Common 0x3A100 (stdcall target, unit); out: ret, unit-list flags after, target list base / full / mod arrays
static_assert(sizeof(D2Vec3A100) == 4 * (13 + 32 + 1 + 32 + 1 + 16), "3A100 record");
struct D2VecObjRgnB510 { uint32_t level; int32_t cachedIn; uint32_t nMons, mons[8]; uint32_t ret; int32_t cachedOut; };   // kind 3263: D2Game 0xB510 (EAX = pOp {pGame, pObject}); in: level id, cached trap id (region +0x1C), monster-region hcIdx list; out: return, cached id after
static_assert(sizeof(D2VecObjRgnB510) == 4 * 13, "B510 record");

// a retail recording D2Client 0xC1580 (no args; 0xC14F0 real with h=0, 0x6E020 / ordinal 10009 thunk / 0xFDB420 stubbed). In: [0x6FBCC3A8], h, b, t, deadline, the 10009 result, the loader's return.
// Out: the eax buffer 0xFDB420 got (NUL padded to 260), its stack word, call count, and the writes h, [0x6FBCBBB4], [0x6FBCB9B0], [0x6FBCC4F8], [0x6FBAAD78].
struct D2VecClientC1580 { uint32_t g3a8, h, b, t, deadline, r10009, loader; uint32_t arg, n, oh, obb4, o9b0, o4f8, oad78; char buf[260]; };
static_assert(sizeof(D2VecClientC1580) == 56 + 260, "c1580 record");
