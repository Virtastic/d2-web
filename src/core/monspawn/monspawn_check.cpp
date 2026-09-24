// Retail-gated monster-spawn check (s05-monspawn): for each (seed, difficulty, level) of
// the retail captures, make the game the headless 1.13c D2Game made,
// load the act, initialise the level, activate every room in Room2-list order the way D2Game's
// room-activation routine does (region tick, preset units, objects, random monsters) and hash the
// units that hang off the rooms exactly as the retail capture tool does (D2VecMonUnit from `room` on).
#include <emscripten.h>
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include <D2Common.h>
#include <D2CMP.h>
#include <D2DataTbls.h>
#include <D2Dungeon.h>
#include <D2Collision.h>
#include <D2Skills.h>
#include <DataTbls/SkillsTbls.h>
#include <DataTbls/BeltsTbls.h>
#include <DataTbls/InvTbls.h>
#include <DataTbls/SkillsIds.h>
#include <DataTbls/MonsterIds.h>
#include <Drlg/D2DrlgDrlgLogic.h>
#include <Drlg/D2DrlgPreset.h>
#include <Drlg/D2DrlgOutRoom.h>
#include <Drlg/D2DrlgDrlgGrid.h>
#include <D2Seed.h>
#include <D2Waypoints.h>
#include <D2StatList.h>
#include <D2States.h>
#include <D2Items.h>
#include <D2QuestRecord.h>
#include <D2Inventory.h>
#include <D2PacketDef.h>
#include <DataTbls/ItemsTbls.h>
#include <Units/Units.h>
#include <Units/Monster.h>
#include <Path/Path.h>
#include <Path/PathMisc.h>
#include <Path/Step.h>
#include <GAME/Game.h>
#include <GAME/Event.h>
#include <GAME/Arena.h>
#include <GAME/Level.h>
#include <GAME/Clients.h>
#include <AI/AiGeneral.h>
#include <UNIT/SUnit.h>
#include <UNIT/SUnitDmg.h>
#include <UNIT/SUnitInactive.h>
#include <UNIT/SUnitProxy.h>
#include <SKILLS/Skills.h>
#include <PLAYER/PlrModes.h>
#include <PLAYER/PlrMsg.h>
#include <PLAYER/PlrIntro.h>
#include <PLAYER/Player.h>
#include <PLAYER/PlayerStats.h>
#include <PLAYER/PlrSave.h>
#include <PLAYER/PlrSave2.h>
#include <ITEMS/ItemMode.h>
#include <UNIT/Party.h>
#include <MONSTER/MonsterRegion.h>
#include <MONSTER/Monster.h>
#include <MONSTER/MonsterAI.h>
#include <MONSTER/MonsterMode.h>
#include <OBJECTS/ObjRgn.h>
#include <OBJECTS/Objects.h>
#include <OBJECTS/ObjMode.h>
#include <QUESTS/Quests.h>
#include <QUESTS/ACT1/A1Q1.h>
#include <QUESTS/ACT1/A1Q6.h>
#include <QUESTS/ACT5/A5Q4.h>
#include <QUESTS/ACT5/A5Q6.h>
#include "../d2moo/compat/datatbls_bridge.h"
#include "../tables/shared.h"
#include "../../../tools/vectorfmt/vectorfmt.h"

static std::string g_err;
static D2GameStrc* g_game;
static uint32_t g_gameSeed = ~0u; static int g_gameDiff = -1;
static std::vector<D2VecMonUnit> g_units;   // the last level's units (D2_MONS_DUMP reads them)
static D2VecMonLevel g_out;
static std::vector<D2VecAiUnit> g_aiUnits;
static D2VecAiLevel g_aiOut;
static D2DrlgLevelStrc* g_lastLevel;
static D2UnitStrc* g_targetPlayer;
static D2UnitStrc* g_targetMonster;
static D2ActiveRoomStrc* g_targetRoom;
static D2ClientStrc* g_targetClient;
// The character's name, as chosen on the creation screen. Retail's header refuses a name that does
// not fit in 16 bytes including its terminator, so this buffer matches that limit exactly.
static char g_sessionCharName[16] = "PLAYER";
static std::vector<D2VecAiTargetUnit> g_targetUnits;
static D2VecAiLevel g_targetOut;
static std::vector<D2VecCombat> g_combatEvents;
static D2VecCombatResult g_combatResult;
static int g_currentAct = -1, g_currentLevel = -1;
// Session construction is class-selectable.  The older vector/AI attachment surface retains
// Amazon (0) unless a browser session explicitly supplies one of the seven CharStats classes.
static int g_sessionPlayerClass = 0;

// Pointer-free state handed to the browser.  Keep this as an explicit wire layout: none of the
// native wasm64 pointers in D2Game/D2Unit are allowed to escape the engine boundary.
#pragma pack(push, 1)
struct D2SessionSnapshot {
    uint32_t version, size, seed, frame;
    uint16_t level;
    uint8_t difficulty, act;
    uint32_t playerClass, playerId;
    int32_t x, y, targetX, targetY, hp, maxHp;
    uint32_t mode, flags, monsters, hash;
};
struct D2SessionRoom {
    uint32_t ordinal, level;
    int32_t x, y, width, height;
    uint32_t flags, firstUnit, unitCount;
};
struct D2SessionUnit {
    uint32_t id, type;
    int32_t classId;
    uint32_t mode;
    int32_t x, y;
    uint32_t flags, flagsEx;
    int32_t hp, maxHp;
    uint32_t room;
};
struct D2SessionMonsterAnimation {
    uint32_t id, classId, code, mode, framePrecise, frameCountPrecise;
    uint32_t componentCodes[16];
    uint16_t animSpeed;
    uint8_t direction, reserved;
};
struct D2SessionObjectPresentation {
    uint32_t id, classId, token, mode;
    int32_t x, y;
    uint32_t framePrecise, frameCountPrecise, componentMask;
    uint16_t animSpeed;
    uint8_t direction, reserved;
};
struct D2SessionItemPresentation {
    uint32_t id, classId, code, mode;
    int32_t x, y;
    uint32_t quality, itemFlags, framePrecise, frameCountPrecise;
    char flippyFile[32];
    char invFile[32];
    int16_t gridX, gridY;
    uint8_t invPage, bodyLoc;
    uint16_t reserved;
};
struct D2SessionItemState {
    uint32_t version, size, commandTarget, result;
    uint32_t groundCount, inventoryCount, cursorItemId, inventoryHash;
    uint32_t flags, hash;
};
struct D2SessionResources { uint32_t version, size; int32_t hp, maxHp, mana, maxMana; };
// One selected DT1 entry as used by an active DRLG room.  Library slots and entry ordinals
// replace native pointers at the browser boundary.  main/sub mirror the DT1 style/sequence
// names; frameIndex is the selected rarity/animation-frame field in the DT1 header.
struct D2SessionTile {
    uint32_t room, layer, ordinal;
    int32_t orientation, mainIndex, subIndex, frameIndex;
    uint32_t material;
    int32_t tileX, tileY, worldX, worldY, screenX, screenY, width, height;
    uint32_t flags, librarySlot, libraryIndex;
};
struct D2SessionMotion {
    uint32_t version, size, gameFrame, mode;
    int32_t x, y, targetX, targetY;
    uint32_t pathPoints, currentPoint, framePrecise, frameCountPrecise;
    uint16_t animSpeed;
    uint8_t direction, moving;
};
struct D2SessionAttack {
    uint32_t version, size, gameFrame, state;
    uint32_t attackerId, targetId, skillId, mode;
    int32_t targetHpBefore, targetHpAfter;
    uint32_t eventFrame, resultFlags, physicalDamage, totalDamage, attacks, hash;
};
struct D2SessionProgress {
    uint32_t version, size, gameFrame, targetId;
    int32_t targetHp;
    uint32_t targetMode;
    int32_t playerExperience, experienceDelta;
    uint32_t itemCount, itemHash, kills, flags;
};
struct D2SessionAdvancement {
    uint32_t version, size, gameFrame, level;
    int32_t experience, lastExperience, nextExperience;
    int32_t statPoints, skillPoints, strength, energy, dexterity, vitality;
    uint32_t skillId, skillBaseLevel, skillTotalLevel, hash;
};
struct D2SessionNpcInteraction {
    uint32_t version, size, commandTarget, result;
    uint32_t npcId, npcClass;
    int32_t distance, interactType, interactGuid;
    uint32_t npcInteractCount, npcHasPlayer, flags, hash;
};
struct D2SessionNpcDialog {
    uint32_t version, size, commandTarget, result, npcId;
    uint32_t packetHeader, packetUnitType, packetGuid, messageCount;
    uint16_t stringIds[8];
    uint8_t menus[8];
    uint32_t flags, hash;
};
struct D2SessionNpcResponse {
    uint32_t version, size, packetHeader, packetSize, npcId, messageId, result;
    uint32_t introBefore, introAfter;
    int32_t interactType, interactGuid;
    uint32_t flags, hash;
};
struct D2SessionVendor {
    uint32_t version, size, packetHeader, packetSize, action, npcId, result;
    uint32_t trading, vendorInit, itemCount, itemHash, firstClass, firstCode, firstQuality, hash;
};
struct D2SessionVendorItem {
    uint32_t id, classId, code, mode, quality, cost, flags, hash;
};
struct D2SessionVendorTransaction {
    uint32_t version, size, packetHeader, packetSize, npcId, itemId;
    uint32_t itemClass, itemCode, itemQuality, itemMode, transactionType;
    int32_t packetCost, handlerResult, goldBefore, goldAfter;
    uint32_t playerItemsBefore, playerItemsAfter, vendorItemsBefore, vendorItemsAfter;
    uint32_t ownedBefore, ownedAfter, resultItemId, flags, hash;
};
struct D2SessionRepair {
    uint32_t version, size, packetHeader, packetSize, npcId, itemId, itemClass;
    int32_t goldBefore, goldAfter, cost, durabilityBefore, durabilityAfter;
    int32_t maxDurability, quantityBefore, quantityAfter, maxQuantity;
    uint32_t ownedBefore, ownedAfter, handlerResult, flags, hash;
};
struct D2SessionAct1Quest1 {
    uint32_t version, size, level, questState, lastState, questFlags;
    uint32_t playerFlags, globalPrimaryGoal, skillPoints;
    int32_t monstersLeft;
    uint32_t enteredDen, akaraActivated, finished, timerActive, hash;
};
// A1Q6 is deliberately an observation record. Its fields come from the live quest controller
// and player bit buffer; it contains no browser-side completion state.
struct D2SessionAct1Quest6 {
    uint32_t version, size, level, questState, lastState, questFlags;
    uint32_t playerFlags, globalPrimaryGoal;
    int32_t andarielGuid;
    uint32_t timerInvocations, andarielKilled, killCallbackActive;
    uint32_t primaryGoalDone, rewardPending, rewardGranted, completedNow, hash;
};
struct D2SessionAct1Quest6Timer {
    uint32_t version, size, level, andarielId;
    uint32_t timerInvocations, timerStillScheduled, portalCount, portalId, portalClass, hash;
};
struct D2SessionAct1Quest6Complete {
    uint32_t version, size, level, timerInvocations;
    uint32_t timerReleased, lastState, state, questFlagsLow, playerFlags;
    uint32_t packetHeader, packetQuestId, packetFlags, packetState, packetField4, packetBytes, hash;
};
// One quest-drop item, the kind-16 vector's fields widened to u32 for the browser reader.
struct D2SessionAct1Quest6Drop {
    uint32_t itemId, classId, quality, ilvl, x, y, index, count;
    uint32_t seedLo, seedHi, bitBytes;
    uint8_t bits[32];
    uint32_t hash;
};
struct D2SessionItemSaveRoundTrip {
    uint32_t version, size, result, saveVersion;
    uint32_t bytesWritten, bytesConsumed, replayBytes, itemCount;
    uint32_t beforeInventoryCount, afterInventoryCount;
    uint32_t sectionHash, replayHash, flags, hash;
};
struct D2SessionStatsSaveRoundTrip {
    uint32_t version, size, result, saveVersion;
    uint32_t bytesWritten, bytesConsumed, replayBytes, statEntries;
    uint32_t sectionHash, replayHash, flags, beforeStatsHash, afterStatsHash;
    int32_t hpBefore, hpAfter, manaBefore, manaAfter;
    uint32_t hash;
};
struct D2SessionCampaignSaveRoundTrip {
    uint32_t version, size, result, saveVersion;
    uint32_t skillBytes, skillConsumed, skillCount, skillHash, skillReplayHash;
    uint32_t skillSemanticBefore, skillSemanticAfter;
    uint32_t waypointBytes, waypointConsumed, waypointHash, waypointReplayHash;
    uint32_t flags, hash, reserved;
};
struct D2SessionQuestIntroSaveRoundTrip {
    uint32_t version, size, result, saveVersion;
    uint32_t questBytes, questConsumed, questHash, questReplayHash;
    uint32_t introBytes, introConsumed, introHash, introReplayHash;
    uint32_t flags, questSemanticBefore, questSemanticAfter;
    uint32_t introSemanticBefore, introSemanticAfter, gheedBefore, gheedAfter, hash;
};
struct D2SessionCorpseSaveRoundTrip {
    uint32_t version, size, result, saveVersion;
    uint32_t bytesWritten, bytesConsumed, replayBytes, sectionCount;
    uint32_t corpseCountBefore, corpseCountAfter, sectionHash, replayHash;
    uint32_t flags, readerDetail, reserved, hash;
};
// One generated level exit. The active DRLG's Vis/Warp slot becomes source/destination IDs;
// the remaining fields are the loaded LvlWarp.txt selection/walk contract.
struct D2SessionWarp {
    uint32_t slot, room, sourceLevel, destinationLevel, warpId, enabled, direction;
    int32_t exitX, exitY;
    int32_t selectX, selectY, selectDX, selectDY, exitWalkX, exitWalkY, offsetX, offsetY;
};
#pragma pack(pop)
static_assert(sizeof(D2SessionSnapshot) == 68);
static_assert(sizeof(D2SessionRoom) == 36);
static_assert(sizeof(D2SessionUnit) == 44);
static_assert(sizeof(D2SessionMonsterAnimation) == 92);
static_assert(sizeof(D2SessionObjectPresentation) == 40);
static_assert(sizeof(D2SessionItemPresentation) == 112);
static_assert(sizeof(D2SessionItemState) == 40);
static_assert(sizeof(D2SessionResources) == 24);
static_assert(sizeof(D2SessionTile) == 76);
static_assert(sizeof(D2SessionMotion) == 52);
static_assert(sizeof(D2SessionAttack) == 64);
static_assert(sizeof(D2SessionProgress) == 48);
static_assert(sizeof(D2SessionAdvancement) == 68);
static_assert(sizeof(D2SessionNpcInteraction) == 52);
static_assert(sizeof(D2SessionNpcDialog) == 68);
static_assert(sizeof(D2SessionNpcResponse) == 52);
static_assert(sizeof(D2SessionVendor) == 60);
static_assert(sizeof(D2SessionVendorItem) == 32);
static_assert(sizeof(D2SessionVendorTransaction) == 96);
static_assert(sizeof(D2SessionRepair) == 84);
static_assert(sizeof(D2SessionAct1Quest1) == 60);
static_assert(sizeof(D2SessionAct1Quest6) == 68);
static_assert(sizeof(D2SessionAct1Quest6Timer) == 40);
static_assert(sizeof(D2SessionAct1Quest6Complete) == 64);
static_assert(sizeof(D2SessionAct1Quest6Drop) == 80);
static_assert(sizeof(D2SessionItemSaveRoundTrip) == 56);
static_assert(sizeof(D2SessionStatsSaveRoundTrip) == 72);
static_assert(sizeof(D2SessionCampaignSaveRoundTrip) == 72);
static_assert(sizeof(D2SessionQuestIntroSaveRoundTrip) == 80);
static_assert(sizeof(D2SessionCorpseSaveRoundTrip) == 64);
static_assert(sizeof(D2SessionWarp) == 68);
static D2SessionSnapshot g_sessionSnapshot;
static D2SessionResources g_sessionResources;
static D2SessionMotion g_sessionMotion;
static D2SessionAttack g_sessionAttack;
static D2SessionProgress g_sessionProgress;
static D2SessionAdvancement g_sessionAdvancement;
static uint32_t g_sessionAdvancementSkillId = SKILL_MAGICARROW;
static D2SessionNpcInteraction g_sessionNpcInteraction;
static D2SessionNpcDialog g_sessionNpcDialog;
static D2SessionNpcResponse g_sessionNpcResponse;
static D2SessionVendor g_sessionVendor;
static std::vector<D2SessionVendorItem> g_sessionVendorItems;
static D2SessionVendorTransaction g_sessionVendorTransaction;
static D2SessionRepair g_sessionRepair;
static D2SessionAct1Quest1 g_sessionAct1Quest1;
static D2SessionAct1Quest6 g_sessionAct1Quest6;
static D2SessionAct1Quest6Timer g_sessionAct1Quest6Timer;
static D2SessionAct1Quest6Complete g_sessionAct1Quest6Complete;
static std::vector<uint32_t> g_sessionAct1Quest6ItemsBefore;
static std::vector<D2SessionAct1Quest6Drop> g_sessionAct1Quest6Drops;
static D2SessionItemSaveRoundTrip g_sessionItemSaveRoundTrip;
static std::vector<uint8_t> g_sessionItemSaveBytes;
static D2SessionStatsSaveRoundTrip g_sessionStatsSaveRoundTrip;
static std::vector<uint8_t> g_sessionStatsSaveBytes;
static D2SessionCampaignSaveRoundTrip g_sessionCampaignSaveRoundTrip;
static std::vector<uint8_t> g_sessionSkillSaveBytes;
static std::vector<uint8_t> g_sessionWaypointSaveBytes;
static D2SessionQuestIntroSaveRoundTrip g_sessionQuestIntroSaveRoundTrip;
static std::vector<uint8_t> g_sessionQuestSaveBytes;
static std::vector<uint8_t> g_sessionIntroSaveBytes;
static D2SessionCorpseSaveRoundTrip g_sessionCorpseSaveRoundTrip;
static std::vector<uint8_t> g_sessionCorpseSaveBytes;
static D2UnitStrc* g_sessionAttackTarget;
static D2SkillStrc* g_sessionAttackSkill;
static uint32_t g_sessionAttackCount;
static uint32_t g_sessionAttackEndFrame;
static bool g_sessionAttackResolved;
static bool g_sessionTargetKillCounted;
static int32_t g_sessionInitialExperience;
static uint32_t g_sessionKills;
static std::vector<D2SessionRoom> g_sessionRooms;
static std::vector<D2SessionUnit> g_sessionUnits;
static std::vector<D2SessionMonsterAnimation> g_sessionMonsterAnimations;
static std::vector<D2SessionObjectPresentation> g_sessionObjectPresentations;
static std::vector<D2SessionItemPresentation> g_sessionItemPresentations;
// The shop's stock as PRESENTATION records, parallel to the existing vendor list rather than bolted
// onto it. D2SessionVendorItem is 32 bytes with a hash over its own fields, and proven rows read it
// at that stride (s50, s54) -- widening it would move their hashes for no reason. Retail's trade
// window is a 10x4 grid of item icons (Inventory.txt row 7), so the client needs each item's
// invFile and grid cell, which is exactly what D2SessionItemPresentation already carries.
static std::vector<D2SessionItemPresentation> g_sessionVendorPresentations;
static D2SessionItemState g_sessionItemState;
static std::vector<D2SessionTile> g_sessionTiles;
// Roll-order trace (DrlgRoomTile.cpp): per session tile, in d2_session_tiles order,
// { room-local GetTileCache ordinal, entries it chose among, index chosen }; ordinal UINT32_MAX
// when the record was initialised without a roll. Empty unless d2_session_trace_rolls(1) was set
// BEFORE the session was created -- the rolls happen at room activation, not at snapshot time.
struct D2WebRollTrace { uint32_t nOrdinal, nEntries, nChosen, nFramePrev, nFrameNext1, nFrameNext2; };
extern bool g_d2webRollTraceEnabled;
extern uint32_t g_d2webRollOrphans;
extern std::unordered_map<const D2DrlgTileDataStrc*, D2WebRollTrace> g_d2webRollTrace;
void D2WEB_ResetRollTrace();
struct D2WebCrossRoll { const D2DrlgRoomStrc* pRoom; uint32_t nOrdinal; const D2DrlgRoomStrc* pCause; };
extern std::vector<D2WebCrossRoll> g_d2webCrossRolls;
static std::vector<D2WebRollTrace> g_sessionTileRolls;
static std::vector<D2SessionWarp> g_sessionWarps;
struct D2SessionWarpPreset { uint32_t room; int32_t destination, x, y; };
static std::vector<D2SessionWarpPreset> g_sessionWarpPresets;
static uint32_t g_sessionWorldHash;
static uint32_t g_sessionTerrainHash;
static uint32_t g_sessionNavigationHash;
static int32_t g_sessionTargetX, g_sessionTargetY;
static bool g_sessionLive, g_sessionMovePending;

extern "C" void D2WEB_ResetLastDialogPacket();
extern "C" int D2WEB_CopyLastDialogPacket(D2GSPacketSrv27* pPacket);
extern "C" void D2WEB_ResetLastQuestStatusPacket();
extern "C" int D2WEB_CopyLastQuestStatusPacket(D2GSPacketSrv5D* pPacket);

static uint32_t sessionHashBytes(uint32_t hash, const void* data, size_t size) {
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < size; ++i) { hash ^= bytes[i]; hash *= 16777619u; }
    return hash;
}

// d2cmp_tilelib.cpp owns these nodes.  They are deliberately native helper objects rather than
// retail packed structs, so repeat the private layout here instead of using D2CMP.h's packed
// 32-bit DLL declarations (whose node pointer begins four bytes earlier under wasm64).
struct D2SessionTileRef { D2TileLibraryEntryStrc* tile; D2SessionTileRef* next; };
struct D2SessionTileNode {
    int32_t style, sequence, type;
    D2SessionTileRef* refs;
    D2SessionTileNode* next;
};
struct D2SessionTileHash { D2SessionTileNode* nodes[128]; };

// The DT1 files the CURRENT level's rooms actually loaded, in first-seen order. A tile's
// `librarySlot` indexes THIS, not LvlTypes: the engine's own slot number is per-room (it indexes
// that room's pTiles[], holding only what that room loaded, in its own order), so it cannot name a
// file on its own. Preset rooms pull DT1s through their DS1 that LvlTypes never lists, which is why
// mapping through the level's table left those levels with no floor.
extern "C" const char* D2WEB_TileLibraryNameFromHash(const D2TileLibraryHashStrc* pHash);
static std::vector<std::string> g_sessionTileLibraries;

static uint32_t sessionTileLibraryFile(const D2TileLibraryHashStrc* pHash) {
    const char* name = pHash ? D2WEB_TileLibraryNameFromHash(pHash) : nullptr;
    if (!name) return UINT32_MAX;
    for (size_t i = 0; i < g_sessionTileLibraries.size(); ++i)
        if (g_sessionTileLibraries[i] == name) return (uint32_t)i;
    g_sessionTileLibraries.emplace_back(name);
    return (uint32_t)(g_sessionTileLibraries.size() - 1);
}

static void sessionTileLibraryIdentity(const D2DrlgRoomStrc* roomEx, const D2TileLibraryEntryStrc* tile,
        uint32_t& slotOut, uint32_t& indexOut) {
    slotOut = UINT32_MAX; indexOut = UINT32_MAX;
    if (!roomEx || !tile) return;
    const uint32_t bucket = (uint32_t)(2 * (uint8_t)tile->nType - (uint8_t)tile->nSequence + (uint8_t)tile->nStyle) & 0x7Fu;
    for (uint32_t slot = 0; slot < 32 && roomEx->pTiles[slot]; ++slot) {
        D2SessionTileHash* hash = reinterpret_cast<D2SessionTileHash*>(roomEx->pTiles[slot]);
        for (D2SessionTileNode* node = hash->nodes[bucket]; node; node = node->next) {
            if (node->type != tile->nType || node->style != tile->nStyle || node->sequence != tile->nSequence) continue;
            uint32_t index = 0;
            for (D2SessionTileRef* ref = node->refs; ref; ref = ref->next, ++index) {
                // Report WHICH FILE, not which of this room's slots.
                if (ref->tile == tile) { slotOut = sessionTileLibraryFile(roomEx->pTiles[slot]); indexOut = index; return; }
            }
        }
    }
}

static uint32_t g_sessionTilesSkipped = 0;

static void appendSessionTiles(const D2DrlgRoomStrc* roomEx, uint32_t roomOrdinal, uint32_t layer,
        const D2DrlgTileDataStrc* tiles, int count) {
    if (!tiles || count <= 0) return;
    for (int i = 0; i < count; ++i) {
        const D2DrlgTileDataStrc& tileData = tiles[i];
        if (!tileData.pTile) { ++g_sessionTilesSkipped; continue; }
        const D2TileLibraryEntryStrc& tile = *tileData.pTile;
        D2SessionTile out = {};
        out.room = roomOrdinal; out.layer = layer; out.ordinal = (uint32_t)i;
        out.orientation = tile.nType; out.mainIndex = tile.nStyle; out.subIndex = tile.nSequence;
        out.frameIndex = tile.nRarity_Frame; out.material = tile.nFlags;
        out.tileX = tileData.nPosX; out.tileY = tileData.nPosY;
        out.worldX = roomEx->nTileXPos + tileData.nPosX;
        out.worldY = roomEx->nTileYPos + tileData.nPosY;
        out.screenX = tileData.nWidth; out.screenY = tileData.nHeight;
        out.width = tile.nWidth; out.height = tile.nTotalHeight;
        out.flags = tileData.dwFlags;
        sessionTileLibraryIdentity(roomEx, tileData.pTile, out.librarySlot, out.libraryIndex);
        g_sessionTiles.push_back(out);
        if (g_d2webRollTraceEnabled) {
            auto it = g_d2webRollTrace.find(&tileData);
            g_sessionTileRolls.push_back(it != g_d2webRollTrace.end() ? it->second : D2WebRollTrace{ UINT32_MAX - 1, 0, 0, 0, 0, 0 });
        }
    }
}

static void appendSessionItemPresentation(D2UnitStrc* item) {
    if (!item || item->dwUnitType != UNIT_ITEM) return;
    D2SessionItemPresentation out = {};
    out.id = item->dwUnitId; out.classId = item->dwClassId; out.mode = item->dwAnimMode;
    out.quality = ITEMS_GetItemQuality(item); out.itemFlags = ITEMS_GetItemFlags(item);
    out.framePrecise = item->nSeqCurrentFramePrecise; out.frameCountPrecise = item->dwFrameCountPrecise;
    if (item->pStaticPath) {
        out.x = item->pStaticPath->tGameCoords.nX; out.y = item->pStaticPath->tGameCoords.nY;
        out.gridX = (int16_t)item->pStaticPath->tGameCoords.nX;
        out.gridY = (int16_t)item->pStaticPath->tGameCoords.nY;
    }
    out.invPage = ITEMS_GetInvPage(item); out.bodyLoc = ITEMS_GetBodyLocation(item);
    if (D2ItemsTxt* txt = DATATBLS_GetItemsTxtRecord(item->dwClassId)) {
        out.code = txt->dwCode;
        memcpy(out.flippyFile, txt->szFlippyFile, sizeof out.flippyFile);
        memcpy(out.invFile, txt->szInvFile, sizeof out.invFile);
    }
    g_sessionItemPresentations.push_back(out);
}

static void syncSessionLevelFromPlayer() {
    if (!g_targetPlayer) return;
    D2ActiveRoomStrc* room = UNITS_GetRoom(g_targetPlayer);
    if (!room || !room->pDrlgRoom || !room->pDrlgRoom->pLevel) return;
    const int level = DUNGEON_GetLevelIdFromRoom(room);
    if (level <= 0) return;
    g_currentLevel = level;
    g_currentAct = DRLG_GetActNoFromLevelId(level);
    g_lastLevel = room->pDrlgRoom->pLevel;
    g_targetRoom = room;
}

// LEVEL_WarpUnit transfers the player to its destination room, but the normal server
// transition has already made that level's rooms visible and run their activation path.
// The headless session owns that second half explicitly so destination monster regions,
// presets, and quest counters are created by the recovered D2Game routine as well.
static void activateSessionLevelRooms(D2DrlgActStrc* act, D2DrlgLevelStrc* level, int levelId) {
    if (!act || !level) return;
    for (D2DrlgRoomStrc* roomEx = level->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext) {
        if (!roomEx->pRoom) DUNGEON_SetClientIsInSight(act, levelId, roomEx->nTileXPos, roomEx->nTileYPos, nullptr);
        if (roomEx->pRoom) sub_6FC385A0(g_game, roomEx->pRoom);
    }
}

// While the client holds the world (d2_session_hold_world), the first read rebuilds these lists and
// the rest reuse them: every read export rebuilds otherwise, and the shell's one snapshot is a dozen
// reads -- ten times the cost of the game tick itself (s000a's budget). Only read-only stretches
// hold; a command between two reads needs a release first.
static int g_worldHold = 0; static bool g_worldHoldBuilt = false;
static void updateSessionWorld() {
    if (g_worldHold && g_worldHoldBuilt) return;
    if (g_worldHold) g_worldHoldBuilt = true;
    g_sessionRooms.clear(); g_sessionUnits.clear(); g_sessionMonsterAnimations.clear(); g_sessionObjectPresentations.clear(); g_sessionItemPresentations.clear(); g_sessionTiles.clear(); g_sessionTileRolls.clear(); g_sessionTileLibraries.clear(); g_sessionWarps.clear();
    g_sessionWorldHash = D2VEC_FNV_INIT; g_sessionTerrainHash = D2VEC_FNV_INIT;
    g_sessionNavigationHash = D2VEC_FNV_INIT;
    if (!g_sessionLive || !g_lastLevel) return;
    uint32_t roomOrdinal = 0;
    for (D2DrlgRoomStrc* roomEx = g_lastLevel->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext, ++roomOrdinal) {
        D2ActiveRoomStrc* room = roomEx->pRoom;
        if (!room) continue;
        D2SessionRoom out = {};
        out.ordinal = roomOrdinal; out.level = (uint32_t)g_currentLevel;
        out.x = room->tCoords.nSubtileX; out.y = room->tCoords.nSubtileY;
        out.width = room->tCoords.nSubtileWidth; out.height = room->tCoords.nSubtileHeight;
        out.flags = room->dwFlags; out.firstUnit = (uint32_t)g_sessionUnits.size();
        if (room->pRoomTiles) {
            appendSessionTiles(roomEx, roomOrdinal, 0, room->pRoomTiles->pFloorTiles, room->pRoomTiles->nFloors);
            appendSessionTiles(roomEx, roomOrdinal, 1, room->pRoomTiles->pWallTiles, room->pRoomTiles->nWalls);
            appendSessionTiles(roomEx, roomOrdinal, 2, room->pRoomTiles->pRoofTiles, room->pRoomTiles->nRoofs);
        }
        for (D2UnitStrc* unit = room->pUnitFirst; unit; unit = unit->pRoomNext) {
            if (unit->dwUnitType == UNIT_OBJECT && unit->pObjectData && unit->pObjectData->pObjectTxt) {
                D2SessionObjectPresentation object = {};
                const D2ObjectsTxt* txt = unit->pObjectData->pObjectTxt;
                object.id = unit->dwUnitId; object.classId = unit->dwClassId;
                memcpy(&object.token, txt->szToken, 3); object.mode = unit->dwAnimMode;
                if (unit->pStaticPath) { object.x = unit->pStaticPath->tGameCoords.nX; object.y = unit->pStaticPath->tGameCoords.nY; }
                object.framePrecise = unit->nSeqCurrentFramePrecise; object.frameCountPrecise = unit->dwFrameCountPrecise;
                object.animSpeed = unit->wAnimSpeed; object.direction = txt->nOrientation;
                const uint8_t* pieces = &txt->nHD;
                for (uint32_t component = 0; component < 16; ++component) if (pieces[component]) object.componentMask |= 1u << component;
                g_sessionObjectPresentations.push_back(object);
            }
            if (unit->dwUnitType != UNIT_PLAYER && unit->dwUnitType != UNIT_MONSTER && unit->dwUnitType != UNIT_ITEM && unit->dwUnitType != UNIT_MISSILE) continue;
            D2SessionUnit actor = {};
            actor.id = (uint32_t)unit->dwUnitId; actor.type = unit->dwUnitType;
            actor.classId = unit->dwClassId; actor.mode = unit->dwAnimMode;
            if ((unit->dwUnitType == UNIT_PLAYER || unit->dwUnitType == UNIT_MONSTER || unit->dwUnitType == UNIT_MISSILE) && unit->pDynamicPath) {
                actor.x = unit->pDynamicPath->tGameCoords.wPosX;
                actor.y = unit->pDynamicPath->tGameCoords.wPosY;
            } else if (unit->pStaticPath) {
                actor.x = unit->pStaticPath->tGameCoords.nX;
                actor.y = unit->pStaticPath->tGameCoords.nY;
            }
            actor.flags = unit->dwFlags; actor.flagsEx = unit->dwFlagEx;
            actor.hp = STATLIST_UnitGetStatValue(unit, STAT_HITPOINTS, 0);
            actor.maxHp = STATLIST_UnitGetStatValue(unit, STAT_MAXHP, 0);
            actor.room = roomOrdinal; g_sessionUnits.push_back(actor); ++out.unitCount;
            if (unit->dwUnitType == UNIT_ITEM) appendSessionItemPresentation(unit);
            if (unit->dwUnitType == UNIT_MONSTER && unit->pMonsterData) {
                D2SessionMonsterAnimation animation = {};
                animation.id = unit->dwUnitId; animation.classId = unit->dwClassId;
                animation.code = unit->pMonsterData->pMonstatsTxt ? unit->pMonsterData->pMonstatsTxt->dwCode : 0;
                animation.mode = unit->dwAnimMode; animation.framePrecise = unit->nSeqCurrentFramePrecise;
                animation.frameCountPrecise = unit->dwFrameCountPrecise; animation.animSpeed = unit->wAnimSpeed;
                animation.direction = unit->pDynamicPath ? unit->pDynamicPath->nDirection : 0;
                for (uint32_t component = 0; component < 16; ++component)
                    animation.componentCodes[component] = D2Common_11069(unit, component, unit->pMonsterData->nComponent[component]);
                g_sessionMonsterAnimations.push_back(animation);
            }
        }
        g_sessionRooms.push_back(out);
    }
    if (g_targetPlayer && g_targetPlayer->pInventory) {
        for (D2UnitStrc* item = INVENTORY_GetFirstItem(g_targetPlayer->pInventory); item; item = INVENTORY_GetNextItem(item)) {
            if (INVENTORY_UnitIsItem(item)) appendSessionItemPresentation(item);
        }
    }
    D2DrlgWarpStrc* dynamicWarps = DRLG_GetDrlgWarpFromLevelId(g_lastLevel->pDrlg, g_currentLevel);
    const D2LevelDefBin* levelDef = DATATBLS_GetLevelDefRecord(g_currentLevel);
    for (uint32_t slot = 0; slot < 8; ++slot) {
        const int32_t destination = dynamicWarps ? dynamicWarps->nVis[slot] : levelDef->dwVis[slot];
        const int32_t warpId = dynamicWarps ? dynamicWarps->nWarp[slot] : (int32_t)levelDef->dwWarp[slot];
        if (destination <= 0 || warpId < 0) continue;
        D2LvlWarpTxt* warp = DATATBLS_GetLvlWarpTxtRecordFromLevelIdAndDirection(warpId, 'b');
        if (!warp) continue;
        D2SessionWarp out = {};
        out.slot = slot; out.room = UINT32_MAX;
        out.sourceLevel = (uint32_t)g_currentLevel; out.destinationLevel = (uint32_t)destination;
        out.warpId = (uint32_t)warpId; out.enabled = 1; out.direction = (uint8_t)warp->szDirection[0];
        uint32_t ordinal = 0;
        for (const D2SessionWarpPreset& preset : g_sessionWarpPresets) {
            if (preset.destination != destination) continue;
            out.room = preset.room; out.exitX = preset.x; out.exitY = preset.y; break;
        }
        for (D2DrlgRoomStrc* room = g_lastLevel->pFirstRoomEx; room; room = room->pDrlgRoomNext, ++ordinal) {
            if (out.room != UINT32_MAX) break;
            for (D2RoomTileStrc* tile = room->pRoomTiles; tile; tile = tile->pNext) {
                if (!tile->bEnabled || !tile->pDrlgRoom || !tile->pDrlgRoom->pLevel || tile->pDrlgRoom->pLevel->nLevelId != destination) continue;
                D2DrlgTileDataStrc* data = tile->unk0x0C ? tile->unk0x0C : tile->unk0x10;
                out.room = ordinal;
                if (data && tile->pLvlWarpTxtRecord) {
                    out.exitX = 5 * (room->nTileXPos + data->nPosX) + (int32_t)tile->pLvlWarpTxtRecord->dwOffsetX;
                    out.exitY = 5 * (room->nTileYPos + data->nPosY) + (int32_t)tile->pLvlWarpTxtRecord->dwOffsetY;
                } else {
                    const D2DrlgRoomStrc* linked = tile->pDrlgRoom;
                    const int32_t roomCenterX = room->nTileXPos + room->nTileWidth / 2;
                    const int32_t roomCenterY = room->nTileYPos + room->nTileHeight / 2;
                    const int32_t linkedCenterX = linked->nTileXPos + linked->nTileWidth / 2;
                    const int32_t linkedCenterY = linked->nTileYPos + linked->nTileHeight / 2;
                    out.exitX = 5 * (linkedCenterX < roomCenterX ? room->nTileXPos :
                        linkedCenterX > roomCenterX ? room->nTileXPos + room->nTileWidth - 1 : roomCenterX) + 2;
                    out.exitY = 5 * (linkedCenterY < roomCenterY ? room->nTileYPos :
                        linkedCenterY > roomCenterY ? room->nTileYPos + room->nTileHeight - 1 : roomCenterY) + 2;
                }
                if (room->pRoom) {
                    D2CoordStrc interaction = { out.exitX, out.exitY };
                    if (COLLISION_GetFreeCoordinatesWithMaxDistance(room->pRoom, &interaction, 2,
                        COLLIDE_MASK_PLAYER_PATH, TRUE, 6)) {
                        out.exitX = interaction.nX; out.exitY = interaction.nY;
                    }
                }
                break;
            }
            if (out.room != UINT32_MAX) break;
        }
        out.selectX = (int32_t)warp->dwSelectX; out.selectY = (int32_t)warp->dwSelectY;
        out.selectDX = (int32_t)warp->dwSelectDX; out.selectDY = (int32_t)warp->dwSelectDY;
        out.exitWalkX = (int32_t)warp->dwExitWalkX; out.exitWalkY = (int32_t)warp->dwExitWalkY;
        out.offsetX = (int32_t)warp->dwOffsetX; out.offsetY = (int32_t)warp->dwOffsetY;
        g_sessionWarps.push_back(out);
    }
    auto hashBytes = [](uint32_t hash, const void* data, size_t size) {
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        for (size_t i = 0; i < size; ++i) { hash ^= bytes[i]; hash *= 16777619u; }
        return hash;
    };
    if (!g_sessionRooms.empty()) g_sessionWorldHash = hashBytes(g_sessionWorldHash, g_sessionRooms.data(), g_sessionRooms.size() * sizeof(D2SessionRoom));
    if (!g_sessionUnits.empty()) g_sessionWorldHash = hashBytes(g_sessionWorldHash, g_sessionUnits.data(), g_sessionUnits.size() * sizeof(D2SessionUnit));
    if (!g_sessionTiles.empty()) g_sessionTerrainHash = hashBytes(g_sessionTerrainHash, g_sessionTiles.data(), g_sessionTiles.size() * sizeof(D2SessionTile));
    if (!g_sessionWarps.empty()) g_sessionNavigationHash = hashBytes(g_sessionNavigationHash, g_sessionWarps.data(), g_sessionWarps.size() * sizeof(D2SessionWarp));
}

static void captureCombat(D2UnitStrc* attacker, D2UnitStrc* defender, const D2DamageStrc* damage) {
    if (!g_targetPlayer || !attacker || !defender || !damage) return;
    D2VecCombat v = {};
    v.seed = g_gameSeed; v.difficulty = (uint8_t)g_gameDiff; v.act = (uint8_t)g_currentAct; v.level = (uint16_t)g_currentLevel;
    v.frame = g_game ? g_game->dwGameFrame : 0;
    v.attackerType = attacker->dwUnitType; v.attackerClass = attacker->dwClassId; v.attackerId = attacker->dwUnitId;
    v.defenderType = defender->dwUnitType; v.defenderClass = defender->dwClassId; v.defenderId = defender->dwUnitId;
    v.seedLo = attacker->pSeed.nLowSeed; v.seedHi = attacker->pSeed.nHighSeed;
    static_assert(sizeof(*damage) == sizeof(v.damage));
    memcpy(v.damage, damage, sizeof(v.damage));
    g_combatEvents.push_back(v);
    if (attacker == g_targetPlayer && defender == g_sessionAttackTarget && g_sessionAttack.state == 1) {
        g_sessionAttack.resultFlags = damage->wResultFlags;
        g_sessionAttack.physicalDamage = (uint32_t)damage->dwPhysDamage;
        g_sessionAttack.totalDamage = (uint32_t)damage->dwDmgTotal;
    }
}

static D2VecAiUnit captureAi(const D2UnitStrc* u, uint16_t room, uint16_t ordinal) {
    D2VecAiUnit a = {};
    a.room = room; a.ordinal = ordinal; a.classId = u->dwClassId; a.unitId = u->dwUnitId;
    if (u->pDynamicPath) { a.x = u->pDynamicPath->tGameCoords.wPosX; a.y = u->pDynamicPath->tGameCoords.wPosY; }
    const D2AiControlStrc* ai = u->pMonsterData ? u->pMonsterData->pAiControl : nullptr;
    if (ai) {
        a.specialState = ai->nAiSpecialState; a.flags = ai->nAiFlags;
        a.ownerGuid = ai->dwOwnerGUID; a.ownerType = ai->dwOwnerType;
        memcpy(a.params, ai->dwAiParam, sizeof(a.params));
        if (ai->pCurrentCmd) { a.hasCurrentCmd = 1; memcpy(a.currentCmdParams, ai->pCurrentCmd->nCmdParam, sizeof(a.currentCmdParams)); }
        for (const D2MinionListStrc* m = ai->pMinionList; m; m = m->pNext) {
            a.minionCount++; a.minionHash = d2vec_fnv1a_step(a.minionHash ? a.minionHash : D2VEC_FNV_INIT, (uint32_t)m->dwMinionGUID);
        }
    }
    a.thinkFrame = -1;
    for (const D2EventTimerStrc* t = u->pTimer; t; t = t->pNext) if (t->nEventType == EVENTTYPE_AITHINK) { a.thinkFrame = t->nExpireFrame; break; }
    return a;
}

static D2VecAiTargetUnit captureAiTarget(const D2UnitStrc* u, uint16_t room, uint16_t ordinal) {
    D2VecAiTargetUnit a = {};
    a.ai = captureAi(u, room, ordinal);
    a.mode = u->dwAnimMode; a.seedLo = u->pSeed.nLowSeed; a.seedHi = u->pSeed.nHighSeed;
    a.hp = STATLIST_UnitGetStatValue((D2UnitStrc*)u, STAT_HITPOINTS, 0);
    a.maxHp = STATLIST_UnitGetStatValue((D2UnitStrc*)u, STAT_MAXHP, 0);
    if (u->pDynamicPath) {
        D2UnitStrc* target = u->pDynamicPath->pTargetUnit;
        a.targetType = target ? target->dwUnitType : u->pDynamicPath->dwTargetType;
        a.targetId = target ? target->dwUnitId : u->pDynamicPath->dwTargetId;
        a.targetX = u->pDynamicPath->tTargetCoord.X; a.targetY = u->pDynamicPath->tTargetCoord.Y;
    }
    a.usedSkillId = -1;
    if (u->pSkills && u->pSkills->pUsedSkill && u->pSkills->pUsedSkill->pSkillsTxt)
        a.usedSkillId = u->pSkills->pUsedSkill->pSkillsTxt->nSkillId;
    a.modeEventFrame = -1;
    for (const D2EventTimerStrc* t = u->pTimer; t; t = t->pNext)
        if (t->nEventType == EVENTTYPE_MODECHANGE) { a.modeEventFrame = t->nExpireFrame; break; }
    a.framePrecise = u->nSeqCurrentFramePrecise; a.frameCountPrecise = u->dwFrameCountPrecise;
    a.animSpeed = u->wAnimSpeed; a.actionFrame = u->nActionFrame;
    return a;
}

static void detachTargetFixture() {
    // The unit stays in the world; its client does not, so nothing sent to every player
    // (0x81 pet assignments iterate the living players) reaches freed memory.
    if (g_targetPlayer && UNITS_GetPlayerData(g_targetPlayer)) UNITS_GetPlayerData(g_targetPlayer)->pClient = nullptr;
    if (g_targetRoom) { g_targetRoom->ppClients = nullptr; g_targetRoom->nNumClients = 0; g_targetRoom->nMaxClients = 0; }
    free(g_targetClient); g_targetClient = nullptr; g_targetRoom = nullptr; g_targetPlayer = nullptr; g_targetMonster = nullptr;
}

// GAME_CreateGame minus the server (game slots, clients, tasks, critical sections): the fields the
// item/monster/object code reads, in the order retail fills them.
static D2GameStrc* makeGame(uint32_t seed, int difficulty) {
    D2GameStrc* g = (D2GameStrc*)calloc(1, sizeof(D2GameStrc));
    g->bExpansion = TRUE;
    g->wItemFormat = 101;
    g->dwGameType = 0;
    g->nDifficulty = (uint8_t)difficulty;
    g->nMaxPlayers = 8;
    g->InitSeed = 1;
    g->dwInitSeed = seed;
    SEED_InitSeed(&g->pGameSeed);
    SEED_InitLowSeed(&g->pGameSeed, seed);
    EVENT_AllocEventQueue(g);
    ARENA_AllocArena(g, 0, ((uint32_t)difficulty << 12) | 0x04 | 0x100000, 0);
    PARTY_AllocPartyControl(g);
    sub_6FC68110(g);                                  // monster regions from the game seed
    g->dwObjSeed = OBJRGN_AllocObjectControl(g);
    SUNITPROXY_InitializeNpcControl(g);
    QUESTS_QuestInit(g);
    if (g->pQuestControl) g->pQuestControl->bPickedSet = 1;   // what the first player's entry does; the capture sets the same
    return g;
}

extern "C" {


EMSCRIPTEN_KEEPALIVE int d2_monspawn_init() {
    if (!d2::drlgLoadTables(d2::sharedVfs(), d2::sharedLang())) { g_err = "tables"; return 0; }
    SUNITPROXY_FillGlobalItemCache();
    gpCombatTraceCallback = captureCombat;
    return 1;
}
EMSCRIPTEN_KEEPALIVE void d2_monspawn_reset() {
    // Force the next level call through makeGame even when seed/difficulty are unchanged.
    // The differential fixtures use one fresh retail process per record.
    detachTargetFixture(); g_gameSeed = ~0u; g_gameDiff = -1; g_lastLevel = nullptr;
    g_currentAct = g_currentLevel = -1; g_combatEvents.clear();
    memset(&g_combatResult, 0, sizeof g_combatResult);
}

// Loads (seed, difficulty) once; returns the game's act for `act` (0-based), loading it like
// LEVEL_LoadAct. Acts stay loaded for the game's lifetime; a new (seed, difficulty) frees the old game.
static D2DrlgActStrc* gameAct(uint32_t seed, int difficulty, int act) {
    if (!g_game || g_gameSeed != seed || g_gameDiff != difficulty) {
        if (g_game) { for (int a = 0; a < 5; a++) if (g_game->pAct[a]) { DUNGEON_FreeAct(g_game->pAct[a]); g_game->pAct[a] = nullptr; } }
        g_game = makeGame(seed, difficulty); g_gameSeed = seed; g_gameDiff = difficulty;
    }
    if (!g_game->pAct[act]) LEVEL_LoadAct(g_game, (uint8_t)act);
    return g_game->pAct[act];
}

// One level: returns 1 and fills d2_monspawn_out() (units, monsters, hash); 0 on failure.
int d2_trace_level = -1;   // D2_MONS_TRACE builds print for this level id (scenario sets it from D2_MONS_TRACE_LEVEL)
EMSCRIPTEN_KEEPALIVE void d2_monspawn_trace(int levelId) { d2_trace_level = levelId; }
EMSCRIPTEN_KEEPALIVE int d2_monspawn_level(uint32_t seed, int difficulty, int act, int levelId) {
    memset(&g_out, 0, sizeof g_out); g_units.clear(); g_aiUnits.clear();
    g_out.seed = seed; g_out.difficulty = (uint8_t)difficulty; g_out.act = (uint8_t)act; g_out.level = (uint16_t)levelId; g_out.hash = D2VEC_FNV_INIT;
    g_aiOut = { seed, (uint8_t)difficulty, (uint8_t)act, (uint16_t)levelId, 0, D2VEC_FNV_INIT, 0 };
    g_currentAct = act; g_currentLevel = levelId; g_combatEvents.clear(); g_sessionWarpPresets.clear();
    D2DrlgActStrc* pAct = gameAct(seed, difficulty, act);
    if (!pAct) { g_err = "act"; return 0; }
    D2DrlgLevelStrc* pLevel = DRLG_GetLevel(pAct->pDrlg, levelId);
    if (!pLevel) { g_err = "level"; return 0; }
    if (!pLevel->pFirstRoomEx) DRLG_InitLevel(pLevel);
    g_lastLevel = pLevel;
    uint16_t roomIdx = 0;
    for (D2DrlgRoomStrc* pRoomEx = pLevel->pFirstRoomEx; pRoomEx; pRoomEx = pRoomEx->pDrlgRoomNext, roomIdx++) {
        for (D2PresetUnitStrc* preset = pRoomEx->pPresetUnits; preset; preset = preset->pNext) if (preset->nUnitType == UNIT_TILE) {
            g_sessionWarpPresets.push_back({ roomIdx, preset->nIndex,
                pRoomEx->nTileXPos * 5 + preset->nXpos, pRoomEx->nTileYPos * 5 + preset->nYpos });
        }
        if (!pRoomEx->pRoom) DUNGEON_SetClientIsInSight(pAct, levelId, pRoomEx->nTileXPos, pRoomEx->nTileYPos, nullptr);
        D2ActiveRoomStrc* pRoom = pRoomEx->pRoom;
        if (!pRoom) continue;
        for (D2UnitStrc* tile = pRoom->pUnitFirst; tile; tile = tile->pRoomNext) if (tile->dwUnitType == UNIT_TILE && tile->pStaticPath) {
            g_sessionWarpPresets.push_back({ roomIdx, tile->dwClassId,
                tile->pStaticPath->tGameCoords.nX, tile->pStaticPath->tGameCoords.nY });
        }
#ifdef D2_MONS_TRACE
        if (levelId == d2_trace_level) fprintf(stderr, "room %u type %d flags %x logical %p lists %d/%x\n", roomIdx, pRoomEx->nType, pRoomEx->dwFlags, (void*)pRoomEx->pLogicalRoomInfo, pRoomEx->pLogicalRoomInfo ? pRoomEx->pLogicalRoomInfo->nLists : -1, pRoomEx->pLogicalRoomInfo ? pRoomEx->pLogicalRoomInfo->dwFlags : 0);
#endif
        { D2VecMonUnit m = {}; m.seed = seed; m.difficulty = (uint8_t)difficulty; m.act = (uint8_t)act; m.level = (uint16_t)levelId; m.room = roomIdx; m.unitType = 254; m.seedLo = pRoom->pSeed.nLowSeed; m.seedHi = pRoom->pSeed.nHighSeed; g_units.push_back(m); }
#ifdef D2_MONS_TRACE
        if (levelId == d2_trace_level) fprintf(stderr, "[room] %u [%d,%d] gameseed %u/%u objseed %u/%u roomseed %u\n", roomIdx, pRoom->tCoords.nSubtileX, pRoom->tCoords.nSubtileY, g_game->pGameSeed.nLowSeed, g_game->pGameSeed.nHighSeed, g_game->pObjectControl->pSeed.nLowSeed, g_game->pObjectControl->pSeed.nHighSeed, pRoom->pSeed.nLowSeed);
#endif
        sub_6FC385A0(g_game, pRoom);   // D2Game 1.13c 0x6FC4CB80 room activation (compat/game_glue.cpp)
        { D2VecMonUnit m = {}; m.seed = seed; m.difficulty = (uint8_t)difficulty; m.act = (uint8_t)act; m.level = (uint16_t)levelId; m.room = roomIdx; m.unitType = 255;   // diagnosis: room seed after activation, not hashed
          m.seedLo = pRoom->pSeed.nLowSeed; m.seedHi = pRoom->pSeed.nHighSeed; m.x = (uint16_t)pRoom->tCoords.nSubtileX; m.y = (uint16_t)pRoom->tCoords.nSubtileY; g_units.push_back(m); }
        uint16_t monOrdinal = 0;
        for (D2UnitStrc* u = pRoom->pUnitFirst; u; u = u->pRoomNext) {
            D2VecMonUnit m = {};
            m.seed = seed; m.difficulty = (uint8_t)difficulty; m.act = (uint8_t)act; m.level = (uint16_t)levelId; m.room = roomIdx;
            m.unitType = (uint8_t)u->dwUnitType; m.classId = u->dwClassId; m.mode = (uint8_t)u->dwAnimMode;
            m.seedLo = u->pSeed.nLowSeed; m.seedHi = u->pSeed.nHighSeed;
            if (u->dwUnitType == UNIT_MONSTER || u->dwUnitType == UNIT_PLAYER || u->dwUnitType == UNIT_MISSILE) { m.x = (uint16_t)u->pDynamicPath->tGameCoords.wPosX; m.y = (uint16_t)u->pDynamicPath->tGameCoords.wPosY; }
            else if (u->pStaticPath) { m.x = (uint16_t)u->pStaticPath->dwClientCoordX; m.y = (uint16_t)u->pStaticPath->dwClientCoordY; }   // capture reads +0x04/+0x08
            if (u->dwUnitType == UNIT_MONSTER && u->pMonsterData) {
                // Native wasm64 pointers shift the runtime fields past their retail x86
                // offsets. Serialize the named fields into the fixed-width vector record.
                const D2MonsterDataStrc* md = u->pMonsterData;
                m.monFlags = md->nTypeFlag;
                memcpy(m.enchants, md->nMonUmod, sizeof(m.enchants));
                m.uniqueNo = md->wBossHcIdx;
                m.hp = (uint32_t)STATLIST_UnitGetStatValue(u, STAT_HITPOINTS, 0); m.maxHp = (uint32_t)STATLIST_UnitGetStatValue(u, STAT_MAXHP, 0); m.lvl = (uint32_t)STATLIST_UnitGetStatValue(u, STAT_LEVEL, 0);
            }
            g_out.hash = d2vec_mon_hash(g_out.hash, &m);
            g_out.units++; if (m.unitType == UNIT_MONSTER) g_out.monsters++;
            g_units.push_back(m);
            if (u->dwUnitType == UNIT_MONSTER) {
                D2VecAiUnit a = captureAi(u, roomIdx, monOrdinal++);
                g_aiOut.hash = d2vec_ai_hash(g_aiOut.hash, &a); g_aiOut.monsters++; g_aiUnits.push_back(a);
            }
        }
    }
    return 1;
}
EMSCRIPTEN_KEEPALIVE const D2VecMonLevel* d2_monspawn_out() { return &g_out; }
EMSCRIPTEN_KEEPALIVE const D2VecMonUnit* d2_monspawn_units(int* count) { *count = (int)g_units.size(); return g_units.data(); }
EMSCRIPTEN_KEEPALIVE const char* d2_monspawn_err() { return g_err.c_str(); }
EMSCRIPTEN_KEEPALIVE const D2VecAiLevel* d2_aispawn_out() { return &g_aiOut; }
EMSCRIPTEN_KEEPALIVE const D2VecAiUnit* d2_aispawn_units(int* count) { *count = (int)g_aiUnits.size(); return g_aiUnits.data(); }
// Anchor class for the player fixture: any monster (0xFFFFFFFF) or a specific class, so a
// session can be created beside one town NPC the way the retail capture's fixture is.
static uint32_t g_sessionAnchorClass = 0xFFFFFFFF;
// Place the session player at a standable subtile of `room` and wire up its client. Split out of
// d2_aitarget_attach so a level with no monster at all can still be entered: the anchor search
// below is what the AI oracle rows need, not what a player needs.
static int attachSessionPlayerAt(D2ActiveRoomStrc* room, int x, int y, D2UnitStrc* anchor) {
    D2UnitStrc* player = SUNIT_AllocUnitData(UNIT_PLAYER, g_sessionPlayerClass, x, y, g_game, room, 1, PLRMODE_NEUTRAL, 0);
    if (!player) return 0;
    STATLIST_SetUnitStat(player, STAT_HITPOINTS, 50 << 8, 0); STATLIST_SetUnitStat(player, STAT_MAXHP, 50 << 8, 0);
    STATLIST_SetUnitStat(player, STAT_LEVEL, 1, 0); STATLIST_SetUnitStat(player, STAT_ATTACKRATE, 100, 0);
    STATLIST_SetUnitStat(player, STAT_VELOCITYPERCENT, 100, 0); STATLIST_SetUnitStat(player, STAT_OTHER_ANIMRATE, 100, 0);
    g_targetClient = (D2ClientStrc*)calloc(1, sizeof(D2ClientStrc));
    g_targetClient->pGame = g_game; g_targetClient->pPlayer = player;
    g_targetClient->nAct = (uint8_t)g_currentAct;
    // The save header reads the character's identity from the client, not the unit: without
    // a name it refuses to write at all, and the class it stores is this field.
    g_targetClient->nClassId = (uint8_t)g_sessionPlayerClass;
    memset(g_targetClient->szName, 0, sizeof g_targetClient->szName);
    memcpy(g_targetClient->szName, g_sessionCharName, sizeof g_targetClient->szName - 1);
    // Deterministic by design: a wall-clock creation stamp would make every saved file
    // differ from the last and defeat byte comparison in a scenario.
    g_targetClient->nCreateTime = 0;
    for (int hk = 0; hk < 16; hk++) g_targetClient->HotkeySkills[hk].nItemGUID = (uint32_t)-1;
    UNITS_GetPlayerData(player)->pClient = g_targetClient;
    sub_6FC33020(g_targetClient, room);
    for (D2UnitStrc* u = room->pUnitFirst; u; u = u->pRoomNext)
        if (u->dwUnitType == UNIT_MONSTER) MONSTER_UpdateAiCallbackEvent(g_game, u);
    g_targetPlayer = player; g_targetMonster = anchor; g_targetRoom = room; return 1;
}

EMSCRIPTEN_KEEPALIVE int d2_aitarget_attach() {
    if (!g_game || !g_lastLevel || g_targetPlayer) return 0;
    uint16_t roomIdx = 0;
    for (D2DrlgRoomStrc* roomEx = g_lastLevel->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext, roomIdx++) {
        // A room a previous fixture left keeps its (empty) client array; only a live client counts.
        D2ActiveRoomStrc* room = roomEx->pRoom; if (!room || room->nNumClients) continue;
        D2UnitStrc* anchor = nullptr;
        for (D2UnitStrc* u = room->pUnitFirst; u; u = u->pRoomNext)
            if (u->dwUnitType == UNIT_MONSTER && (g_sessionAnchorClass == 0xFFFFFFFF || (uint32_t)u->dwClassId == g_sessionAnchorClass)) { anchor = u; break; }
        if (!anchor || !anchor->pDynamicPath) continue;
        const int ax = anchor->pDynamicPath->tGameCoords.wPosX, ay = anchor->pDynamicPath->tGameCoords.wPosY;
        for (int radius = 2; radius <= 12; radius++) for (int dy = -radius; dy <= radius; dy++) for (int dx = -radius; dx <= radius; dx++) {
            if (abs(dx) != radius && abs(dy) != radius) continue;
            const int x = ax + dx, y = ay + dy;
            if (x < room->tCoords.nSubtileX + 2 || x >= room->tCoords.nSubtileX + room->tCoords.nSubtileWidth - 2 ||
                y < room->tCoords.nSubtileY + 2 || y >= room->tCoords.nSubtileY + room->tCoords.nSubtileHeight - 2) continue;
            if (COLLISION_CheckMaskWithSize(room, x, y, 2, 0xFFFF)) continue;
            if (attachSessionPlayerAt(room, x, y, anchor)) return 1;
        }
    }

    // No room held a monster to anchor to. For the AI and spawn oracles that is the answer -- and
    // for a fixture that asked for a specific anchor class it still is -- but a LEVEL WITH NO
    // MONSTERS IS STILL A LEVEL A PLAYER WALKS INTO. Act 1 level 20, the Forgotten Tower, is one:
    // it generated fine and every caller of d2_session_create was refused, so the level could not
    // be entered, drawn, or compared against retail at all.
    //
    // Levels that do have monsters never reach here, so no existing row's placement or roll
    // sequence moves.
    if (g_sessionAnchorClass != 0xFFFFFFFF) return 0;
    for (D2DrlgRoomStrc* roomEx = g_lastLevel->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext) {
        D2ActiveRoomStrc* room = roomEx->pRoom;
        if (!room || room->nNumClients) continue;
        // Scan outward from the room's middle, so the player lands in open floor rather than
        // against the first wall the iteration order happens to reach.
        const int cx = room->tCoords.nSubtileX + room->tCoords.nSubtileWidth / 2;
        const int cy = room->tCoords.nSubtileY + room->tCoords.nSubtileHeight / 2;
        const int span = (room->tCoords.nSubtileWidth + room->tCoords.nSubtileHeight) / 2;
        for (int radius = 0; radius <= span; radius++) for (int dy = -radius; dy <= radius; dy++) for (int dx = -radius; dx <= radius; dx++) {
            if (radius && abs(dx) != radius && abs(dy) != radius) continue;
            const int x = cx + dx, y = cy + dy;
            if (x < room->tCoords.nSubtileX + 2 || x >= room->tCoords.nSubtileX + room->tCoords.nSubtileWidth - 2 ||
                y < room->tCoords.nSubtileY + 2 || y >= room->tCoords.nSubtileY + room->tCoords.nSubtileHeight - 2) continue;
            if (COLLISION_CheckMaskWithSize(room, x, y, 2, 0xFFFF)) continue;
            if (attachSessionPlayerAt(room, x, y, nullptr)) return 1;
        }
    }
    return 0;
}
EMSCRIPTEN_KEEPALIVE const D2VecAiLevel* d2_aitarget_out() { return &g_targetOut; }
EMSCRIPTEN_KEEPALIVE const D2VecAiTargetUnit* d2_aitarget_units(int* count) { *count = (int)g_targetUnits.size(); return g_targetUnits.data(); }
EMSCRIPTEN_KEEPALIVE const D2VecCombat* d2_combat_events(int* count) { *count = (int)g_combatEvents.size(); return g_combatEvents.data(); }
EMSCRIPTEN_KEEPALIVE const D2VecCombatResult* d2_combat_result() {
    memset(&g_combatResult, 0, sizeof g_combatResult);
    if (!g_targetPlayer) return &g_combatResult;
    D2UnitStrc* u = g_targetPlayer;
    g_combatResult.seed = g_gameSeed; g_combatResult.difficulty = (uint8_t)g_gameDiff;
    g_combatResult.act = (uint8_t)g_currentAct; g_combatResult.level = (uint16_t)g_currentLevel;
    g_combatResult.frame = g_game ? g_game->dwGameFrame : 0;
    g_combatResult.unitType = u->dwUnitType; g_combatResult.unitClass = u->dwClassId;
    g_combatResult.unitId = u->dwUnitId; g_combatResult.mode = u->dwAnimMode;
    g_combatResult.seedLo = u->pSeed.nLowSeed; g_combatResult.seedHi = u->pSeed.nHighSeed;
    g_combatResult.hp = STATLIST_UnitGetStatValue(u, STAT_HITPOINTS, 0);
    g_combatResult.maxHp = STATLIST_UnitGetStatValue(u, STAT_MAXHP, 0);
    g_combatResult.mana = STATLIST_UnitGetStatValue(u, STAT_MANA, 0);
    g_combatResult.maxMana = STATLIST_UnitGetStatValue(u, STAT_MAXMANA, 0);
    g_combatResult.stamina = STATLIST_UnitGetStatValue(u, STAT_STAMINA, 0);
    g_combatResult.maxStamina = STATLIST_UnitGetStatValue(u, STAT_MAXSTAMINA, 0);
    g_combatResult.lastHitClass = u->dwLastHitClass; g_combatResult.flags = u->dwFlags; g_combatResult.flagsEx = u->dwFlagEx;
    return &g_combatResult;
}
EMSCRIPTEN_KEEPALIVE int d2_damagecalc_run(D2VecDamageCalc* v) {
    if (!v || !g_game || !g_targetPlayer || !g_targetMonster) return 0;
    const int statIds[8] = { STAT_NORMAL_DAMAGE_REDUCTION, STAT_MAGIC_DAMAGE_REDUCTION, STAT_DAMAGERESIST,
        STAT_MAGICRESIST, STAT_FIRERESIST, STAT_LIGHTRESIST, STAT_COLDRESIST, STAT_POISONRESIST };
    for (int i = 0; i < 8; ++i) STATLIST_SetUnitStat(g_targetPlayer, statIds[i], v->defenderStats[i], 0);
    g_targetMonster->pSeed.nLowSeed = v->seedLoIn; g_targetMonster->pSeed.nHighSeed = v->seedHiIn;
    D2DamageStrc damage; static_assert(sizeof damage == sizeof v->damageIn); memcpy(&damage, v->damageIn, sizeof damage);
    SUNITDMG_CalculateTotalDamage(g_game, g_targetMonster, g_targetPlayer, &damage);
    v->seedLoOut = g_targetMonster->pSeed.nLowSeed; v->seedHiOut = g_targetMonster->pSeed.nHighSeed;
    memcpy(v->damageOut, &damage, sizeof damage); return 1;
}
EMSCRIPTEN_KEEPALIVE int d2_aiframe_advance(int frames) {
    if (!g_game || !g_lastLevel || frames < 0) return 0;
    for (int i = 0; i < frames; i++) {
        ++g_game->dwGameFrame;
        d2web_AdvanceTickCount(40);
        EVENT_IterateEvents(g_game);
        // The retail game frame runs the quest timers every 20th frame (D2Game.0x6FC3xxxx game
        // loop: QUESTS_QuestUpdater when dwGameFrame % 20 == 0).
        if (!(g_game->dwGameFrame % 20)) QUESTS_QuestUpdater(g_game);
    }
    g_aiUnits.clear();
    g_aiOut.monsters = 0; g_aiOut.hash = D2VEC_FNV_INIT; g_aiOut.reserved = (uint32_t)frames;
    uint16_t roomIdx = 0;
    for (D2DrlgRoomStrc* roomEx = g_lastLevel->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext, roomIdx++) {
        if (!roomEx->pRoom) continue;
        uint16_t monOrdinal = 0;
        for (D2UnitStrc* u = roomEx->pRoom->pUnitFirst; u; u = u->pRoomNext) {
            if (u->dwUnitType != UNIT_MONSTER) continue;
            D2VecAiUnit a = captureAi(u, roomIdx, monOrdinal++);
            g_aiOut.hash = d2vec_ai_hash(g_aiOut.hash, &a); g_aiOut.monsters++; g_aiUnits.push_back(a);
        }
    }
    if (g_targetPlayer) {
        g_targetUnits.clear(); g_targetOut = g_aiOut; g_targetOut.hash = D2VEC_FNV_INIT;
        roomIdx = 0;
        for (D2DrlgRoomStrc* roomEx = g_lastLevel->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext, roomIdx++) {
            if (!roomEx->pRoom) continue; uint16_t monOrdinal = 0;
            for (D2UnitStrc* u = roomEx->pRoom->pUnitFirst; u; u = u->pRoomNext) {
                if (u->dwUnitType != UNIT_MONSTER) continue;
                D2VecAiTargetUnit a = captureAiTarget(u, roomIdx, monOrdinal++);
                g_targetOut.hash = d2vec_aitarget_hash(g_targetOut.hash, &a); g_targetUnits.push_back(a);
            }
        }
    }
    return 1;
}

static void updateSessionSnapshot() {
    memset(&g_sessionSnapshot, 0, sizeof g_sessionSnapshot);
    g_sessionSnapshot.version = 1; g_sessionSnapshot.size = sizeof g_sessionSnapshot;
    updateSessionWorld();
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pDynamicPath) return;
    const D2DynamicPathStrc* path = g_targetPlayer->pDynamicPath;
    g_sessionSnapshot.seed = g_gameSeed; g_sessionSnapshot.frame = g_game->dwGameFrame;
    g_sessionSnapshot.level = (uint16_t)g_currentLevel; g_sessionSnapshot.difficulty = (uint8_t)g_gameDiff;
    g_sessionSnapshot.act = (uint8_t)g_currentAct; g_sessionSnapshot.playerClass = g_targetPlayer->dwClassId;
    g_sessionSnapshot.playerId = g_targetPlayer->dwUnitId;
    g_sessionSnapshot.x = path->tGameCoords.wPosX; g_sessionSnapshot.y = path->tGameCoords.wPosY;
    g_sessionSnapshot.targetX = g_sessionMovePending ? g_sessionTargetX : g_sessionSnapshot.x;
    g_sessionSnapshot.targetY = g_sessionMovePending ? g_sessionTargetY : g_sessionSnapshot.y;
    g_sessionSnapshot.hp = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_HITPOINTS, 0);
    g_sessionSnapshot.maxHp = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_MAXHP, 0);
    g_sessionSnapshot.mode = g_targetPlayer->dwAnimMode; g_sessionSnapshot.flags = g_targetPlayer->dwFlags;
    g_sessionSnapshot.monsters = 0;
    for (const D2SessionUnit& unit : g_sessionUnits) if (unit.type == UNIT_MONSTER) ++g_sessionSnapshot.monsters;
    uint32_t h = D2VEC_FNV_INIT;
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&g_sessionSnapshot);
    for (size_t i = 0; i < offsetof(D2SessionSnapshot, hash); ++i) { h ^= bytes[i]; h *= 16777619u; }
    g_sessionSnapshot.hash = h;
}

// First browser-facing lifetime API.  It deliberately builds on the same game, level and player
// fixture used by the retail-diff scenarios, but keeps them alive across commands and ticks.
static int d2_session_create_for_class(uint32_t seed, int difficulty, int act, int levelId, int playerClass, bool startItems = true) {
    if (difficulty < 0 || difficulty > 2 || act < 0 || act > 4 || levelId < 0 || playerClass < 0 || playerClass >= 7) return 0;
    g_sessionPlayerClass = playerClass;
    d2_monspawn_reset(); g_sessionLive = false; g_sessionMovePending = false;
    memset(&g_sessionAttack, 0, sizeof g_sessionAttack); memset(&g_sessionProgress, 0, sizeof g_sessionProgress);
    memset(&g_sessionAdvancement, 0, sizeof g_sessionAdvancement); g_sessionAdvancementSkillId = SKILL_MAGICARROW;
    memset(&g_sessionItemState, 0, sizeof g_sessionItemState);
    memset(&g_sessionNpcInteraction, 0, sizeof g_sessionNpcInteraction);
    memset(&g_sessionNpcDialog, 0, sizeof g_sessionNpcDialog); D2WEB_ResetLastDialogPacket();
    memset(&g_sessionNpcResponse, 0, sizeof g_sessionNpcResponse);
    memset(&g_sessionVendor, 0, sizeof g_sessionVendor);
    g_sessionVendorItems.clear(); memset(&g_sessionVendorTransaction, 0, sizeof g_sessionVendorTransaction);
    memset(&g_sessionRepair, 0, sizeof g_sessionRepair);
    memset(&g_sessionItemSaveRoundTrip, 0, sizeof g_sessionItemSaveRoundTrip);
    g_sessionItemSaveBytes.clear();
    memset(&g_sessionStatsSaveRoundTrip, 0, sizeof g_sessionStatsSaveRoundTrip);
    g_sessionStatsSaveBytes.clear();
    memset(&g_sessionCampaignSaveRoundTrip, 0, sizeof g_sessionCampaignSaveRoundTrip);
    g_sessionSkillSaveBytes.clear(); g_sessionWaypointSaveBytes.clear();
    memset(&g_sessionQuestIntroSaveRoundTrip, 0, sizeof g_sessionQuestIntroSaveRoundTrip);
    g_sessionQuestSaveBytes.clear(); g_sessionIntroSaveBytes.clear();
    g_sessionAttackTarget = nullptr;
    g_sessionAttackSkill = nullptr; g_sessionAttackCount = 0; g_sessionAttackEndFrame = 0;
    g_sessionAttackResolved = false; g_sessionTargetKillCounted = false; g_sessionKills = 0;
    if (!d2_monspawn_init() || !d2_monspawn_level(seed, difficulty, act, levelId) || !d2_aitarget_attach()) {
        updateSessionSnapshot(); return 0;
    }
    if (D2CharStatsTxt* stats = UNITS_GetCharStatsTxtRecord(g_targetPlayer->dwClassId)) {
        STATLIST_SetUnitStat(g_targetPlayer, STAT_STRENGTH, stats->nStr, 0);
        STATLIST_SetUnitStat(g_targetPlayer, STAT_ENERGY, stats->nInt, 0);
        STATLIST_SetUnitStat(g_targetPlayer, STAT_DEXTERITY, stats->nDex, 0);
        STATLIST_SetUnitStat(g_targetPlayer, STAT_VITALITY, stats->nVit, 0);
        STATLIST_SetUnitStat(g_targetPlayer, STAT_TOHIT, 0, 0);
        STATLIST_SetUnitStat(g_targetPlayer, STAT_TOBLOCK, 0, 0);
        STATLIST_SetUnitStat(g_targetPlayer, STAT_HITPOINTS, (stats->nVit + stats->nLifeAdd) << 8, 0);
        STATLIST_SetUnitStat(g_targetPlayer, STAT_MAXHP, (stats->nVit + stats->nLifeAdd) << 8, 0);
    }
    // The headless save callback creates a bare unit. Retail new-character creation immediately
    // follows it with the CharStats start-item callback (javelin/shield/potions/scrolls for Amazon).
    if (startItems) PLAYER_CreateStartItemsFromCharStatsTxt(g_targetPlayer, g_game, nullptr);
    g_sessionInitialExperience = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_EXPERIENCE, 0);
    g_sessionLive = true; updateSessionSnapshot(); return 1;
}

EMSCRIPTEN_KEEPALIVE int d2_session_create(uint32_t seed, int difficulty, int act, int levelId) {
    return d2_session_create_for_class(seed, difficulty, act, levelId, 0);
}

EMSCRIPTEN_KEEPALIVE int d2_session_create_class(uint32_t seed, int difficulty, int act, int levelId, int playerClass) {
    return d2_session_create_for_class(seed, difficulty, act, levelId, playerClass);
}

// The retail capture's player fixture is a bare allocated unit with no starting kit; unit GUIDs
// and the game item seed the quest-drop oracle records depend on that. Mirror it exactly.
EMSCRIPTEN_KEEPALIVE int d2_session_create_raw(uint32_t seed, int difficulty, int act, int levelId) {
    return d2_session_create_for_class(seed, difficulty, act, levelId, 0, false);
}
// The same bare fixture placed beside the first monster of the given class (a town NPC).
EMSCRIPTEN_KEEPALIVE int d2_session_create_raw_near(uint32_t seed, int difficulty, int act, int levelId, uint32_t anchorClass) {
    g_sessionAnchorClass = anchorClass;
    const int ok = d2_session_create_for_class(seed, difficulty, act, levelId, 0, false);
    g_sessionAnchorClass = 0xFFFFFFFF;
    return ok;
}

// Could the player stand on this subtile? Retail's own collision test, with the player's unit size
// and the player-path mask -- the same call d2_session_command_move's handler ends up making.
//
// Why the client needs this: the walk handler above enforces retail's rule that a walk target is
// within 50 subtiles, and the server paths AROUND obstacles between here and there. So a long walk
// is a sequence of short walk commands -- but each one's target must itself be somewhere the player
// could stand. Marching in a straight line toward a destination puts intermediate targets inside
// tents and fences, and the handler rejects them, which is why walking across town stalled.
EMSCRIPTEN_KEEPALIVE int d2_session_walkable(int x, int y) {
    if (!g_sessionLive || !g_targetPlayer || !g_targetPlayer->pDynamicPath) return 0;
    D2ActiveRoomStrc* room = COLLISION_GetRoomBySubTileCoordinates(g_targetPlayer->pDynamicPath->pRoom, x, y);
    if (!room) return 0;
    return COLLISION_CheckMaskWithSize(room, x, y, UNITS_GetUnitSizeX(g_targetPlayer), COLLIDE_MASK_PLAYER_PATH) ? 0 : 1;
}

// Why the last walk command was refused. 0 = accepted; see d2_session_move_reason.
// A walk that is simply rejected is indistinguishable from one that was never sent, and chasing
// that difference by guesswork cost real time.
static int g_sessionMoveReason = 0;
enum {
    MOVE_OK = 0, MOVE_NO_SESSION = 1, MOVE_OUT_OF_RANGE = 2, MOVE_NO_ROOM = 3,
    MOVE_HANDLER_REFUSED = 4, MOVE_NO_PATH = 5,
};
EMSCRIPTEN_KEEPALIVE int d2_session_move_reason() { return g_sessionMoveReason; }

EMSCRIPTEN_KEEPALIVE int d2_session_command_move(int targetX, int targetY) {
    g_sessionMoveReason = MOVE_NO_SESSION;
    if (!g_sessionLive || !g_targetPlayer || !g_targetPlayer->pDynamicPath) return 0;
    const D2DynamicPathStrc* path = g_targetPlayer->pDynamicPath;
    // PlrMsg's retail 0x01 validator accepts each coordinate within 50 subtiles.
    g_sessionMoveReason = MOVE_OUT_OF_RANGE;
    if (abs(targetX - (int)path->tGameCoords.wPosX) > 50 || abs(targetY - (int)path->tGameCoords.wPosY) > 50) return 0;
    g_sessionMoveReason = MOVE_NO_ROOM;
    D2ActiveRoomStrc* targetRoom = COLLISION_GetRoomBySubTileCoordinates(path->pRoom, targetX, targetY);
    if (!targetRoom) return 0;
    g_sessionTargetX = targetX; g_sessionTargetY = targetY;
    uint8_t packet[5] = { 0x01, 0, 0, 0, 0 };
    const uint16_t x = (uint16_t)targetX, y = (uint16_t)targetY;
    memcpy(packet + 1, &x, sizeof x); memcpy(packet + 3, &y, sizeof y);
    g_sessionMoveReason = MOVE_HANDLER_REFUSED;
    if (D2GAME_PACKETCALLBACK_Rcv0x01_Walk_6FC82CB0(g_game, g_targetPlayer, packet, sizeof packet)) return 0;
    g_sessionMovePending = path->dwPathPoints > 0;
    // The handler accepted the packet but produced no path: the destination is unreachable from
    // here, which is a different failure from "the packet was invalid".
    g_sessionMoveReason = g_sessionMovePending ? MOVE_OK : MOVE_NO_PATH;
    updateSessionSnapshot(); return g_sessionMovePending ? 1 : 0;
}

// After LEVEL_WarpUnit (an exit, or a portal object's operate function) moved the player, adopt
// the resulting room and level. Retail's exit operation invokes the quest level-change dispatcher
// after its successful room transfer; LEVEL_WarpUnit only moves the unit, so that recovered
// second half lives here.
static int adoptSessionLevelChange(int32_t oldLevel, int32_t destinationLevel) {
    D2ActiveRoomStrc* room = UNITS_GetRoom(g_targetPlayer);
    if (!room || DUNGEON_GetLevelIdFromRoom(room) != destinationLevel || !room->pDrlgRoom || !room->pDrlgRoom->pLevel) return 0;
    g_currentAct = DRLG_GetActNoFromLevelId(destinationLevel);
    g_currentLevel = destinationLevel; g_lastLevel = room->pDrlgRoom->pLevel;
    QUESTS_ChangeLevel(g_game, oldLevel, destinationLevel, g_targetPlayer);
    activateSessionLevelRooms(g_game->pAct[g_currentAct], g_lastLevel, destinationLevel);
    g_targetRoom = room; g_targetMonster = nullptr; g_sessionMovePending = false;
    g_sessionAttackTarget = nullptr; g_sessionAttackSkill = nullptr; memset(&g_sessionAttack, 0, sizeof g_sessionAttack);
    updateSessionSnapshot(); return 1;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_warp(uint32_t destinationLevel, uint32_t warpId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetClient) return 0;
    updateSessionSnapshot();
    bool generatedExit = false;
    for (const D2SessionWarp& warp : g_sessionWarps) {
        if (warp.destinationLevel == destinationLevel && warp.warpId == warpId && warp.enabled) {
            generatedExit = true; break;
        }
    }
    if (!generatedExit) return 0;
    const int32_t oldLevel = DUNGEON_GetLevelIdFromRoom(UNITS_GetRoom(g_targetPlayer));
    LEVEL_WarpUnit(g_game, g_targetPlayer, (int32_t)destinationLevel, (int32_t)warpId);
    return adoptSessionLevelChange(oldLevel, (int32_t)destinationLevel);
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_nearest_warp(int radius) {
    if (!g_sessionLive || !g_targetPlayer || !g_targetPlayer->pDynamicPath || radius < 0 || radius > 6) return 0;
    updateSessionSnapshot();
    const int x = g_targetPlayer->pDynamicPath->tGameCoords.wPosX;
    const int y = g_targetPlayer->pDynamicPath->tGameCoords.wPosY;
    const D2SessionWarp* best = nullptr; int bestDistance = radius + 1;
    for (const D2SessionWarp& warp : g_sessionWarps) {
        if (!warp.enabled || warp.room == UINT32_MAX) continue;
        const int distance = std::max(abs(warp.exitX - x), abs(warp.exitY - y));
        if (distance < bestDistance || (distance == bestDistance && best && warp.slot < best->slot)) {
            best = &warp; bestDistance = distance;
        }
    }
    if (!best || bestDistance > radius) return 0;
    const uint32_t destination = best->destinationLevel, warpId = best->warpId;
    return d2_session_command_warp(destination, warpId);
}

// The current level's link table as DRLG holds it after generation: Levels.txt Vis/Warp plus
// the outdoor links DRLG_SetWarpId wrote while placing the act. Eight {vis, warp} pairs; warp -1
// is a seam (continuous terrain, walked), otherwise a generated exit.
static int32_t g_sessionLevelLinks[16];
EMSCRIPTEN_KEEPALIVE const int32_t* d2_session_level_links() {
    memset(g_sessionLevelLinks, 0, sizeof g_sessionLevelLinks);
    if (!g_sessionLive || !g_lastLevel) return g_sessionLevelLinks;
    D2DrlgWarpStrc* dynamicWarps = DRLG_GetDrlgWarpFromLevelId(g_lastLevel->pDrlg, g_currentLevel);
    const D2LevelDefBin* levelDef = DATATBLS_GetLevelDefRecord(g_currentLevel);
    for (uint32_t slot = 0; slot < 8; ++slot) {
        g_sessionLevelLinks[2 * slot] = dynamicWarps ? dynamicWarps->nVis[slot] : levelDef->dwVis[slot];
        g_sessionLevelLinks[2 * slot + 1] = dynamicWarps ? dynamicWarps->nWarp[slot] : (int32_t)levelDef->dwWarp[slot];
    }
    return g_sessionLevelLinks;
}

// Why an object operate may be refused (OBJECTS_OperateHandler's own guard): {interact unit type
// (-1 none), its GUID, bBusy, cursor item GUID (0 none)}. Read-only.
static int32_t g_sessionPlayerBusy[4];
EMSCRIPTEN_KEEPALIVE const int32_t* d2_session_player_busy() {
    g_sessionPlayerBusy[0] = -1; g_sessionPlayerBusy[1] = -1; g_sessionPlayerBusy[2] = 0; g_sessionPlayerBusy[3] = 0;
    if (!g_sessionLive || !g_targetPlayer) return g_sessionPlayerBusy;
    int32_t type = -1, guid = -1;
    if (SUNIT_GetInteractInfo(g_targetPlayer, &type, &guid)) { g_sessionPlayerBusy[0] = type; g_sessionPlayerBusy[1] = guid; }
    g_sessionPlayerBusy[2] = g_targetPlayer->pPlayerData ? g_targetPlayer->pPlayerData->bBusy : 0;
    D2UnitStrc* cursor = g_targetPlayer->pInventory ? INVENTORY_GetCursorItem(g_targetPlayer->pInventory) : nullptr;
    g_sessionPlayerBusy[3] = cursor ? (int32_t)cursor->dwUnitId : 0;
    return g_sessionPlayerBusy;
}

// 1 if an ACTIVE room (one with a collision grid) covers this subtile, else 0. d2_session_walkable
// cannot tell a wall from ground whose room is not active yet; a player exploring can -- the
// automap shows where they have not been. Read-only.
EMSCRIPTEN_KEEPALIVE int d2_session_room_active_at(int x, int y) {
    if (!g_sessionLive || !g_targetPlayer || !g_targetPlayer->pDynamicPath) return 0;
    return COLLISION_GetRoomBySubTileCoordinates(g_targetPlayer->pDynamicPath->pRoom, x, y) ? 1 : 0;
}

// A level's rectangle in subtiles as the act's DRLG placed it -- what the automap shows a player
// of where the next area lies. Read-only; {x, y, width, height}, zeros if the level is not in the
// current act.
static int32_t g_sessionLevelRect[4];
EMSCRIPTEN_KEEPALIVE const int32_t* d2_session_level_rect(int32_t levelId) {
    memset(g_sessionLevelRect, 0, sizeof g_sessionLevelRect);
    if (!g_sessionLive || !g_lastLevel || !g_lastLevel->pDrlg) return g_sessionLevelRect;
    D2DrlgLevelStrc* level = DRLG_GetLevel(g_lastLevel->pDrlg, levelId);
    if (!level) return g_sessionLevelRect;
    g_sessionLevelRect[0] = level->nPosX * 5; g_sessionLevelRect[1] = level->nPosY * 5;
    g_sessionLevelRect[2] = level->nWidth * 5; g_sessionLevelRect[3] = level->nHeight * 5;
    return g_sessionLevelRect;
}

// Where this level's rooms share an edge with rooms of `destinationLevel` (a seam), as up to 32
// segments {x0, y0, x1, y1} in subtiles -- the DRLG's own room adjacency, what the automap shows.
// Read-only: unlike d2_session_command_seam it activates nothing. Returns the segment count.
static int32_t g_sessionSeamEdges[32 * 4];
EMSCRIPTEN_KEEPALIVE int d2_session_seam_edges(uint32_t destinationLevel) {
    int count = 0;
    if (!g_sessionLive || !g_targetPlayer) return 0;
    // The level the player is standing in -- a walk across a seam does not refresh g_lastLevel.
    D2ActiveRoomStrc* here = UNITS_GetRoom(g_targetPlayer);
    D2DrlgLevelStrc* level = here && here->pDrlgRoom ? here->pDrlgRoom->pLevel : g_lastLevel;
    if (!level) return 0;
    for (D2DrlgRoomStrc* roomEx = level->pFirstRoomEx; roomEx && count < 32; roomEx = roomEx->pDrlgRoomNext) {
        for (int32_t n = 0; n < roomEx->nRoomsNear && count < 32; ++n) {
            D2DrlgRoomStrc* near = roomEx->ppRoomsNear[n];
            if (!near || !near->pLevel || near->pLevel->nLevelId != (int32_t)destinationLevel) continue;
            int32_t dx = 0, dy = 0;
            if (near->nTileXPos == roomEx->nTileXPos + roomEx->nTileWidth) dx = 1;
            else if (near->nTileXPos + near->nTileWidth == roomEx->nTileXPos) dx = -1;
            else if (near->nTileYPos == roomEx->nTileYPos + roomEx->nTileHeight) dy = 1;
            else if (near->nTileYPos + near->nTileHeight == roomEx->nTileYPos) dy = -1;
            else continue;
            const int32_t lo = dx ? std::max(roomEx->nTileYPos, near->nTileYPos) : std::max(roomEx->nTileXPos, near->nTileXPos);
            const int32_t hi = dx ? std::min(roomEx->nTileYPos + roomEx->nTileHeight, near->nTileYPos + near->nTileHeight)
                                  : std::min(roomEx->nTileXPos + roomEx->nTileWidth, near->nTileXPos + near->nTileWidth);
            if (lo >= hi) continue;
            const int32_t line = dx > 0 ? 5 * (roomEx->nTileXPos + roomEx->nTileWidth) : dx < 0 ? 5 * roomEx->nTileXPos :
                                 dy > 0 ? 5 * (roomEx->nTileYPos + roomEx->nTileHeight) : 5 * roomEx->nTileYPos;
            int32_t* e = &g_sessionSeamEdges[4 * count++];
            e[0] = dx ? line : 5 * lo; e[1] = dx ? 5 * lo : line; e[2] = dx ? line : 5 * hi; e[3] = dx ? 5 * hi : line;
        }
    }
    return count;
}
EMSCRIPTEN_KEEPALIVE const int32_t* d2_session_seam_edge_data() { return g_sessionSeamEdges; }

// Levels.txt links with Warp -1 are seams: the two levels are continuous generated terrain and
// the player simply walks across (s60). The route driver needs that crossing for any adjacent
// pair, so this command finds the first passable subtile pair across the shared edge of two
// neighbouring rooms, places the player on the source side by the recovered teleport path
// (sub_6FCBDFE0, the town-portal/teleport placement) and crosses with a retail 0x01 walk; the
// per-frame client update (sub_6FC33670) then performs the level change exactly as for a walk.
EMSCRIPTEN_KEEPALIVE int d2_session_tick(int frames);
EMSCRIPTEN_KEEPALIVE int d2_session_command_object(uint32_t objectId);
EMSCRIPTEN_KEEPALIVE int d2_session_command_seam(uint32_t destinationLevel) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pDynamicPath || !g_lastLevel) return 0;
    updateSessionSnapshot();
    D2DrlgWarpStrc* dynamicWarps = DRLG_GetDrlgWarpFromLevelId(g_lastLevel->pDrlg, g_currentLevel);
    const D2LevelDefBin* levelDef = DATATBLS_GetLevelDefRecord(g_currentLevel);
    bool seam = false;
    for (uint32_t slot = 0; slot < 8 && !seam; ++slot) {
        const int32_t destination = dynamicWarps ? dynamicWarps->nVis[slot] : levelDef->dwVis[slot];
        const int32_t warpId = dynamicWarps ? dynamicWarps->nWarp[slot] : (int32_t)levelDef->dwWarp[slot];
        seam = destination == (int32_t)destinationLevel && warpId < 0;
    }
    if (!seam) return 0;
    D2DrlgActStrc* act = g_game->pAct[g_currentAct];
    const int32_t oldLevel = g_currentLevel;
    for (D2DrlgRoomStrc* roomEx = g_lastLevel->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext) {
        for (int32_t n = 0; n < roomEx->nRoomsNear; ++n) {
            D2DrlgRoomStrc* near = roomEx->ppRoomsNear[n];
            if (!near || !near->pLevel || near->pLevel->nLevelId != (int32_t)destinationLevel) continue;
            // Shared edge in tile space: the neighbour touches one side of this room.
            int32_t dx = 0, dy = 0;
            if (near->nTileXPos == roomEx->nTileXPos + roomEx->nTileWidth) dx = 1;
            else if (near->nTileXPos + near->nTileWidth == roomEx->nTileXPos) dx = -1;
            else if (near->nTileYPos == roomEx->nTileYPos + roomEx->nTileHeight) dy = 1;
            else if (near->nTileYPos + near->nTileHeight == roomEx->nTileYPos) dy = -1;
            else continue;
            const int32_t lo = dx ? std::max(roomEx->nTileYPos, near->nTileYPos) : std::max(roomEx->nTileXPos, near->nTileXPos);
            const int32_t hi = dx ? std::min(roomEx->nTileYPos + roomEx->nTileHeight, near->nTileYPos + near->nTileHeight)
                                  : std::min(roomEx->nTileXPos + roomEx->nTileWidth, near->nTileXPos + near->nTileWidth);
            if (lo >= hi) continue;
            if (!roomEx->pRoom) DUNGEON_SetClientIsInSight(act, oldLevel, roomEx->nTileXPos, roomEx->nTileYPos, nullptr);
            if (!near->pRoom) DUNGEON_SetClientIsInSight(act, (int32_t)destinationLevel, near->nTileXPos, near->nTileYPos, nullptr);
            if (!roomEx->pRoom || !near->pRoom) continue;
            const int32_t srcLine = dx > 0 ? 5 * (roomEx->nTileXPos + roomEx->nTileWidth) - 1 : dx < 0 ? 5 * roomEx->nTileXPos :
                                    dy > 0 ? 5 * (roomEx->nTileYPos + roomEx->nTileHeight) - 1 : 5 * roomEx->nTileYPos;
            const int32_t dstLine = srcLine + (dx ? dx : dy);
            for (int32_t along = 5 * lo; along < 5 * hi; ++along) {
                const int32_t srcX = dx ? srcLine : along, srcY = dx ? along : srcLine;
                const int32_t dstX = dx ? dstLine : along, dstY = dx ? along : dstLine;
                // A closed door across the seam (the monastery gates) is opened as the player would:
                // by operating the door object once it is in interact range.
                const uint16_t srcMask = COLLISION_CheckMaskWithSize(roomEx->pRoom, srcX, srcY, UNITS_GetUnitSizeX(g_targetPlayer), COLLIDE_MASK_PLAYER_PATH);
                const uint16_t dstMask = COLLISION_CheckMaskWithSize(near->pRoom, dstX, dstY, UNITS_GetUnitSizeX(g_targetPlayer), COLLIDE_MASK_PLAYER_PATH);
                if ((srcMask | dstMask) & ~COLLIDE_DOOR) continue;
                int32_t standX = srcX, standY = srcY;
                if ((srcMask | dstMask) & COLLIDE_DOOR) {
                    // Stand back from the door on the source side, as close as the player's footprint allows.
                    int32_t back = 1;
                    for (; back <= 4; ++back) {
                        standX = srcX - back * dx; standY = srcY - back * dy;
                        if (!COLLISION_CheckMaskWithSize(roomEx->pRoom, standX, standY, UNITS_GetUnitSizeX(g_targetPlayer), COLLIDE_MASK_PLAYER_PATH)) break;
                    }
                    if (back > 4) continue;
                }
                if (!sub_6FCBDFE0(g_game, g_targetPlayer, roomEx->pRoom, standX, standY, 1, 0)) continue;
                g_sessionMovePending = false;
                d2_session_tick(1);
                if ((srcMask | dstMask) & COLLIDE_DOOR) {
                    D2UnitStrc* door = nullptr; int32_t doorDistance = INT32_MAX;
                    for (D2ActiveRoomStrc* room : { roomEx->pRoom, near->pRoom }) {
                        for (D2UnitStrc* unit = room->pUnitFirst; unit; unit = unit->pRoomNext) {
                            if (unit->dwUnitType != UNIT_OBJECT || !unit->pObjectData || !unit->pObjectData->pObjectTxt ||
                                !unit->pObjectData->pObjectTxt->nIsDoor || !unit->pStaticPath) continue;
                            const int32_t distance = abs(unit->pStaticPath->tGameCoords.nX - dstX) + abs(unit->pStaticPath->tGameCoords.nY - dstY);
                            if (distance < doorDistance) { doorDistance = distance; door = unit; }
                        }
                    }
                    if (!door || !d2_session_command_object(door->dwUnitId)) continue;
                    d2_session_tick(32);
                    if (COLLISION_CheckMaskWithSize(roomEx->pRoom, srcX, srcY, UNITS_GetUnitSizeX(g_targetPlayer), COLLIDE_MASK_PLAYER_PATH) ||
                        COLLISION_CheckMaskWithSize(near->pRoom, dstX, dstY, UNITS_GetUnitSizeX(g_targetPlayer), COLLIDE_MASK_PLAYER_PATH)) continue;
                }
                if (!d2_session_command_move(dstX, dstY)) continue;
                for (int frame = 0; frame < 64 && g_sessionMovePending; ++frame) d2_session_tick(1);
                syncSessionLevelFromPlayer();
                if (g_currentLevel != (int32_t)destinationLevel) continue;
                activateSessionLevelRooms(act, g_lastLevel, g_currentLevel);
                g_targetRoom = UNITS_GetRoom(g_targetPlayer); g_targetMonster = nullptr;
                g_sessionAttackTarget = nullptr; g_sessionAttackSkill = nullptr; memset(&g_sessionAttack, 0, sizeof g_sessionAttack);
                updateSessionSnapshot(); return 1;
            }
        }
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_object(uint32_t objectId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return 0;
    D2UnitStrc* object = SUNIT_GetServerUnit(g_game, UNIT_OBJECT, (int32_t)objectId);
    if (!object || !object->pObjectData || !object->pObjectData->pObjectTxt ||
        !object->pObjectData->pObjectTxt->nOperateFn || !UNITS_IsObjectInInteractRange(g_targetPlayer, object)) return 0;
    const uint32_t before = object->dwAnimMode;
    const int32_t oldLevel = DUNGEON_GetLevelIdFromRoom(UNITS_GetRoom(g_targetPlayer));
    int32_t result = 0;
    const int32_t operated = OBJECTS_OperateHandler(g_game, g_targetPlayer, UNIT_OBJECT, (int32_t)objectId, &result);
    // Portal objects (Arcane Sanctuary, Canyon of the Magi, the red portals) warp the player
    // through LEVEL_WarpUnit or the teleport placement without changing their own mode, and the
    // town-portal operate function returns 0 after a successful transfer.
    const int32_t newLevel = DUNGEON_GetLevelIdFromRoom(UNITS_GetRoom(g_targetPlayer));
    if (newLevel != oldLevel) return adoptSessionLevelChange(oldLevel, newLevel);
    if (!operated) return 0;
    updateSessionSnapshot();
    return object->dwAnimMode != before ? 1 : 0;
}

static void updateSessionNpcInteraction(uint32_t npcId) {
    const uint32_t commandTarget = g_sessionNpcInteraction.commandTarget;
    const uint32_t result = g_sessionNpcInteraction.result;
    memset(&g_sessionNpcInteraction, 0, sizeof g_sessionNpcInteraction);
    g_sessionNpcInteraction.version = 1; g_sessionNpcInteraction.size = sizeof g_sessionNpcInteraction;
    g_sessionNpcInteraction.commandTarget = commandTarget; g_sessionNpcInteraction.result = result;
    if (!g_sessionLive || !g_game || !g_targetPlayer) return;
    int32_t interactType = -1, interactGuid = -1;
    if (SUNIT_GetInteractInfo(g_targetPlayer, &interactType, &interactGuid)) {
        g_sessionNpcInteraction.interactType = interactType;
        g_sessionNpcInteraction.interactGuid = interactGuid;
        g_sessionNpcInteraction.flags |= 4;
    } else {
        g_sessionNpcInteraction.interactType = -1;
        g_sessionNpcInteraction.interactGuid = -1;
    }
    D2UnitStrc* npc = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)npcId);
    if (npc) {
        g_sessionNpcInteraction.npcId = npc->dwUnitId;
        g_sessionNpcInteraction.npcClass = npc->dwClassId;
        g_sessionNpcInteraction.distance = D2Common_10399(g_targetPlayer, npc);
        D2MonsterInteractStrc* monInteract = npc->pMonsterData ? npc->pMonsterData->pMonInteract : nullptr;
        if (monInteract) {
            g_sessionNpcInteraction.flags |= 1;
            g_sessionNpcInteraction.npcInteractCount = MONSTERAI_GetInteractUnitCount(monInteract);
            g_sessionNpcInteraction.npcHasPlayer = MONSTERAI_IsInteractingWith(monInteract, g_targetPlayer);
        }
    }
    if (PLAYER_IsBusy(g_targetPlayer)) g_sessionNpcInteraction.flags |= 2;
    g_sessionNpcInteraction.hash = sessionHashBytes(D2VEC_FNV_INIT, &g_sessionNpcInteraction,
        offsetof(D2SessionNpcInteraction, hash));
}

// Recovered UNIT_MONSTER branch of PlrMsg's interaction dispatcher: the click is accepted at
// six subtiles, then D2Game owns the player/NPC interaction records and quest speech activation.
extern "C" void D2WEB_ResetPacketStream();
extern "C" const uint8_t* D2WEB_PacketStream(uint32_t* pLength, uint32_t* pPackets);
static uint32_t g_sessionPacketStreamLength, g_sessionPacketStreamPackets;
// Client packet 0x13 (interact with entity) through the recovered handler, exactly as a client
// sends it: within 6 subtiles the NPC activates immediately and its reply is queued.
// Fixture: place the player at (x,y) via the recovered teleport placement, finding the active
// room that contains (x,y). Used to bring the player to Baal's throne so the crab-stairs form's
// AITHINK (AiFunction03 case 1) acquires a target and runs AITHINK_Fn138_BaalToStairs.
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_place_player(int32_t x, int32_t y) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pDynamicPath) return 0;
    D2ActiveRoomStrc* dest = nullptr;
    for (D2DrlgRoomStrc* roomEx = g_lastLevel ? g_lastLevel->pFirstRoomEx : nullptr; roomEx; roomEx = roomEx->pDrlgRoomNext)
        if (roomEx->pRoom) { D2ActiveRoomStrc* r = COLLISION_GetRoomBySubTileCoordinates(roomEx->pRoom, x, y); if (r) { dest = r; break; } }
    if (!sub_6FCBDFE0(g_game, g_targetPlayer, dest, x, y, 0, 0)) return 0;
    g_sessionMovePending = false;
    updateSessionSnapshot();
    return 1;
}
EMSCRIPTEN_KEEPALIVE int d2_session_command_npc_packet(uint32_t npcId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return -1;
    D2WEB_ResetPacketStream();
    uint8_t packet[9] = { 0x13 };
    const uint32_t type = UNIT_MONSTER; memcpy(packet + 1, &type, 4); memcpy(packet + 5, &npcId, 4);
    const int32_t result = D2GAME_PACKETCALLBACK_Rcv0x13_InteractWithEntity_6FC84D70(g_game, g_targetPlayer, packet, sizeof packet);
    D2WEB_PacketStream(&g_sessionPacketStreamLength, &g_sessionPacketStreamPackets);
    updateSessionSnapshot();
    return result;
}
// Client packet 0x31 {npcGUID, messageId} through the recovered handler: the player picked one
// of the dialogue's messages (quest starts, intro acknowledgements).
EMSCRIPTEN_KEEPALIVE int d2_session_command_npc_response_packet(uint32_t npcId, uint32_t messageId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || messageId > UINT16_MAX) return -1;
    D2WEB_ResetPacketStream();
    uint8_t packet[9] = { 0x31 };
    const uint16_t message = (uint16_t)messageId;
    memcpy(packet + 1, &npcId, 4); memcpy(packet + 5, &message, 2);
    const int32_t result = D2GAME_PACKETCALLBACK_Rcv0x31_QuestMessage_6FC86AB0(g_game, g_targetPlayer, packet, sizeof packet);
    D2WEB_PacketStream(&g_sessionPacketStreamLength, &g_sessionPacketStreamPackets);
    updateSessionSnapshot();
    return result;
}
// The retail capture visits a town's NPCs with one bare fixture per monster class, in room
// order, leaving each previous player unit in the world: unit GUIDs and the game item seed the
// vendor stock depends on follow that sequence.  Replay it: detach the current fixture (room
// client list cleared, unit left behind) and allocate the next one beside the given class.
EMSCRIPTEN_KEEPALIVE int d2_session_reattach_near(uint32_t anchorClass) {
    if (!g_sessionLive || !g_game) return 0;
    // The fixture joined its room and the adjacent ones through the recovered client-room path;
    // leave them the same way so the next room scan sees them client-free.
    if (g_targetClient) sub_6FC33020(g_targetClient, nullptr);
    detachTargetFixture();
    g_sessionAnchorClass = anchorClass;
    const int ok = d2_aitarget_attach();
    g_sessionAnchorClass = 0xFFFFFFFF;
    updateSessionSnapshot();
    return ok;
}
// Distinct monster classes of the session level in room order -- the capture's probe order.
static std::vector<uint32_t> g_sessionTownClasses;
EMSCRIPTEN_KEEPALIVE const uint32_t* d2_session_town_classes(uint32_t* count) {
    g_sessionTownClasses.clear();
    if (g_sessionLive && g_lastLevel)
        for (D2DrlgRoomStrc* roomEx = g_lastLevel->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext) {
            if (!roomEx->pRoom) continue;
            for (D2UnitStrc* u = roomEx->pRoom->pUnitFirst; u; u = u->pRoomNext)
                if (u->dwUnitType == UNIT_MONSTER && std::find(g_sessionTownClasses.begin(), g_sessionTownClasses.end(), (uint32_t)u->dwClassId) == g_sessionTownClasses.end())
                    g_sessionTownClasses.push_back((uint32_t)u->dwClassId);
        }
    if (count) *count = (uint32_t)g_sessionTownClasses.size();
    return g_sessionTownClasses.data();
}
// Client packet 0x38 {action, npcGUID, -1} through the recovered handler (trade/gamble/hire).
EMSCRIPTEN_KEEPALIVE int d2_session_command_npc_action_packet(uint32_t npcId, uint32_t action) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return -1;
    D2WEB_ResetPacketStream();
    D2GSPacketClt38 packet = {};
    packet.nHeader = 0x38; packet.nAction = action; packet.nNpcGUID = (int32_t)npcId; packet.nItemGUID = -1;
    const int32_t oldLevel = DUNGEON_GetLevelIdFromRoom(UNITS_GetRoom(g_targetPlayer));
    const int32_t result = D2GAME_PACKETCALLBACK_Rcv0x38_EntityAction_6FC86CB0(g_game, g_targetPlayer, &packet, sizeof packet);
    g_sessionVendor.action = action; g_sessionVendor.npcId = npcId;   // the transaction commands key on the open action
    D2WEB_PacketStream(&g_sessionPacketStreamLength, &g_sessionPacketStreamPackets);
    // A caravan/travel action moved the player into another act's town: the session follows
    // the unit the way the exit command does (level bookkeeping, quest level-change dispatch,
    // destination room activation).
    D2ActiveRoomStrc* room = UNITS_GetRoom(g_targetPlayer);
    if (room && room->pDrlgRoom && room->pDrlgRoom->pLevel && DUNGEON_GetLevelIdFromRoom(room) != oldLevel) {
        const int32_t newLevel = DUNGEON_GetLevelIdFromRoom(room);
        g_currentAct = DRLG_GetActNoFromLevelId(newLevel); g_currentLevel = newLevel; g_lastLevel = room->pDrlgRoom->pLevel;
        QUESTS_ChangeLevel(g_game, oldLevel, newLevel, g_targetPlayer);
        activateSessionLevelRooms(g_game->pAct[g_currentAct], g_lastLevel, newLevel);
        g_targetRoom = room; g_targetMonster = nullptr; g_sessionMovePending = false;
        g_sessionAttackTarget = nullptr; g_sessionAttackSkill = nullptr; memset(&g_sessionAttack, 0, sizeof g_sessionAttack);
    }
    updateSessionSnapshot();
    return result;
}
// The nearest caravan NPC (Warriv, Meshif, Tyrael, Cain's Harrogath portal) within a radius,
// or 0: the browser's travel key.
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_nearest_travel_npc(int radius) {
    if (!g_sessionLive || !g_targetPlayer || !g_targetPlayer->pDynamicPath) return 0;
    updateSessionSnapshot();
    static const uint32_t kTravelNpcs[] = { 155, 175, 210, 264, 367, 476 };
    const int x = g_targetPlayer->pDynamicPath->tGameCoords.wPosX, y = g_targetPlayer->pDynamicPath->tGameCoords.wPosY;
    uint32_t best = 0; int bestDistance = radius + 1;
    for (const D2SessionUnit& unit : g_sessionUnits) {
        if (unit.type != UNIT_MONSTER) continue;
        bool travel = false; for (uint32_t cls : kTravelNpcs) travel |= unit.classId == cls;
        if (!travel) continue;
        const int d = std::max(abs(unit.x - x), abs(unit.y - y));
        if (d < bestDistance || (d == bestDistance && best && unit.id < best)) { best = unit.id; bestDistance = d; }
    }
    return best;
}
// Talk to the caravan NPC and take the trip (the dialogue's travel choice).
EMSCRIPTEN_KEEPALIVE int d2_session_command_travel(uint32_t npcId) {
    if (!g_sessionLive || !g_targetPlayer) return 0;
    if (d2_session_command_npc_packet(npcId) != 0) return 0;
    const int32_t before = DUNGEON_GetLevelIdFromRoom(UNITS_GetRoom(g_targetPlayer));
    d2_session_command_npc_action_packet(npcId, 0);
    return DUNGEON_GetLevelIdFromRoom(UNITS_GetRoom(g_targetPlayer)) != before;
}
// A unit's inventory items in list order with their ITEMS_Serialize bitstreams (vendor stock).
struct D2SessionInventoryItem {
    uint32_t itemId, classId, quality, ilvl, seedLo, seedHi, bitBytes;
    uint8_t bits[80];
};
static_assert(sizeof(D2SessionInventoryItem) == 108);
static std::vector<D2SessionInventoryItem> g_sessionInventoryItems;
EMSCRIPTEN_KEEPALIVE const D2SessionInventoryItem* d2_session_inventory_items(uint32_t unitType, uint32_t unitId, uint32_t* count) {
    g_sessionInventoryItems.clear();
    D2UnitStrc* unit = (g_sessionLive && g_game) ? SUNIT_GetServerUnit(g_game, (int32_t)unitType, (int32_t)unitId) : nullptr;
    if (unit && unit->pInventory)
        for (D2UnitStrc* item = INVENTORY_GetFirstItem(unit->pInventory); item; item = INVENTORY_GetNextItem(item)) {
            if (!INVENTORY_UnitIsItem(item)) continue;
            D2SessionInventoryItem out = {};
            out.itemId = item->dwUnitId; out.classId = item->dwClassId;
            out.quality = (uint32_t)ITEMS_GetItemQuality(item); out.ilvl = (uint32_t)ITEMS_GetItemLevel(item);
            out.seedLo = item->pSeed.nLowSeed; out.seedHi = item->pSeed.nHighSeed;
            uint8_t bits[512] = {};
            out.bitBytes = (uint32_t)ITEMS_SerializeItemToBitstream(item, bits, sizeof bits, 1, 1, 0);
            memcpy(out.bits, bits, out.bitBytes < sizeof out.bits ? out.bitBytes : sizeof out.bits);
            g_sessionInventoryItems.push_back(out);
        }
    if (count) *count = (uint32_t)g_sessionInventoryItems.size();
    return g_sessionInventoryItems.data();
}
// Every item unit of the game with a GUID above minId, in GUID order (the capture walks
// pUnitList[3] the same way): what a gamble request generated.  Also reports the highest GUID.
EMSCRIPTEN_KEEPALIVE const D2SessionInventoryItem* d2_session_item_units_above(uint32_t minId, uint32_t* count, uint32_t* maxId) {
    g_sessionInventoryItems.clear();
    std::vector<D2UnitStrc*> units; uint32_t highest = 0;
    if (g_sessionLive && g_game)
        for (int b = 0; b < 128; ++b)
            for (D2UnitStrc* u = g_game->pUnitList[GAME_RemapUnitTypeToListIndex(UNIT_ITEM)][b]; u; u = u->pListNext) {
                if ((uint32_t)u->dwUnitId > highest) highest = (uint32_t)u->dwUnitId;
                if ((uint32_t)u->dwUnitId > minId) units.push_back(u);
            }
    std::sort(units.begin(), units.end(), [](D2UnitStrc* a, D2UnitStrc* b) { return (uint32_t)a->dwUnitId < (uint32_t)b->dwUnitId; });
    for (D2UnitStrc* item : units) {
        D2SessionInventoryItem out = {};
        out.itemId = item->dwUnitId; out.classId = item->dwClassId;
        out.quality = (uint32_t)ITEMS_GetItemQuality(item); out.ilvl = (uint32_t)ITEMS_GetItemLevel(item);
        out.seedLo = item->pSeed.nLowSeed; out.seedHi = item->pSeed.nHighSeed;
        uint8_t bits[512] = {};
        out.bitBytes = (uint32_t)ITEMS_SerializeItemToBitstream(item, bits, sizeof bits, 1, 1, 0);
        memcpy(out.bits, bits, out.bitBytes < sizeof out.bits ? out.bitBytes : sizeof out.bits);
        g_sessionInventoryItems.push_back(out);
    }
    if (count) *count = (uint32_t)g_sessionInventoryItems.size();
    if (maxId) *maxId = highest;
    return g_sessionInventoryItems.data();
}
// The gamble selection table as loaded: count, then the selection ids, then the 100 limits.
static std::vector<uint32_t> g_gambleTableDump;
EMSCRIPTEN_KEEPALIVE const uint32_t* d2_gamble_table(uint32_t* count) {
    g_gambleTableDump.clear();
    D2GambleDataTbl* t = DATATBLS_GetGambleDataTables();
    if (t) {
        g_gambleTableDump.push_back((uint32_t)t->nGambleTxtRecordCount);
        for (int i = 0; i < t->nGambleTxtRecordCount; ++i) g_gambleTableDump.push_back(t->pGambleSelection[i]);
        for (int i = 0; i < 100; ++i) g_gambleTableDump.push_back((uint32_t)t->pGambleChooseLimit[i]);
    }
    if (count) *count = (uint32_t)g_gambleTableDump.size();
    return g_gambleTableDump.data();
}
// Client packet 0x36 {npcGUID, mercName} through the recovered handler (hire from the 0x4E list).
EMSCRIPTEN_KEEPALIVE int d2_session_command_hire_packet(uint32_t npcId, uint32_t mercName) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return -1;
    D2WEB_ResetPacketStream();
    uint8_t packet[9] = { 0x36 };
    const uint16_t name = (uint16_t)mercName;
    memcpy(packet + 1, &npcId, 4); memcpy(packet + 5, &name, 2);
    const int32_t result = D2GAME_PACKETCALLBACK_Rcv0x36_HireMerc_6FC86BD0(g_game, g_targetPlayer, packet, sizeof packet);
    D2WEB_PacketStream(&g_sessionPacketStreamLength, &g_sessionPacketStreamPackets);
    updateSessionSnapshot();
    return result;
}
// Monster units of the game above a GUID, in GUID order (the hireling a hire spawned):
// id, class, seeds, level, life, max life, strength, dexterity, experience.
struct D2SessionMonsterSummary { uint32_t id, classId, seedLo, seedHi; int32_t level, life, maxLife, str, dex, exp; };
static std::vector<D2SessionMonsterSummary> g_sessionMonsterSummaries;
EMSCRIPTEN_KEEPALIVE const D2SessionMonsterSummary* d2_session_monster_units_above(uint32_t minId, uint32_t* count, uint32_t* maxId) {
    g_sessionMonsterSummaries.clear();
    std::vector<D2UnitStrc*> units; uint32_t highest = 0;
    if (g_sessionLive && g_game)
        for (int b = 0; b < 128; ++b)
            for (D2UnitStrc* u = g_game->pUnitList[GAME_RemapUnitTypeToListIndex(UNIT_MONSTER)][b]; u; u = u->pListNext) {
                if ((uint32_t)u->dwUnitId > highest) highest = (uint32_t)u->dwUnitId;
                if ((uint32_t)u->dwUnitId > minId) units.push_back(u);
            }
    std::sort(units.begin(), units.end(), [](D2UnitStrc* a, D2UnitStrc* b) { return (uint32_t)a->dwUnitId < (uint32_t)b->dwUnitId; });
    for (D2UnitStrc* u : units)
        g_sessionMonsterSummaries.push_back({ (uint32_t)u->dwUnitId, (uint32_t)u->dwClassId, u->pSeed.nLowSeed, u->pSeed.nHighSeed,
            STATLIST_UnitGetStatValue(u, STAT_LEVEL, 0), STATLIST_UnitGetStatValue(u, STAT_HITPOINTS, 0), STATLIST_UnitGetStatValue(u, STAT_MAXHP, 0),
            STATLIST_UnitGetStatValue(u, STAT_STRENGTH, 0), STATLIST_UnitGetStatValue(u, STAT_DEXTERITY, 0), STATLIST_UnitGetStatValue(u, STAT_EXPERIENCE, 0) });
    if (count) *count = (uint32_t)g_sessionMonsterSummaries.size();
    if (maxId) *maxId = highest;
    return g_sessionMonsterSummaries.data();
}
// Fixture quest state: set one player quest flag (the state earlier play would have left).
EMSCRIPTEN_KEEPALIVE int d2_session_set_quest_flag(uint32_t questFilter, uint32_t flag) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return 0;
    QUESTRECORD_SetQuestState(UNITS_GetPlayerData(g_targetPlayer)->pQuestData[g_game->nDifficulty], (int32_t)questFilter, (int32_t)flag);
    return 1;
}
// Read a quest's controller fState (main state) and a player quest-record flag. Lets a scenario
// observe a boss fight's quest consequence (e.g. A5Q6 advancing when the chamber opens).
// Fixture: put a quest into its non-intro started state (bNotIntro + fState), the state a bare
// session lacks, so a boss fight's quest consequence (e.g. A5Q6 advancing when the chamber opens)
// can be observed. Mirrors QUESTS_QuestInit's bNotIntro and QUESTS_StateDebug's fState.
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_quest_start(uint32_t questId, int32_t fState) {
    if (!g_sessionLive || !g_game) return 0;
    D2QuestDataStrc* q = QUESTS_GetQuestData(g_game, (int32_t)questId);
    if (!q) return 0;
    q->bNotIntro = 1; q->fLastState = 0; QUESTS_StateDebug(q, fState, __FILE__, __LINE__);
    updateSessionSnapshot();
    return 1;
}
EMSCRIPTEN_KEEPALIVE int d2_session_quest_fstate(uint32_t questId) {
    if (!g_sessionLive || !g_game) return -1;
    D2QuestDataStrc* q = QUESTS_GetQuestData(g_game, (int32_t)questId);
    return q ? (int)q->fState : -1;
}
EMSCRIPTEN_KEEPALIVE int d2_session_quest_flag(uint32_t stateFlag, uint32_t bit) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return -1;
    return QUESTRECORD_GetQuestState(UNITS_GetPlayerData(g_targetPlayer)->pQuestData[g_game->nDifficulty], (int32_t)stateFlag, (int32_t)bit) ? 1 : 0;
}
// Where the player stands: level id, act, room subtile origin.
EMSCRIPTEN_KEEPALIVE int d2_session_player_level() {
    D2ActiveRoomStrc* room = g_targetPlayer ? UNITS_GetRoom(g_targetPlayer) : nullptr;
    return room ? DUNGEON_GetLevelIdFromRoom(room) : -1;
}
EMSCRIPTEN_KEEPALIVE int d2_session_player_act() { return g_targetPlayer ? (int)g_targetPlayer->nAct : -1; }
// Fixture stat (the capture gives its bare player gold the same way).
EMSCRIPTEN_KEEPALIVE int d2_session_set_stat(uint32_t statId, int32_t value) {
    if (!g_sessionLive || !g_targetPlayer) return 0;
    STATLIST_SetUnitStat(g_targetPlayer, (int32_t)statId, value, 0);
    return 1;
}
// Load the game's global quest record (the state earlier play left; packet 0x29 echoes it).
EMSCRIPTEN_KEEPALIVE int d2_session_set_global_quest_record(const uint8_t* bytes) {
    if (!g_sessionLive || !g_game || !g_game->pQuestControl || !g_game->pQuestControl->pQuestFlags || !bytes) return 0;
    QUESTRECORD_CopyBufferToRecord(g_game->pQuestControl->pQuestFlags, const_cast<uint8_t*>(bytes), 96, 0);
    return 1;
}
// The player's quest record of the game's difficulty, as packet 0x28 carries it.
static uint8_t g_sessionQuestRecord[96];
EMSCRIPTEN_KEEPALIVE const uint8_t* d2_session_quest_record() {
    memset(g_sessionQuestRecord, 0, sizeof g_sessionQuestRecord);
    if (g_sessionLive && g_game && g_targetPlayer)
        QUESTRECORD_CopyRecordToBuffer(UNITS_GetPlayerData(g_targetPlayer)->pQuestData[g_game->nDifficulty], g_sessionQuestRecord, sizeof g_sessionQuestRecord, 0);
    return g_sessionQuestRecord;
}
EMSCRIPTEN_KEEPALIVE const uint8_t* d2_session_packet_stream() { return D2WEB_PacketStream(nullptr, nullptr); }
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_packet_stream_length() { return g_sessionPacketStreamLength; }
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_packet_stream_packets() { return g_sessionPacketStreamPackets; }
EMSCRIPTEN_KEEPALIVE int d2_session_player_x() { return g_targetPlayer && g_targetPlayer->pDynamicPath ? g_targetPlayer->pDynamicPath->tGameCoords.wPosX : -1; }
EMSCRIPTEN_KEEPALIVE int d2_session_player_y() { return g_targetPlayer && g_targetPlayer->pDynamicPath ? g_targetPlayer->pDynamicPath->tGameCoords.wPosY : -1; }
EMSCRIPTEN_KEEPALIVE int d2_session_player_gold() { return g_targetPlayer ? STATLIST_UnitGetStatValue(g_targetPlayer, STAT_GOLD, 0) : -1; }
EMSCRIPTEN_KEEPALIVE int d2_session_player_id() { return g_targetPlayer ? (int)g_targetPlayer->dwUnitId : -1; }

EMSCRIPTEN_KEEPALIVE int d2_session_command_npc(uint32_t npcId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return 0;
    g_sessionNpcInteraction.commandTarget = npcId; g_sessionNpcInteraction.result = 0;
    D2WEB_ResetLastDialogPacket();
    memset(&g_sessionNpcDialog, 0, sizeof g_sessionNpcDialog);
    g_sessionNpcDialog.version = 1; g_sessionNpcDialog.size = sizeof g_sessionNpcDialog;
    g_sessionNpcDialog.commandTarget = npcId;
    D2UnitStrc* npc = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)npcId);
    if (!npc || !npc->pMonsterData || !npc->pMonsterData->pMonInteract ||
        D2Common_10399(g_targetPlayer, npc) > 6 || PLAYER_IsBusy(g_targetPlayer)) {
        updateSessionNpcInteraction(npcId); return 0;
    }
    if (PLRINTRO_GetQuestIntroFlag(g_targetPlayer, g_game, npc->dwClassId)) g_sessionNpcDialog.flags |= 2;
    if (g_game->pQuestControl) {
        for (D2QuestDataStrc* quest = g_game->pQuestControl->pLastQuest; quest; quest = quest->pPrev) {
            if (quest->nActNo == ACT_I && quest->pfCallback[QUESTEVENT_NPCACTIVATE]) g_sessionNpcDialog.flags |= 4;
            if (quest->nQuestNo == 37 && quest->pfCallback[QUESTEVENT_NPCACTIVATE]) g_sessionNpcDialog.flags |= 8;
        }
    }
    D2Common_10153(g_targetPlayer->pDynamicPath);
    sub_6FC61C70(g_game, g_targetPlayer, npc, npc->pMonsterData->pMonInteract, 0);
    int32_t type = -1, guid = -1;
    const int accepted = SUNIT_GetInteractInfo(g_targetPlayer, &type, &guid) &&
        type == UNIT_MONSTER && guid == (int32_t)npcId &&
        MONSTERAI_IsInteractingWith(npc->pMonsterData->pMonInteract, g_targetPlayer);
    g_sessionNpcInteraction.result = accepted ? 1u : 0u;
    D2GSPacketSrv27 packet = {};
    g_sessionNpcDialog.result = accepted ? 1u : 0u; g_sessionNpcDialog.npcId = npcId;
    if (D2WEB_CopyLastDialogPacket(&packet)) {
        g_sessionNpcDialog.packetHeader = packet.nHeader;
        g_sessionNpcDialog.packetUnitType = packet.nUnitType;
        g_sessionNpcDialog.packetGuid = packet.dwUnitGUID;
        g_sessionNpcDialog.messageCount = std::min<uint32_t>(packet.pMessageList.nCount, 8);
        for (uint32_t i = 0; i < g_sessionNpcDialog.messageCount; ++i) {
            g_sessionNpcDialog.stringIds[i] = packet.pMessageList.pMessages[i].nStringId;
            g_sessionNpcDialog.menus[i] = packet.pMessageList.pMessages[i].nMenu;
        }
        g_sessionNpcDialog.flags |= 1;
    }
    g_sessionNpcDialog.hash = sessionHashBytes(D2VEC_FNV_INIT, &g_sessionNpcDialog,
        offsetof(D2SessionNpcDialog, hash));
    updateSessionNpcInteraction(npcId);
    return accepted;
}

// What each unit IS, one byte per unit in d2_session_units order.
//
// bit 0  talkable NPC   -- MonStats INTERACT flag and the unit has interaction data. This is the
//                          same test d2_session_command_nearest_npc applies below.
// bit 1  hostile        -- MonStats nAlign == MONALIGN_EVIL.
//
// Both matter because every town NPC is a UNIT_MONSTER in D2: Charsi and a Fallen are the same
// unit type. A client that calls every monster hostile paints Akara as an enemy AND the Rogue
// guards standing around town as enemies -- the guards are not talkable, so an INTERACT-only test
// still gets them wrong. Retail decides hostility from the monster's ALIGNMENT, which is what
// bit 1 reports.
//
// Emitted beside D2SessionUnit rather than inside it: that record's 44-byte stride is hardcoded in
// a dozen scenarios, and widening it to carry two bits would churn all of them.
EMSCRIPTEN_KEEPALIVE int d2_session_unit_kinds(uint8_t* out, int cap) {
    if (!g_sessionLive || !g_game || !out || cap <= 0) return 0;
    updateSessionSnapshot();
    int written = 0;
    for (const D2SessionUnit& unit : g_sessionUnits) {
        if (written >= cap) break;
        uint8_t kind = 0;
        if (unit.type == UNIT_MONSTER) {
            D2UnitStrc* npc = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)unit.id);
            D2MonStatsTxt* stats = npc ? MONSTERMODE_GetMonStatsTxtRecord(npc->dwClassId) : nullptr;
            if (stats) {
                if (npc->pMonsterData && npc->pMonsterData->pMonInteract &&
                    (stats->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_INTERACT])) kind |= 1u;
                if (stats->nAlign == MONALIGN_EVIL) kind |= 2u;
            }
        }
        out[written++] = kind;
    }
    return written;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_nearest_npc(int radius) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || radius < 0 || radius > 6) return 0;
    updateSessionSnapshot();
    const D2SessionUnit* best = nullptr; int bestDistance = radius + 1;
    for (const D2SessionUnit& unit : g_sessionUnits) {
        if (unit.type != UNIT_MONSTER) continue;
        D2UnitStrc* npc = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)unit.id);
        if (!npc || !npc->pMonsterData || !npc->pMonsterData->pMonInteract) continue;
        D2MonStatsTxt* stats = MONSTERMODE_GetMonStatsTxtRecord(npc->dwClassId);
        if (!stats || !(stats->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_INTERACT])) continue;
        const int distance = D2Common_10399(g_targetPlayer, npc);
        if (distance < bestDistance || (distance == bestDistance && best && unit.id < best->id)) {
            best = &unit; bestDistance = distance;
        }
    }
    return best && bestDistance <= radius ? d2_session_command_npc(best->id) : 0;
}

EMSCRIPTEN_KEEPALIVE const D2SessionNpcInteraction* d2_session_npc_interaction() {
    updateSessionNpcInteraction(g_sessionNpcInteraction.commandTarget);
    return &g_sessionNpcInteraction;
}
EMSCRIPTEN_KEEPALIVE const D2SessionNpcDialog* d2_session_npc_dialog() {
    return &g_sessionNpcDialog;
}
EMSCRIPTEN_KEEPALIVE int d2_session_command_npc_response(uint32_t npcId, uint32_t messageId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || messageId > UINT16_MAX) return 0;
    memset(&g_sessionNpcResponse, 0, sizeof g_sessionNpcResponse);
    g_sessionNpcResponse.version = 1; g_sessionNpcResponse.size = sizeof g_sessionNpcResponse;
    g_sessionNpcResponse.packetHeader = 0x31; g_sessionNpcResponse.packetSize = 9;
    g_sessionNpcResponse.npcId = npcId; g_sessionNpcResponse.messageId = messageId;
    D2UnitStrc* npc = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)npcId);
    int32_t interactType = -1, interactGuid = -1;
    const bool active = npc && SUNIT_GetInteractInfo(g_targetPlayer, &interactType, &interactGuid) &&
        interactType == UNIT_MONSTER && interactGuid == (int32_t)npcId;
    bool offered = false;
    for (uint32_t i = 0; i < g_sessionNpcDialog.messageCount && i < 8; ++i)
        if (g_sessionNpcDialog.stringIds[i] == messageId) offered = true;
    if (!active || !offered) return 0;
    g_sessionNpcResponse.introBefore = PLRINTRO_GetQuestIntroFlag(g_targetPlayer, g_game, npc->dwClassId);
    uint8_t packet[9] = { 0x31 };
    const int32_t guid = (int32_t)npcId; const uint16_t message = (uint16_t)messageId;
    memcpy(packet + 1, &guid, sizeof guid); memcpy(packet + 5, &message, sizeof message);
    const int32_t handler = D2GAME_PACKETCALLBACK_Rcv0x31_QuestMessage_6FC86AB0(g_game, g_targetPlayer, packet, sizeof packet);
    g_sessionNpcResponse.introAfter = PLRINTRO_GetQuestIntroFlag(g_targetPlayer, g_game, npc->dwClassId);
    SUNIT_GetInteractInfo(g_targetPlayer, &g_sessionNpcResponse.interactType, &g_sessionNpcResponse.interactGuid);
    g_sessionNpcResponse.result = handler == 0 ? 1u : 0u;
    if (active) g_sessionNpcResponse.flags |= 1;
    if (offered) g_sessionNpcResponse.flags |= 2;
    if (!g_sessionNpcResponse.introBefore && g_sessionNpcResponse.introAfter) g_sessionNpcResponse.flags |= 4;
    g_sessionNpcResponse.hash = sessionHashBytes(D2VEC_FNV_INIT, &g_sessionNpcResponse,
        offsetof(D2SessionNpcResponse, hash));
    return g_sessionNpcResponse.result;
}
extern "C" uint32_t D2WEB_TakeUiEvents(uint8_t* out, uint32_t cap);
extern "C" int D2WEB_TakeWaypointMenu(D2GSPacketSrv63* pPacket);
// The waypoint menu packet (0x63) the server sent since the last call: 1 and the waypoint unit id
// in *pWaypointId when one arrived, else 0. The ACTIVATED set is read below from the player's own
// waypoint data, which is what the packet carries.
EMSCRIPTEN_KEEPALIVE int d2_session_take_waypoint_menu(uint32_t* pWaypointId) {
    D2GSPacketSrv63 packet = {};
    if (!D2WEB_TakeWaypointMenu(&packet)) return 0;
    if (pWaypointId) *pWaypointId = packet.dwUnitId;
    return 1;
}
// Every waypoint retail knows, in waypoint-number order, as {waypointNo, levelId, act, activated}
// int32 quads, activation from the player's waypoint data for the current difficulty. Returns the
// number of quads.
EMSCRIPTEN_KEEPALIVE int d2_session_waypoints(int32_t* out, int cap) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !out) return 0;
    D2PlayerDataStrc* data = UNITS_GetPlayerData(g_targetPlayer);
    D2WaypointDataStrc* flags = data ? data->pWaypointData[g_game->nDifficulty] : nullptr;
    int n = 0;
    for (int wp = 0; wp < 64 && n + 4 <= cap; ++wp) {
        int levelId = 0;
        if (!WAYPOINTS_GetLevelIdFromWaypointNo((short)wp, &levelId) || levelId <= 0) continue;
        out[n++] = wp; out[n++] = levelId; out[n++] = DRLG_GetActNoFromLevelId(levelId);
        out[n++] = flags ? (WAYPOINTS_IsActivated(flags, (uint16_t)wp) ? 1 : 0) : 0;
    }
    return n / 4;
}
// Levels.txt's LevelName for a level: the string-table KEY, resolved by the client like any other.
// MonStats NameStr of a monster class: the string index the client names an NPC by (its menu header).
EMSCRIPTEN_KEEPALIVE int d2_monster_name_str(int classId) {
    D2MonStatsTxt* rec = DATATBLS_GetMonStatsTxtRecord(classId);
    return rec ? rec->wNameStr : -1;
}
EMSCRIPTEN_KEEPALIVE const char* d2_level_name(int levelId) {
    D2LevelsTxt* rec = DATATBLS_GetLevelsTxtRecord(levelId);
    return rec ? rec->szLevelName : "";
}
// Retail's 0x49 TakeOrCloseWp: travel to levelId through that waypoint, or close the menu with 0.
EMSCRIPTEN_KEEPALIVE int d2_session_command_waypoint(uint32_t waypointId, int32_t levelId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return 0;
    uint8_t packet[9] = { 0x49 };
    const int32_t guid = (int32_t)waypointId; memcpy(packet + 1, &guid, 4); memcpy(packet + 5, &levelId, 4);
    const int32_t oldLevel = DUNGEON_GetLevelIdFromRoom(UNITS_GetRoom(g_targetPlayer));
    const int32_t rc = D2GAME_PACKETCALLBACK_Rcv0x49_TakeOrCloseWp_6FC87E60(g_game, g_targetPlayer, packet, sizeof packet);
    const int32_t newLevel = DUNGEON_GetLevelIdFromRoom(UNITS_GetRoom(g_targetPlayer));
    if (newLevel != oldLevel) return adoptSessionLevelChange(oldLevel, newLevel);
    return rc == 0;
}
// The 0x77 UpdateUi codes the server has sent since the last call, in order, then cleared.
// 16 = UPDATEUI_OPENSTASH, 17 = UPDATEUI_CLOSESTASH (D2Constants.h).
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_take_ui_events(uint8_t* out, uint32_t cap) {
    return D2WEB_TakeUiEvents(out, cap);
}
// Retail's 0x4F ClickButton, e.g. TRADEBTN_CLOSESTASH (18) when the player closes the stash, or
// withdraw/deposit (19/20) with a gold amount. Through retail's own handler.
EMSCRIPTEN_KEEPALIVE int d2_session_command_button(uint32_t button, int32_t gold) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || button > 0xFFFF) return 0;
    uint8_t packet[7] = { 0x4F };
    const uint16_t b = (uint16_t)button, hi = (uint16_t)((uint32_t)gold >> 16), lo = (uint16_t)gold;
    memcpy(packet + 1, &b, 2); memcpy(packet + 3, &hi, 2); memcpy(packet + 5, &lo, 2);
    return D2GAME_PACKETCALLBACK_Rcv0x4F_ClickButton_6FC881D0(g_game, g_targetPlayer, packet, sizeof packet) == 0;
}
EMSCRIPTEN_KEEPALIVE int d2_session_command_end_npc(uint32_t npcId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return 0;
    int32_t type = -1, guid = -1;
    if (!SUNIT_GetInteractInfo(g_targetPlayer, &type, &guid) || type != UNIT_MONSTER || guid != (int32_t)npcId) return 0;
    uint8_t packet[9] = { 0x30 };
    const int32_t npcGuid = (int32_t)npcId; memcpy(packet + 5, &npcGuid, sizeof npcGuid);
    const bool ended = D2GAME_PACKETCALLBACK_Rcv0x30_TerminateEntityChat_6FC869F0(g_game, g_targetPlayer, packet, sizeof packet) == 0;
    // g_sessionNpcDialog records the conversation the player OPENED; once retail's own 0x30 handler
    // has ended it, that record no longer describes anything, and a client reading it would keep the
    // window up forever (s000 step "Esc closes the NPC window"). Clear it only on success, so a
    // refused end still shows the conversation retail kept open.
    if (ended) memset(&g_sessionNpcDialog, 0, sizeof g_sessionNpcDialog);
    return ended;
}
EMSCRIPTEN_KEEPALIVE const D2SessionNpcResponse* d2_session_npc_response() {
    return &g_sessionNpcResponse;
}
static uint32_t sessionInventoryItemCount(D2InventoryStrc* inventory) {
    uint32_t count = 0;
    if (inventory) for (D2UnitStrc* item = INVENTORY_GetFirstItem(inventory); item; item = INVENTORY_GetNextItem(item))
        if (INVENTORY_UnitIsItem(item)) ++count;
    return count;
}
// Fill a presentation record for one item, without touching the player's own list.
static void appendVendorPresentation(D2UnitStrc* item) {
    if (!item || item->dwUnitType != UNIT_ITEM) return;
    D2SessionItemPresentation out = {};
    out.id = item->dwUnitId; out.classId = item->dwClassId; out.mode = item->dwAnimMode;
    out.quality = ITEMS_GetItemQuality(item); out.itemFlags = ITEMS_GetItemFlags(item);
    out.framePrecise = item->nSeqCurrentFramePrecise; out.frameCountPrecise = item->dwFrameCountPrecise;
    if (item->pStaticPath) {
        out.x = item->pStaticPath->tGameCoords.nX; out.y = item->pStaticPath->tGameCoords.nY;
        out.gridX = (int16_t)item->pStaticPath->tGameCoords.nX;
        out.gridY = (int16_t)item->pStaticPath->tGameCoords.nY;
    }
    out.invPage = ITEMS_GetInvPage(item); out.bodyLoc = ITEMS_GetBodyLocation(item);
    if (D2ItemsTxt* txt = DATATBLS_GetItemsTxtRecord(item->dwClassId)) {
        out.code = txt->dwCode;
        memcpy(out.flippyFile, txt->szFlippyFile, sizeof out.flippyFile);
        memcpy(out.invFile, txt->szInvFile, sizeof out.invFile);
    }
    g_sessionVendorPresentations.push_back(out);
}

static void updateSessionVendorItems(D2UnitStrc* npc) {
    g_sessionVendorItems.clear();
    g_sessionVendorPresentations.clear();
    if (!npc || !g_targetPlayer || !g_game) return;
    D2InventoryStrc* inventory = g_sessionVendor.action == 2
        ? SUNITPROXY_GetGambleInventory(g_game, g_targetPlayer, npc) : npc->pInventory;
    if (!inventory) return;
    for (D2UnitStrc* item = INVENTORY_GetFirstItem(inventory); item; item = INVENTORY_GetNextItem(item)) {
        if (!INVENTORY_UnitIsItem(item)) continue;
        D2SessionVendorItem out = {};
        out.id = item->dwUnitId; out.classId = item->dwClassId; out.mode = item->dwAnimMode;
        out.quality = ITEMS_GetItemQuality(item); out.flags = item->dwFlags;
        if (D2ItemsTxt* txt = DATATBLS_GetItemsTxtRecord(item->dwClassId)) out.code = txt->dwCode;
        out.cost = ITEMS_GetTransactionCost(g_targetPlayer, item, (D2C_Difficulties)g_game->nDifficulty,
            UNITS_GetPlayerData(g_targetPlayer)->pQuestData[g_game->nDifficulty], npc->dwClassId,
            g_sessionVendor.action == 2 ? D2C_TransactionTypes::TRANSACTIONTYPE_GAMBLE
                                        : D2C_TransactionTypes::TRANSACTIONTYPE_BUY);
        out.hash = sessionHashBytes(D2VEC_FNV_INIT, &out, offsetof(D2SessionVendorItem, hash));
        g_sessionVendorItems.push_back(out);
        appendVendorPresentation(item);
    }
}
// Fixture: set a stat on a specific item unit (to damage it before a repair). Mirrors the
// player set_stat but targets an item the player owns.
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_set_item_stat(uint32_t itemId, uint32_t statId, int32_t value) {
    if (!g_sessionLive || !g_game) return 0;
    D2UnitStrc* item = SUNIT_GetServerUnit(g_game, UNIT_ITEM, (int32_t)itemId);
    if (!item) return 0;
    STATLIST_SetUnitStat(item, (int32_t)statId, value, 0);
    updateSessionSnapshot();
    return STATLIST_UnitGetStatValue(item, (int32_t)statId, 0) == value ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_npc_vendor(uint32_t npcId, uint32_t action) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || (action != 1 && action != 2)) return 0;
    memset(&g_sessionVendor, 0, sizeof g_sessionVendor);
    g_sessionVendor.version = 1; g_sessionVendor.size = sizeof g_sessionVendor;
    g_sessionVendor.packetHeader = 0x38; g_sessionVendor.packetSize = sizeof(D2GSPacketClt38);
    g_sessionVendor.action = action; g_sessionVendor.npcId = npcId;
    int32_t type = -1, guid = -1;
    if (!SUNIT_GetInteractInfo(g_targetPlayer, &type, &guid) || type != UNIT_MONSTER || guid != (int32_t)npcId) return 0;
    D2GSPacketClt38 packet = {};
    packet.nHeader = 0x38; packet.nAction = action; packet.nNpcGUID = (int32_t)npcId; packet.nItemGUID = -1;
    const int32_t handler = D2GAME_PACKETCALLBACK_Rcv0x38_EntityAction_6FC86CB0(g_game, g_targetPlayer, &packet, sizeof packet);
    D2UnitStrc* npc = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)npcId);
    updateSessionVendorItems(npc);
    int32_t unused = 0; D2NpcRecordStrc* record = npc ? SUNITPROXY_GetNpcRecordFromUnit(g_game, npc, &unused) : nullptr;
    if (record) { g_sessionVendor.trading = record->bTrading; g_sessionVendor.vendorInit = record->npcTrade.bVendorInit; }
    uint32_t hash = D2VEC_FNV_INIT;
    if (npc && npc->pInventory) for (D2UnitStrc* item = INVENTORY_GetFirstItem(npc->pInventory); item; item = INVENTORY_GetNextItem(item)) {
        if (!INVENTORY_UnitIsItem(item)) continue;
        const uint32_t fields[] = { (uint32_t)item->dwClassId, (uint32_t)item->dwAnimMode,
            (uint32_t)ITEMS_GetItemQuality(item), item->dwFlags, item->dwFlagEx };
        hash = sessionHashBytes(hash, fields, sizeof fields); ++g_sessionVendor.itemCount;
        if (g_sessionVendor.itemCount == 1) {
            g_sessionVendor.firstClass = item->dwClassId; g_sessionVendor.firstQuality = ITEMS_GetItemQuality(item);
            if (D2ItemsTxt* txt = DATATBLS_GetItemsTxtRecord(item->dwClassId)) g_sessionVendor.firstCode = txt->dwCode;
        }
    }
    g_sessionVendor.itemHash = hash; g_sessionVendor.result = handler == 0 && record && record->bTrading ? 1u : 0u;
    g_sessionVendor.hash = sessionHashBytes(D2VEC_FNV_INIT, &g_sessionVendor, offsetof(D2SessionVendor, hash));
    return g_sessionVendor.result;
}
EMSCRIPTEN_KEEPALIVE const D2SessionVendor* d2_session_vendor() { return &g_sessionVendor; }
EMSCRIPTEN_KEEPALIVE const D2SessionVendorItem* d2_session_vendor_items() { return g_sessionVendorItems.data(); }
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_vendor_item_count() { return (uint32_t)g_sessionVendorItems.size(); }

static int sessionVendorTransaction(uint32_t npcId, uint32_t itemId, bool buy, bool multibuy = false) {
    memset(&g_sessionVendorTransaction, 0, sizeof g_sessionVendorTransaction);
    D2SessionVendorTransaction& out = g_sessionVendorTransaction;
    out.version = 1; out.size = sizeof out; out.packetHeader = buy ? 0x32 : 0x33; out.packetSize = 17;
    out.npcId = npcId; out.itemId = itemId; out.transactionType = buy ? 0 : 1;
    if (!g_sessionLive || !g_game || !g_targetPlayer) return 0;
    D2UnitStrc* npc = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)npcId);
    D2UnitStrc* item = SUNIT_GetServerUnit(g_game, UNIT_ITEM, (int32_t)itemId);
    int32_t interactType = -1, interactGuid = -1;
    if (!npc || !item || !SUNIT_GetInteractInfo(g_targetPlayer, &interactType, &interactGuid) ||
        interactType != UNIT_MONSTER || interactGuid != (int32_t)npcId) return 0;
    out.itemClass = item->dwClassId; out.itemMode = item->dwAnimMode; out.itemQuality = ITEMS_GetItemQuality(item);
    if (D2ItemsTxt* txt = DATATBLS_GetItemsTxtRecord(item->dwClassId)) out.itemCode = txt->dwCode;
    out.packetCost = ITEMS_GetTransactionCost(g_targetPlayer, item, (D2C_Difficulties)g_game->nDifficulty,
        UNITS_GetPlayerData(g_targetPlayer)->pQuestData[g_game->nDifficulty], npc->dwClassId,
        buy ? D2C_TransactionTypes::TRANSACTIONTYPE_BUY : D2C_TransactionTypes::TRANSACTIONTYPE_SELL);
    out.goldBefore = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_GOLD, 0);
    out.playerItemsBefore = sessionInventoryItemCount(g_targetPlayer->pInventory);
    out.vendorItemsBefore = sessionInventoryItemCount(npc->pInventory);
    out.ownedBefore = ITEMS_IsInPlayersInventory(g_targetPlayer, item, 0) == 1;
    D2WEB_ResetPacketStream();
    if (buy) {
        D2GSPacketClt32 packet = {};
        packet.nHeader = 0x32; packet.dwNpcGUID = (int32_t)npcId; packet.dwItemGUID = (int32_t)itemId;
        packet.nTransactionType = D2C_TransactionTypes::TRANSACTIONTYPE_BUY;
        packet.nItemMode = item->dwAnimMode; packet.bMultibuy = multibuy ? 1 : 0; packet.dwCost = out.packetCost;
        out.handlerResult = D2GAME_PACKETCALLBACK_Rcv0x32_BuyItemFromNpcBuffer_6FC86AE0(g_game, g_targetPlayer, &packet, sizeof packet);
    } else {
        D2GSPacketClt33 packet = {};
        packet.nHeader = 0x33; packet.dwMonGUID = npcId; packet.dwItemGUID = itemId;
        packet.nTab = item->dwAnimMode; packet.dwGold = out.packetCost;
        out.handlerResult = D2GAME_PACKETCALLBACK_Rcv0x33_SellItemToNpcBuffer_6FC86B30(g_game, g_targetPlayer, &packet, sizeof packet);
    }
    if (buy) out.resultItemId = UNITS_GetPlayerData(g_targetPlayer)->dwBoughtItemId;
    D2UnitStrc* after = SUNIT_GetServerUnit(g_game, UNIT_ITEM, buy ? (int32_t)out.resultItemId : (int32_t)itemId);
    out.goldAfter = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_GOLD, 0);
    out.playerItemsAfter = sessionInventoryItemCount(g_targetPlayer->pInventory);
    out.vendorItemsAfter = sessionInventoryItemCount(npc->pInventory);
    out.ownedAfter = after && ITEMS_IsInPlayersInventory(g_targetPlayer, after, 0) == 1;
    if (out.handlerResult == 0) out.flags |= 1;
    if (buy ? (out.goldAfter == out.goldBefore - out.packetCost) : (out.goldAfter == out.goldBefore + out.packetCost)) out.flags |= 2;
    if (buy ? (out.playerItemsAfter > out.playerItemsBefore) : (out.playerItemsAfter < out.playerItemsBefore)) out.flags |= 4;
    if (out.ownedBefore) out.flags |= 8;
    if (out.ownedAfter) out.flags |= 16;
    D2WEB_PacketStream(&g_sessionPacketStreamLength, &g_sessionPacketStreamPackets);
    out.hash = sessionHashBytes(D2VEC_FNV_INIT, &out, offsetof(D2SessionVendorTransaction, hash));
    updateSessionVendorItems(npc);
    return (out.flags & 7) == 7;
}
EMSCRIPTEN_KEEPALIVE int d2_session_command_vendor_gamble(uint32_t npcId, uint32_t itemId) {
    memset(&g_sessionVendorTransaction, 0, sizeof g_sessionVendorTransaction);
    D2SessionVendorTransaction& out = g_sessionVendorTransaction;
    out.version = 1; out.size = sizeof out; out.packetHeader = 0x32; out.packetSize = 17;
    out.npcId = npcId; out.itemId = itemId; out.transactionType = D2C_TransactionTypes::TRANSACTIONTYPE_GAMBLE;
    if (!g_sessionLive || !g_game || !g_targetPlayer || g_sessionVendor.action != 2) return 0;
    D2UnitStrc* npc = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)npcId);
    D2UnitStrc* item = SUNIT_GetServerUnit(g_game, UNIT_ITEM, (int32_t)itemId);
    D2InventoryStrc* gamble = npc ? SUNITPROXY_GetGambleInventory(g_game, g_targetPlayer, npc) : nullptr;
    int32_t interactType = -1, interactGuid = -1; bool listed = false;
    if (gamble && item) for (D2UnitStrc* it = INVENTORY_GetFirstItem(gamble); it; it = INVENTORY_GetNextItem(it))
        if (it == item) { listed = true; break; }
    if (!npc || !item || !listed || !SUNIT_GetInteractInfo(g_targetPlayer, &interactType, &interactGuid) ||
        interactType != UNIT_MONSTER || interactGuid != (int32_t)npcId) return 0;
    out.itemClass = item->dwClassId; out.itemMode = item->dwAnimMode; out.itemQuality = ITEMS_GetItemQuality(item);
    if (D2ItemsTxt* txt = DATATBLS_GetItemsTxtRecord(item->dwClassId)) out.itemCode = txt->dwCode;
    out.packetCost = ITEMS_GetTransactionCost(g_targetPlayer, item, (D2C_Difficulties)g_game->nDifficulty,
        UNITS_GetPlayerData(g_targetPlayer)->pQuestData[g_game->nDifficulty], npc->dwClassId,
        D2C_TransactionTypes::TRANSACTIONTYPE_GAMBLE);
    out.goldBefore = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_GOLD, 0);
    out.playerItemsBefore = sessionInventoryItemCount(g_targetPlayer->pInventory);
    out.vendorItemsBefore = sessionInventoryItemCount(gamble);
    D2WEB_ResetPacketStream();
    D2GSPacketClt32 packet = {};
    packet.nHeader = 0x32; packet.dwNpcGUID = (int32_t)npcId; packet.dwItemGUID = (int32_t)itemId;
    packet.nTransactionType = D2C_TransactionTypes::TRANSACTIONTYPE_GAMBLE;
    packet.nItemMode = item->dwAnimMode; packet.dwCost = out.packetCost;
    out.handlerResult = D2GAME_PACKETCALLBACK_Rcv0x32_BuyItemFromNpcBuffer_6FC86AE0(g_game, g_targetPlayer, &packet, sizeof packet);
    out.resultItemId = UNITS_GetPlayerData(g_targetPlayer)->dwBoughtItemId;
    D2UnitStrc* after = SUNIT_GetServerUnit(g_game, UNIT_ITEM, (int32_t)out.resultItemId);
    out.goldAfter = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_GOLD, 0);
    out.playerItemsAfter = sessionInventoryItemCount(g_targetPlayer->pInventory);
    out.vendorItemsAfter = sessionInventoryItemCount(gamble);
    out.ownedAfter = after && ITEMS_IsInPlayersInventory(g_targetPlayer, after, 0) == 1;
    if (out.handlerResult == 0) out.flags |= 1;
    if (out.goldAfter == out.goldBefore - out.packetCost) out.flags |= 2;
    if (out.playerItemsAfter > out.playerItemsBefore) out.flags |= 4;
    if (out.ownedAfter) out.flags |= 16;
    D2WEB_PacketStream(&g_sessionPacketStreamLength, &g_sessionPacketStreamPackets);
    out.hash = sessionHashBytes(D2VEC_FNV_INIT, &out, offsetof(D2SessionVendorTransaction, hash));
    updateSessionVendorItems(npc);
    return (out.flags & 23) == 23;
}
EMSCRIPTEN_KEEPALIVE int d2_session_command_vendor_repair(uint32_t npcId, uint32_t itemId) {
    memset(&g_sessionRepair, 0, sizeof g_sessionRepair);
    D2SessionRepair& out = g_sessionRepair;
    out.version = 1; out.size = sizeof out; out.packetHeader = 0x35; out.packetSize = 17;
    out.npcId = npcId; out.itemId = itemId;
    if (!g_sessionLive || !g_game || !g_targetPlayer) return 0;
    D2UnitStrc* npc = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)npcId);
    D2UnitStrc* item = SUNIT_GetServerUnit(g_game, UNIT_ITEM, (int32_t)itemId);
    int32_t interactType = -1, interactGuid = -1;
    if (!npc || !item || !SUNIT_GetInteractInfo(g_targetPlayer, &interactType, &interactGuid) ||
        interactType != UNIT_MONSTER || interactGuid != (int32_t)npcId) return 0;
    out.itemClass = item->dwClassId;
    out.goldBefore = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_GOLD, 0);
    out.cost = ITEMS_GetTransactionCost(g_targetPlayer, item, (D2C_Difficulties)g_game->nDifficulty,
        UNITS_GetPlayerData(g_targetPlayer)->pQuestData[g_game->nDifficulty], npc->dwClassId,
        D2C_TransactionTypes::TRANSACTIONTYPE_REPAIR);
    out.durabilityBefore = STATLIST_UnitGetStatValue(item, STAT_DURABILITY, 0);
    out.maxDurability = STATLIST_GetMaxDurabilityFromUnit(item);
    out.quantityBefore = STATLIST_UnitGetStatValue(item, STAT_QUANTITY, 0);
    out.maxQuantity = ITEMS_GetTotalMaxStack(item);
    out.ownedBefore = ITEMS_IsInPlayersInventory(g_targetPlayer, item, 0) == 1;
    uint8_t packet[17] = { 0x35 };
    const int32_t npcGuid = (int32_t)npcId, itemGuid = (int32_t)itemId, individual = 0;
    memcpy(packet + 1, &npcGuid, sizeof npcGuid); memcpy(packet + 5, &itemGuid, sizeof itemGuid);
    memcpy(packet + 13, &individual, sizeof individual);
    out.handlerResult = D2GAME_PACKETCALLBACK_Rcv0x35_Repair_6FC86B70(g_game, g_targetPlayer, packet, sizeof packet);
    out.goldAfter = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_GOLD, 0);
    out.durabilityAfter = STATLIST_UnitGetStatValue(item, STAT_DURABILITY, 0);
    out.quantityAfter = STATLIST_UnitGetStatValue(item, STAT_QUANTITY, 0);
    out.ownedAfter = ITEMS_IsInPlayersInventory(g_targetPlayer, item, 0) == 1;
    if (out.handlerResult == 0) out.flags |= 1;
    if (out.goldAfter == out.goldBefore - out.cost) out.flags |= 2;
    if ((out.maxDurability && out.durabilityAfter == out.maxDurability) ||
        (out.maxQuantity && out.quantityAfter == out.maxQuantity)) out.flags |= 4;
    if (out.ownedBefore && out.ownedAfter) out.flags |= 8;
    out.hash = sessionHashBytes(D2VEC_FNV_INIT, &out, offsetof(D2SessionRepair, hash));
    return (out.flags & 15) == 15;
}
EMSCRIPTEN_KEEPALIVE const D2SessionRepair* d2_session_vendor_repair() { return &g_sessionRepair; }
EMSCRIPTEN_KEEPALIVE int d2_session_command_vendor_sell(uint32_t npcId, uint32_t itemId) {
    return sessionVendorTransaction(npcId, itemId, false);
}
EMSCRIPTEN_KEEPALIVE int d2_session_command_vendor_buy(uint32_t npcId, uint32_t itemId) {
    return sessionVendorTransaction(npcId, itemId, true);
}
EMSCRIPTEN_KEEPALIVE int d2_session_command_vendor_multibuy(uint32_t npcId, uint32_t itemId) {
    return sessionVendorTransaction(npcId, itemId, true, true);
}
// STAT_QUANTITY of a server item unit, for reading a multibuy's filled stack.
EMSCRIPTEN_KEEPALIVE int32_t d2_session_item_quantity(uint32_t itemId) {
    if (!g_sessionLive || !g_game) return -1;
    D2UnitStrc* item = SUNIT_GetServerUnit(g_game, UNIT_ITEM, (int32_t)itemId);
    return item ? STATLIST_UnitGetStatValue(item, STAT_QUANTITY, 0) : -1;
}
EMSCRIPTEN_KEEPALIVE const D2SessionVendorTransaction* d2_session_vendor_transaction() {
    return &g_sessionVendorTransaction;
}
EMSCRIPTEN_KEEPALIVE const D2SessionAct1Quest1* d2_session_act1q1() {
    memset(&g_sessionAct1Quest1, 0, sizeof g_sessionAct1Quest1);
    D2SessionAct1Quest1& out = g_sessionAct1Quest1;
    out.version = 1; out.size = sizeof out; out.level = g_currentLevel;
    if (!g_sessionLive || !g_game || !g_targetPlayer) return &out;
    D2QuestDataStrc* quest = QUESTS_GetQuestData(g_game, QUEST_A1Q1_DENOFEVIL);
    D2BitBufferStrc* flags = UNITS_GetPlayerData(g_targetPlayer)->pQuestData[g_game->nDifficulty];
    if (quest) {
        out.questState = quest->fState; out.lastState = quest->fLastState; out.questFlags = quest->dwFlags;
        if (D2Act1Quest1Strc* ex = static_cast<D2Act1Quest1Strc*>(quest->pQuestDataEx)) {
            out.monstersLeft = ex->nMonstersLeft; out.enteredDen = ex->bEnteredDen;
            out.akaraActivated = ex->bAkaraActivated; out.finished = ex->bFinished; out.timerActive = ex->bTimerActive;
        }
    }
    for (uint32_t bit = 0; bit < 16; ++bit)
        if (QUESTRECORD_GetQuestState(flags, QUESTSTATEFLAG_A1Q1, bit)) out.playerFlags |= 1u << bit;
    out.globalPrimaryGoal = QUESTS_GetGlobalState(g_game, QUESTSTATEFLAG_A1Q1, QFLAG_PRIMARYGOALDONE);
    out.skillPoints = STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_SKILLPTS, 0);
    out.hash = sessionHashBytes(D2VEC_FNV_INIT, &out, offsetof(D2SessionAct1Quest1, hash));
    return &out;
}

EMSCRIPTEN_KEEPALIVE const D2SessionAct1Quest6* d2_session_act1q6() {
    memset(&g_sessionAct1Quest6, 0, sizeof g_sessionAct1Quest6);
    D2SessionAct1Quest6& out = g_sessionAct1Quest6;
    out.version = 1; out.size = sizeof out; out.level = g_currentLevel; out.andarielGuid = -1;
    if (!g_sessionLive || !g_game || !g_targetPlayer) return &out;
    D2QuestDataStrc* quest = QUESTS_GetQuestData(g_game, QUEST_A1Q6_ANDARIEL);
    D2BitBufferStrc* flags = UNITS_GetPlayerData(g_targetPlayer)->pQuestData[g_game->nDifficulty];
    if (quest) {
        out.questState = quest->fState; out.lastState = quest->fLastState; out.questFlags = quest->dwFlags;
        out.killCallbackActive = quest->pfCallback[QUESTEVENT_MONSTERKILLED] != nullptr;
        if (D2Act1Quest6Strc* ex = static_cast<D2Act1Quest6Strc*>(quest->pQuestDataEx)) {
            out.andarielGuid = ex->nAndarielGUID; out.timerInvocations = ex->nTimerInvocations;
            out.andarielKilled = ex->bAndarielKilled;
        }
    }
    for (uint32_t bit = 0; bit < 16; ++bit)
        if (QUESTRECORD_GetQuestState(flags, QUESTSTATEFLAG_A1Q6, bit)) out.playerFlags |= 1u << bit;
    out.globalPrimaryGoal = QUESTS_GetGlobalState(g_game, QUESTSTATEFLAG_A1Q6, QFLAG_PRIMARYGOALDONE);
    out.primaryGoalDone = QUESTRECORD_GetQuestState(flags, QUESTSTATEFLAG_A1Q6, QFLAG_PRIMARYGOALDONE);
    out.rewardPending = QUESTRECORD_GetQuestState(flags, QUESTSTATEFLAG_A1Q6, QFLAG_REWARDPENDING);
    out.rewardGranted = QUESTRECORD_GetQuestState(flags, QUESTSTATEFLAG_A1Q6, QFLAG_REWARDGRANTED);
    out.completedNow = QUESTRECORD_GetQuestState(flags, QUESTSTATEFLAG_A1Q6, QFLAG_COMPLETEDNOW);
    out.hash = sessionHashBytes(D2VEC_FNV_INIT, &out, offsetof(D2SessionAct1Quest6, hash));
    return &out;
}

// The retail A1Q6 oracle's first slice observes the callback with no player argument.  Keep
// that exact surface here: progression/reward flags remain a subsequent player-record vector.
// Fixture: dispatch a quest's MONSTERKILLED callback for the first monster of a class in the
// session's rooms, with the target player as the killer (the generic form of the Andariel kill
// fixture below; Mephisto -> A3Q6 opens the Hell gate portal and sets the act-change flag).
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_quest_monster_killed(uint32_t questId, uint32_t monsterClass) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return 0;
    updateSessionSnapshot();
    D2UnitStrc* monster = nullptr;
    for (const D2SessionUnit& unit : g_sessionUnits) {
        if (unit.type == UNIT_MONSTER && unit.classId == monsterClass) {
            monster = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)unit.id);
            break;
        }
    }
    D2QuestDataStrc* quest = QUESTS_GetQuestData(g_game, (int32_t)questId);
    if (!monster || !quest || !quest->pfCallback[QUESTEVENT_MONSTERKILLED]) return 0;
    D2QuestArgStrc args = {}; args.pGame = g_game; args.nEvent = QUESTEVENT_MONSTERKILLED;
    args.pTarget = monster; args.pPlayer = g_targetPlayer;
    quest->pfCallback[QUESTEVENT_MONSTERKILLED](quest, &args);
    updateSessionSnapshot();
    return 1;
}

// Fixture: place the player beside a unit by the recovered teleport placement (sub_6FCBDFE0
// with its free-coordinate search), so an operate or attack command is in range.
static int standNearUnit(D2UnitStrc* unit) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pDynamicPath || !unit) return 0;
    D2ActiveRoomStrc* room = UNITS_GetRoom(unit);
    D2CoordStrc at = {}; UNITS_GetCoords(unit, &at);
    if (!room) return 0;
    const int32_t size = UNITS_GetUnitSizeX(unit);
    for (int32_t reach = size; reach <= size + 3; ++reach) {
        for (int32_t dx = -reach; dx <= reach; ++dx) for (int32_t dy = -reach; dy <= reach; ++dy) {
            if (abs(dx) != reach && abs(dy) != reach) continue;
            if (!sub_6FCBDFE0(g_game, g_targetPlayer, room, at.nX + dx, at.nY + dy, 0, 0)) continue;
            g_sessionMovePending = false;
            d2_session_tick(1);
            const bool inRange = unit->dwUnitType == UNIT_OBJECT ? UNITS_IsObjectInInteractRange(g_targetPlayer, unit) != 0
                               : UNITS_GetDistanceToOtherUnit(g_targetPlayer, unit) <= 4;
            if (inRange) { updateSessionSnapshot(); return 1; }
        }
    }
    updateSessionSnapshot();
    return 0;
}
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_stand_near_object(uint32_t objectId) {
    if (!g_sessionLive || !g_game) return 0;
    return standNearUnit(SUNIT_GetServerUnit(g_game, UNIT_OBJECT, (int32_t)objectId));
}
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_stand_near_unit(uint32_t unitType, uint32_t unitId) {
    if (!g_sessionLive || !g_game) return 0;
    return standNearUnit(SUNIT_GetServerUnit(g_game, (int32_t)unitType, (int32_t)unitId));
}

// Fixture: kill a monster through the recovered kill routine (SUNITDMG_KillMonster: death mode,
// drops, experience, quest MONSTERKILLED events) with the target player as the killer.
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_kill_monster(uint32_t unitId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return 0;
    D2UnitStrc* monster = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)unitId);
    if (!monster || monster->dwAnimMode == MONMODE_DEATH || monster->dwAnimMode == MONMODE_DEAD) return 0;
    STATLIST_SetUnitStat(monster, STAT_HITPOINTS, 0, 0);
    SUNITDMG_KillMonster(g_game, monster, g_targetPlayer, 0);
    updateSessionSnapshot();
    return monster->dwAnimMode == MONMODE_DEATH || monster->dwAnimMode == MONMODE_DEAD ? 1 : 0;
}

// Fixture: give the player a quest item by code through the quest system's own item creation
// (QUESTS_CreateItem: placed in the inventory, never dropped). Returns the item's unit id or 0.
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_fixture_quest_item(uint32_t code) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return 0;
    D2UnitStrc* item = QUESTS_CreateItem(g_game, g_targetPlayer, code, 0, ITEMQUAL_NORMAL, 0);
    updateSessionSnapshot();
    return item ? item->dwUnitId : 0;
}

// Client packet 0x19 {itemGUID}: lift a stored inventory item onto the cursor.
EMSCRIPTEN_KEEPALIVE int d2_session_command_item_to_cursor_packet(uint32_t itemId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return -1;
    uint8_t packet[5] = { 0x19 };
    memcpy(packet + 1, &itemId, 4);
    const int32_t result = D2GAME_PACKETCALLBACK_Rcv0x19_RemoveItemFromBuffer_6FC850C0(g_game, g_targetPlayer, packet, sizeof packet);
    updateSessionSnapshot();
    return result;
}

// Client packet 0x1A {itemGUID, bodyLoc}: equip the cursor item to a body slot.
EMSCRIPTEN_KEEPALIVE int d2_session_command_equip_packet(uint32_t itemId, uint32_t bodyLoc) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return -1;
    uint8_t packet[9] = { 0x1A };
    memcpy(packet + 1, &itemId, 4); packet[5] = (uint8_t)bodyLoc;
    const int32_t result = D2GAME_PACKETCALLBACK_Rcv0x1A_EquipItem_6FC85280(g_game, g_targetPlayer, packet, sizeof packet);
    updateSessionSnapshot();
    return result;
}

// Client packet 0x44 {playerGUID, objectGUID, itemGUID, state}: the staff-in-orifice (and
// obelisk) exchange; state 3 offers the cursor item to the object the player is interacting with.
EMSCRIPTEN_KEEPALIVE int d2_session_command_item_to_object_packet(uint32_t objectId, uint32_t itemId, uint32_t state) {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return -1;
    D2GSPacketClt44 packet = {};
    packet.nHeader = 0x44; packet.nPlayerGUID = (int32_t)g_targetPlayer->dwUnitId;
    packet.nAnvilInteractGUID = (int32_t)objectId; packet.nItemGUID = (int32_t)itemId; packet.nState = (int16_t)state;
    const int32_t result = D2GAME_PACKETCALLBACK_Rcv0x44_StaffInOrifice_6FC87780(g_game, g_targetPlayer, &packet, sizeof packet);
    updateSessionSnapshot();
    return result;
}

// Fixture: prime Baal's throne AI so its wave sequence can begin. The throne Baal is present in
// the Throne of Destruction at runtime class 543 (the loaded MonStats offsets baalthrone by one
// from the reference enum's 544); its wave-spawn (AITHINK_Fn134_BaalThrone) advances only when
// AI-control param 1 bit 0 is set (retail restores it via SUNITINACTIVE_RestoreSpecialMonsterParameters).
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_prime_throne_baal() {
    if (!g_sessionLive || !g_game || !g_lastLevel) return 0;
    for (D2DrlgRoomStrc* roomEx = g_lastLevel->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext) {
        if (!roomEx->pRoom) continue;
        for (D2UnitStrc* u = roomEx->pRoom->pUnitFirst; u; u = u->pRoomNext) {
            if (u->dwUnitType == UNIT_MONSTER && (u->dwClassId == 543 || u->dwClassId == MONSTER_BAALTHRONE)) {
                AIGENERAL_SetAiControlParam(u, 1, 1);
                updateSessionSnapshot();
                return (int)u->dwUnitId;
            }
        }
    }
    return 0;
}

// Fixture: open Anya's portal to Nihlathak's Temple (ACT5Q4_AnyaOpenPortal), the portal Drehya
// spawns in Harrogath after her rescue. The waypoint-not-activated precondition (which the town
// and waypoint state would set) is supplied so the recovered routine runs; it creates the portal.
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_anya_open_portal() {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return 0;
    D2QuestDataStrc* quest = QUESTS_GetQuestData(g_game, QUEST_A5Q4_NIHLATHAK);
    if (!quest || !quest->pQuestDataEx) return 0;
    static_cast<D2Act5Quest4Strc*>(quest->pQuestDataEx)->bWaypointNotActivated = 1;
    ACT5Q4_AnyaOpenPortal(g_game, g_targetPlayer);
    updateSessionSnapshot();
    return 1;
}

// Fixture: open the Worldstone Chamber (ACT5Q6_OpenWorldStoneChamber), the state Baal's throne
// AI sets when its stair form reaches the portal; the throne portal (563) then warps to it.
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_open_worldstone() {
    if (!g_sessionLive || !g_game) return 0;
    ACT5Q6_OpenWorldStoneChamber(g_game);
    updateSessionSnapshot();
    return 1;
}

EMSCRIPTEN_KEEPALIVE int d2_session_fixture_kill_andariel() {
    if (!g_sessionLive || !g_game) return 0;
    D2UnitStrc* andariel = nullptr;
    for (const D2SessionUnit& unit : g_sessionUnits) {
        if (unit.type == UNIT_MONSTER && unit.classId == 156) {
            andariel = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)unit.id);
            break;
        }
    }
    D2QuestDataStrc* quest = QUESTS_GetQuestData(g_game, QUEST_A1Q6_ANDARIEL);
    if (!andariel || !quest || !quest->pfCallback[QUESTEVENT_MONSTERKILLED]) return 0;
    D2QuestArgStrc args = {}; args.pGame = g_game; args.nEvent = QUESTEVENT_MONSTERKILLED; args.pTarget = andariel;
    quest->pfCallback[QUESTEVENT_MONSTERKILLED](quest, &args);
    updateSessionSnapshot();
    return 1;
}

// Retail's player-attached kill path sets A1Q6's reward-pending and primary-goal bits before
// scheduling its timer. Keep it separately callable so the no-player global-state vector stays
// an exact, independent oracle.
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_kill_andariel_for_player() {
    if (!g_sessionLive || !g_game || !g_targetPlayer) return 0;
    D2UnitStrc* andariel = nullptr;
    for (const D2SessionUnit& unit : g_sessionUnits) {
        if (unit.type == UNIT_MONSTER && unit.classId == 156) {
            andariel = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)unit.id);
            break;
        }
    }
    D2QuestDataStrc* quest = QUESTS_GetQuestData(g_game, QUEST_A1Q6_ANDARIEL);
    if (!andariel || !quest || !quest->pfCallback[QUESTEVENT_MONSTERKILLED]) return 0;
    D2QuestArgStrc args = {}; args.pGame = g_game; args.nEvent = QUESTEVENT_MONSTERKILLED;
    args.pTarget = andariel; args.pPlayer = g_targetPlayer;
    // Level population already leaves items in Catacombs 4; the quest drop is what the kill adds.
    g_sessionAct1Quest6ItemsBefore.clear();
    for (const D2SessionUnit& unit : g_sessionUnits) if (unit.type == UNIT_ITEM) g_sessionAct1Quest6ItemsBefore.push_back(unit.id);
    quest->pfCallback[QUESTEVENT_MONSTERKILLED](quest, &args);
    updateSessionSnapshot();
    return 1;
}

// The player-attached kill rolls two chipped gems and one normal gem off the global quest seed
// and drops them at Andariel.  Report every item the kill added, in the same room/unit order
// and with the same ITEMS_Serialize bitstream the retail capture recorded.
EMSCRIPTEN_KEEPALIVE const D2SessionAct1Quest6Drop* d2_session_a1q6_drops() {
    g_sessionAct1Quest6Drops.clear();
    if (!g_sessionLive || !g_game || !g_lastLevel) return nullptr;
    for (D2DrlgRoomStrc* roomEx = g_lastLevel->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext) {
        if (!roomEx->pRoom) continue;
        for (D2UnitStrc* item = roomEx->pRoom->pUnitFirst; item; item = item->pRoomNext) {
            if (item->dwUnitType != UNIT_ITEM) continue;
            if (std::find(g_sessionAct1Quest6ItemsBefore.begin(), g_sessionAct1Quest6ItemsBefore.end(), (uint32_t)item->dwUnitId) != g_sessionAct1Quest6ItemsBefore.end()) continue;
            D2SessionAct1Quest6Drop out = {};
            out.itemId = item->dwUnitId; out.classId = item->dwClassId;
            out.quality = (uint32_t)ITEMS_GetItemQuality(item); out.ilvl = (uint32_t)ITEMS_GetItemLevel(item);
            if (item->pStaticPath) { out.x = (uint16_t)item->pStaticPath->tGameCoords.nX; out.y = (uint16_t)item->pStaticPath->tGameCoords.nY; }
            out.seedLo = item->pSeed.nLowSeed; out.seedHi = item->pSeed.nHighSeed;
            uint8_t bits[512] = {};
            out.bitBytes = (uint32_t)ITEMS_SerializeItemToBitstream(item, bits, sizeof bits, 1, 1, 0);
            memcpy(out.bits, bits, out.bitBytes < sizeof out.bits ? out.bitBytes : sizeof out.bits);
            g_sessionAct1Quest6Drops.push_back(out);
        }
    }
    for (size_t i = 0; i < g_sessionAct1Quest6Drops.size(); ++i) {
        D2SessionAct1Quest6Drop& out = g_sessionAct1Quest6Drops[i];
        out.index = (uint32_t)i; out.count = (uint32_t)g_sessionAct1Quest6Drops.size();
        // The retail record's hash covers its packed 76-byte prefix; recompute it over that layout.
        uint8_t packed[76] = {};
        uint32_t seed = g_game->dwInitSeed; uint16_t level = (uint16_t)g_currentLevel;
        uint8_t difficulty = g_game->nDifficulty;
        memcpy(packed + 0, &seed, 4); packed[4] = difficulty; packed[5] = 0; memcpy(packed + 6, &level, 2);
        memcpy(packed + 8, &out.itemId, 4); memcpy(packed + 12, &out.classId, 4); memcpy(packed + 16, &out.quality, 4); memcpy(packed + 20, &out.ilvl, 4);
        uint16_t x = (uint16_t)out.x, y = (uint16_t)out.y, index = (uint16_t)out.index, count = (uint16_t)out.count;
        memcpy(packed + 24, &x, 2); memcpy(packed + 26, &y, 2); memcpy(packed + 28, &index, 2); memcpy(packed + 30, &count, 2);
        memcpy(packed + 32, &out.seedLo, 4); memcpy(packed + 36, &out.seedHi, 4); memcpy(packed + 40, &out.bitBytes, 4);
        memcpy(packed + 44, out.bits, 32);
        out.hash = sessionHashBytes(D2VEC_FNV_INIT, packed, sizeof packed);
    }
    return g_sessionAct1Quest6Drops.data();
}
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_a1q6_drop_count() { return (uint32_t)g_sessionAct1Quest6Drops.size(); }

// Replay the live scheduled A1Q6 timer, rather than a browser-owned countdown. At invocation
// 10 its recovered callback opens the normal class-59 Catacombs return portal for the player.
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_a1q6_timer_to_portal() {
    memset(&g_sessionAct1Quest6Timer, 0, sizeof g_sessionAct1Quest6Timer);
    D2SessionAct1Quest6Timer& out = g_sessionAct1Quest6Timer;
    out.version = 1; out.size = sizeof out; out.level = g_currentLevel;
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_game->pQuestControl) return 0;
    D2QuestTimerStrc* timer = g_game->pQuestControl->pTimer;
    D2QuestDataStrc* quest = QUESTS_GetQuestData(g_game, QUEST_A1Q6_ANDARIEL);
    if (!timer || !quest || timer->pQuest != quest || !timer->pfUpdate) return 0;
    D2Act1Quest6Strc* ex = static_cast<D2Act1Quest6Strc*>(quest->pQuestDataEx);
    if (!ex || ex->nTimerInvocations != 1) return 0;
    for (int i = 0; i < 9; ++i) timer->pfUpdate(g_game, quest);
    out.andarielId = (uint32_t)ex->nAndarielGUID;
    out.timerInvocations = ex->nTimerInvocations;
    out.timerStillScheduled = g_game->pQuestControl->pTimer != nullptr;
    updateSessionWorld();
    for (const D2SessionObjectPresentation& object : g_sessionObjectPresentations) {
        if (object.classId != 59) continue;
        ++out.portalCount; out.portalId = object.id; out.portalClass = object.classId;
    }
    out.hash = sessionHashBytes(D2VEC_FNV_INIT, &out, offsetof(D2SessionAct1Quest6Timer, hash));
    return out.timerInvocations == 10 && out.portalCount == 1;
}
EMSCRIPTEN_KEEPALIVE const D2SessionAct1Quest6Timer* d2_session_act1q6_timer() { return &g_sessionAct1Quest6Timer; }

// Invocations 11-12 of the same live timer: retail's status cycle sets lastState 3, clears the
// quest flag byte, composes the player's 0x5D status packet, and releases the timer (return 1).
// The callback is driven directly, as the capture does, so the return value is the release.
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_a1q6_timer_to_completion() {
    memset(&g_sessionAct1Quest6Complete, 0, sizeof g_sessionAct1Quest6Complete);
    D2SessionAct1Quest6Complete& out = g_sessionAct1Quest6Complete;
    out.version = 1; out.size = sizeof out; out.level = g_currentLevel;
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_game->pQuestControl) return 0;
    D2QuestTimerStrc* timer = g_game->pQuestControl->pTimer;
    D2QuestDataStrc* quest = QUESTS_GetQuestData(g_game, QUEST_A1Q6_ANDARIEL);
    if (!timer || !quest || timer->pQuest != quest || !timer->pfUpdate) return 0;
    D2Act1Quest6Strc* ex = static_cast<D2Act1Quest6Strc*>(quest->pQuestDataEx);
    if (!ex || ex->nTimerInvocations != 10) return 0;
    D2WEB_ResetLastQuestStatusPacket();
    timer->pfUpdate(g_game, quest);
    out.timerReleased = timer->pfUpdate(g_game, quest);
    out.timerInvocations = ex->nTimerInvocations;
    out.lastState = quest->fLastState; out.state = quest->fState; out.questFlagsLow = quest->dwFlags & 0xFF;
    D2BitBufferStrc* flags = UNITS_GetPlayerData(g_targetPlayer)->pQuestData[g_game->nDifficulty];
    for (uint32_t bit = 0; bit < 16; ++bit)
        if (QUESTRECORD_GetQuestState(flags, QUESTSTATEFLAG_A1Q6, bit)) out.playerFlags |= 1u << bit;
    D2GSPacketSrv5D packet = {};
    if (const int count = D2WEB_CopyLastQuestStatusPacket(&packet)) {
        out.packetHeader = packet.nHeader; out.packetQuestId = packet.nQuestId; out.packetFlags = packet.nFlags;
        out.packetState = packet.nState; out.packetField4 = packet.field_4; out.packetBytes = count * sizeof packet;
    }
    out.hash = sessionHashBytes(D2VEC_FNV_INIT, &out, offsetof(D2SessionAct1Quest6Complete, hash));
    return out.timerInvocations == 12 && out.packetHeader == 0x5D;
}
EMSCRIPTEN_KEEPALIVE const D2SessionAct1Quest6Complete* d2_session_act1q6_complete() { return &g_sessionAct1Quest6Complete; }

// GAME_UpdateProgress's pending-rooms step (D2Game, D2MOO GAME/Game.cpp), run where retail runs it:
// at the start of a frame, before events and the client update. The client-room change
// (sub_6FC33670, below) is retail's activation wave -- it creates active rooms around the player
// and the DRLG raises the act's bHasPendingRoomsUpdates -- and THIS is what populates them:
// sub_6FC385A0 spawns a new room's presets, objects and monsters, or restores an old one's.
// Without it, rooms reached by walking stayed empty (s000: the Blood Moor held only the player).
static void updateSessionPendingRooms() {
    for (int act = 0; act < 5; ++act) {
        D2DrlgActStrc* pAct = g_game->pAct[act];
        if (!pAct || !pAct->bHasPendingRoomsUpdates) continue;
        for (D2ActiveRoomStrc* room = DUNGEON_GetRoomFromAct(pAct); room; room = room->pRoomNext)
            sub_6FC385A0(g_game, room);
        pAct->bHasPendingRoomsUpdates = 0;
    }
}

EMSCRIPTEN_KEEPALIVE int d2_session_tick(int frames) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pDynamicPath || frames < 0) return 0;
    for (int i = 0; i < frames; ++i) {
        updateSessionPendingRooms();
        const bool wasMoving = g_sessionMovePending;
        if (g_sessionAttack.state == 2) {
            D2ActiveRoomStrc* room = g_targetPlayer->pDynamicPath->pRoom;
            SUNIT_SetCombatMode(g_game, g_targetPlayer,
                room && DUNGEON_IsRoomInTown(room) ? PLRMODE_TOWNNEUTRAL : PLRMODE_NEUTRAL);
            D2GAME_DeletePlayerPerFrameEvents_6FCBCE50(g_game, g_targetPlayer);
            UNITS_SetUsedSkill(g_targetPlayer, nullptr); UNITS_SetTargetUnitForDynamicUnit(g_targetPlayer, nullptr);
            g_sessionAttack.state = 3;
        }
        // Deferred item interaction still relies on the headless session's legacy pre-event path
        // step. Keep it until the missing retail interaction-completion callback is recovered.
        if (g_sessionMovePending && !D2Common_10226(g_targetPlayer, 1024)) {
            D2ActiveRoomStrc* room = g_targetPlayer->pDynamicPath->pRoom;
            SUNIT_SetCombatMode(g_game, g_targetPlayer,
                room && DUNGEON_IsRoomInTown(room) ? PLRMODE_TOWNNEUTRAL : PLRMODE_NEUTRAL);
        }
        if (!d2_aiframe_advance(1)) return 0;
        sub_6FC33670(g_game, g_targetClient);
        if (g_sessionAttack.state == 1 && !g_sessionAttackResolved && g_sessionAttack.eventFrame &&
            g_game->dwGameFrame >= g_sessionAttack.eventFrame) {
            g_sessionAttackResolved = true;
        }
        if (g_sessionAttack.state == 1 && g_sessionAttackEndFrame &&
            g_game->dwGameFrame >= g_sessionAttackEndFrame) g_sessionAttack.state = 2;
        if (g_sessionAttackTarget && !g_sessionTargetKillCounted &&
            STATLIST_UnitGetStatValue(g_sessionAttackTarget, STAT_HITPOINTS, 0) <= 0) {
            g_sessionTargetKillCounted = true; ++g_sessionKills;
        }
        D2DynamicPathStrc* path = g_targetPlayer->pDynamicPath;
        g_sessionMovePending = path->dwPathPoints > 0 && path->dwCurrentPointIdx < path->dwPathPoints;
        if (wasMoving) syncSessionLevelFromPlayer();
    }
    updateSessionSnapshot(); return 1;
}

// Why the last primary attack was refused (0 = not refused), so a refusal is never guessed at.
static int g_sessionAttackReason = 0;
enum { ATTACK_OK = 0, ATTACK_NO_SESSION = 1, ATTACK_BUSY = 2, ATTACK_NO_TARGET = 3, ATTACK_NOT_NEAR = 4,
       ATTACK_TARGET_DEAD = 5, ATTACK_UNTARGETABLE = 6, ATTACK_NO_SKILL = 7, ATTACK_NO_PATH = 8 };
EMSCRIPTEN_KEEPALIVE int d2_session_attack_reason() { return g_sessionAttackReason; }
// A target is reachable from the player's room or any room adjacent to it. The session used to
// require the SAME room, which retail does not: a monster standing across a room seam could not be
// fought at all (s000, Blood Moor). Distance is still decided by melee range and the walk below.
static bool sessionRoomIsNear(D2ActiveRoomStrc* from, D2ActiveRoomStrc* room) {
    if (!from || !room) return false;
    if (from == room) return true;
    D2ActiveRoomStrc** near = nullptr; int32_t count = 0;
    DUNGEON_GetAdjacentRoomsListFromRoom(from, &near, &count);
    for (int32_t i = 0; i < count; ++i) if (near[i] == room) return true;
    return false;
}
EMSCRIPTEN_KEEPALIVE int d2_session_command_primary_attack(uint32_t targetUnitId) {
    g_sessionAttackReason = ATTACK_NO_SESSION;
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pDynamicPath ||
        !g_targetPlayer->pDynamicPath->pRoom || g_targetPlayer->dwAnimMode == PLRMODE_DEAD) return 0;
    g_sessionAttackReason = ATTACK_BUSY;
    if (g_sessionAttack.state == 1 || g_sessionAttack.state == 2) return 0;
    D2UnitStrc* target = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, targetUnitId);
    g_sessionAttackReason = ATTACK_NO_TARGET;
    if (!target || !target->pDynamicPath) return 0;
    g_sessionAttackReason = ATTACK_NOT_NEAR;
    if (!sessionRoomIsNear(g_targetPlayer->pDynamicPath->pRoom, target->pDynamicPath->pRoom)) return 0;
    g_sessionAttackReason = ATTACK_TARGET_DEAD;
    if (target->dwAnimMode == MONMODE_DEAD || STATLIST_UnitGetStatValue(target, STAT_HITPOINTS, 0) <= 0) return 0;
    g_sessionAttackReason = ATTACK_UNTARGETABLE;
    if (!(target->dwFlags & UNITFLAG_TARGETABLE)) return 0;
    g_sessionAttackReason = ATTACK_OK;
    if (!UNITS_IsInMeleeRange(g_targetPlayer, target, 1)) {
        PLRMODE_StartID_WalkRunKnockback(g_game, g_targetPlayer, PLRMODE_WALK, target);
        const D2DynamicPathStrc* path = g_targetPlayer->pDynamicPath;
        g_sessionMovePending = path->dwPathPoints > 0;
        g_sessionTargetX = target->pDynamicPath->tGameCoords.wPosX;
        g_sessionTargetY = target->pDynamicPath->tGameCoords.wPosY;
        if (!g_sessionMovePending) g_sessionAttackReason = ATTACK_NO_PATH;
        updateSessionSnapshot(); return g_sessionMovePending ? 2 : 0;
    }
    D2SkillStrc* skill = SKILLS_GetHighestLevelSkillFromUnitAndId(g_targetPlayer, SKILL_ATTACK);
    if (!skill) { g_sessionAttackReason = ATTACK_NO_SKILL; return 0; }
    memset(&g_sessionAttack, 0, sizeof g_sessionAttack); g_sessionAttack.version = 1;
    g_sessionAttack.size = sizeof g_sessionAttack; g_sessionAttack.gameFrame = g_game->dwGameFrame;
    g_sessionAttack.state = 1; g_sessionAttack.attackerId = g_targetPlayer->dwUnitId;
    g_sessionAttack.targetId = target->dwUnitId; g_sessionAttack.skillId = SKILL_ATTACK;
    g_sessionAttack.targetHpBefore = STATLIST_UnitGetStatValue(target, STAT_HITPOINTS, 0);
    g_sessionAttack.targetHpAfter = g_sessionAttack.targetHpBefore; g_sessionAttack.attacks = ++g_sessionAttackCount;
    g_sessionAttackTarget = target; g_sessionAttackSkill = skill;
    g_sessionAttackResolved = false; g_sessionTargetKillCounted = false;
    UNITS_SetUsedSkill(g_targetPlayer, skill); UNITS_SetTargetUnitForDynamicUnit(g_targetPlayer, target);
    SUNIT_SetCombatMode(g_game, g_targetPlayer, PLRMODE_ATTACK1);
    g_targetPlayer->dwFlags &= ~UNITFLAG_SKSRVDOFUNC;
    SKILLS_SrvSt01_Attack_LeftHandSwing(g_game, g_targetPlayer, SKILL_ATTACK,
        SKILLS_GetSkillLevel(g_targetPlayer, skill, TRUE));
    sub_6FCBCE70(g_game, g_targetPlayer);
    g_sessionAttack.eventFrame = EVENT_GetEventFrame(g_game, g_targetPlayer, EVENTTYPE_MODECHANGE);
    g_sessionAttackEndFrame = EVENT_GetEventFrame(g_game, g_targetPlayer, EVENTTYPE_ENDANIM);
    g_sessionAttack.mode = g_targetPlayer->dwAnimMode;
    return 1;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_throw(uint32_t targetUnitId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pDynamicPath ||
        g_targetPlayer->dwAnimMode == PLRMODE_DEAD || g_sessionAttack.state == 1 || g_sessionAttack.state == 2) return 0;
    D2UnitStrc* target = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)targetUnitId);
    D2SkillStrc* skill = SKILLS_GetHighestLevelSkillFromUnitAndId(g_targetPlayer, SKILL_THROW);
    if (!target || !target->pDynamicPath || !skill || SKILLS_GetUseState(g_targetPlayer, skill) != SKILLUSTATE_USABLE ||
        target->dwAnimMode == MONMODE_DEAD || !(target->dwFlags & UNITFLAG_TARGETABLE)) return 0;
    memset(&g_sessionAttack, 0, sizeof g_sessionAttack); g_sessionAttack.version = 1;
    g_sessionAttack.size = sizeof g_sessionAttack; g_sessionAttack.gameFrame = g_game->dwGameFrame;
    g_sessionAttack.state = 1; g_sessionAttack.attackerId = g_targetPlayer->dwUnitId;
    g_sessionAttack.targetId = target->dwUnitId; g_sessionAttack.skillId = SKILL_THROW;
    g_sessionAttack.targetHpBefore = STATLIST_UnitGetStatValue(target, STAT_HITPOINTS, 0);
    g_sessionAttack.targetHpAfter = g_sessionAttack.targetHpBefore; g_sessionAttack.attacks = ++g_sessionAttackCount;
    g_sessionAttackTarget = target; g_sessionAttackSkill = skill; g_sessionAttackResolved = false;
    const int32_t result = sub_6FC836D0(g_game, g_targetPlayer, skill, UNIT_MONSTER, (int32_t)targetUnitId, 1);
    if (result != 0) { g_sessionAttack.state = 0; return 0; }
    g_sessionAttack.eventFrame = EVENT_GetEventFrame(g_game, g_targetPlayer, EVENTTYPE_MODECHANGE);
    g_sessionAttackEndFrame = EVENT_GetEventFrame(g_game, g_targetPlayer, EVENTTYPE_ENDANIM);
    g_sessionAttack.mode = g_targetPlayer->dwAnimMode;
    return 1;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_kick(uint32_t targetUnitId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pDynamicPath ||
        g_targetPlayer->dwAnimMode == PLRMODE_DEAD || g_sessionAttack.state == 1 || g_sessionAttack.state == 2) return 0;
    D2UnitStrc* target = SUNIT_GetServerUnit(g_game, UNIT_MONSTER, (int32_t)targetUnitId);
    D2SkillStrc* skill = SKILLS_GetHighestLevelSkillFromUnitAndId(g_targetPlayer, SKILL_KICK);
    if (!target || !target->pDynamicPath || !skill || SKILLS_GetUseState(g_targetPlayer, skill) != SKILLUSTATE_USABLE ||
        target->dwAnimMode == MONMODE_DEAD || !(target->dwFlags & UNITFLAG_TARGETABLE)) return 0;
    memset(&g_sessionAttack, 0, sizeof g_sessionAttack); g_sessionAttack.version = 1;
    g_sessionAttack.size = sizeof g_sessionAttack; g_sessionAttack.gameFrame = g_game->dwGameFrame;
    g_sessionAttack.state = 1; g_sessionAttack.attackerId = g_targetPlayer->dwUnitId;
    g_sessionAttack.targetId = target->dwUnitId; g_sessionAttack.skillId = SKILL_KICK;
    g_sessionAttack.targetHpBefore = STATLIST_UnitGetStatValue(target, STAT_HITPOINTS, 0);
    g_sessionAttack.targetHpAfter = g_sessionAttack.targetHpBefore; g_sessionAttack.attacks = ++g_sessionAttackCount;
    g_sessionAttackTarget = target; g_sessionAttackSkill = skill; g_sessionAttackResolved = false;
    const int32_t result = sub_6FC836D0(g_game, g_targetPlayer, skill, UNIT_MONSTER, (int32_t)targetUnitId, 1);
    if (result != 0) { g_sessionAttack.state = 0; return 0; }
    g_sessionAttack.eventFrame = EVENT_GetEventFrame(g_game, g_targetPlayer, EVENTTYPE_MODECHANGE);
    g_sessionAttackEndFrame = EVENT_GetEventFrame(g_game, g_targetPlayer, EVENTTYPE_ENDANIM);
    g_sessionAttack.mode = g_targetPlayer->dwAnimMode;
    return 1;
}

EMSCRIPTEN_KEEPALIVE int d2_session_equipped_quantity() {
    if (!g_sessionLive || !g_targetPlayer || !g_targetPlayer->pInventory) return -1;
    D2UnitStrc* item = INVENTORY_GetItemFromBodyLoc(g_targetPlayer->pInventory, BODYLOC_RARM);
    return item ? STATLIST_UnitGetStatValue(item, STAT_QUANTITY, 0) : -1;
}

// Deterministic progression fixture: initialize experience through the recovered stat list,
// then let the normal kill/experience path decide whether a level is earned. This never writes
// level or award points and is accepted only on an otherwise fresh level-1 player.
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_experience(uint32_t experience) {
    if (!g_sessionLive || !g_targetPlayer ||
        STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_LEVEL, 0) != 1 ||
        STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_EXPERIENCE, 0) != 0 ||
        STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_STATPTS, 0) != 0 ||
        STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_SKILLPTS, 0) != 0) return 0;
    const uint32_t threshold = DATATBLS_GetLevelThreshold(g_targetPlayer->dwClassId, 1);
    if (experience >= threshold) return 0;
    STATLIST_SetUnitStat(g_targetPlayer, STAT_EXPERIENCE, (int32_t)experience, 0);
    STATLIST_SetUnitStat(g_targetPlayer, STAT_LASTEXP, 0, 0);
    g_sessionInitialExperience = (int32_t)experience;
    return 1;
}

// Fixture: raise the player to a level the way a kill's experience does
// (SUNITDMG_AddExperienceForPlayer: experience to the level's threshold, then PLAYERSTATS_LevelUp).
EMSCRIPTEN_KEEPALIVE int d2_session_fixture_level(uint32_t level) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || level < 1) return 0;
    const int32_t classId = (int32_t)g_targetPlayer->dwClassId;
    if ((int32_t)level > DATATBLS_GetMaxLevel(classId)) return 0;
    const uint32_t current = (uint32_t)STATLIST_UnitGetStatValue(g_targetPlayer, STAT_LEVEL, 0);
    const uint32_t experience = (uint32_t)STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_EXPERIENCE, 0);
    const uint32_t target = DATATBLS_GetLevelThreshold(classId, (int32_t)level - 1);
    if (target > experience) SUNITDMG_AddExperienceForPlayer(g_game, g_targetPlayer, current, target - experience);
    updateSessionSnapshot();
    return STATLIST_UnitGetStatValue(g_targetPlayer, STAT_LEVEL, 0) == (int32_t)level;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_add_stat(uint32_t statId, uint32_t count) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || statId > 15 || count < 1 || count > 100) return 3;
    uint8_t packet[3] = { 0x3A, 0, 0 };
    const uint16_t encoded = (uint16_t)(statId | ((count - 1) << 8));
    memcpy(packet + 1, &encoded, sizeof encoded);
    return D2GAME_PACKETCALLBACK_Rcv0x3A_AddStatPoint_6FC86E80(g_game, g_targetPlayer, packet, sizeof packet);
}

// The player's class skill tree, straight out of Skills.txt and SkillDesc.txt.
//
// The client cannot invent this: which skills a class has, which of the three tabs each sits on and
// at which row and column are retail table data, and so is the level a skill needs. Without it a
// skill tree would be a hand-drawn picture of one. The CURRENT level of
// each skill comes from the player's own skill list, so the panel shows what the character has, not
// what the table allows.
//
// One record per skill of the player's class, in skill-id order:
//   id, page (1..3), row, column, requiredLevel, baseLevel, iconCel, nameStr
// iconCel indexes SkillIcon.dc6, so the panel draws retail's icon for the skill rather than one
// picked by position; nameStr is the .tbl index of the skill's name, for d2_lang_string_by_index.
struct D2SessionSkill { uint16_t id, page, row, column, requiredLevel, baseLevel, iconCel, nameStr; };

EMSCRIPTEN_KEEPALIVE uint32_t d2_session_class_skills(D2SessionSkill* out, uint32_t cap) {
    if (!g_sessionLive || !g_targetPlayer || !sgptDataTables || !out) return 0;
    const int charClass = (int)g_targetPlayer->dwClassId;
    uint32_t n = 0;
    for (int id = 0; id < sgptDataTables->nSkillsTxtRecordCount && n < cap; ++id) {
        D2SkillsTxt* skill = DATATBLS_GetSkillsTxtRecord(id);
        if (!skill || skill->nCharClass != charClass) continue;
        D2SkillDescTxt* desc = DATATBLS_GetSkillDescTxtRecord(skill->wSkillDesc);
        // A skill with no description record has no place on the tree to draw it at.
        if (!desc || !desc->nSkillPage) continue;
        D2SessionSkill& row = out[n++];
        row.id = (uint16_t)id;
        row.page = desc->nSkillPage; row.row = desc->nSkillRow; row.column = desc->nSkillColumn;
        row.requiredLevel = skill->wReqLevel;
        row.iconCel = desc->nIconCel; row.nameStr = desc->wStrName;
        D2SkillStrc* known = SKILLS_GetHighestLevelSkillFromUnitAndId(g_targetPlayer, id);
        row.baseLevel = (uint16_t)(known ? SKILLS_GetSkillLevel(g_targetPlayer, known, FALSE) : 0);
    }
    return n;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_add_skill(uint32_t skillId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || skillId > UINT16_MAX) return 3;
    uint8_t packet[3] = { 0x3B, 0, 0 };
    const uint16_t encoded = (uint16_t)skillId;
    memcpy(packet + 1, &encoded, sizeof encoded);
    const int result = D2GAME_PACKETCALLBACK_Rcv0x3B_AddSkillPoint_6FC86EF0(g_game, g_targetPlayer, packet, sizeof packet);
    if (result == 0) g_sessionAdvancementSkillId = skillId;
    return result;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_pickup(uint32_t itemUnitId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pInventory) return 0;
    D2UnitStrc* item = SUNIT_GetServerUnit(g_game, UNIT_ITEM, (int32_t)itemUnitId);
    if (!item || item->dwAnimMode != IMODE_ONGROUND || !UNITS_GetRoom(item)) return 0;
    g_sessionItemState.commandTarget = itemUnitId;
    sub_6FC828D0(g_targetPlayer, UNIT_ITEM, (int32_t)itemUnitId, 0, g_game);
    if (INVENTORY_IsItemInInventory(g_targetPlayer->pInventory, item)) {
        g_sessionItemState.result = 1;
        return 1;
    }
    if (item->dwAnimMode == IMODE_ONGROUND && UNITS_GetRoom(item)) {
        g_sessionItemState.result = 2;
        return 2;
    }
    g_sessionItemState.result = 0;
    return 0;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_drop(uint32_t itemUnitId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pInventory) return 0;
    D2UnitStrc* item = SUNIT_GetServerUnit(g_game, UNIT_ITEM, (int32_t)itemUnitId);
    if (!item || item->dwAnimMode != IMODE_STORED || ITEMS_GetInvPage(item) != INVPAGE_INVENTORY ||
        !INVENTORY_CompareWithItemsParentInventory(g_targetPlayer->pInventory, item)) return 0;
    int32_t removeError = 0;
    if (!sub_6FC446B0(g_game, g_targetPlayer, (int32_t)itemUnitId, &removeError, 1, 0, nullptr, 0)) return 0;
    if (INVENTORY_GetCursorItem(g_targetPlayer->pInventory) != item) return 0;
    sub_6FC44030(g_game, g_targetPlayer, (int32_t)itemUnitId);
    updateSessionSnapshot();
    return item->dwAnimMode == IMODE_ONGROUND && UNITS_GetRoom(item) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_use_belt(uint32_t slot) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pInventory || slot >= 16) return 0;
    D2UnitStrc* item = INVENTORY_GetItemFromBeltSlot(g_targetPlayer->pInventory, (int32_t)slot);
    if (!item || item->dwAnimMode != IMODE_INBELT ||
        !INVENTORY_CompareWithItemsParentInventory(g_targetPlayer->pInventory, item)) return 0;
    int32_t error = 0;
    const int32_t id = item->dwUnitId;
    if (!sub_6FC49220(g_game, g_targetPlayer, id, CLIENTS_GetUnitX(g_targetPlayer), CLIENTS_GetUnitY(g_targetPlayer), &error, 0)) return 0;
    updateSessionSnapshot();
    return SUNIT_GetServerUnit(g_game, UNIT_ITEM, id) ? 0 : 1;
}

// Right-clicking an item in the inventory: retail's 0x20 UseGridItem, through its own handler (a
// Town Portal scroll opens a portal, a potion is drunk, a tome is read). 1 if the handler accepted.
EMSCRIPTEN_KEEPALIVE int d2_session_command_use_item(uint32_t itemUnitId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pDynamicPath) return 0;
    uint8_t packet[13] = { 0x20 };
    const int32_t id = (int32_t)itemUnitId;
    const int32_t x = g_targetPlayer->pDynamicPath->tGameCoords.wPosX, y = g_targetPlayer->pDynamicPath->tGameCoords.wPosY;
    memcpy(packet + 1, &id, 4); memcpy(packet + 5, &x, 4); memcpy(packet + 9, &y, 4);
    const int32_t result = D2GAME_PACKETCALLBACK_Rcv0x20_UseGridItem_6FC85990(g_game, g_targetPlayer, packet, sizeof packet);
    updateSessionSnapshot();
    return result == 0 ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_stow_body(uint32_t bodyLoc) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pInventory || bodyLoc >= 13) return 0;
    int32_t result = 0;
    if (!sub_6FC45B30(g_game, g_targetPlayer, (uint8_t)bodyLoc, 0, &result)) return 0;
    D2UnitStrc* cursor = INVENTORY_GetCursorItem(g_targetPlayer->pInventory);
    if (!cursor || !sub_6FC43E60(g_game, g_targetPlayer, cursor, 1, 0, __FILE__, __LINE__)) return 0;
    updateSessionSnapshot();
    return INVENTORY_GetItemFromBodyLoc(g_targetPlayer->pInventory, (int32_t)bodyLoc) ? 0 : 1;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_respawn() {
    if (!g_sessionLive || !g_game || !g_targetPlayer || g_targetPlayer->dwAnimMode != PLRMODE_DEAD) return 0;
    uint8_t packet = 0x41;
    const int32_t oldLevel = DUNGEON_GetLevelIdFromRoom(UNITS_GetRoom(g_targetPlayer));
    const int32_t result = D2GAME_PACKETCALLBACK_Rcv0x41_Resurrect_6FC87480(g_game, g_targetPlayer, &packet, 1);
    // Resurrect moves the player to town (D2GAME_PlayerChangeAct): the session adopts the new level
    // as it does after an exit, a portal or a caravan, or it keeps the level the body lies in.
    D2ActiveRoomStrc* room = UNITS_GetRoom(g_targetPlayer);
    if (result == 0 && room && DUNGEON_GetLevelIdFromRoom(room) != oldLevel) adoptSessionLevelChange(oldLevel, DUNGEON_GetLevelIdFromRoom(room));
    updateSessionSnapshot();
    return result == 0 && g_targetPlayer->dwAnimMode != PLRMODE_DEAD ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int d2_session_command_recover_corpse(uint32_t corpseId) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pInventory) return 0;
    D2UnitStrc* corpse = SUNIT_GetServerUnit(g_game, UNIT_PLAYER, (int32_t)corpseId);
    if (!corpse || corpse == g_targetPlayer || corpse->dwAnimMode != PLRMODE_DEAD ||
        !STATES_CheckState(corpse, STATE_PLAYERBODY) || INVENTORY_GetOwnerId(corpse->pInventory) != g_targetPlayer->dwUnitId) return 0;
    sub_6FC80440(g_game, g_targetPlayer, corpse);
    updateSessionSnapshot();
    return SUNIT_GetServerUnit(g_game, UNIT_PLAYER, (int32_t)corpseId) ? 0 : 1;
}

static uint32_t sessionInventoryItemCount(const D2InventoryStrc* inventory) {
    if (!inventory) return 0;
    uint32_t count = INVENTORY_GetCursorItem(const_cast<D2InventoryStrc*>(inventory)) ? 1u : 0u;
    for (D2UnitStrc* item = INVENTORY_GetFirstItem(const_cast<D2InventoryStrc*>(inventory));
         item; item = INVENTORY_GetNextItem(item)) {
        if (INVENTORY_UnitIsItem(item)) ++count;
    }
    return count;
}

// Executes the recovered retail MJ item-section writer and current-version reader against the
// live player.  The returned record and byte buffer contain no wasm pointers.
EMSCRIPTEN_KEEPALIVE const D2SessionItemSaveRoundTrip* d2_session_item_save_roundtrip() {
    memset(&g_sessionItemSaveRoundTrip, 0, sizeof g_sessionItemSaveRoundTrip);
    g_sessionItemSaveRoundTrip.version = 1;
    g_sessionItemSaveRoundTrip.size = sizeof g_sessionItemSaveRoundTrip;
    g_sessionItemSaveRoundTrip.saveVersion = 96;
    g_sessionItemSaveBytes.clear();
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pInventory) {
        g_sessionItemSaveRoundTrip.result = 1;
        return &g_sessionItemSaveRoundTrip;
    }

    constexpr uint32_t kCapacity = 64u * 1024u;
    g_sessionItemSaveBytes.resize(kCapacity);
    g_sessionItemSaveRoundTrip.beforeInventoryCount = sessionInventoryItemCount(g_targetPlayer->pInventory);
    const int32_t written = PLRSAVE_WriteItems_6FC89B50(g_targetPlayer->pInventory,
        g_sessionItemSaveBytes.data(), kCapacity, 0, 0);
    if (written < 4 || (uint32_t)written > kCapacity) {
        g_sessionItemSaveBytes.clear();
        g_sessionItemSaveRoundTrip.result = 2;
        return &g_sessionItemSaveRoundTrip;
    }
    g_sessionItemSaveBytes.resize((size_t)written);
    g_sessionItemSaveRoundTrip.bytesWritten = (uint32_t)written;
    g_sessionItemSaveRoundTrip.itemCount = (uint32_t)g_sessionItemSaveBytes[2] |
        ((uint32_t)g_sessionItemSaveBytes[3] << 8);
    g_sessionItemSaveRoundTrip.sectionHash = sessionHashBytes(D2VEC_FNV_INIT,
        g_sessionItemSaveBytes.data(), g_sessionItemSaveBytes.size());
    if (g_sessionItemSaveBytes[0] == 'J' && g_sessionItemSaveBytes[1] == 'M')
        g_sessionItemSaveRoundTrip.flags |= 1u;
    if (g_sessionItemSaveRoundTrip.itemCount == g_sessionItemSaveRoundTrip.beforeInventoryCount)
        g_sessionItemSaveRoundTrip.flags |= 2u;

    SUNIT_RemoveAllItemsFromInventory(g_game, g_targetPlayer);
    int32_t consumed = 0;
    const int32_t readResult = PLRSAVE_ReadItems_6FC8B8A0(g_game, g_targetPlayer,
        g_sessionItemSaveBytes.data(), 96, written, 0, &consumed);
    g_sessionItemSaveRoundTrip.bytesConsumed = consumed > 0 ? (uint32_t)consumed : 0;
    if (readResult != 0) {
        g_sessionItemSaveRoundTrip.result = 0x100u + (uint32_t)readResult;
        return &g_sessionItemSaveRoundTrip;
    }
    if (consumed == written) g_sessionItemSaveRoundTrip.flags |= 4u;
    g_sessionItemSaveRoundTrip.afterInventoryCount = sessionInventoryItemCount(g_targetPlayer->pInventory);
    if (g_sessionItemSaveRoundTrip.afterInventoryCount == g_sessionItemSaveRoundTrip.beforeInventoryCount)
        g_sessionItemSaveRoundTrip.flags |= 8u;

    std::vector<uint8_t> replay(kCapacity);
    const int32_t replayWritten = PLRSAVE_WriteItems_6FC89B50(g_targetPlayer->pInventory,
        replay.data(), kCapacity, 0, 0);
    if (replayWritten > 0 && (uint32_t)replayWritten <= kCapacity) {
        g_sessionItemSaveRoundTrip.replayBytes = (uint32_t)replayWritten;
        g_sessionItemSaveRoundTrip.replayHash = sessionHashBytes(D2VEC_FNV_INIT, replay.data(), (size_t)replayWritten);
        if (replayWritten == written && !memcmp(replay.data(), g_sessionItemSaveBytes.data(), (size_t)written))
            g_sessionItemSaveRoundTrip.flags |= 16u;
    }
    g_sessionItemSaveRoundTrip.result = (g_sessionItemSaveRoundTrip.flags & 15u) == 15u ? 0u : 3u;
    updateSessionSnapshot();
    g_sessionItemSaveRoundTrip.hash = sessionHashBytes(D2VEC_FNV_INIT, &g_sessionItemSaveRoundTrip,
        offsetof(D2SessionItemSaveRoundTrip, hash));
    return &g_sessionItemSaveRoundTrip;
}

EMSCRIPTEN_KEEPALIVE const uint8_t* d2_session_item_save_bytes() {
    return g_sessionItemSaveBytes.data();
}

EMSCRIPTEN_KEEPALIVE uint32_t d2_session_item_save_length() {
    return (uint32_t)g_sessionItemSaveBytes.size();
}

static uint32_t sessionBaseStatsHash(D2UnitStrc* player, uint32_t* countOut) {
    D2StatStrc stats[512] = {};
    const int count = player && player->pStatListEx
        ? STATLIST_GetBaseStatsData((D2StatListStrc*)player->pStatListEx, stats, 512) : 0;
    std::sort(stats, stats + count, [](const D2StatStrc& a, const D2StatStrc& b) {
        return a.nStat != b.nStat ? a.nStat < b.nStat : a.nLayer < b.nLayer;
    });
    uint32_t hash = D2VEC_FNV_INIT;
    for (int i = 0; i < count; ++i) {
        hash = sessionHashBytes(hash, &stats[i].nStat, sizeof stats[i].nStat);
        hash = sessionHashBytes(hash, &stats[i].nLayer, sizeof stats[i].nLayer);
        hash = sessionHashBytes(hash, &stats[i].nValue, sizeof stats[i].nValue);
    }
    if (countOut) *countOut = (uint32_t)count;
    return hash;
}

// Current-version retail `gf` base-stat section. The recovered reader is applied to the exact
// writer output, then the live base-stat list is serialized again for an exact round-trip gate.
EMSCRIPTEN_KEEPALIVE const D2SessionStatsSaveRoundTrip* d2_session_stats_save_roundtrip() {
    memset(&g_sessionStatsSaveRoundTrip, 0, sizeof g_sessionStatsSaveRoundTrip);
    g_sessionStatsSaveRoundTrip.version = 1;
    g_sessionStatsSaveRoundTrip.size = sizeof g_sessionStatsSaveRoundTrip;
    g_sessionStatsSaveRoundTrip.saveVersion = 96;
    g_sessionStatsSaveBytes.clear();
    if (!g_sessionLive || !g_game || !g_targetPlayer) {
        g_sessionStatsSaveRoundTrip.result = 1;
        return &g_sessionStatsSaveRoundTrip;
    }
    constexpr size_t kCapacity = 4096;
    g_sessionStatsSaveBytes.resize(kCapacity);
    uint8_t* writeEnd = g_sessionStatsSaveBytes.data();
    g_sessionStatsSaveRoundTrip.beforeStatsHash = sessionBaseStatsHash(
        g_targetPlayer, &g_sessionStatsSaveRoundTrip.statEntries);
    g_sessionStatsSaveRoundTrip.hpBefore = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_HITPOINTS, 0);
    g_sessionStatsSaveRoundTrip.manaBefore = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_MANA, 0);
    const int32_t writeResult = PLRSAVE2_WritePlayerStats(g_game, g_targetPlayer, &writeEnd,
        g_sessionStatsSaveBytes.data() + kCapacity);
    if (writeResult || writeEnd <= g_sessionStatsSaveBytes.data() || writeEnd > g_sessionStatsSaveBytes.data() + kCapacity) {
        g_sessionStatsSaveRoundTrip.result = 2u + (uint32_t)writeResult;
        g_sessionStatsSaveBytes.clear();
        return &g_sessionStatsSaveRoundTrip;
    }
    g_sessionStatsSaveRoundTrip.bytesWritten = (uint32_t)(writeEnd - g_sessionStatsSaveBytes.data());
    g_sessionStatsSaveBytes.resize(g_sessionStatsSaveRoundTrip.bytesWritten);
    g_sessionStatsSaveRoundTrip.sectionHash = sessionHashBytes(D2VEC_FNV_INIT,
        g_sessionStatsSaveBytes.data(), g_sessionStatsSaveBytes.size());
    if (g_sessionStatsSaveBytes.size() >= 2 && g_sessionStatsSaveBytes[0] == 'g' && g_sessionStatsSaveBytes[1] == 'f')
        g_sessionStatsSaveRoundTrip.flags |= 1u;

    uint8_t* readEnd = g_sessionStatsSaveBytes.data();
    const int32_t readResult = PLRSAVE2_ReadStatsEx(g_game, g_targetPlayer, &readEnd,
        g_sessionStatsSaveBytes.data() + g_sessionStatsSaveBytes.size(), 96);
    g_sessionStatsSaveRoundTrip.bytesConsumed = (uint32_t)(readEnd - g_sessionStatsSaveBytes.data());
    if (readResult) {
        g_sessionStatsSaveRoundTrip.result = 0x100u + (uint32_t)readResult;
        return &g_sessionStatsSaveRoundTrip;
    }
    if (g_sessionStatsSaveRoundTrip.bytesConsumed == g_sessionStatsSaveRoundTrip.bytesWritten)
        g_sessionStatsSaveRoundTrip.flags |= 2u;
    g_sessionStatsSaveRoundTrip.afterStatsHash = sessionBaseStatsHash(g_targetPlayer, nullptr);
    g_sessionStatsSaveRoundTrip.hpAfter = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_HITPOINTS, 0);
    g_sessionStatsSaveRoundTrip.manaAfter = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_MANA, 0);
    if (g_sessionStatsSaveRoundTrip.afterStatsHash == g_sessionStatsSaveRoundTrip.beforeStatsHash &&
        g_sessionStatsSaveRoundTrip.hpAfter == g_sessionStatsSaveRoundTrip.hpBefore &&
        g_sessionStatsSaveRoundTrip.manaAfter == g_sessionStatsSaveRoundTrip.manaBefore)
        g_sessionStatsSaveRoundTrip.flags |= 4u;

    std::vector<uint8_t> replay(kCapacity);
    uint8_t* replayEnd = replay.data();
    const int32_t replayResult = PLRSAVE2_WritePlayerStats(g_game, g_targetPlayer, &replayEnd, replay.data() + replay.size());
    if (!replayResult) {
        g_sessionStatsSaveRoundTrip.replayBytes = (uint32_t)(replayEnd - replay.data());
        g_sessionStatsSaveRoundTrip.replayHash = sessionHashBytes(D2VEC_FNV_INIT, replay.data(), g_sessionStatsSaveRoundTrip.replayBytes);
        if (g_sessionStatsSaveRoundTrip.replayBytes == g_sessionStatsSaveRoundTrip.bytesWritten &&
            !memcmp(replay.data(), g_sessionStatsSaveBytes.data(), g_sessionStatsSaveRoundTrip.bytesWritten))
            g_sessionStatsSaveRoundTrip.flags |= 8u;
    }
    g_sessionStatsSaveRoundTrip.result = g_sessionStatsSaveRoundTrip.flags == 15u ? 0u : 3u;
    g_sessionStatsSaveRoundTrip.hash = sessionHashBytes(D2VEC_FNV_INIT, &g_sessionStatsSaveRoundTrip,
        offsetof(D2SessionStatsSaveRoundTrip, hash));
    return &g_sessionStatsSaveRoundTrip;
}

EMSCRIPTEN_KEEPALIVE const uint8_t* d2_session_stats_save_bytes() { return g_sessionStatsSaveBytes.data(); }
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_stats_save_length() { return (uint32_t)g_sessionStatsSaveBytes.size(); }

static uint32_t sessionSkillSemanticHash(D2UnitStrc* player, int32_t classId, int32_t skillCount) {
    uint32_t hash = D2VEC_FNV_INIT;
    for (int32_t i = 0; i < skillCount; ++i) {
        const int32_t skillId = SKILLS_GetClassSkillId(classId, i);
        const int32_t level = SKILLS_GetSkillLevel(player, SKILLS_GetSkillById(player, skillId, -1u), 0);
        hash = sessionHashBytes(hash, &skillId, sizeof skillId);
        hash = sessionHashBytes(hash, &level, sizeof level);
    }
    return hash;
}

// Two independent recovered sections in their native forms: `if` player skills and `WS`
// waypoints. They intentionally remain separate byte buffers rather than an invented container.
EMSCRIPTEN_KEEPALIVE const D2SessionCampaignSaveRoundTrip* d2_session_campaign_save_roundtrip() {
    memset(&g_sessionCampaignSaveRoundTrip, 0, sizeof g_sessionCampaignSaveRoundTrip);
    g_sessionCampaignSaveRoundTrip.version = 1;
    g_sessionCampaignSaveRoundTrip.size = sizeof g_sessionCampaignSaveRoundTrip;
    g_sessionCampaignSaveRoundTrip.saveVersion = 96;
    g_sessionSkillSaveBytes.clear(); g_sessionWaypointSaveBytes.clear();
    if (!g_sessionLive || !g_game || !g_targetPlayer) {
        g_sessionCampaignSaveRoundTrip.result = 1; return &g_sessionCampaignSaveRoundTrip;
    }
    D2ClientStrc* client = SUNIT_GetClientFromPlayer(g_targetPlayer, __FILE__, __LINE__);
    if (!client) { g_sessionCampaignSaveRoundTrip.result = 2; return &g_sessionCampaignSaveRoundTrip; }
    const int32_t classId = client->nClassId;
    const int32_t skillCount = SKILLS_GetPlayerSkillCount(classId);
    if (classId < 0 || classId >= 7 || skillCount < 0 || skillCount > 255) {
        g_sessionCampaignSaveRoundTrip.result = 3; return &g_sessionCampaignSaveRoundTrip;
    }
    g_sessionCampaignSaveRoundTrip.skillCount = (uint32_t)skillCount;
    g_sessionCampaignSaveRoundTrip.skillSemanticBefore = sessionSkillSemanticHash(g_targetPlayer, classId, skillCount);
    g_sessionSkillSaveBytes.assign((size_t)skillCount + 2, 0);
    g_sessionSkillSaveBytes[0] = 'i'; g_sessionSkillSaveBytes[1] = 'f';
    for (int32_t i = 0; i < skillCount; ++i) {
        const int32_t skillId = SKILLS_GetClassSkillId(classId, i);
        g_sessionSkillSaveBytes[(size_t)i + 2] = (uint8_t)SKILLS_GetSkillLevel(
            g_targetPlayer, SKILLS_GetSkillById(g_targetPlayer, skillId, -1u), 0);
    }
    g_sessionCampaignSaveRoundTrip.skillBytes = (uint32_t)g_sessionSkillSaveBytes.size();
    g_sessionCampaignSaveRoundTrip.skillHash = sessionHashBytes(D2VEC_FNV_INIT,
        g_sessionSkillSaveBytes.data(), g_sessionSkillSaveBytes.size());
    if (g_sessionSkillSaveBytes[0] == 'i' && g_sessionSkillSaveBytes[1] == 'f')
        g_sessionCampaignSaveRoundTrip.flags |= 1u;
    uint8_t* skillRead = g_sessionSkillSaveBytes.data();
    const int32_t skillReadResult = PLRSAVE2_ReadSkills(g_game, g_targetPlayer, &skillRead,
        g_sessionSkillSaveBytes.data() + g_sessionSkillSaveBytes.size(), 96, skillCount);
    g_sessionCampaignSaveRoundTrip.skillConsumed = (uint32_t)(skillRead - g_sessionSkillSaveBytes.data());
    if (skillReadResult) { g_sessionCampaignSaveRoundTrip.result = 0x100u + (uint32_t)skillReadResult; return &g_sessionCampaignSaveRoundTrip; }
    if (g_sessionCampaignSaveRoundTrip.skillConsumed == g_sessionCampaignSaveRoundTrip.skillBytes)
        g_sessionCampaignSaveRoundTrip.flags |= 2u;
    g_sessionCampaignSaveRoundTrip.skillSemanticAfter = sessionSkillSemanticHash(g_targetPlayer, classId, skillCount);
    if (g_sessionCampaignSaveRoundTrip.skillSemanticAfter == g_sessionCampaignSaveRoundTrip.skillSemanticBefore)
        g_sessionCampaignSaveRoundTrip.flags |= 4u;
    std::vector<uint8_t> skillReplay((size_t)skillCount + 2, 0);
    skillReplay[0] = 'i'; skillReplay[1] = 'f';
    for (int32_t i = 0; i < skillCount; ++i) {
        const int32_t skillId = SKILLS_GetClassSkillId(classId, i);
        skillReplay[(size_t)i + 2] = (uint8_t)SKILLS_GetSkillLevel(
            g_targetPlayer, SKILLS_GetSkillById(g_targetPlayer, skillId, -1u), 0);
    }
    g_sessionCampaignSaveRoundTrip.skillReplayHash = sessionHashBytes(D2VEC_FNV_INIT, skillReplay.data(), skillReplay.size());
    if (skillReplay == g_sessionSkillSaveBytes) g_sessionCampaignSaveRoundTrip.flags |= 8u;

    g_sessionWaypointSaveBytes.assign(256, 0);
    uint8_t* waypointWrite = g_sessionWaypointSaveBytes.data();
    const int32_t waypointWriteResult = PLRSAVE2_WriteWaypointData(g_game, g_targetPlayer, &waypointWrite,
        g_sessionWaypointSaveBytes.data() + g_sessionWaypointSaveBytes.size());
    if (waypointWriteResult) { g_sessionCampaignSaveRoundTrip.result = 0x200u + (uint32_t)waypointWriteResult; return &g_sessionCampaignSaveRoundTrip; }
    g_sessionWaypointSaveBytes.resize((size_t)(waypointWrite - g_sessionWaypointSaveBytes.data()));
    g_sessionCampaignSaveRoundTrip.waypointBytes = (uint32_t)g_sessionWaypointSaveBytes.size();
    g_sessionCampaignSaveRoundTrip.waypointHash = sessionHashBytes(D2VEC_FNV_INIT,
        g_sessionWaypointSaveBytes.data(), g_sessionWaypointSaveBytes.size());
    // Bytes 'W','S' -- see the marker note in PlrSaveItems.cpp. This check carried the reversed
    // order too, so the writer, the reader and this verdict all agreed on the wrong bytes and the
    // round-trip stayed green. Three copies of one mistake is why the marker now has one comment
    // explaining it rather than three constants repeating it.
    if (g_sessionWaypointSaveBytes.size() >= 2 && g_sessionWaypointSaveBytes[0] == 'W' && g_sessionWaypointSaveBytes[1] == 'S')
        g_sessionCampaignSaveRoundTrip.flags |= 16u;
    uint8_t* waypointRead = g_sessionWaypointSaveBytes.data();
    const int32_t waypointReadResult = PLRSAVE2_ReadWaypointData(g_game, g_targetPlayer, &waypointRead,
        g_sessionWaypointSaveBytes.data() + g_sessionWaypointSaveBytes.size(), 96);
    g_sessionCampaignSaveRoundTrip.waypointConsumed = (uint32_t)(waypointRead - g_sessionWaypointSaveBytes.data());
    if (waypointReadResult) { g_sessionCampaignSaveRoundTrip.result = 0x300u + (uint32_t)waypointReadResult; return &g_sessionCampaignSaveRoundTrip; }
    if (g_sessionCampaignSaveRoundTrip.waypointConsumed == g_sessionCampaignSaveRoundTrip.waypointBytes)
        g_sessionCampaignSaveRoundTrip.flags |= 32u;
    std::vector<uint8_t> waypointReplay(256, 0); uint8_t* waypointReplayEnd = waypointReplay.data();
    const int32_t waypointReplayResult = PLRSAVE2_WriteWaypointData(g_game, g_targetPlayer, &waypointReplayEnd,
        waypointReplay.data() + waypointReplay.size());
    waypointReplay.resize((size_t)(waypointReplayEnd - waypointReplay.data()));
    g_sessionCampaignSaveRoundTrip.waypointReplayHash = sessionHashBytes(D2VEC_FNV_INIT, waypointReplay.data(), waypointReplay.size());
    if (!waypointReplayResult && waypointReplay == g_sessionWaypointSaveBytes) g_sessionCampaignSaveRoundTrip.flags |= 64u;
    g_sessionCampaignSaveRoundTrip.result = g_sessionCampaignSaveRoundTrip.flags == 127u ? 0u : 4u;
    g_sessionCampaignSaveRoundTrip.hash = sessionHashBytes(D2VEC_FNV_INIT, &g_sessionCampaignSaveRoundTrip,
        offsetof(D2SessionCampaignSaveRoundTrip, hash));
    return &g_sessionCampaignSaveRoundTrip;
}

EMSCRIPTEN_KEEPALIVE const uint8_t* d2_session_skill_save_bytes() { return g_sessionSkillSaveBytes.data(); }
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_skill_save_length() { return (uint32_t)g_sessionSkillSaveBytes.size(); }
EMSCRIPTEN_KEEPALIVE const uint8_t* d2_session_waypoint_save_bytes() { return g_sessionWaypointSaveBytes.data(); }
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_waypoint_save_length() { return (uint32_t)g_sessionWaypointSaveBytes.size(); }

EMSCRIPTEN_KEEPALIVE const D2SessionQuestIntroSaveRoundTrip* d2_session_quest_intro_save_roundtrip() {
    memset(&g_sessionQuestIntroSaveRoundTrip, 0, sizeof g_sessionQuestIntroSaveRoundTrip);
    g_sessionQuestIntroSaveRoundTrip.version = 1; g_sessionQuestIntroSaveRoundTrip.size = sizeof g_sessionQuestIntroSaveRoundTrip;
    g_sessionQuestIntroSaveRoundTrip.saveVersion = 96; g_sessionQuestSaveBytes.clear(); g_sessionIntroSaveBytes.clear();
    memset(&g_sessionCorpseSaveRoundTrip, 0, sizeof g_sessionCorpseSaveRoundTrip); g_sessionCorpseSaveBytes.clear();
    if (!g_sessionLive || !g_game || !g_targetPlayer) { g_sessionQuestIntroSaveRoundTrip.result = 1; return &g_sessionQuestIntroSaveRoundTrip; }
    g_sessionQuestIntroSaveRoundTrip.gheedBefore = PLRINTRO_GetQuestIntroFlag(g_targetPlayer, g_game, MONSTER_GHEED);
    g_sessionQuestSaveBytes.assign(298, 0); uint8_t* qw = g_sessionQuestSaveBytes.data();
    int rc = D2WEB_PLRSAVE2_WriteQuestSection(g_targetPlayer, &qw, g_sessionQuestSaveBytes.data() + g_sessionQuestSaveBytes.size());
    if (rc) { g_sessionQuestIntroSaveRoundTrip.result = 0x100u + rc; return &g_sessionQuestIntroSaveRoundTrip; }
    g_sessionQuestIntroSaveRoundTrip.questBytes = (uint32_t)(qw - g_sessionQuestSaveBytes.data());
    g_sessionQuestIntroSaveRoundTrip.questHash = sessionHashBytes(D2VEC_FNV_INIT, g_sessionQuestSaveBytes.data(), g_sessionQuestSaveBytes.size());
    g_sessionQuestIntroSaveRoundTrip.questSemanticBefore = g_sessionQuestIntroSaveRoundTrip.questHash;
    if (g_sessionQuestSaveBytes[0] == 'W' && g_sessionQuestSaveBytes[1] == 'o' && g_sessionQuestSaveBytes[2] == 'o' && g_sessionQuestSaveBytes[3] == '!') g_sessionQuestIntroSaveRoundTrip.flags |= 1u;
    uint8_t* qr = g_sessionQuestSaveBytes.data(); rc = D2WEB_PLRSAVE2_ReadQuestSection(g_targetPlayer, &qr, g_sessionQuestSaveBytes.data() + g_sessionQuestSaveBytes.size());
    g_sessionQuestIntroSaveRoundTrip.questConsumed = (uint32_t)(qr - g_sessionQuestSaveBytes.data());
    if (rc) { g_sessionQuestIntroSaveRoundTrip.result = 0x200u + rc; return &g_sessionQuestIntroSaveRoundTrip; }
    if (g_sessionQuestIntroSaveRoundTrip.questConsumed == 298) g_sessionQuestIntroSaveRoundTrip.flags |= 2u;
    std::vector<uint8_t> qrep(298); uint8_t* qre = qrep.data(); rc = D2WEB_PLRSAVE2_WriteQuestSection(g_targetPlayer, &qre, qrep.data() + qrep.size());
    g_sessionQuestIntroSaveRoundTrip.questReplayHash = sessionHashBytes(D2VEC_FNV_INIT, qrep.data(), qrep.size());
    g_sessionQuestIntroSaveRoundTrip.questSemanticAfter = g_sessionQuestIntroSaveRoundTrip.questReplayHash;
    if (!rc && qrep == g_sessionQuestSaveBytes) g_sessionQuestIntroSaveRoundTrip.flags |= 4u;

    g_sessionIntroSaveBytes.assign(52, 0); uint8_t* iw = g_sessionIntroSaveBytes.data();
    rc = PLRSAVE2_WritePlrIntroSection(g_game, g_targetPlayer, &iw, g_sessionIntroSaveBytes.data() + g_sessionIntroSaveBytes.size());
    if (rc) { g_sessionQuestIntroSaveRoundTrip.result = 0x300u + rc; return &g_sessionQuestIntroSaveRoundTrip; }
    g_sessionQuestIntroSaveRoundTrip.introBytes = (uint32_t)(iw - g_sessionIntroSaveBytes.data());
    g_sessionQuestIntroSaveRoundTrip.introHash = sessionHashBytes(D2VEC_FNV_INIT, g_sessionIntroSaveBytes.data(), g_sessionIntroSaveBytes.size());
    g_sessionQuestIntroSaveRoundTrip.introSemanticBefore = g_sessionQuestIntroSaveRoundTrip.introHash;
    if (g_sessionIntroSaveBytes[0] == 1 && g_sessionIntroSaveBytes[1] == 0x77) g_sessionQuestIntroSaveRoundTrip.flags |= 8u;
    uint8_t* ir = g_sessionIntroSaveBytes.data(); rc = D2WEB_PLRSAVE2_ReadPlrIntroSection(g_targetPlayer, &ir, g_sessionIntroSaveBytes.data() + g_sessionIntroSaveBytes.size());
    g_sessionQuestIntroSaveRoundTrip.introConsumed = (uint32_t)(ir - g_sessionIntroSaveBytes.data());
    if (rc) { g_sessionQuestIntroSaveRoundTrip.result = 0x400u + rc; return &g_sessionQuestIntroSaveRoundTrip; }
    if (g_sessionQuestIntroSaveRoundTrip.introConsumed == 52) g_sessionQuestIntroSaveRoundTrip.flags |= 16u;
    std::vector<uint8_t> irep(52); uint8_t* ire = irep.data(); rc = PLRSAVE2_WritePlrIntroSection(g_game, g_targetPlayer, &ire, irep.data() + irep.size());
    g_sessionQuestIntroSaveRoundTrip.introReplayHash = sessionHashBytes(D2VEC_FNV_INIT, irep.data(), irep.size());
    g_sessionQuestIntroSaveRoundTrip.introSemanticAfter = g_sessionQuestIntroSaveRoundTrip.introReplayHash;
    if (!rc && irep == g_sessionIntroSaveBytes) g_sessionQuestIntroSaveRoundTrip.flags |= 32u;
    g_sessionQuestIntroSaveRoundTrip.gheedAfter = PLRINTRO_GetQuestIntroFlag(g_targetPlayer, g_game, MONSTER_GHEED);
    if (g_sessionQuestIntroSaveRoundTrip.gheedAfter == g_sessionQuestIntroSaveRoundTrip.gheedBefore) g_sessionQuestIntroSaveRoundTrip.flags |= 64u;
    g_sessionQuestIntroSaveRoundTrip.result = g_sessionQuestIntroSaveRoundTrip.flags == 127u ? 0u : 2u;
    g_sessionQuestIntroSaveRoundTrip.hash = sessionHashBytes(D2VEC_FNV_INIT, &g_sessionQuestIntroSaveRoundTrip, offsetof(D2SessionQuestIntroSaveRoundTrip, hash));
    return &g_sessionQuestIntroSaveRoundTrip;
}
EMSCRIPTEN_KEEPALIVE const uint8_t* d2_session_quest_save_bytes() { return g_sessionQuestSaveBytes.data(); }
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_quest_save_length() { return (uint32_t)g_sessionQuestSaveBytes.size(); }
EMSCRIPTEN_KEEPALIVE const uint8_t* d2_session_intro_save_bytes() { return g_sessionIntroSaveBytes.data(); }
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_intro_save_length() { return (uint32_t)g_sessionIntroSaveBytes.size(); }

static uint32_t sessionCorpseCount(D2InventoryStrc* inventory) {
    uint32_t count = 0;
    if (inventory) for (D2CorpseStrc* corpse = INVENTORY_GetFirstCorpse(inventory); corpse; corpse = INVENTORY_GetNextCorpse(corpse)) ++count;
    return count;
}
EMSCRIPTEN_KEEPALIVE const D2SessionCorpseSaveRoundTrip* d2_session_corpse_save_roundtrip() {
    memset(&g_sessionCorpseSaveRoundTrip, 0, sizeof g_sessionCorpseSaveRoundTrip);
    g_sessionCorpseSaveRoundTrip.version=1; g_sessionCorpseSaveRoundTrip.size=sizeof g_sessionCorpseSaveRoundTrip; g_sessionCorpseSaveRoundTrip.saveVersion=96;
    g_sessionCorpseSaveBytes.clear();
    if(!g_sessionLive||!g_game||!g_targetPlayer||!g_targetPlayer->pInventory){g_sessionCorpseSaveRoundTrip.result=1;return &g_sessionCorpseSaveRoundTrip;}
    g_sessionCorpseSaveRoundTrip.corpseCountBefore=sessionCorpseCount(g_targetPlayer->pInventory);
    g_sessionCorpseSaveBytes.assign(65536,0); uint8_t* w=g_sessionCorpseSaveBytes.data();
    int rc=PLRSAVE2_WriteCorpsesSection(g_game,g_targetPlayer,&w,g_sessionCorpseSaveBytes.data()+g_sessionCorpseSaveBytes.size(),0);
    if(rc){g_sessionCorpseSaveRoundTrip.result=0x100u+rc;return &g_sessionCorpseSaveRoundTrip;}
    g_sessionCorpseSaveRoundTrip.bytesWritten=(uint32_t)(w-g_sessionCorpseSaveBytes.data());g_sessionCorpseSaveBytes.resize(g_sessionCorpseSaveRoundTrip.bytesWritten);
    if(g_sessionCorpseSaveBytes.size()>=4){g_sessionCorpseSaveRoundTrip.sectionCount=(uint32_t)g_sessionCorpseSaveBytes[2]|((uint32_t)g_sessionCorpseSaveBytes[3]<<8);if(g_sessionCorpseSaveBytes[0]=='J'&&g_sessionCorpseSaveBytes[1]=='M')g_sessionCorpseSaveRoundTrip.flags|=1u;}
    g_sessionCorpseSaveRoundTrip.sectionHash=sessionHashBytes(D2VEC_FNV_INIT,g_sessionCorpseSaveBytes.data(),g_sessionCorpseSaveBytes.size());
    uint8_t* r=g_sessionCorpseSaveBytes.data();int32_t detail=0;rc=PLRSAVE2_ReadCorpses(g_game,g_targetPlayer,&r,g_sessionCorpseSaveBytes.data()+g_sessionCorpseSaveBytes.size(),96,&detail);
    g_sessionCorpseSaveRoundTrip.readerDetail=(uint32_t)detail;g_sessionCorpseSaveRoundTrip.bytesConsumed=(uint32_t)(r-g_sessionCorpseSaveBytes.data());
    if(rc){g_sessionCorpseSaveRoundTrip.result=0x200u+rc;return &g_sessionCorpseSaveRoundTrip;}if(g_sessionCorpseSaveRoundTrip.bytesConsumed==g_sessionCorpseSaveRoundTrip.bytesWritten)g_sessionCorpseSaveRoundTrip.flags|=2u;
    g_sessionCorpseSaveRoundTrip.corpseCountAfter=sessionCorpseCount(g_targetPlayer->pInventory);if(g_sessionCorpseSaveRoundTrip.corpseCountAfter==g_sessionCorpseSaveRoundTrip.corpseCountBefore+g_sessionCorpseSaveRoundTrip.sectionCount)g_sessionCorpseSaveRoundTrip.flags|=4u;
    std::vector<uint8_t> rep(65536,0);uint8_t* re=rep.data();rc=PLRSAVE2_WriteCorpsesSection(g_game,g_targetPlayer,&re,rep.data()+rep.size(),0);rep.resize((size_t)(re-rep.data()));
    g_sessionCorpseSaveRoundTrip.replayBytes=(uint32_t)rep.size();g_sessionCorpseSaveRoundTrip.replayHash=sessionHashBytes(D2VEC_FNV_INIT,rep.data(),rep.size());if(!rc&&rep==g_sessionCorpseSaveBytes)g_sessionCorpseSaveRoundTrip.flags|=8u;
    g_sessionCorpseSaveRoundTrip.result=g_sessionCorpseSaveRoundTrip.flags==15u?0u:2u;g_sessionCorpseSaveRoundTrip.hash=sessionHashBytes(D2VEC_FNV_INIT,&g_sessionCorpseSaveRoundTrip,offsetof(D2SessionCorpseSaveRoundTrip,hash));return &g_sessionCorpseSaveRoundTrip;
}
EMSCRIPTEN_KEEPALIVE const uint8_t* d2_session_corpse_save_bytes(){return g_sessionCorpseSaveBytes.data();}
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_corpse_save_length(){return(uint32_t)g_sessionCorpseSaveBytes.size();}

EMSCRIPTEN_KEEPALIVE const D2SessionAttack* d2_session_attack() {
    g_sessionAttack.version = 1; g_sessionAttack.size = sizeof g_sessionAttack;
    if (g_sessionLive && g_game) g_sessionAttack.gameFrame = g_game->dwGameFrame;
    if (g_targetPlayer) g_sessionAttack.mode = g_targetPlayer->dwAnimMode;
    if (g_sessionAttackTarget) g_sessionAttack.targetHpAfter =
        STATLIST_UnitGetStatValue(g_sessionAttackTarget, STAT_HITPOINTS, 0);
    uint32_t hash = D2VEC_FNV_INIT; const uint8_t* bytes = (const uint8_t*)&g_sessionAttack;
    for (size_t i = 0; i < offsetof(D2SessionAttack, hash); ++i) { hash ^= bytes[i]; hash *= 16777619u; }
    g_sessionAttack.hash = hash; return &g_sessionAttack;
}

EMSCRIPTEN_KEEPALIVE const D2SessionProgress* d2_session_progress() {
    memset(&g_sessionProgress, 0, sizeof g_sessionProgress);
    g_sessionProgress.version = 1; g_sessionProgress.size = sizeof g_sessionProgress;
    if (!g_sessionLive || !g_game || !g_targetPlayer) return &g_sessionProgress;
    g_sessionProgress.gameFrame = g_game->dwGameFrame; g_sessionProgress.kills = g_sessionKills;
    if (g_sessionAttackTarget) {
        g_sessionProgress.targetId = g_sessionAttackTarget->dwUnitId;
        g_sessionProgress.targetHp = STATLIST_UnitGetStatValue(g_sessionAttackTarget, STAT_HITPOINTS, 0);
        g_sessionProgress.targetMode = g_sessionAttackTarget->dwAnimMode;
        if (g_sessionProgress.targetHp <= 0) g_sessionProgress.flags |= 1;
    }
    g_sessionProgress.playerExperience = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_EXPERIENCE, 0);
    g_sessionProgress.experienceDelta = g_sessionProgress.playerExperience - g_sessionInitialExperience;
    updateSessionSnapshot();
    uint32_t hash = D2VEC_FNV_INIT;
    for (const D2SessionUnit& unit : g_sessionUnits) if (unit.type == UNIT_ITEM) {
        ++g_sessionProgress.itemCount;
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&unit);
        // Unit ids come from a process-global allocator and drop scatter can choose either
        // adjacent free subtile after game teardown.  Hash the generated item identity/state,
        // excluding allocator identity and presentation position, for stable session replay.
        for (size_t i = offsetof(D2SessionUnit, type); i < offsetof(D2SessionUnit, x); ++i) {
            hash ^= bytes[i]; hash *= 16777619u;
        }
        for (size_t i = offsetof(D2SessionUnit, flags); i < sizeof unit; ++i) {
            hash ^= bytes[i]; hash *= 16777619u;
        }
    }
    g_sessionProgress.itemHash = hash;
    return &g_sessionProgress;
}

EMSCRIPTEN_KEEPALIVE const D2SessionAdvancement* d2_session_advancement() {
    memset(&g_sessionAdvancement, 0, sizeof g_sessionAdvancement);
    g_sessionAdvancement.version = 1; g_sessionAdvancement.size = sizeof g_sessionAdvancement;
    g_sessionAdvancement.skillId = g_sessionAdvancementSkillId;
    if (!g_sessionLive || !g_game || !g_targetPlayer) return &g_sessionAdvancement;
    g_sessionAdvancement.gameFrame = g_game->dwGameFrame;
    g_sessionAdvancement.level = STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_LEVEL, 0);
    g_sessionAdvancement.experience = STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_EXPERIENCE, 0);
    g_sessionAdvancement.lastExperience = STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_LASTEXP, 0);
    g_sessionAdvancement.nextExperience = STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_NEXTEXP, 0);
    g_sessionAdvancement.statPoints = STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_STATPTS, 0);
    g_sessionAdvancement.skillPoints = STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_SKILLPTS, 0);
    g_sessionAdvancement.strength = STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_STRENGTH, 0);
    g_sessionAdvancement.energy = STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_ENERGY, 0);
    g_sessionAdvancement.dexterity = STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_DEXTERITY, 0);
    g_sessionAdvancement.vitality = STATLIST_GetUnitBaseStat(g_targetPlayer, STAT_VITALITY, 0);
    if (D2SkillStrc* skill = SKILLS_GetSkillById(g_targetPlayer, (int32_t)g_sessionAdvancementSkillId, -1)) {
        g_sessionAdvancement.skillBaseLevel = SKILLS_GetSkillLevel(g_targetPlayer, skill, 0);
        g_sessionAdvancement.skillTotalLevel = SKILLS_GetSkillLevel(g_targetPlayer, skill, 1);
    }
    g_sessionAdvancement.hash = sessionHashBytes(D2VEC_FNV_INIT, &g_sessionAdvancement,
        offsetof(D2SessionAdvancement, hash));
    return &g_sessionAdvancement;
}

EMSCRIPTEN_KEEPALIVE const D2SessionItemState* d2_session_item_state() {
    const uint32_t commandTarget = g_sessionItemState.commandTarget;
    const uint32_t result = g_sessionItemState.result;
    memset(&g_sessionItemState, 0, sizeof g_sessionItemState);
    g_sessionItemState.version = 1; g_sessionItemState.size = sizeof g_sessionItemState;
    g_sessionItemState.commandTarget = commandTarget; g_sessionItemState.result = result;
    if (!g_sessionLive || !g_targetPlayer || !g_targetPlayer->pInventory) return &g_sessionItemState;
    updateSessionSnapshot();
    for (const D2SessionUnit& unit : g_sessionUnits) if (unit.type == UNIT_ITEM) ++g_sessionItemState.groundCount;
    uint32_t hash = D2VEC_FNV_INIT;
    for (D2UnitStrc* item = INVENTORY_GetFirstItem(g_targetPlayer->pInventory); item; item = INVENTORY_GetNextItem(item)) {
        if (!INVENTORY_UnitIsItem(item)) continue;
        ++g_sessionItemState.inventoryCount;
        const uint32_t fields[] = { (uint32_t)item->dwClassId, (uint32_t)item->dwAnimMode,
            (uint32_t)ITEMS_GetItemQuality(item), ITEMS_GetItemFlags(item), ITEMS_GetInvPage(item), ITEMS_GetBodyLocation(item) };
        hash = sessionHashBytes(hash, fields, sizeof fields);
    }
    if (D2UnitStrc* cursor = INVENTORY_GetCursorItem(g_targetPlayer->pInventory)) {
        g_sessionItemState.cursorItemId = cursor->dwUnitId; g_sessionItemState.flags |= 1;
    }
    g_sessionItemState.inventoryHash = hash;
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&g_sessionItemState);
    g_sessionItemState.hash = sessionHashBytes(D2VEC_FNV_INIT, bytes, offsetof(D2SessionItemState, hash));
    return &g_sessionItemState;
}

EMSCRIPTEN_KEEPALIVE const D2SessionMotion* d2_session_motion() {
    memset(&g_sessionMotion, 0, sizeof g_sessionMotion);
    g_sessionMotion.version = 1; g_sessionMotion.size = sizeof g_sessionMotion;
    if (!g_sessionLive || !g_game || !g_targetPlayer || !g_targetPlayer->pDynamicPath) return &g_sessionMotion;
    const D2DynamicPathStrc* path = g_targetPlayer->pDynamicPath;
    g_sessionMotion.gameFrame = g_game->dwGameFrame; g_sessionMotion.mode = g_targetPlayer->dwAnimMode;
    g_sessionMotion.x = path->tGameCoords.wPosX; g_sessionMotion.y = path->tGameCoords.wPosY;
    g_sessionMotion.targetX = g_sessionMovePending ? g_sessionTargetX : g_sessionMotion.x;
    g_sessionMotion.targetY = g_sessionMovePending ? g_sessionTargetY : g_sessionMotion.y;
    g_sessionMotion.pathPoints = path->dwPathPoints; g_sessionMotion.currentPoint = path->dwCurrentPointIdx;
    g_sessionMotion.framePrecise = g_targetPlayer->nSeqCurrentFramePrecise;
    g_sessionMotion.frameCountPrecise = g_targetPlayer->dwFrameCountPrecise;
    g_sessionMotion.animSpeed = g_targetPlayer->wAnimSpeed; g_sessionMotion.direction = path->nDirection;
    g_sessionMotion.moving = g_sessionMovePending;
    return &g_sessionMotion;
}

EMSCRIPTEN_KEEPALIVE void d2_session_hold_world(int hold) { g_worldHold = hold != 0; g_worldHoldBuilt = false; }
EMSCRIPTEN_KEEPALIVE const D2SessionSnapshot* d2_session_snapshot() {
    updateSessionSnapshot(); return &g_sessionSnapshot;
}
EMSCRIPTEN_KEEPALIVE const D2SessionResources* d2_session_resources() {
    memset(&g_sessionResources, 0, sizeof g_sessionResources);
    g_sessionResources.version = 1; g_sessionResources.size = sizeof g_sessionResources;
    if (g_targetPlayer) {
        g_sessionResources.hp = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_HITPOINTS, 0);
        g_sessionResources.maxHp = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_MAXHP, 0);
        g_sessionResources.mana = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_MANA, 0);
        g_sessionResources.maxMana = STATLIST_UnitGetStatValue(g_targetPlayer, STAT_MAXMANA, 0);
    }
    return &g_sessionResources;
}

EMSCRIPTEN_KEEPALIVE const D2SessionRoom* d2_session_rooms() {
    updateSessionSnapshot(); return g_sessionRooms.data();
}
EMSCRIPTEN_KEEPALIVE int d2_session_room_count() {
    updateSessionSnapshot(); return (int)g_sessionRooms.size();
}
EMSCRIPTEN_KEEPALIVE const D2SessionUnit* d2_session_units() {
    updateSessionSnapshot(); return g_sessionUnits.data();
}
EMSCRIPTEN_KEEPALIVE int d2_session_unit_count() {
    updateSessionSnapshot(); return (int)g_sessionUnits.size();
}
EMSCRIPTEN_KEEPALIVE const D2SessionMonsterAnimation* d2_session_monster_animations() {
    updateSessionSnapshot(); return g_sessionMonsterAnimations.data();
}
EMSCRIPTEN_KEEPALIVE int d2_session_monster_animation_count() {
    updateSessionSnapshot(); return (int)g_sessionMonsterAnimations.size();
}
EMSCRIPTEN_KEEPALIVE const D2SessionObjectPresentation* d2_session_object_presentations() {
    updateSessionSnapshot(); return g_sessionObjectPresentations.data();
}
EMSCRIPTEN_KEEPALIVE const D2SessionItemPresentation* d2_session_item_presentations() {
    updateSessionSnapshot(); return g_sessionItemPresentations.data();
}
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_item_presentation_count() {
    updateSessionSnapshot(); return (uint32_t)g_sessionItemPresentations.size();
}
EMSCRIPTEN_KEEPALIVE int d2_session_object_presentation_count() {
    updateSessionSnapshot(); return (int)g_sessionObjectPresentations.size();
}
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_world_hash() {
    updateSessionSnapshot(); return g_sessionWorldHash;
}
// How many tiles the room arrays held that never reached the draw list because their library
// entry was unresolved. Retail draws those tiles, so a non-zero count here is a defect, not a
// statistic -- s140 measures it against retail's own record count.
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_tiles_skipped() { return g_sessionTilesSkipped; }

// Probe for the missing ninth row/column. Retail's room tile lists carry
// floor records at tile offset 8 -- the row and column past the room's own 8x8 -- and ours do not.
// The fill only emits a floor where the grid cell has bIsFloor, so this reports, per room, how many
// of those edge cells carry bIsFloor and how many carry bLOS. Read-only: it walks the same grids
// the generator already built and changes nothing.
//   out[0] grid width, out[1] grid height, out[2] edge cells with bIsFloor, out[3] edge with bLOS,
//   out[4] interior cells with bIsFloor.
// A PRESET room's edge decision, so the remaining missing tiles can be correlated with it rather
// than guessed at (T4: 5,285 ninth-row cells still missing, all in levels
// whose rooms have no outdoor grid).
//
// DRLGPRESET_LoadRoomTiles kills an edge only when the preset's LvlPrest KillEdge column is set AND
// the room's far edge coincides with the maze's. A killed edge is never visited by the fill, so it
// cannot produce tiles; an unkilled one can, and if retail has tiles there and we do not, the fault
// is downstream.
//   out[0] killEdgeColumn, out[1] bKillEdgeX, out[2] bKillEdgeY,
//   out[3] emitted floors, out[4] emitted walls, out[5] tileWidth, out[6] tileHeight,
//   out[7] fillBlanks column, out[8] floor layers
extern int32_t g_d2webPresetEdgeMerged;
extern int32_t g_d2webPresetEdgeAdded;
extern int32_t g_d2webPresetEdgeSameMap;
extern int32_t g_d2webPresetEdgeOtherMap;
// How a preset room's own ninth row/column was resolved during the last level build: merged into a
// neighbour, or added here. Diagnostic for the last remaining candidate.
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_preset_edge_counts(int32_t* out, uint32_t cap) {
    if (!out || cap < 4) return 0;
    out[0] = g_d2webPresetEdgeMerged; out[1] = g_d2webPresetEdgeAdded;
    out[2] = g_d2webPresetEdgeSameMap; out[3] = g_d2webPresetEdgeOtherMap;
    g_d2webPresetEdgeMerged = 0; g_d2webPresetEdgeAdded = 0;
    g_d2webPresetEdgeSameMap = 0; g_d2webPresetEdgeOtherMap = 0;
    return 1;
}

EMSCRIPTEN_KEEPALIVE uint32_t d2_session_preset_edge(uint32_t roomOrdinal, int32_t* out, uint32_t cap) {
    if (!g_sessionLive || !g_lastLevel || !out || cap < 9) return 0;
    uint32_t ordinal = 0;
    for (D2DrlgRoomStrc* roomEx = g_lastLevel->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext, ++ordinal) {
        if (ordinal != roomOrdinal) continue;
        if (roomEx->nType != DRLGTYPE_PRESET || !roomEx->pMaze || !roomEx->pMaze->pMap) return 0;
        D2DrlgMapStrc* map = roomEx->pMaze->pMap;
        if (!map->pLvlPrestTxtRecord) return 0;
        const int kill = (int)map->pLvlPrestTxtRecord->dwKillEdge;
        out[0] = kill;
        out[1] = kill && (roomEx->nTileXPos + roomEx->nTileWidth == map->pDrlgCoord.nPosX + map->pDrlgCoord.nWidth);
        out[2] = kill && (roomEx->nTileYPos + roomEx->nTileHeight == map->pDrlgCoord.nPosY + map->pDrlgCoord.nHeight);
        out[3] = roomEx->pRoom && roomEx->pRoom->pRoomTiles ? roomEx->pRoom->pRoomTiles->nFloors : -1;
        out[4] = roomEx->pRoom && roomEx->pRoom->pRoomTiles ? roomEx->pRoom->pRoomTiles->nWalls : -1;
        out[5] = roomEx->nTileWidth; out[6] = roomEx->nTileHeight;
        // dwFillBlanks drives bCheckCoordinatesValidity on floor layer 0, which makes every cell
        // INSIDE the room's coordinates count as floor whether or not it carries bIsFloor. An edge
        // cell is by definition outside those coordinates, so a FillBlanks level and a plain one
        // may well treat the ninth row differently -- the caves are the short ones and the town is
        // not, so this is the column to check.
        out[7] = (int32_t)map->pLvlPrestTxtRecord->dwFillBlanks;
        out[8] = map->pFile ? map->pFile->nFloorLayers : -1;
        return 1;
    }
    return 0;
}

// For a preset room: how many of its NEAR rooms share its preset map, and how many do not.
//
// s163 showed every short preset room is downstream of the fill, not a killed edge -- the same
// merge that caused the outdoor case. But the outdoor rule applied wholesale to presets produced
// 498 extra floors in a town that is only 3 rooms short, so the preset rule is narrower. The
// candidate: within ONE preset map an edge cell is shared and recorded once, across maps both
// rooms record it. This reports the adjacency so that can be checked against the short rooms.
//   out[0] near rooms, out[1] near rooms sharing this room's pMap, out[2] near rooms on another map,
//   out[3] near rooms that are not preset rooms at all
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_preset_neighbours(uint32_t roomOrdinal, int32_t* out, uint32_t cap) {
    if (!g_sessionLive || !g_lastLevel || !out || cap < 4) return 0;
    uint32_t ordinal = 0;
    for (D2DrlgRoomStrc* roomEx = g_lastLevel->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext, ++ordinal) {
        if (ordinal != roomOrdinal) continue;
        if (roomEx->nType != DRLGTYPE_PRESET || !roomEx->pMaze) return 0;
        const D2DrlgMapStrc* mine = roomEx->pMaze->pMap;
        out[0] = roomEx->nRoomsNear; out[1] = out[2] = out[3] = 0;
        for (int i = 0; i < roomEx->nRoomsNear; ++i) {
            D2DrlgRoomStrc* near = roomEx->ppRoomsNear[i];
            if (!near || near == roomEx) continue;
            if (near->nType != DRLGTYPE_PRESET || !near->pMaze) { ++out[3]; continue; }
            if (near->pMaze->pMap == mine) ++out[1]; else ++out[2];
        }
        return 1;
    }
    return 0;
}

// An Inventory.txt row's panel rectangle and item grid, from retail's own accessors
// (DATATBLS_GetInventoryRect #10635, DATATBLS_GetInventoryGridInfo #10636), at the requested
// resolution. The rows cover the character panels AND the trade/stash/cube screens, so this is the
// source for any panel that shows an item grid -- the vendor window included, which currently
// draws a text list at coordinates that were tuned by trial.
//
// out: panelLeft, panelRight, panelTop, panelBottom, gridX, gridY, gridLeft, gridRight, gridTop,
//      gridBottom, boxWidth, boxHeight  (12 int32)
// The shop's stock, in the same 112-byte presentation format as the player's items, so the client
// parses both with one reader. Needed for the vendor ICON GRID.
EMSCRIPTEN_KEEPALIVE const D2SessionItemPresentation* d2_session_vendor_presentations() {
    return g_sessionVendorPresentations.data();
}
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_vendor_presentation_count() {
    return (uint32_t)g_sessionVendorPresentations.size();
}

EMSCRIPTEN_KEEPALIVE uint32_t d2_session_inventory_layout(int rowId, int higherRes, int32_t* out, uint32_t cap) {
    // Inventory.txt is indexed nIndex = rowId + 16 * higherRes and the accessors D2_ASSERT on it,
    // so an out-of-range id does not return an error -- it aborts the whole engine. Refuse here.
    if (!out || cap < 12 || rowId < 0 || rowId >= 16 || higherRes < 0 || higherRes > 1) return 0;
    if (!sgptDataTables || !sgptDataTables->pInventoryTxt) return 0;
    if (rowId + 16 * higherRes >= sgptDataTables->nInventoryTxtRecordCount) return 0;
    D2InvRectStrc rect = {};
    D2InventoryGridInfoStrc grid = {};
    DATATBLS_GetInventoryRect(rowId, higherRes, &rect);
    DATATBLS_GetInventoryGridInfo(rowId, higherRes, &grid);
    out[0] = rect.nLeft; out[1] = rect.nRight; out[2] = rect.nTop; out[3] = rect.nBottom;
    out[4] = grid.nGridX; out[5] = grid.nGridY;
    out[6] = grid.nGridLeft; out[7] = grid.nGridRight; out[8] = grid.nGridTop; out[9] = grid.nGridBottom;
    out[10] = grid.nGridBoxWidth; out[11] = grid.nGridBoxHeight;
    // A row that exists has a panel with area; an id past the end of the table reads back zeros.
    return (rect.nRight > rect.nLeft && rect.nBottom > rect.nTop) ? 1 : 0;
}

// The belt's box rectangles, from retail's own Belts.txt accessor.
//
// The client had these hardcoded as {x: 342, y: 561, cell: 31}, invented and undeclared
//. They are table data: DATATBLS_GetInvRectFromBeltsTxt is D2Common #10638
// and takes the 800x600 flag, so retail computes the layout and the client only draws it.
//
// out[0] = number of boxes; then 4 int32 per box (left, right, top, bottom).
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_belt_boxes(int beltIndex, int higherRes, int32_t* out, uint32_t cap) {
    if (!out || cap < 1) return 0;
    D2BeltsTxt record = {};
    DATATBLS_GetBeltsTxtRecord(beltIndex, higherRes, &record);
    const int boxes = record.nBoxes;
    if (boxes <= 0 || boxes > 16) return 0;
    if (cap < (uint32_t)(1 + boxes * 4)) return 0;
    out[0] = boxes;
    for (int i = 0; i < boxes; ++i) {
        D2InvRectStrc rect = {};
        DATATBLS_GetInvRectFromBeltsTxt(beltIndex, higherRes, &rect, i);
        out[1 + i * 4 + 0] = rect.nLeft;
        out[1 + i * 4 + 1] = rect.nRight;
        out[1 + i * 4 + 2] = rect.nTop;
        out[1 + i * 4 + 3] = rect.nBottom;
    }
    return (uint32_t)boxes;
}

EMSCRIPTEN_KEEPALIVE uint32_t d2_session_room_edge_flags(uint32_t roomOrdinal, uint32_t* out, uint32_t cap) {
    if (!g_sessionLive || !g_lastLevel || cap < 5) return 0;
    uint32_t ordinal = 0;
    for (D2DrlgRoomStrc* roomEx = g_lastLevel->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext, ++ordinal) {
        if (ordinal != roomOrdinal) continue;
        if (!roomEx->pOutdoor) return 0;          // preset rooms keep their grids elsewhere
        D2DrlgGridStrc* grid = &roomEx->pOutdoor->pFloorGrid;
        // A room may carry an outdoor struct whose grid was never built (preset rooms reuse the
        // slot). Walking it on the strength of a non-null pOutdoor reads out of bounds.
        if (!grid->pCellsFlags || !grid->pCellsRowOffsets) return 0;
        if (grid->nWidth <= 0 || grid->nHeight <= 0 || grid->nWidth > 64 || grid->nHeight > 64) return 0;
        out[0] = (uint32_t)grid->nWidth; out[1] = (uint32_t)grid->nHeight;
        out[2] = out[3] = out[4] = 0;
        for (int y = 0; y < grid->nHeight; ++y) {
            for (int x = 0; x < grid->nWidth; ++x) {
                const uint32_t flags = (uint32_t)DRLGGRID_GetGridEntry(grid, x, y);
                const bool edge = (x == roomEx->nTileWidth) || (y == roomEx->nTileHeight);
                if (edge) { if (flags & 0x2) ++out[2]; if (flags & 0x4) ++out[3]; }
                else if (flags & 0x2) ++out[4];
            }
        }
        return 1;
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE void d2_session_trace_rolls(int enable) {
    g_d2webRollTraceEnabled = enable != 0; D2WEB_ResetRollTrace();
}
EMSCRIPTEN_KEEPALIVE const D2WebRollTrace* d2_session_tile_rolls() {
    updateSessionSnapshot(); return g_sessionTileRolls.data();
}
EMSCRIPTEN_KEEPALIVE int d2_session_tile_roll_count() {
    updateSessionSnapshot(); return (int)g_sessionTileRolls.size();
}
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_roll_orphans() { return g_d2webRollOrphans; }
// Cross-room rolls as (room ordinal, that room's roll ordinal, causing room ordinal) triples,
// room ordinals in d2_session_rooms order; UINT32_MAX for a room outside the current level.
EMSCRIPTEN_KEEPALIVE int d2_session_cross_rolls(uint32_t* out, int cap) {
    if (!g_sessionLive || !g_lastLevel || !out) return 0;
    auto ordinalOf = [](const D2DrlgRoomStrc* p) {
        uint32_t n = 0;
        for (D2DrlgRoomStrc* r = g_lastLevel->pFirstRoomEx; r; r = r->pDrlgRoomNext, ++n) if (r == p) return n;
        return UINT32_MAX;
    };
    int written = 0;
    for (const D2WebCrossRoll& c : g_d2webCrossRolls) {
        if (written + 3 > cap) break;
        out[written++] = ordinalOf(c.pRoom); out[written++] = c.nOrdinal; out[written++] = ordinalOf(c.pCause);
    }
    return written / 3;
}
EMSCRIPTEN_KEEPALIVE const D2SessionTile* d2_session_tiles() {
    updateSessionSnapshot(); return g_sessionTiles.data();
}
EMSCRIPTEN_KEEPALIVE int d2_session_tile_count() {
    updateSessionSnapshot(); return (int)g_sessionTiles.size();
}
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_terrain_hash() {
    updateSessionSnapshot(); return g_sessionTerrainHash;
}

// The DT1 tile libraries the current level's rooms actually loaded.
//
// This is what a client needs to draw the real map: `d2_session_tiles` gives a per-tile draw list
// straight out of the DRLG -- orientation, mainIndex/subIndex/frameIndex and screen coordinates --
// and each tile's `librarySlot` indexes THIS list.
//
// It deliberately does NOT walk LvlTypes.txt. That table lists the level TYPE's files, while rooms
// load what they need: a preset room pulls DT1s in through its DS1 that LvlTypes never mentions
// (Andariel's Lair loads Act1\Outdoors\Blank.dt1, absent from the Catacombs entry). Resolving
// through the type's table therefore pointed tiles at the wrong file or at none, and whole dungeon
// floors went undrawn. Paths are written back to back as NUL-terminated strings; returns the count.
// What ONE room loaded, for the dwDT1Mask question.
//
// d2_session_level_tile_files lists only libraries a placed tile came from, which cannot show a
// library that SHOULD have been loaded and was not -- the exact shape of the 43 fallbacks. This
// reports the room's own view instead, as newline-separated text:
//   mask <hex> type <levelType>
//   slot <i> <file>          -- pTiles[] in slot order, i.e. GetTiles' concatenation order
//   lvltype <bit> <file>     -- every non-empty LvlTypes.txt szFile the mask indexes
// Diagnostic only; nothing in the client reads it.
EMSCRIPTEN_KEEPALIVE int d2_session_room_libraries(uint32_t roomOrdinal, char* out, int cap) {
    if (!g_sessionLive || !g_lastLevel || !out || cap <= 0) return 0;
    uint32_t ordinal = 0;
    for (D2DrlgRoomStrc* roomEx = g_lastLevel->pFirstRoomEx; roomEx; roomEx = roomEx->pDrlgRoomNext, ++ordinal) {
        if (ordinal != roomOrdinal) continue;
        std::string text;
        char line[160];
        snprintf(line, sizeof line, "mask %08x type %d\n", roomEx->dwDT1Mask, (int)g_lastLevel->nLevelType);
        text += line;
        for (int slot = 0; slot < 32 && roomEx->pTiles[slot]; ++slot) {
            const char* name = D2WEB_TileLibraryNameFromHash(roomEx->pTiles[slot]);
            snprintf(line, sizeof line, "slot %d %s\n", slot, name ? name : "?");
            text += line;
        }
        if (D2LvlTypesTxt* types = DATATBLS_GetLevelTypesTxtRecord(g_lastLevel->nLevelType)) {
            for (int bit = 0; bit < 32; ++bit) {
                if (!types->szFile[bit][0]) continue;
                snprintf(line, sizeof line, "lvltype %d %.60s\n", bit, types->szFile[bit]);
                text += line;
            }
        }
        const int n = (int)std::min<size_t>(text.size(), (size_t)cap - 1);
        memcpy(out, text.data(), (size_t)n); out[n] = 0;
        return n;
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE int d2_session_level_tile_files(char* out, int cap) {
    if (!g_sessionLive || !out || cap <= 0) return 0;
    // The libraries the level's rooms ACTUALLY loaded, in the order a tile's librarySlot indexes.
    // This used to walk LvlTypes.txt instead; that table is the level TYPE's list and does not
    // include DT1s a preset room pulls in through its DS1, so tiles from those libraries resolved
    // against the wrong file or none at all and whole dungeon floors went undrawn.
    int written = 0, used = 0;
    for (const std::string& name : g_sessionTileLibraries) {
        const int n = (int)name.size() + 1;
        if (used + n > cap) break;
        memcpy(out + used, name.c_str(), (size_t)n);
        used += n; written++;
    }
    return written;
}

EMSCRIPTEN_KEEPALIVE int d2_session_set_character_name(const char* name) {
    if (!name) return 0;
    const size_t n = strlen(name);
    if (n == 0 || n > sizeof g_sessionCharName - 1) return 0;
    memset(g_sessionCharName, 0, sizeof g_sessionCharName);
    memcpy(g_sessionCharName, name, n);
    if (g_targetClient) {
        memset(g_targetClient->szName, 0, sizeof g_targetClient->szName);
        memcpy(g_targetClient->szName, g_sessionCharName, n);
    }
    return 1;
}

// The complete .d2s for the live character. Returns the byte count written, 0 on failure; the
// PLRSAVE2 error code is available from d2_session_save_d2s_error().
static std::vector<uint8_t> g_sessionSaveFile;
static int g_sessionSaveError = 0;

EMSCRIPTEN_KEEPALIVE int d2_session_save_d2s(uint8_t* out, int cap) {
    g_sessionSaveError = 0;
    if (!g_sessionLive || !g_game || !g_targetPlayer || !out || cap <= 0) { g_sessionSaveError = -1; return 0; }
    constexpr uint32_t kCapacity = 0x2000;   // retail's own buffer size in D2GAME_SAVE_WriteFile
    g_sessionSaveFile.assign(kCapacity, 0);
    uint32_t written = 0;
    g_sessionSaveError = PLRSAVE2_CreateSaveFile(g_game, g_targetPlayer, g_sessionSaveFile.data(),
        &written, kCapacity, 0, 0);
    if (g_sessionSaveError != 0 || written == 0 || (int)written > cap) {
        if (g_sessionSaveError == 0) g_sessionSaveError = -2;
        return 0;
    }
    memcpy(out, g_sessionSaveFile.data(), written);
    return (int)written;
}

EMSCRIPTEN_KEEPALIVE int d2_session_save_d2s_error() { return g_sessionSaveError; }

// Load a .d2s into the live player. Returns 0 on success, otherwise the PLRSAVE2 error code.
EMSCRIPTEN_KEEPALIVE int d2_session_load_d2s(const uint8_t* data, int len) {
    if (!g_sessionLive || !g_game || !g_targetPlayer || !data || len <= 0) return -1;
    std::vector<uint8_t> copy(data, data + len);
    return D2WEB_PLRSAVE2_LoadIntoPlayer(g_game, g_targetPlayer, copy.data(), (uint32_t)copy.size());
}

EMSCRIPTEN_KEEPALIVE int d2_session_level_type() {
    return (g_sessionLive && g_lastLevel) ? g_lastLevel->nLevelType : -1;
}
EMSCRIPTEN_KEEPALIVE const D2SessionWarp* d2_session_warps() {
    updateSessionSnapshot(); return g_sessionWarps.data();
}
EMSCRIPTEN_KEEPALIVE int d2_session_warp_count() {
    updateSessionSnapshot(); return (int)g_sessionWarps.size();
}
EMSCRIPTEN_KEEPALIVE uint32_t d2_session_navigation_hash() {
    updateSessionSnapshot(); return g_sessionNavigationHash;
}

EMSCRIPTEN_KEEPALIVE void d2_session_destroy() {
    d2_monspawn_reset(); g_sessionLive = false; g_sessionMovePending = false;
    g_sessionAttackTarget = nullptr; g_sessionAttackSkill = nullptr;
    g_sessionRooms.clear(); g_sessionUnits.clear(); g_sessionMonsterAnimations.clear(); g_sessionObjectPresentations.clear(); g_sessionItemPresentations.clear(); g_sessionTiles.clear(); g_sessionTileRolls.clear(); g_sessionTileLibraries.clear(); g_sessionWarps.clear();
    memset(&g_sessionItemState, 0, sizeof g_sessionItemState);
    memset(&g_sessionNpcInteraction, 0, sizeof g_sessionNpcInteraction);
    memset(&g_sessionNpcDialog, 0, sizeof g_sessionNpcDialog); D2WEB_ResetLastDialogPacket();
    memset(&g_sessionNpcResponse, 0, sizeof g_sessionNpcResponse);
    memset(&g_sessionVendor, 0, sizeof g_sessionVendor);
    g_sessionVendorItems.clear(); memset(&g_sessionVendorTransaction, 0, sizeof g_sessionVendorTransaction);
    g_sessionWorldHash = D2VEC_FNV_INIT; g_sessionTerrainHash = D2VEC_FNV_INIT;
    g_sessionNavigationHash = D2VEC_FNV_INIT;
    updateSessionSnapshot();
}

} // extern "C"
