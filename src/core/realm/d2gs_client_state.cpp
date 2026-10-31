// Client-side D2GS state. Packet schemas stay in d2gs_client_packets.cpp; server GUIDs own their
// render and path records here and the offline game session remains separate.
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cstdlib>
#include <D2Seed.h>
#include <D2StatList.h>
#include <Units/Missile.h>
#include <DataTbls/MissilesTbls.h>
#include <cstring>
#include <emscripten.h>
#include <utility>
#include <vector>

#include <D2DataTbls.h>
#include <D2Collision.h>
#include <D2Dungeon.h>
#include <DataTbls/ItemsTbls.h>
#include <DataTbls/MonsterTbls.h>
#include <D2Monsters.h>
#include <D2States.h>
#include <D2StatList.h>
#include <D2Skills.h>
#include <D2Skills.h>
#include <D2Inventory.h>
#include <DataTbls/SkillsIds.h>
#include <Path/Path.h>
#include <Path/PathMisc.h>
#include <Units/Units.h>
#include <Units/Player.h>
#include <Units/UnitRoom.h>
#include <Drlg/D2DrlgDrlg.h>
#include "d2gs_client_motion.h"
#include "d2gs_client_target_relations.h"
#include "d2gs_client_owned_item.h"
#include "d2gs_client_unit_resources.h"
#include "client_fireball_hit.h"
#include "client_missile_sound.h"
#include "d2gs_client_player_motion.h"
#include "d2gs_client_player_stop.h"
#include "d2gs_client_monster_death_events.h"
#include "cast_target_relation.h"
#include "remote_unit_skill_start.h"
#include "d2gs_client_monster_death_clock.h"

struct D2RealmPlayerAssign {
    uint32_t guid;
    uint8_t classId;
    uint8_t name[16];
    uint16_t x;
    uint16_t y;
};

struct D2RealmPlayerReassign {
    uint32_t guid;
    uint16_t x;
    uint16_t y;
    uint8_t unitType;
    uint8_t value;
    uint8_t reserved[2];
};

struct D2RealmGameChat {
    uint32_t version, size, messageType, unknown2, subType, unitGuid, unknown8, color, nameLength, messageLength;
    uint8_t name[16];
    uint8_t message[256];
};

static_assert(offsetof(D2RealmPlayerReassign, guid) == 0);
static_assert(offsetof(D2RealmPlayerReassign, x) == 4);
static_assert(offsetof(D2RealmPlayerReassign, y) == 6);
static_assert(offsetof(D2RealmPlayerReassign, unitType) == 8);
static_assert(offsetof(D2RealmPlayerReassign, value) == 9);
static_assert(sizeof(D2RealmPlayerReassign) == 12);
static_assert(offsetof(D2RealmGameChat, name) == 40);
static_assert(offsetof(D2RealmGameChat, message) == 56);
static_assert(sizeof(D2RealmGameChat) == 312);

struct D2RealmClientPlayerState {
    uint32_t version;
    uint32_t size;
    uint32_t active;
    uint32_t guid;
    uint32_t x;
    uint32_t y;
    uint8_t classId;
    uint8_t reassignValue;
    uint8_t reserved[2];
    uint32_t reassignments;
    uint8_t name[16];
    uint32_t clientRoomActive;
    uint32_t clientRoomLevel;
    uint32_t clientRoomTileX;
    uint32_t clientRoomTileY;
    uint32_t cunitInitSeed;
    uint32_t cunitInitSeedValid;
    uint32_t cunitRngLow;
    uint32_t cunitRngHigh;
    uint32_t cunitRngValid;
    uint32_t initialMode;
    uint32_t initialDirection;
};

static_assert(offsetof(D2RealmClientPlayerState, active) == 8);
static_assert(offsetof(D2RealmClientPlayerState, guid) == 12);
static_assert(offsetof(D2RealmClientPlayerState, x) == 16);
static_assert(offsetof(D2RealmClientPlayerState, y) == 20);
static_assert(offsetof(D2RealmClientPlayerState, classId) == 24);
static_assert(offsetof(D2RealmClientPlayerState, reassignValue) == 25);
static_assert(offsetof(D2RealmClientPlayerState, reassignments) == 28);
static_assert(offsetof(D2RealmClientPlayerState, name) == 32);
static_assert(offsetof(D2RealmClientPlayerState, clientRoomActive) == 48);
static_assert(offsetof(D2RealmClientPlayerState, clientRoomLevel) == 52);
static_assert(offsetof(D2RealmClientPlayerState, clientRoomTileX) == 56);
static_assert(offsetof(D2RealmClientPlayerState, clientRoomTileY) == 60);
static_assert(offsetof(D2RealmClientPlayerState, cunitInitSeed) == 64);
static_assert(offsetof(D2RealmClientPlayerState, cunitInitSeedValid) == 68);
static_assert(offsetof(D2RealmClientPlayerState, cunitRngLow) == 72);
static_assert(offsetof(D2RealmClientPlayerState, cunitRngHigh) == 76);
static_assert(offsetof(D2RealmClientPlayerState, cunitRngValid) == 80);
static_assert(offsetof(D2RealmClientPlayerState, initialMode) == 84);
static_assert(offsetof(D2RealmClientPlayerState, initialDirection) == 88);
static_assert(sizeof(D2RealmClientPlayerState) == 92);

// Keep the renderer/API record at its established 92-byte ABI. The canonical roster entry owns
// the one client path unit associated with that server GUID; there is no parallel motion map.
struct D2RealmClientPlayerEntry {
    D2RealmClientPlayerState view{};
    D2UnitStrc* motionUnit = nullptr;
    uint8_t motionDirection = 0;
    struct MotionView {
        uint32_t version = 1;
        uint32_t size = 48;
        uint32_t active = 0;
        uint32_t mode = 0;
        uint32_t direction = 0;
        uint32_t framePrecise = 0;
        uint32_t frameCountPrecise = 0;
        uint32_t animSpeed = 0;
        uint32_t positionHintX = 0;
        uint32_t positionHintY = 0;
        uint32_t seqMode = 0;   // PLRMODE_SEQUENCE: the current frame's mode (dwSeqMode), else 0
        uint32_t seqFrame = 0;  // PLRMODE_SEQUENCE: the current frame index (nSeqCurrentFramePrecise >> 8), else 0
    } motionView{};
};
static_assert(sizeof(D2RealmClientPlayerEntry::MotionView) == 48);

// The 0x03 packet's final dword is retained raw until its meaning is fully resolved for 1.13c.
struct D2RealmClientWorldState {
    uint32_t version;
    uint32_t size;
    uint32_t hasLoadAct;
    uint32_t act;
    uint32_t mapSeed;
    uint32_t area;
    uint32_t rawLoadActTail;
    uint32_t loadCompleteEvent;
    uint32_t loadActCount;
    uint32_t unloadCompleteEvent;
    uint32_t hasMapReveal;
    uint32_t mapLevel;
    uint32_t mapX;
    uint32_t mapY;
    uint32_t mapRevealCount;
    uint32_t difficulty;
    uint32_t mapHideCount;
    uint32_t mapRevision;
};

static_assert(offsetof(D2RealmClientWorldState, hasLoadAct) == 8);
static_assert(offsetof(D2RealmClientWorldState, act) == 12);
static_assert(offsetof(D2RealmClientWorldState, mapSeed) == 16);
static_assert(offsetof(D2RealmClientWorldState, area) == 20);
static_assert(offsetof(D2RealmClientWorldState, rawLoadActTail) == 24);
static_assert(offsetof(D2RealmClientWorldState, loadCompleteEvent) == 28);
static_assert(offsetof(D2RealmClientWorldState, loadActCount) == 32);
static_assert(offsetof(D2RealmClientWorldState, unloadCompleteEvent) == 36);
static_assert(offsetof(D2RealmClientWorldState, hasMapReveal) == 40);
static_assert(offsetof(D2RealmClientWorldState, mapLevel) == 44);
static_assert(offsetof(D2RealmClientWorldState, mapX) == 48);
static_assert(offsetof(D2RealmClientWorldState, mapY) == 52);
static_assert(offsetof(D2RealmClientWorldState, mapRevealCount) == 56);
static_assert(offsetof(D2RealmClientWorldState, difficulty) == 60);
static_assert(offsetof(D2RealmClientWorldState, mapHideCount) == 64);
static_assert(offsetof(D2RealmClientWorldState, mapRevision) == 68);
static_assert(sizeof(D2RealmClientWorldState) == 72);

// Renderer-facing remote unit projection. The server stream owns these records; motionUnit is a
// client-side DynamicPath carrier, not an offline D2Game session actor with local AI.
struct D2RealmClientMonsterView {
    uint32_t version;
    uint32_t size;
    uint32_t guid;
    uint32_t classId;
    uint32_t code;
    uint32_t mode;
    uint32_t framePrecise;
    uint32_t frameCountPrecise;
    uint32_t componentCodes[16];
    uint16_t animSpeed;
    uint8_t direction;
    uint8_t reserved;
    uint32_t x;
    uint32_t y;
    uint32_t hpPct;
    uint32_t typeFlags;
};

static_assert(offsetof(D2RealmClientMonsterView, guid) == 8);
static_assert(offsetof(D2RealmClientMonsterView, componentCodes) == 32);
static_assert(offsetof(D2RealmClientMonsterView, animSpeed) == 96);
static_assert(offsetof(D2RealmClientMonsterView, x) == 100);
static_assert(offsetof(D2RealmClientMonsterView, typeFlags) == 112);
static_assert(sizeof(D2RealmClientMonsterView) == 116);

struct D2RealmClientMonsterStat {
    uint16_t statId;
    int32_t parameter;
    int32_t value;
};

// Narrow diagnostic snapshot for parser verification. Kept separate from the stable renderer
// record so exposing optional packet sections does not silently widen its ABI.
struct D2RealmClientMonsterAux {
    uint32_t version;
    uint32_t size;
    uint32_t guid;
    uint32_t hasExtra31;
    uint32_t extra31;
    uint32_t hasStats;
    uint32_t statCount;
    uint32_t firstStatId;
    int32_t firstStatParameter;
    int32_t firstStatValue;
    uint32_t unitFlags;
};
static_assert(offsetof(D2RealmClientMonsterAux, guid) == 8);
static_assert(offsetof(D2RealmClientMonsterAux, extra31) == 16);
static_assert(offsetof(D2RealmClientMonsterAux, firstStatValue) == 36);
static_assert(offsetof(D2RealmClientMonsterAux, unitFlags) == 40);
static_assert(sizeof(D2RealmClientMonsterAux) == 44);

// Read-only route snapshot used by browser fixture assertions to distinguish failed replanning
// from a tick that simply has no path to advance.
struct D2RealmClientMotionStatus {
    uint32_t version;
    uint32_t size;
    uint32_t guid;
    uint32_t attached;
    uint32_t pathFlags;
    uint32_t pathType;
    uint32_t pathPoints;
    uint32_t currentPoint;
    uint32_t startX;
    uint32_t startY;
    uint32_t targetX;
    uint32_t targetY;
    uint32_t mode;
    uint32_t framePrecise;
    uint32_t frameCountPrecise;
    uint32_t animSpeed;
    int32_t lastPlanResult;
    int32_t lastTickResult;
};
static_assert(offsetof(D2RealmClientMotionStatus, guid) == 8);
static_assert(offsetof(D2RealmClientMotionStatus, pathFlags) == 16);
static_assert(offsetof(D2RealmClientMotionStatus, lastPlanResult) == 64);
static_assert(sizeof(D2RealmClientMotionStatus) == 72);

struct D2RealmClientMotionStatusV2 {
    D2RealmClientMotionStatus base;
    uint32_t roomLevel;
    uint32_t roomTileX;
    uint32_t roomTileY;
    uint32_t roomTileWidth;
    uint32_t roomTileHeight;
    uint32_t clientSightRefs;
};
static_assert(offsetof(D2RealmClientMotionStatusV2, roomLevel) == 72);
static_assert(offsetof(D2RealmClientMotionStatusV2, clientSightRefs) == 92);
static_assert(sizeof(D2RealmClientMotionStatusV2) == 96);

struct D2RealmClientMotionStatusV3 {
    D2RealmClientMotionStatusV2 base;
    uint32_t currentDirection;
    uint32_t targetDirection;
    uint32_t directionDelta;
    uint32_t directionFlags;
};
static_assert(offsetof(D2RealmClientMotionStatusV3, currentDirection) == 96);
static_assert(offsetof(D2RealmClientMotionStatusV3, directionFlags) == 108);
static_assert(sizeof(D2RealmClientMotionStatusV3) == 112);

struct D2RealmClientMonster {
    D2RealmClientMonsterView view{};
    uint8_t rawPrefix;
    bool hasExtra31 = false;
    bool hasStats = false;
    int16_t superUniqueIndex;
    uint16_t nameId;
    uint32_t ownerGuid;
    uint32_t extra31;
    std::vector<uint8_t> nameBytes;
    std::vector<D2RealmClientMonsterStat> stats;
    D2UnitStrc* motionUnit = nullptr;
    int32_t lastPlanResult = 0;
    int32_t lastTickResult = 0;
};

namespace {
constexpr int kPendingCapacity = 131072;
uint8_t g_pending[kPendingCapacity];
int g_pendingLength;
D2RealmClientPlayerState g_player;
std::vector<D2RealmClientPlayerEntry> g_players;
int g_selectedPlayer = -1;
D2RealmClientWorldState g_world;
D2RealmGameChat g_gameChat{1, sizeof(D2RealmGameChat)};
uint32_t g_gameChatCount;
uint32_t g_clientDifficulty;
std::vector<D2RealmClientMonster> g_monsters;
std::vector<D2RealmClientMonsterView> g_monsterViews;
std::vector<d2::realm::MonsterDeathModeEvent> g_monsterDeathModeEvents;
std::vector<d2::realm::MonsterDeathModeEvent> g_monsterDeathModeEventView;
uint32_t g_nextMonsterDeathModeSequence = 1;

void clear_monster_death_mode_events() {
    g_monsterDeathModeEvents.clear();
    g_monsterDeathModeEventView.clear();
}

void reset_monster_death_mode_events() {
    clear_monster_death_mode_events();
    g_nextMonsterDeathModeSequence = 1;
}

class D2RealmBitReader {
public:
    D2RealmBitReader(const uint8_t* bytes, int length) : bytes_(bytes), bits_(length > 0 ? length * 8 : 0) {}
    bool read(uint32_t count, uint32_t& value) {
        if (count > 32 || position_ > bits_ || count > bits_ - position_) return false;
        value = 0;
        for (uint32_t i = 0; i < count; ++i) {
            value |= static_cast<uint32_t>((bytes_[position_ >> 3] >> (position_ & 7)) & 1u) << i;
            ++position_;
        }
        return true;
    }
    bool readSigned(uint32_t count, int32_t& value) {
        if (count == 0 || count > 32) return false;
        uint32_t raw = 0;
        if (!read(count, raw)) return false;
        if (count == 32) value = static_cast<int32_t>(raw);
        else if (raw & (1u << (count - 1))) value = static_cast<int32_t>(raw | (~0u << count));
        else value = static_cast<int32_t>(raw);
        return true;
    }
    uint32_t remaining() const { return position_ <= bits_ ? bits_ - position_ : 0; }
private:
    const uint8_t* bytes_;
    uint32_t bits_;
    uint32_t position_ = 0;
};

uint32_t componentWidth(uint8_t count) {
    if (count < 3) return 1; // Exact native 1.13c rule, including counts 0 and 1.
    uint32_t value = static_cast<uint32_t>(count) - 1;
    uint32_t width = 0;
    while (value) { ++width; value >>= 1; }
    return width;
}

void clear_player() {
    std::memset(&g_player, 0, sizeof(g_player));
    g_player.version = 5;
    g_player.size = sizeof(g_player);
}

static void destroy_player_owned_items(D2UnitStrc* owner) {
    // Free item CUnits before the inventory/resource owner is released; INVENTORY_FreeInventory
    // only detaches items from the inventory.
    if (!owner || !owner->pInventory || owner->pInventory->dwSignature != D2C_InventoryHeader) return;
    D2UnitStrc* item = INVENTORY_GetFirstItem(owner->pInventory);
    while (item) {
        D2UnitStrc* next = INVENTORY_GetNextItem(item);
        INVENTORY_RemoveItem(item);
        d2_realm_client_owned_item_destroy(item);
        item = next;
    }
}

void clear_players() {
    for (auto& player : g_players) {
        if (!player.motionUnit) continue;
        d2_realm_client_motion_detach(player.motionUnit);
        destroy_player_owned_items(player.motionUnit);
        d2_realm_client_unit_resources_destroy(player.motionUnit);
        UNITS_FreeUnit(player.motionUnit);
        player.motionUnit = nullptr;
    }
    g_players.clear();
    g_selectedPlayer = -1;
    clear_player();
}

void store_selected_player() {
    if (g_selectedPlayer >= 0 && static_cast<size_t>(g_selectedPlayer) < g_players.size())
        g_players[g_selectedPlayer].view = g_player;
}

void clear_world() {
    std::memset(&g_world, 0, sizeof(g_world));
    g_world.version = 2;
    g_world.size = sizeof(g_world);
}

void clear_game_chat() {
    std::memset(&g_gameChat, 0, sizeof(g_gameChat));
    g_gameChat.version = 1;
    g_gameChat.size = sizeof(g_gameChat);
    g_gameChatCount = 0;
}
}

extern "C" {
int d2_realm_d2gs_parse_gamechat(const uint8_t*, int, D2RealmGameChat*);
int d2_realm_d2gs_parse_player_assign(const uint8_t*, int, D2RealmPlayerAssign*);
int d2_realm_d2gs_parse_player_reassign(const uint8_t*, int, D2RealmPlayerReassign*);
int d2_realm_d2gs_parse_player_move(const uint8_t*, int, D2RealmPlayerMove*);
int d2_realm_client_apply_player_move(D2UnitStrc*, const D2RealmPlayerMove*);
int d2_realm_d2gs_packet_size(const uint8_t*, int);
int d2_drlg_init();
int d2_drlg_client_act_begin(uint32_t, int, int, int);
void d2_drlg_act_end();
int d2_drlg_client_map_reveal(int, int, int, int, int, int, int);
int d2_drlg_client_map_hide(int, int, int, int, int, int, int);
int d2_drlg_client_render_level();
int d2_drlg_client_select_render_level(int);
int d2_drlg_client_level_at(int, int);
int d2_drlg_client_player_context(int, int, int, uint32_t*, uint32_t*, uint32_t*, uint32_t*);
int d2_drlg_client_unit_context(int, int, int, uint32_t*, uint32_t*, uint32_t*, uint32_t*);
int d2_session_client_cunit_seed_init(D2SeedStrc*, uint32_t*, int, int, int);
int d2_drlg_client_tile_count();
D2DrlgActStrc* d2_drlg_client_act();
D2ActiveRoomStrc* d2_drlg_client_active_room(int, int);
int d2_drlg_client_room_freed_by_hide(int, int, int, int, int, int, int, void*);

bool d2_realm_client_motion_attach(D2UnitStrc* unit, int x, int y) {
    if (!unit) return false;
    if (unit->pDynamicPath) return true;
    D2DrlgActStrc* act = d2_drlg_client_act();
    D2ActiveRoomStrc* room = d2_drlg_client_active_room(x, y);
    if (!act || !room) return false;
    unit->nAct = act->nAct;
    unit->pDrlgAct = act;
    unit->dwFlags |= UNITFLAG_ISCLIENTUNIT;
    PATH_AllocDynamicPath(nullptr, room, x, y, unit, FALSE);
    return unit->pDynamicPath != nullptr;
}

static int d2_realm_client_unit_set_mode(D2UnitStrc* unit, uint32_t mode) {
    if (!unit) return -1;
    if (d2::realm::monsterAnimationModeNeedsSetup(unit->dwAnimMode, mode,
            unit->dwFrameCountPrecise)) {
        unit->dwAnimMode = mode;
        UNITS_SetAnimStartFrame(unit);
    }
    return 1;
}

int d2_realm_client_motion_set_mode(D2UnitStrc* unit, uint32_t mode) {
    if (!unit || !unit->pDynamicPath) return -1;
    return d2_realm_client_unit_set_mode(unit, mode);
}

EMSCRIPTEN_KEEPALIVE uint32_t d2_realm_client_motion_refusal_mode(uint32_t unitType, uint32_t currentMode) {
    // FUN_6FAFDC30's native guard: monster type, mode 1..15, except MONMODE_DEAD (12).
    return unitType == UNIT_MONSTER && currentMode > MONMODE_DEATH && currentMode < NUMBER_OF_MONMODES &&
        currentMode != MONMODE_DEAD ? MONMODE_NEUTRAL : currentMode;
}

int d2_realm_client_motion_set_target(D2UnitStrc* unit, int x, int y) {
    if (!unit || !unit->pDynamicPath) return -1;
    D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, x, y);
    return D2Common_10142(unit->pDynamicPath, unit, FALSE); // Local D2Common ordinal 10334.
}

static uint8_t d2_realm_client_motion_step_facing(uint8_t current, uint8_t target,
        uint8_t delta, uint32_t flags) {
    // Re-derived from local D2Common 1.13c #10603 (VA 0x6FD84D70). Flag 0x40 holds
    // the current facing. Otherwise take the signed-byte step modulo 64 and snap only
    // when the native overshoot predicates show the target was crossed.
    if (current == target || (flags & PATH_UNKNOWN_FLAG_0x00040)) return current;
    current = static_cast<uint8_t>((static_cast<int8_t>(current) + delta) & 0x3f);
    const uint8_t remaining = static_cast<uint8_t>((static_cast<int8_t>(current) -
        static_cast<int8_t>(target)) & 0x3f);
    if (remaining > 31 && delta > 63) current = target;
    else if (remaining < 31 && delta < 63) current = target;
    return current;
}

EMSCRIPTEN_KEEPALIVE int d2_realm_client_motion_facing_step(uint32_t current, uint32_t target,
        uint32_t delta, uint32_t flags) {
    if (current > 255 || target > 255 || delta > 255) return -1;
    return d2_realm_client_motion_step_facing(static_cast<uint8_t>(current),
        static_cast<uint8_t>(target), static_cast<uint8_t>(delta), flags);
}

int d2_realm_client_motion_advance_unit(D2UnitStrc* unit) {
    if (!unit || !unit->pDynamicPath) return -1;
    if (!unit->dwFrameCountPrecise) UNITS_SetAnimStartFrame(unit);
    const int moving = D2Common_10226(unit, 1024);
    if (!d2_realm_client_apply_monster_death_terminal_clock(*unit,
            [](D2UnitStrc* candidate) { return UNITS_IsAtEndOfFrameCycle(candidate) != 0; })) {
        UNITS_StopSequence(unit);
    }
    D2DynamicPathStrc* path = unit->pDynamicPath;
    path->nDirection = d2_realm_client_motion_step_facing(path->nDirection, path->nNewDirection,
        path->nDiffDirection, path->dwFlags);
    return moving;
}

void d2_realm_client_motion_detach(D2UnitStrc* unit) {
    if (!unit || !unit->pDynamicPath) return;
    PATH_RemoveCollisionFootprintForUnit(unit, TRUE);
    UNITROOM_RemoveUnitFromRoom(unit);
    PATH_FreeDynamicPath(nullptr, unit->pDynamicPath);
    unit->pDynamicPath = nullptr;
}

int d2_realm_client_motion_apply_player_move(D2UnitStrc* unit, uint8_t action, uint8_t opaque,
        uint16_t targetX, uint16_t targetY) {
    if (!unit || unit->dwUnitType != UNIT_PLAYER || !unit->pDynamicPath) return -1;
    (void)opaque;
    if (action != 0x01 && action != 0x17) return 0;
    // FAC9830 selects both its success mode and its shared planner-refusal fallback from the
    // current room. It plans first; only a successful route receives the requested walk/run mode.
    D2ActiveRoomStrc* room = UNITS_GetRoom(unit);
    if (!room) return -1;
    const bool inTown = DUNGEON_IsRoomInTown(room) != FALSE;
    const uint32_t mode = action == 0x17 ? PLRMODE_RUN :
        (inTown ? PLRMODE_TOWNWALK : PLRMODE_WALK);
    const uint32_t refusalMode = inTown ? PLRMODE_TOWNNEUTRAL : PLRMODE_NEUTRAL;
    D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, targetX, targetY);
    if (D2Common_10142(unit->pDynamicPath, unit, FALSE) <= 0) {
        // The native failed-route branch goes through FUN_6FAC9710 with EAX=1/5. That helper
        // clears unit flag 0x40 and sets the normal/town neutral mode; it does not keep the old
        // mode. It is shared by failed action-1 and action-0x17 route attempts.
        unit->dwFlags &= ~0x40u;
        d2_realm_client_motion_set_mode(unit, refusalMode);
        return 0;
    }
    // FUN_6FADA6C0 clears this same flag before its successful mode transition.
    unit->dwFlags &= ~0x40u;
    return d2_realm_client_motion_set_mode(unit, mode) > 0 ? 1 : -1;
}

static void clear_target_relations_to(D2UnitStrc* target);

static void destroy_monster_motion(D2RealmClientMonster& monster) {
    D2UnitStrc* unit = monster.motionUnit;
    if (!unit) return;
    clear_target_relations_to(unit);
    d2_realm_client_motion_detach(unit);
    d2_realm_client_unit_resources_destroy(unit);
    UNITS_FreeUnit(unit);
    monster.motionUnit = nullptr;
}

static void destroy_all_monster_motion() {
    for (auto& monster : g_monsters) destroy_monster_motion(monster);
}

static void destroy_player_motion(D2RealmClientPlayerEntry& player);

static void refresh_monster_motion_view(D2RealmClientMonster& monster);

static bool create_monster_motion(D2RealmClientMonster& monster) {
    D2MonStatsTxt* stats = DATATBLS_GetMonStatsTxtRecord(static_cast<int>(monster.view.classId));
    if (!stats) return false;
    D2UnitStrc* unit = monster.motionUnit;
    if (!unit) {
        D2MonStats2Txt* stats2 = UNITS_GetMonStats2TxtRecordFromMonsterId(static_cast<int>(monster.view.classId));
        if (!stats2) return false;
        unit = UNITS_AllocUnit(nullptr, UNIT_MONSTER);
        if (!unit) return false;
        unit->dwClassId = static_cast<int32_t>(monster.view.classId);
        unit->dwUnitId = monster.view.guid;
        unit->dwAnimMode = MONMODE_NEUTRAL;
        // D2Client 1.13c FUN_6FAFD160 always marks a new monster as a valid target, then
        // mirrors MonStats2.isAtt into CANBEATTACKED. The collision predicate requires both.
        unit->dwFlags |= UNITFLAG_ISVALIDTARGET;
        if (stats2->dwFlags & MONSTATS2FLAG_ISATT) unit->dwFlags |= UNITFLAG_CANBEATTACKED;
        else unit->dwFlags &= ~UNITFLAG_CANBEATTACKED;
        if (sgptDataTables && sgptDataTables->nStatesTxtRecordCount > 0)
            STATLIST_AllocStatListEx(unit, 0, nullptr, nullptr);
        // the monster's base walk speed, as D2Game's monster init sets it (Monster.cpp STAT_VELOCITYPERCENT 75); the client unit needs it to walk at the server's pace
        STATLIST_SetUnitStat(unit, STAT_VELOCITYPERCENT, 75, 0);
        if (!d2_realm_client_unit_resources_init(unit)) {
            d2_realm_client_unit_resources_destroy(unit);
            UNITS_FreeUnit(unit);
            return false;
        }
        monster.motionUnit = unit;
    }
    // Initialize the client-owned frame state even before MapReveal attaches a dynamic path.
    // This is the CUnit state Common #11090 initializes when D2Client receives a mode update.
    if (unit->dwAnimMode != monster.view.mode || !unit->dwFrameCountPrecise)
        d2_realm_client_unit_set_mode(unit, monster.view.mode);
    // The GUID-owned CUnit can receive state packets before MapReveal activates its room.
    // Keep the canonical unit/stat lists alive and attach its path when the room becomes active.
    if (!unit->pDynamicPath && !d2_realm_client_motion_attach(unit,
            static_cast<int>(monster.view.x), static_cast<int>(monster.view.y))) {
        refresh_monster_motion_view(monster);
        return true;
    }
    refresh_monster_motion_view(monster);
    return true;
}

static void refresh_monster_motion_view(D2RealmClientMonster& monster) {
    if (!monster.motionUnit) return;
    monster.view.mode = monster.motionUnit->dwAnimMode;
    monster.view.framePrecise = monster.motionUnit->nSeqCurrentFramePrecise;
    monster.view.frameCountPrecise = monster.motionUnit->dwFrameCountPrecise;
    monster.view.animSpeed = static_cast<uint16_t>(monster.motionUnit->wAnimSpeed);
    D2MonStats2Txt* stats2 = UNITS_GetMonStats2TxtRecordFromMonsterId(static_cast<int>(monster.view.classId));
    monster.view.reserved = (monster.motionUnit->pAnimSeq ? 0x01u : 0u) |
        (stats2 && (stats2->dwModeFlags & MONSTATS2MODEFLAG_DD) ? 0x02u : 0u) |
        (monster.motionUnit->pAnimSeq ? ((monster.motionUnit->dwSeqMode & 0xFu) << 4) : 0u);   // bits 4-7: the sequence's current mode (SQ draws as it)
    if (!monster.motionUnit->pDynamicPath) return;
    monster.view.x = static_cast<uint32_t>(PATH_GetXPosition(monster.motionUnit->pDynamicPath));
    monster.view.y = static_cast<uint32_t>(PATH_GetYPosition(monster.motionUnit->pDynamicPath));
    monster.view.direction = PATH_GetDirection(monster.motionUnit->pDynamicPath);
}

static int apply_monster_move(const uint8_t* packet, int length) {
    // The local 1.13c 0x67 action-1 and action-0x17 branches use the packet GUID, mode,
    // and destination. The remaining fixed bytes are framed but unused by those branches.
    if (!packet || length != 16 || packet[0] != 0x67) return -1;
    const uint32_t guid = static_cast<uint32_t>(packet[1]) |
        (static_cast<uint32_t>(packet[2]) << 8) | (static_cast<uint32_t>(packet[3]) << 16) |
        (static_cast<uint32_t>(packet[4]) << 24);
    const uint8_t action = packet[5];
    if (action != 1 && action != 0x17) return 0;
    const int x = static_cast<int>(packet[6]) | (static_cast<int>(packet[7]) << 8);
    const int y = static_cast<int>(packet[8]) | (static_cast<int>(packet[9]) << 8);
    auto it = std::find_if(g_monsters.begin(), g_monsters.end(), [guid](const auto& unit) {
        return unit.view.guid == guid;
    });
    if (it == g_monsters.end() || !create_monster_motion(*it)) return 0;
    const uint32_t mode = action == 1 ? MONMODE_WALK : MONMODE_RUN;
    D2UnitStrc* const unit = it->motionUnit;
    const int pathPoints = d2_realm_client_motion_set_target(unit, x, y);
    it->lastPlanResult = pathPoints;
    if (pathPoints > 0) {
        d2_realm_client_motion_set_mode(unit, mode);
    } else {
        // FUN_6FAFDC70 falls back to FUN_6FAFDC30 after a rejected route. The recovered
        // helper selects neutral mode 1 for monster modes 1..15 except mode 12.
        const uint32_t fallbackMode = d2_realm_client_motion_refusal_mode(unit->dwUnitType, unit->dwAnimMode);
        if (fallbackMode != unit->dwAnimMode) d2_realm_client_motion_set_mode(unit, fallbackMode);
    }
    refresh_monster_motion_view(*it);
    return pathPoints > 0 ? 1 : 0;
}

static void refresh_player_room_level(D2RealmClientPlayerState& player, bool selected) {
    if (!player.active) return;
    uint32_t level = 0, roomX = 0, roomY = 0, initSeed = 0;
    if (!d2_drlg_client_player_context(static_cast<int>(player.x), static_cast<int>(player.y), 0,
            &level, &roomX, &roomY, &initSeed) || !level) return;
    const bool changed = !player.clientRoomActive || player.clientRoomLevel != level ||
        player.clientRoomTileX != roomX || player.clientRoomTileY != roomY ||
        (g_world.hasMapReveal && g_world.mapLevel != level);
    player.clientRoomActive = 1;
    player.clientRoomLevel = level;
    player.clientRoomTileX = roomX;
    player.clientRoomTileY = roomY;
    if (!selected) return;
    d2_drlg_client_select_render_level(static_cast<int>(level));
    if (changed && g_world.hasMapReveal) {
        g_world.mapLevel = static_cast<uint32_t>(level);
        g_world.hasMapReveal = d2_drlg_client_tile_count() > 0;
        g_world.mapRevision++;
    }
}

static void detach_player_motion_path(D2RealmClientPlayerEntry& player) {
    if (!player.motionUnit || !player.motionUnit->pDynamicPath) return;
    player.motionDirection = PATH_GetDirection(player.motionUnit->pDynamicPath);
    d2_realm_client_motion_detach(player.motionUnit);
}

static void destroy_player_motion(D2RealmClientPlayerEntry& player) {
    if (!player.motionUnit) return;
    detach_player_motion_path(player);
    destroy_player_owned_items(player.motionUnit);
    d2_realm_client_unit_resources_destroy(player.motionUnit);
    UNITS_FreeUnit(player.motionUnit);
    player.motionUnit = nullptr;
}

static void clear_client_missiles();
static void destroy_all_player_motion() {
    clear_client_missiles();
    for (auto& player : g_players) destroy_player_motion(player);
}

static bool ensure_player_unit(D2RealmClientPlayerEntry& player) {
    if (player.motionUnit) return true;
    const auto& view = player.view;
    player.motionUnit = UNITS_AllocUnit(nullptr, UNIT_PLAYER);
    if (!player.motionUnit) return false;
    player.motionUnit->dwClassId = static_cast<int32_t>(view.classId);
    player.motionUnit->dwUnitId = view.guid;
    if (!d2_realm_client_unit_resources_init(player.motionUnit)) {
        d2_realm_client_unit_resources_destroy(player.motionUnit);
        UNITS_FreeUnit(player.motionUnit);
        player.motionUnit = nullptr;
        return false;
    }
    player.motionUnit->dwAnimMode = view.initialMode;
    player.motionDirection = static_cast<uint8_t>(view.initialDirection & 63u);
    if (sgptDataTables && sgptDataTables->nStatesTxtRecordCount > 0)
        STATLIST_AllocStatListEx(player.motionUnit, 0, nullptr, nullptr);
    return true;
}

static bool ensure_player_motion(D2RealmClientPlayerEntry& player, bool forceReattach = false) {
    D2RealmClientPlayerState& view = player.view;
    if (!view.active || !ensure_player_unit(player)) return false;
    D2ActiveRoomStrc* room = d2_drlg_client_active_room(static_cast<int>(view.x), static_cast<int>(view.y));
    if (!room) {
        detach_player_motion_path(player);
        return true; // Assignment can precede the MapReveal that creates its room.
    }
    if (!forceReattach && player.motionUnit && player.motionUnit->pDynamicPath &&
        player.motionUnit->pDynamicPath->pRoom == room) return true;
    detach_player_motion_path(player);
    if (!d2_realm_client_motion_attach(player.motionUnit, static_cast<int>(view.x), static_cast<int>(view.y)))
        return false;
    PATH_SetDirection(player.motionUnit->pDynamicPath, player.motionDirection);
    UNITS_SetAnimStartFrame(player.motionUnit);
    return true;
}

static void refresh_player_motion_view(D2RealmClientPlayerEntry& player, bool selected) {
    if (!player.motionUnit || !player.motionUnit->pDynamicPath) return;
    D2RealmClientPlayerState& view = selected ? g_player : player.view;
    view.x = static_cast<uint32_t>(PATH_GetXPosition(player.motionUnit->pDynamicPath));
    view.y = static_cast<uint32_t>(PATH_GetYPosition(player.motionUnit->pDynamicPath));
    player.motionDirection = PATH_GetDirection(player.motionUnit->pDynamicPath);
    refresh_player_room_level(view, selected);
    if (selected) store_selected_player();
}

static void refresh_all_player_motion_rooms() {
    for (size_t i = 0; i < g_players.size(); ++i) {
        const bool selected = static_cast<int>(i) == g_selectedPlayer;
        auto& entry = g_players[i];
        if (selected) {
            refresh_player_room_level(g_player, true);
            entry.view = g_player;
        } else {
            refresh_player_room_level(entry.view, false);
        }
        ensure_player_motion(entry);
    }
    store_selected_player();
}

static bool parse_monster_assign(const uint8_t* packet, int length, D2RealmClientMonster& out) {
    if (!packet || length < 13 || packet[0] != 0xac || packet[12] != length || !sgptDataTables) return false;
    const uint32_t guid = static_cast<uint32_t>(packet[1]) |
        (static_cast<uint32_t>(packet[2]) << 8) | (static_cast<uint32_t>(packet[3]) << 16) |
        (static_cast<uint32_t>(packet[4]) << 24);
    const uint32_t classId = static_cast<uint32_t>(packet[5]) | (static_cast<uint32_t>(packet[6]) << 8);
    const uint32_t x = static_cast<uint32_t>(packet[7]) | (static_cast<uint32_t>(packet[8]) << 8);
    const uint32_t y = static_cast<uint32_t>(packet[9]) | (static_cast<uint32_t>(packet[10]) << 8);
    const uint32_t hpPct = packet[11];
    auto* monStats = DATATBLS_GetMonStatsTxtRecord(static_cast<int>(classId));
    if (!monStats) return false;
    auto* monStats2 = UNITS_GetMonStats2TxtRecordFromMonsterId(static_cast<int>(classId));
    if (!monStats2) return false;

    D2RealmBitReader bits(packet + 13, length - 13);
    uint32_t value = 0;
    if (!bits.read(4, value)) return false;
    out.rawPrefix = static_cast<uint8_t>(value); // Input-block +0x18; native fresh and duplicate mode setter input.
    uint32_t hasComponents = 0;
    if (!bits.read(1, hasComponents)) return false;
    uint8_t choices[16]{};
    if (hasComponents) {
        for (size_t i = 0; i < 16; ++i) {
            uint32_t choice = 0;
            if (!bits.read(componentWidth(monStats2->nComponentChoiceCounts[i]), choice)) return false;
            choices[i] = static_cast<uint8_t>(choice);
        }
    }

    uint32_t hasTypeFlags = 0;
    if (!bits.read(1, hasTypeFlags)) return false;
    uint32_t typeFlags = 0;
    int32_t superUnique = -1;
    uint16_t nameId = 0;
    uint32_t ownerGuid = 0;
    if (hasTypeFlags) {
        const uint32_t masks[5] = { 0x04, 0x08, 0x02, 0x10, 0x40 };
        for (uint32_t mask : masks) {
            uint32_t flag = 0;
            if (!bits.read(1, flag)) return false;
            if (flag) typeFlags |= mask;
        }
        if (typeFlags & 0x02) {
            int32_t raw = 0;
            if (!bits.readSigned(16, raw)) return false;
            superUnique = raw;
        }
        bool terminated = false;
        while (bits.remaining() >= 8) {
            if (!bits.read(8, value)) return false;
            if (value == 0) { terminated = true; break; }
            out.nameBytes.push_back(static_cast<uint8_t>(value));
        }
        if (!terminated || !bits.read(16, value)) return false;
        nameId = static_cast<uint16_t>(value);
        uint32_t hasOwner = 0;
        if (!bits.read(1, hasOwner)) return false;
        if (hasOwner && !bits.read(32, ownerGuid)) return false;
    }

    uint32_t hasExtra31 = 0;
    if (!bits.read(1, hasExtra31)) return false;
    out.hasExtra31 = hasExtra31 != 0;
    uint32_t extra31 = 0;
    if (hasExtra31 && !bits.read(31, extra31)) return false;
    uint32_t hasStats = 0;
    if (!bits.read(1, hasStats)) return false;
    out.hasStats = hasStats != 0;
    std::vector<D2RealmClientMonsterStat> stats;
    if (hasStats) {
        if (!sgptDataTables->pItemStatCostTxt || sgptDataTables->nItemStatCostTxtRecordCount <= 0) return false;
        uint32_t statId = 0;
        if (!bits.read(9, statId)) return false;
        while (statId < 0x1ff) {
            if (statId >= static_cast<uint32_t>(sgptDataTables->nItemStatCostTxtRecordCount)) return false;
            const D2ItemStatCostTxt& cost = sgptDataTables->pItemStatCostTxt[statId];
            if (cost.nSendBits == 0) break;
            int32_t parameter = 0;
            int32_t statValue = 0;
            if (cost.nSendParamBits && !bits.readSigned(cost.nSendParamBits, parameter)) return false;
            if (cost.nSendBits < 32 && (cost.dwItemStatFlags & ITEMSTATCOSTFLAG_SIGNED)) {
                if (!bits.readSigned(cost.nSendBits, statValue)) return false;
            } else {
                if (!bits.read(cost.nSendBits, value)) return false;
                statValue = static_cast<int32_t>(value);
            }
            stats.push_back({ static_cast<uint16_t>(statId), parameter, statValue });
            if (!bits.read(9, statId)) return false;
        }
    }

    out.view = {};
    out.view.version = 1;
    out.view.size = sizeof(out.view);
    out.view.guid = guid;
    out.view.classId = classId;
    out.view.code = monStats->dwCode;
    out.view.mode = out.rawPrefix;
    out.view.x = x;
    out.view.y = y;
    out.view.hpPct = hpPct;
    out.view.typeFlags = typeFlags;
    out.view.direction = 0; // 0xAC has no direction field; constructor default still needs native confirmation.
    for (size_t i = 0; i < 16; ++i) {
        const uint8_t count = monStats2->nComponentChoiceCounts[i];
        const uint8_t choice = choices[i];
        if (count == 0 || choice >= count || choice >= 12) continue;
        const int compCode = static_cast<int8_t>(monStats2->unk0x26[i].nComposit[choice]);
        if (compCode < 0 || compCode >= sgptDataTables->nCompCodeTxtRecordCount) continue;
        out.view.componentCodes[i] = DATATBLS_GetCodeFromCompCodeTxt(compCode);
    }
    out.superUniqueIndex = static_cast<int16_t>(superUnique);
    out.nameId = nameId;
    out.ownerGuid = ownerGuid;
    out.extra31 = extra31;
    out.stats = std::move(stats);
    return true;
}

static bool apply_monster_assign(const uint8_t* packet, int length) {
    D2RealmClientMonster monster{};
    if (!parse_monster_assign(packet, length, monster)) return false;
    for (auto& existing : g_monsters) {
        if (existing.view.guid == monster.view.guid) {
            // Native duplicate-GUID handling reuses the CUnit and calls its mode setter; it does
            // not replace class, component set, or position from this assignment record.
            if (existing.motionUnit) d2_realm_client_unit_set_mode(existing.motionUnit, monster.view.mode);
            refresh_monster_motion_view(existing);
            existing.view.hpPct = monster.view.hpPct;
            if (monster.hasStats) {
                existing.stats = std::move(monster.stats);
                existing.hasStats = true;
            }
            if (monster.hasExtra31) {
                existing.extra31 = monster.extra31;
                existing.hasExtra31 = true;
            }
            return true;
        }
    }
    g_monsters.push_back(std::move(monster));
    create_monster_motion(g_monsters.back());
    return true;
}

static bool remove_monster(const uint8_t* packet, int length) {
    if (!packet || length != 6 || packet[0] != 0x0a) return false;
    // D2GS RemoveObject carries a unit type followed by its GUID. Type 1 is UNIT_MONSTER;
    // other unit classes are owned by their corresponding client projections.
    if (packet[1] != 1) return true;
    const uint32_t guid = static_cast<uint32_t>(packet[2]) |
        (static_cast<uint32_t>(packet[3]) << 8) | (static_cast<uint32_t>(packet[4]) << 16) |
        (static_cast<uint32_t>(packet[5]) << 24);
    for (auto it = g_monsters.begin(); it != g_monsters.end();) {
        if (it->view.guid != guid) { ++it; continue; }
        g_monsterDeathModeEvents.erase(std::remove_if(g_monsterDeathModeEvents.begin(), g_monsterDeathModeEvents.end(),
            [guid](const auto& event) { return event.guid == guid; }), g_monsterDeathModeEvents.end());
        destroy_monster_motion(*it);
        it = g_monsters.erase(it);
    }
    return true;
}

static int apply_monster_death_mode(const uint8_t* packet, int length) {
    d2::realm::MonsterDeathModePacket decoded{};
    const int decodedResult = d2::realm::decodeMonsterDeathModePacket(packet, length, &decoded);
    if (decodedResult <= 0) return decodedResult;
    auto it = std::find_if(g_monsters.begin(), g_monsters.end(), [&decoded](const auto& monster) {
        return monster.view.guid == decoded.guid;
    });
    if (it == g_monsters.end() || !it->motionUnit) return 0;

    D2UnitStrc* const unit = it->motionUnit;
    const uint32_t previousMode = unit->dwAnimMode;
    if (decoded.wireState == 8) {
        D2MonStatsTxt* stats = DATATBLS_GetMonStatsTxtRecord(static_cast<int>(it->view.classId));
        const int minion = stats ? stats->wMinion[0] : -1;
        if (stats && stats->nSplEndDeath == 1 && minion >= 0 &&
            sgptDataTables && minion < sgptDataTables->nMonStatsTxtRecordCount) return 0;
    }
    // State 9 has an Izual-specific SQ replacement path. The ordinary DD path is
    // grounded here; leave that class-specific transition to its own client behavior.
    if (decoded.wireState == 9 && it->view.classId == 0x11c && previousMode == MONMODE_SEQUENCE) return 0;
    // The projected family is the whole 0x69/0x6A selector column, so a GETHIT selector (6)
    // reaches the same mode setter as the two death selectors.
    if (d2_realm_client_unit_set_mode(unit, decoded.mode) < 0) return -1;
    refresh_monster_motion_view(*it);
    if (d2::realm::appendMonsterDeathModeEvent(g_monsterDeathModeEvents,
            g_nextMonsterDeathModeSequence, decoded.guid, decoded.opcode, decoded.wireState,
            previousMode, decoded.mode)) {
        ++g_nextMonsterDeathModeSequence;
        return 1;
    }
    return 0;
}

static int reject_stream(int error) {
    clear_client_missiles();
    reset_monster_death_mode_events();
    destroy_all_monster_motion();
    clear_players();
    d2_drlg_act_end();
    clear_world();
    g_monsters.clear();
    g_monsterViews.clear();
    clear_game_chat();
    g_pendingLength = 0;
    return error;
}

EMSCRIPTEN_KEEPALIVE void d2_realm_client_player_reset() {
    clear_client_missiles();
    reset_monster_death_mode_events();
    destroy_all_monster_motion();
    clear_players();
    d2_drlg_act_end();
    clear_world();
    g_monsters.clear();
    g_monsterViews.clear();
    clear_game_chat();
    g_pendingLength = 0;
}

EMSCRIPTEN_KEEPALIVE void d2_realm_client_set_difficulty(uint32_t difficulty) {
    // MCP CREATEGAME difficulty is retained as the exact client mode used by AllocAct.
    g_clientDifficulty = difficulty;
}

// A server assignment is the only event allowed to establish the remote GUID mapping.
static int assign_player(const uint8_t* packet, int length) {
    D2RealmPlayerAssign assigned{};
    if (d2_realm_d2gs_parse_player_assign(packet, length, &assigned) != 26) return 0;
    D2RealmClientPlayerEntry entry{};
    D2RealmClientPlayerState& created = entry.view;
    created.version = 5;
    created.size = sizeof(created);
    uint32_t level = 0, roomX = 0, roomY = 0, initSeed = 0;
    if (!d2_drlg_client_player_context(assigned.x, assigned.y, 1,
            &level, &roomX, &roomY, &initSeed)) return -1;
    created.active = 1;
    created.guid = assigned.guid;
    created.x = assigned.x;
    created.y = assigned.y;
    created.classId = assigned.classId;
    std::memcpy(created.name, assigned.name, sizeof(created.name));
    created.cunitInitSeed = initSeed;
    created.cunitInitSeedValid = level != 0;
    // FUN_6FB57630 always initializes CUnit.pSeed to {1,666}. A resolved room then
    // replaces it with {dwInitSeed,666}; FUN_6FAC91E0 advances it once because a newly
    // constructed unit differs from the currently selected CUnit (including fresh null state).
    created.cunitRngLow = level ? initSeed : 1;
    created.cunitRngHigh = 666;
    const uint64_t advanced = static_cast<uint64_t>(created.cunitRngLow) * 0x6ac690c5ULL +
        created.cunitRngHigh;
    created.cunitRngLow = static_cast<uint32_t>(advanced);
    created.cunitRngHigh = static_cast<uint32_t>(advanced >> 32);
    created.cunitRngValid = 1;
    // FUN_6FAC91E0 sets the fresh player to PLRMODE_TOWNNEUTRAL (5), advances pSeed
    // when the new unit differs from the current unit, then calls D2Common #10706 with
    // the resulting low six bits. In 1.13c #10706 writes both path direction fields.
    created.initialMode = 5;
    created.initialDirection = created.cunitRngLow & 0x3f;
    entry.motionDirection = static_cast<uint8_t>(created.initialDirection);
    refresh_player_room_level(created, false);

    // Native FUN_6FB57360 replaces a duplicate type/GUID unit: it removes the old unit from its
    // registry, destroys it (clearing current-unit selection if applicable), then inserts the new
    // allocation. Preserve that one-owner lifecycle instead of appending another render/path row.
    for (size_t i = 0; i < g_players.size(); ++i) {
        if (g_players[i].view.guid != assigned.guid) continue;
        const bool wasSelected = static_cast<int>(i) == g_selectedPlayer;
        destroy_player_motion(g_players[i]);
        g_players.erase(g_players.begin() + static_cast<std::ptrdiff_t>(i));
        if (wasSelected) {
            g_selectedPlayer = -1;
            clear_player();
        } else if (g_selectedPlayer > static_cast<int>(i)) {
            --g_selectedPlayer;
        }
        break;
    }
    g_players.push_back(entry);
    if (!ensure_player_motion(g_players.back())) return -1;
    return 1;
}

static int select_player(const uint8_t* packet, int length) {
    if (!packet || length != 6 || packet[0] != 0x0b) return -1;
    const uint8_t unitType = packet[1];
    const uint32_t guid = static_cast<uint32_t>(packet[2]) |
        (static_cast<uint32_t>(packet[3]) << 8) |
        (static_cast<uint32_t>(packet[4]) << 16) |
        (static_cast<uint32_t>(packet[5]) << 24);
    if (unitType != 0) return 0;
    for (size_t i = 0; i < g_players.size(); ++i) {
        if (g_players[i].view.guid != guid) continue;
        store_selected_player();
        g_selectedPlayer = static_cast<int>(i);
        g_player = g_players[i].view;
        refresh_player_room_level(g_player, true);
        ensure_player_motion(g_players[i]);
        store_selected_player();
        return 0;
    }
    return 0; // Native lookup miss leaves the previously selected unit unchanged.
}

static int remove_player(const uint8_t* packet, int length) {
    if (!packet || length != 6 || packet[0] != 0x0a) return -1;
    const uint8_t unitType = packet[1];
    const uint32_t guid = static_cast<uint32_t>(packet[2]) |
        (static_cast<uint32_t>(packet[3]) << 8) |
        (static_cast<uint32_t>(packet[4]) << 16) |
        (static_cast<uint32_t>(packet[5]) << 24);
    if (unitType != 0) return 0;
    for (size_t i = 0; i < g_players.size(); ++i) {
        if (g_players[i].view.guid != guid) continue;
        const bool selected = static_cast<int>(i) == g_selectedPlayer;
        if (selected) clear_player();
        clear_target_relations_to(g_players[i].motionUnit);
        destroy_player_motion(g_players[i]);
        g_players.erase(g_players.begin() + static_cast<std::ptrdiff_t>(i));
        if (selected) g_selectedPlayer = -1;
        else if (g_selectedPlayer > static_cast<int>(i)) --g_selectedPlayer;
        return 0;
    }
    return 0;
}

static int apply_player_move(const uint8_t* packet, int length) {
    D2RealmPlayerMove move{};
    if (d2_realm_d2gs_parse_player_move(packet, length, &move) != 16) return -1;
    if (move.unitType != UNIT_PLAYER) return 0;
    auto it = std::find_if(g_players.begin(), g_players.end(), [&move](const auto& player) {
        return player.view.guid == move.guid;
    });
    if (it == g_players.end() || !it->motionUnit || !it->motionUnit->pDynamicPath) return 0;
    const int result = d2_realm_client_apply_player_move(it->motionUnit, &move);
    return result > 0 ? 1 : 0;
}

struct D2RealmClientStateStat {
    uint16_t statId;
    uint16_t layer;
    int32_t value;
};

struct D2RealmClientStateUpdate {
    uint8_t state;
    std::vector<D2RealmClientStateStat> stats;
    bool hasStats = false;
};

static D2UnitStrc* find_realm_client_unit(uint8_t unitType, uint32_t guid) {
    if (unitType == UNIT_PLAYER) {
        for (auto& player : g_players)
            if (player.view.active && player.view.guid == guid) return player.motionUnit;
    } else if (unitType == UNIT_MONSTER) {
        for (auto& monster : g_monsters)
            if (monster.view.guid == guid) return monster.motionUnit;
    }
    return nullptr;
}

// The selector-6 arm writes the record's hit-class byte to D2UnitStrc+0xB0 (retail:
// 0x199DF mov [ebx+0xB0],edx); the wasm64 struct widens the 18 pointers before it.
static_assert(offsetof(D2UnitStrc, dwLastHitClass) == 0xB0 + 18 * (sizeof(void*) - 4),
    "D2UnitStrc dwLastHitClass is the 0x0D hit-class slot");

// 1.13c D2Client+0xAF590: the D2GS 0x0D receiver. It builds a
// three-field record {x, y, hitClass}, hands it to FUN_6FADA750 with the record's action selector
// (byte +6), then stores the record's life byte on the client-unit-list node its by-id lookup
// returns. Selector 6 is the GETHIT column of FUN_6FAC9830; every other selector reaches its own
// client handler and is not projected here.
static int apply_player_stop(const uint8_t* packet, int length) {
    d2::realm::D2RealmPlayerStop stop{};
    if (d2::realm::decodePlayerStopPacket(packet, length, &stop) != 1) return -1;
    const int mode = d2::realm::playerStopModeFromAction(stop.action);
    if (mode < 0) return 0;
    D2UnitStrc* const unit = find_realm_client_unit(stop.unitType, stop.guid);
    if (!unit) return 0;
    // Selector 6 first writes the record's third field into the unit's last-hit-class slot
    // (D2Client+0x19830: mov [ebx+0xB0], the zero-extended packet byte at +11), then sets the mode.
    unit->dwLastHitClass = stop.hitClass;
    if (d2_realm_client_unit_set_mode(unit, static_cast<uint32_t>(mode)) < 0) return -1;
    return 1;
}

static D2UnitStrc* find_realm_client_owned_item(uint32_t guid) {
    for (auto& player : g_players) {
        D2UnitStrc* owner = player.motionUnit;
        if (!owner || !owner->pInventory || owner->pInventory->dwSignature != D2C_InventoryHeader) continue;
        for (D2UnitStrc* item = INVENTORY_GetFirstItem(owner->pInventory); item;
                item = INVENTORY_GetNextItem(item)) {
            if (item->dwUnitType == UNIT_ITEM && item->dwUnitId == guid) return item;
        }
    }
    return nullptr;
}

static int apply_owned_item(const uint8_t* packet, int length) {
    D2RealmOwnedItemPacket owned{};
    if (d2_realm_d2gs_parse_owned_item(packet, length, &owned) != length) return -1;
    if (owned.action != ITEMACTION_EQUIP) return 0;
    if (owned.ownerType != UNIT_PLAYER) return 0;
    D2UnitStrc* owner = find_realm_client_unit(owned.ownerType, owned.ownerGuid);
    if (!owner) return 0;
    return d2_realm_client_owned_item_apply(owner, find_realm_client_owned_item(owned.itemGuid),
        packet, length);
}

static bool read_state_stats(D2RealmBitReader& bits, std::vector<D2RealmClientStateStat>& stats) {
    if (!sgptDataTables || !sgptDataTables->pItemStatCostTxt ||
        sgptDataTables->nItemStatCostTxtRecordCount <= 0) return false;
    uint32_t statId = 0;
    if (!bits.read(9, statId)) return false;
    while (statId != 0x1ff) {
        if (statId >= static_cast<uint32_t>(sgptDataTables->nItemStatCostTxtRecordCount)) return false;
        const D2ItemStatCostTxt& cost = sgptDataTables->pItemStatCostTxt[statId];
        // The local client stops on the first zero-width StatCost row, as it does for 0xAC.
        if (cost.nSendBits == 0) return true;
        if (cost.nSendBits > 32 || cost.nSendParamBits > 32) return false;
        int32_t layer = 0;
        int32_t value = 0;
        if (cost.nSendParamBits && !bits.readSigned(cost.nSendParamBits, layer)) return false;
        if (cost.dwItemStatFlags & ITEMSTATCOSTFLAG_SIGNED) {
            if (!bits.readSigned(cost.nSendBits, value)) return false;
        } else {
            uint32_t raw = 0;
            if (!bits.read(cost.nSendBits, raw)) return false;
            value = static_cast<int32_t>(raw);
        }
        stats.push_back({static_cast<uint16_t>(statId), static_cast<uint16_t>(layer), value});
        if (!bits.read(9, statId)) return false;
    }
    return true;
}

static bool decode_state_set(const uint8_t* packet, int length, D2RealmClientStateUpdate& update) {
    if (!packet || packet[0] != 0xa8 || length < 8 || packet[6] != length) return false;
    update.state = packet[7];
    update.hasStats = true;
    D2RealmBitReader bits(packet + 8, length - 8);
    return read_state_stats(bits, update.stats);
}

static bool decode_state_add(const uint8_t* packet, int length, std::vector<D2RealmClientStateUpdate>& updates) {
    if (!packet || packet[0] != 0xaa || length < 7 || packet[6] != length) return false;
    D2RealmBitReader bits(packet + 7, length - 7);
    for (;;) {
        uint32_t state = 0;
        if (!bits.read(8, state)) return false;
        if (state == 0xff) return true;
        uint32_t hasStats = 0;
        if (!bits.read(1, hasStats)) return false;
        D2RealmClientStateUpdate update{};
        update.state = static_cast<uint8_t>(state);
        update.hasStats = hasStats != 0;
        if (update.hasStats && !read_state_stats(bits, update.stats)) return false;
        updates.push_back(std::move(update));
    }
}

static bool apply_state_on(D2UnitStrc* unit, uint8_t state) {
    if (!unit || !sgptDataTables || state >= sgptDataTables->nStatesTxtRecordCount) return false;
    if (!unit->pStatListEx) STATLIST_AllocStatListEx(unit, 0, nullptr, nullptr);
    if (!unit->pStatListEx) return false;
    STATES_ToggleState(unit, state, TRUE);
    return true;
}

static int apply_state_packet(const uint8_t* packet, int length) {
    if (!packet || length < 7) return -1;
    const uint8_t type = packet[1];
    const uint32_t guid = static_cast<uint32_t>(packet[2]) |
        (static_cast<uint32_t>(packet[3]) << 8) | (static_cast<uint32_t>(packet[4]) << 16) |
        (static_cast<uint32_t>(packet[5]) << 24);
    D2UnitStrc* unit = find_realm_client_unit(type, guid);
    if (!unit) return 0; // The native type/GUID lookup miss leaves client state unchanged.

    if (packet[0] == 0xa7 || packet[0] == 0xa9) {
        if (length != 7) return -1;
        const uint8_t state = packet[6];
        if (!sgptDataTables || state >= sgptDataTables->nStatesTxtRecordCount) return 0;
        if (!unit->pStatListEx) STATLIST_AllocStatListEx(unit, 0, nullptr, nullptr);
        if (!unit->pStatListEx) return -1;
        if (packet[0] == 0xa7) {
            STATES_ToggleState(unit, state, TRUE);
        } else {
            D2StatListStrc* stateList = STATLIST_GetStatListFromUnitAndState(unit, state);
            if (stateList) STATLIST_FreeStatList(stateList);
            STATES_ToggleState(unit, state, FALSE);
        }
        return 1;
    }

    D2RealmClientStateUpdate single{};
    std::vector<D2RealmClientStateUpdate> updates;
    if (packet[0] == 0xa8) {
        if (!decode_state_set(packet, length, single)) return -1;
        updates.push_back(std::move(single));
    } else if (packet[0] == 0xaa) {
        if (!decode_state_add(packet, length, updates)) return -1;
    } else {
        return 0;
    }

    int applied = 0;
    for (const auto& update : updates) {
        if (!sgptDataTables || update.state >= sgptDataTables->nStatesTxtRecordCount) continue;
        if (!apply_state_on(unit, update.state)) return -1;
        // Native FUN_6FB22610 is called once per decoded stat entry. An immediate 0x1ff
        // terminator therefore leaves no per-state list to allocate. Existing lists run a
        // #10680 context helper, but the raw call sequence confirms the 32-bit wire value is still
        // passed unchanged to #10188; STATLIST_SetStat expresses its set/replace behavior.
        if (update.hasStats && !update.stats.empty()) {
            D2StatListStrc* list = STATLIST_GetStatListFromUnitAndState(unit, update.state);
            if (!list) {
                list = STATLIST_AllocStatList(unit->pMemoryPool, STATLIST_MAGIC, 0,
                    unit->dwUnitType, unit->dwUnitId);
                if (!list) return -1;
                STATLIST_SetState(list, update.state);
                D2COMMON_10475_PostStatToStatList(unit, list, FALSE);
                list = STATLIST_GetStatListFromUnitAndState(unit, update.state);
            }
            if (!list) return -1;
            for (const auto& stat : update.stats)
                STATLIST_SetStat(list, stat.statId, stat.value, stat.layer);
        }
        applied++;
    }
    return applied;
}

// Client-created units have a local identity space; no D2GS packet assigns missile GUIDs.
struct RealmClientMissile { D2UnitStrc* unit; uint32_t level; };
static std::vector<RealmClientMissile> g_clientMissiles;
static uint32_t g_nextClientMissileId = 1;
// 1.13c D2Client+0xDE834 (0x6FB8E834): the client unit-creation counter, incremented by the CUnit constructor wrapper
// (D2Client+0xA7BF0) and stored as the new unit's id. A session's earlier creations (tile/object/item units the test runner
// does not model) have already advanced it; the caller pins the value it read, the next missile takes counter+1.
EMSCRIPTEN_KEEPALIVE void d2_realm_client_unit_counter_set(uint32_t counter) { g_nextClientMissileId = counter + 1; }

static void clear_target_relations_to(D2UnitStrc* target) {
    if (!target) return;
    for (auto& player : g_players) {
        d2::realm::clearTargetRelationTo(target, player.motionUnit,
            [](D2UnitStrc* unit) { return unit->pDynamicPath; }, PATH_GetTargetUnit,
            [](D2UnitStrc* unit) { UNITS_SetTargetUnitForDynamicUnit(unit, nullptr); });
    }
    for (auto& monster : g_monsters) {
        d2::realm::clearTargetRelationTo(target, monster.motionUnit,
            [](D2UnitStrc* unit) { return unit->pDynamicPath; }, PATH_GetTargetUnit,
            [](D2UnitStrc* unit) { UNITS_SetTargetUnitForDynamicUnit(unit, nullptr); });
    }
    for (auto& missile : g_clientMissiles) {
        d2::realm::clearTargetRelationTo(target, missile.unit,
            [](D2UnitStrc* unit) { return unit->pDynamicPath; }, PATH_GetTargetUnit,
            [](D2UnitStrc* unit) { UNITS_SetTargetUnitForDynamicUnit(unit, nullptr); });
    }
}
// Native generic SC casts dispatch from loaded Skills/Missiles records.
static bool is_natural_client_projectile(int id) {
    auto* skill = DATATBLS_GetSkillsTxtRecord(id);
    if (!skill || skill->wCltStFunc != 0 || skill->wCltDoFunc != 0 ||
        skill->nAnim != PLRMODE_CAST || skill->wCltMissile == 0 || skill->wCltMissile == 0xffffu) return false;
    auto* missile = DATATBLS_GetMissilesTxtRecord(skill->wCltMissile);
    if (!missile || missile->wCltDoFunc != 1 || missile->nCollideType != 3) return false;
    if (missile->wCltHitFunc == 0) return true;
    return id == SKILL_FIREBALL && skill->wCltMissile == 62 && missile->wCltHitFunc == 1;
}

EMSCRIPTEN_KEEPALIVE int d2_realm_client_projectile_supported(int skillId) {
    return is_natural_client_projectile(skillId) ? 1 : 0;
}

struct RealmMissileEvent { uint32_t version,event,tick,id,classId,ownerGuid,skill,targetType,targetGuid; };
static_assert(sizeof(RealmMissileEvent) == 36);
static std::vector<RealmMissileEvent> g_clientMissileEvents;
static uint32_t g_clientMissileTick = 0;
struct RealmMissileSoundEvent { uint32_t version, tick, missileId, classId, event, soundRow; };
static_assert(sizeof(RealmMissileSoundEvent) == 24);
static std::vector<RealmMissileSoundEvent> g_clientMissileSoundEvents;
static void record_client_missile_event(uint32_t event, D2UnitStrc* unit,
        uint32_t targetType = 0xffffffffu, uint32_t targetGuid = 0xffffffffu) {
    if (!unit) return;
    if (g_clientMissileEvents.size() == 128) g_clientMissileEvents.erase(g_clientMissileEvents.begin());
    const bool isMissile = unit->dwUnitType == UNIT_MISSILE && unit->pMissileData;
    g_clientMissileEvents.push_back({1,event,g_clientMissileTick,unit->dwUnitId,
        static_cast<uint32_t>(unit->dwClassId), isMissile ? unit->pMissileData->dwOwnerGUID : unit->dwUnitId,
        isMissile ? static_cast<uint32_t>(MISSILE_GetSkill(unit)) :
            (UNITS_GetUsedSkill(unit) ? static_cast<uint32_t>(SKILLS_GetSkillIdFromSkill(UNITS_GetUsedSkill(unit), __FILE__, __LINE__)) : 0xffffffffu),targetType,targetGuid});
}
static void record_client_missile_impact_sound(uint32_t event, D2UnitStrc* unit,
        const D2MissilesTxt* txt) {
    if (!unit || !txt) return;
    const int32_t row = d2::realm::fireballImpactSoundRow(
        static_cast<uint32_t>(unit->dwClassId), event, static_cast<int16_t>(txt->wHitSound));
    if (row < 0) return;
    if (g_clientMissileSoundEvents.size() == 128)
        g_clientMissileSoundEvents.erase(g_clientMissileSoundEvents.begin());
    g_clientMissileSoundEvents.push_back({1, g_clientMissileTick, unit->dwUnitId,
        static_cast<uint32_t>(unit->dwClassId), event, static_cast<uint32_t>(row)});
}
static void free_client_missile(D2UnitStrc* unit) {
    if (!unit) return;
    d2_realm_client_motion_detach(unit);
    MISSILE_FreeMissileData(unit);
    UNITS_FreeUnit(unit);
}
static void clear_client_missiles() {
    for (const auto& entry : g_clientMissiles) { record_client_missile_event(5, entry.unit); free_client_missile(entry.unit); }
    g_clientMissiles.clear();
    g_clientMissileSoundEvents.clear();
}

// Recovered table-driven ordinary flight and separate static impact265 descriptor.
static D2UnitStrc* create_client_missile(const D2MissileStrc* params) {
    if (!params || !params->pOwner || (params->dwFlags != 0x21u && params->dwFlags != 0x201u)) return nullptr;
    auto* owner = params->pOwner;
    const int missileId = params->nMissile;
    auto* txt = DATATBLS_GetMissilesTxtRecord(missileId);
    if (!txt || txt->wCltDoFunc != 1) return nullptr;
    if (params->dwFlags == 0x201u) {
        // 1.13c D2Client+0xB1420: Fire Ball spawns its hit children (265) and its
        // Missiles.txt +0x16 ExplosionMissile through the same create path.
        const auto* fireball = DATATBLS_GetMissilesTxtRecord(62);
        if (params->nSkill != 62 || !fireball ||
            (missileId != 265 && missileId != static_cast<int>(fireball->wExplosionMissile))) return nullptr;
    } else {
        const auto* skill = DATATBLS_GetSkillsTxtRecord(params->nSkill);
        if (!skill || !is_natural_client_projectile(params->nSkill) || skill->wCltMissile != missileId) return nullptr;
    }
    const int x = params->nX, y = params->nY;
    D2UnitStrc* target = params->pTarget;
    int tx = params->dwFlags & 0x20u ? params->nTargetX : x;
    int ty = params->dwFlags & 0x20u ? params->nTargetY : y;
    const int velocity = (static_cast<int>(txt->nVel) + (txt->nVelLev * params->nSkillLevel) / 8) * 256 * 75 / 100;
    d2::realm::normalizeMovingMissileTarget(target, owner, velocity, x, y, tx, ty,
        [](D2UnitStrc* value) { return UNITS_GetXPosition(value); },
        [](D2UnitStrc* value) { return UNITS_GetYPosition(value); },
        [](D2UnitStrc* value) { return UNITS_GetDirection(value); });
    const int aimX = target ? UNITS_GetXPosition(target) : tx;
    const int aimY = target ? UNITS_GetYPosition(target) : ty;
    if (velocity && (std::abs(aimX - x) >= 100 || std::abs(aimY - y) >= 100)) return nullptr;
    auto* unit = UNITS_AllocUnit(nullptr, UNIT_MISSILE);
    if (!unit) return nullptr;
    unit->dwClassId = missileId;
    unit->dwUnitId = g_nextClientMissileId++;
    unit->dwAnimMode = txt->nCollideType; // Client 6FB601BE passes collision type to #11090.
    if (!d2_realm_client_motion_attach(unit, x, y)) { free_client_missile(unit); return nullptr; }
    uint32_t level = 0, roomX = 0, roomY = 0, initSeed = 0;
    if (!d2_drlg_client_unit_context(x, y, 0, &level, &roomX, &roomY, &initSeed) ||
        !d2_session_client_cunit_seed_init(&unit->pSeed, &unit->dwInitSeed, x, y, 0)) {
        free_client_missile(unit); return nullptr;
    }
    MISSILE_AllocMissileData(unit);
    if (!unit->pMissileData) { free_client_missile(unit); return nullptr; }
    // Native type-3 initializer clears targetability and uses its collision-type table.
    unit->dwFlags = (unit->dwFlags & ~0xau) | 0x600020u;
    PATH_SetFootprintCollisionMask(unit->pDynamicPath, 0);
    PATH_SetMoveTestCollisionMask(unit->pDynamicPath, txt->nCollideType == 3 ? 0x184 : 0);
    UNITS_SetAnimStartFrame(unit);
    unit->dwFrameCountPrecise = static_cast<uint32_t>(txt->nAnimLen) << 8;
    MISSILE_SetAnimSpeed(unit, static_cast<int>(txt->nAnimSpeed) << 4);
    const int levelValue = params->nSkillLevel;
    int range = static_cast<int16_t>(txt->wRange) + static_cast<int16_t>(txt->wLevRange) * levelValue;
    if (params->dwFlags & 0x200u) {
        range -= params->nFrame;
        unit->nSeqCurrentFramePrecise = params->nFrame << 8;
    }
    MISSILE_SetTotalFrames(unit, range);
    MISSILE_SetCurrentFrame(unit, range);
    MISSILE_SetActivateFrame(unit, range - txt->nActivate);
    PATH_SetVelocity(unit->pDynamicPath, 0, __FILE__, __LINE__);
    if (target) UNITS_SetTargetUnitForDynamicUnit(unit, target);
    else D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, tx, ty);
    PATH_SetVelocity(unit->pDynamicPath, velocity, __FILE__, __LINE__);
    if (velocity) D2Common_10142(unit->pDynamicPath, unit, 0);
    if (txt->nInitSteps) unit->dwFlagEx |= 0x40000u;
    MISSILE_SetOwner(unit, owner);
    PATH_SetAcceleration(unit->pDynamicPath, static_cast<int16_t>(txt->wAccel));
    PATH_SetMaxVelocity(unit->pDynamicPath, static_cast<int>(txt->nMaxVel) << 8);
    MISSILE_SetSkill(unit, params->nSkill);
    MISSILE_SetLevel(unit, static_cast<uint16_t>(levelValue));
    g_clientMissiles.push_back({unit, level});
    record_client_missile_event(1, unit);
    if ((unit->dwClassId == 62 || unit->dwClassId == 42) && txt->wTravelSound < 0x8000) // D2Client create: TravelSound row on the first client tick
        g_clientMissileSoundEvents.push_back({1, g_clientMissileTick, unit->dwUnitId, static_cast<uint32_t>(unit->dwClassId), 1u, txt->wTravelSound});
    return unit;
}

static D2UnitStrc* create_client_projectile(D2UnitStrc* owner, D2SkillStrc* skill) {
    if (!owner || !owner->pDynamicPath || !skill || !skill->pSkillsTxt ||
        !is_natural_client_projectile(skill->pSkillsTxt->nSkillId)) return nullptr;
    D2MissileStrc params{};
    params.dwFlags = 0x21;
    params.pOwner = owner;
    params.nMissile = skill->pSkillsTxt->wCltMissile;
    params.nX = UNITS_GetXPosition(owner); params.nY = UNITS_GetYPosition(owner);
    params.pTarget = PATH_GetTargetUnit(owner->pDynamicPath);
    params.nTargetX = owner->pDynamicPath->tTargetCoord.X;
    params.nTargetY = owner->pDynamicPath->tTargetCoord.Y;
    if (params.pTarget) {
        if (!UNITS_GetRoom(params.pTarget)) return nullptr;
    } else if (!params.nTargetX || !params.nTargetY) return nullptr;
    params.nSkill = skill->pSkillsTxt->nSkillId;
    params.nSkillLevel = SKILLS_GetSkillLevel(owner, skill, TRUE);
    return create_client_missile(&params);
}

struct RealmMissileCollision { D2UnitStrc* missile; D2UnitStrc* owner; uint32_t hitGuid = 0xffffffffu; };
static int __fastcall ordinary_projectile_target(D2UnitStrc* target, void* context) {
    auto* collision = static_cast<RealmMissileCollision*>(context);
    // Bounded player->ordinary-monster relation branch. Player/PvP and pets require
    // the Client relation roster and are not approximated from engine damage rules.
    if (!target || target->dwUnitType != UNIT_MONSTER || !collision->owner ||
        collision->owner->dwUnitType != UNIT_PLAYER || (target->dwFlags & 0xcu) != 0xcu ||
        MISSILE_CheckUnitIfOwner(collision->missile, target)) return 0;
    const auto it = std::find_if(g_monsters.begin(), g_monsters.end(), [target](const auto& entry) {
        return entry.motionUnit == target;
    });
    if (it == g_monsters.end() || it->ownerGuid != 0) return 0;
    if (STATLIST_AreUnitsAligned(target, collision->owner)) return 0;
    collision->hitGuid = target->dwUnitId;
    return 1;
}
static int __fastcall query_specific_monster(D2UnitStrc* target, void* context) {
    return target && target->dwUnitType == UNIT_MONSTER && target->dwUnitId == *static_cast<uint32_t*>(context);
}
// Read the real native collision query at this canonical target's current coordinates.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_monster_collision_query(uint32_t guid) {
    for (auto& monster : g_monsters) if (monster.view.guid == guid && monster.motionUnit) {
        auto* unit = monster.motionUnit;
        auto* hit = D2Common_10407(UNITS_GetRoom(unit), UNITS_GetXPosition(unit), UNITS_GetYPosition(unit),
            query_specific_monster, &guid, 1);
        return hit ? static_cast<int>(hit->dwUnitId) : -1;
    }
    return -1;
}

static bool create_fireball_hit_child(D2UnitStrc* source,
        const d2::realm::FireballHitChild& child, void*) {
    D2UnitStrc* owner = nullptr;
    for (auto& player : g_players) if (MISSILE_CheckUnitIfOwner(source, player.motionUnit)) {
        owner = player.motionUnit; break;
    }
    if (!owner) return false;
    D2MissileStrc params{};
    params.dwFlags = 0x201;
    params.pOwner = owner;
    params.nMissile = child.childClass;
    params.nX = child.x; params.nY = child.y;
    params.nSkill = child.sourceClass;
    params.nSkillLevel = child.sourceLevel;
    params.nFrame = child.startFrame;
    auto* created = create_client_missile(&params);
    if (created) record_client_missile_event(9, created, UNIT_MISSILE, source->dwUnitId);
    return created != nullptr;
}
// 1.13c D2Client+0xB1420: after the callback-table hit children, a Fire Ball that
// collided (or expired with AlwaysExplode) spawns Missiles.txt +0x16 at its own
// position. Negative +0x16 (signed) means no explosion, as in the native guard.
static void spawn_fireball_explosion(D2UnitStrc* missile) {
    const auto* fireball = DATATBLS_GetMissilesTxtRecord(62);
    if (!fireball || static_cast<int16_t>(fireball->wExplosionMissile) < 0) return;
    D2UnitStrc* owner = nullptr;
    for (auto& player : g_players) if (MISSILE_CheckUnitIfOwner(missile, player.motionUnit)) {
        owner = player.motionUnit; break;
    }
    if (!owner) return;
    D2MissileStrc params{};
    params.dwFlags = 0x201;
    params.pOwner = owner;
    params.nMissile = fireball->wExplosionMissile;
    params.nX = UNITS_GetXPosition(missile); params.nY = UNITS_GetYPosition(missile);
    params.nSkill = 62;
    params.nSkillLevel = static_cast<int>(MISSILE_GetLevel(missile));
    params.nFrame = 0;
    auto* created = create_client_missile(&params);
    if (created) record_client_missile_event(9, created, UNIT_MISSILE, missile->dwUnitId);
}
// Native 6FB55440 is applied to the first query result, not inside the query's
// callback: skipping it there would search a different occupant of the same cell.
static bool client_collision_target_dead(const D2UnitStrc* unit) {
    if (unit->dwFlags & UNITFLAG_ISDEAD) return true;
    if (unit->dwUnitType == UNIT_PLAYER)
        return unit->dwAnimMode == PLRMODE_DEATH || unit->dwAnimMode == PLRMODE_DEAD;
    if (unit->dwUnitType == UNIT_MONSTER)
        return unit->dwAnimMode == MONMODE_DEATH || unit->dwAnimMode == MONMODE_DEAD;
    return false;
}
static void advance_client_missiles() {
    // Native local missile table is visited by ascending GUID hash bucket. Each
    // callback saves its next link before running, so a new head in this bucket
    // waits; children in a later bucket can advance during this same pass.
    for (uint32_t bucket = 0; bucket < 128; ++bucket) {
      std::vector<D2UnitStrc*> members;
      for (auto it = g_clientMissiles.rbegin(); it != g_clientMissiles.rend(); ++it)
          if ((it->unit->dwUnitId & 127u) == bucket) members.push_back(it->unit);
      for (auto* unit : members) {
        const bool localOwner = g_selectedPlayer >= 0 && static_cast<size_t>(g_selectedPlayer) < g_players.size() &&
            MISSILE_CheckUnitIfOwner(unit, g_players[g_selectedPlayer].motionUnit);
        int passes = 1;
        for (int pass = 0; pass < passes; ++pass) {
        auto* path = unit->pDynamicPath;
        auto* txt = DATATBLS_GetMissilesTxtRecord(unit->dwClassId);
        // 6FB617C0's ordinary looping missile advances the simple unit clock,
        // moves the real dynamic path, then decrements its missile-frame lifetime.
        if (txt && MISSILE_GetRemainingFrames(unit) > txt->nInitSteps) unit->dwFlagEx &= ~0x40000u;
        if (txt && (txt->nLoopAnim || !UNITS_IsAtEndOfFrameCycle(unit))) UNITS_UpdateFrame(unit);
        if (path->dwVelocity) D2Common_10226(unit, 1024);
        MISSILE_SetCurrentFrame(unit, MISSILE_GetCurrentFrame(unit) - 1);
        bool expired = MISSILE_GetCurrentFrame(unit) < 1;
        bool collided = !expired && (path->nCollidedWithMask & 5u) != 0;
        uint32_t hitGuid = 0xffffffffu;
        if (!expired && !collided && txt && txt->nClientCol && path->nCollidedWithMask) {
            D2UnitStrc* owner = nullptr;
            for (auto& player : g_players) if (MISSILE_CheckUnitIfOwner(unit, player.motionUnit)) {
                owner = player.motionUnit; break;
            }
            RealmMissileCollision context{unit, owner};
            D2PathPointStrc* steps = nullptr;
            const int count = D2COMMON_10198_PathGetSaveStep(path, &steps);
            for (int step = 0; step < count && !collided; ++step) {
                auto* target = D2Common_10407(UNITS_GetRoom(unit), steps[step].X, steps[step].Y,
                    ordinary_projectile_target, &context, UNITS_GetUnitSizeX(unit));
                collided = target && !client_collision_target_dead(target);
                if (target && !collided) record_client_missile_event(8, unit, target->dwUnitType, target->dwUnitId);
            }
            if (collided) hitGuid = context.hitGuid;
        }
        if (expired || collided) {
            const uint32_t terminalEvent = expired ? 4 : (hitGuid != 0xffffffffu ? 3 : 2);
            record_client_missile_event(terminalEvent, unit,
                hitGuid != 0xffffffffu ? UNIT_MONSTER : 0xffffffffu, hitGuid);
            // Lifetime expiry runs the hit callback only with AlwaysExplode; Fire Ball has none (a retail recording expiry, no impact row).
            if (collided || (txt && txt->nAlwaysExplode)) record_client_missile_impact_sound(terminalEvent, unit, txt);
            if (unit->dwClassId == 62 && (collided || (txt && txt->nAlwaysExplode)) && g_selectedPlayer >= 0 &&
                static_cast<size_t>(g_selectedPlayer) < g_players.size()) {
                d2::realm::createFireballHitChildren(unit, g_players[g_selectedPlayer].motionUnit,
                    create_fireball_hit_child, nullptr);
                spawn_fireball_explosion(unit);
            }
            const auto found = std::find_if(g_clientMissiles.begin(), g_clientMissiles.end(),
                [unit](const auto& missile) { return missile.unit == unit; });
            if (found != g_clientMissiles.end()) g_clientMissiles.erase(found);
            free_client_missile(unit);
            break;
        }
        // Native ordinary-realm prediction catches up a newly owned moving missile
        // once: the first completed do-pass has elapsed frame1, then clears the flag.
        if (pass == 0 && localOwner && MISSILE_GetRemainingFrames(unit) == 1 && path->dwVelocity)
            passes = 2;
        }
      }
    }
}

// Native SC policy (2,1,3): consume the previous action event, advance without
// wrapping a completed cast, then leave SC when the next advance reaches its end.
static bool advance_client_projectile_cast(D2UnitStrc* unit) {
    if (!unit) return false;
    auto* skill = UNITS_GetUsedSkill(unit);
    if ( unit->dwAnimMode != PLRMODE_CAST || !skill || !skill->pSkillsTxt ||
        !is_natural_client_projectile(skill->pSkillsTxt->nSkillId) || (skill->dwFlags & 1u)) return false;
    if (!(unit->dwFlags & 0x40u) && unit->nActionFrame >= 1 && unit->nActionFrame <= 3) {
        record_client_missile_event(7, unit);
        create_client_projectile(unit, skill);
        unit->dwFlags |= 0x40u; // Native marks emission even when allocation fails.
    }
    if (!UNITS_IsAtEndOfFrameCycle(unit)) UNITS_StopSequence(unit);
    if (UNITS_IsAtEndOfFrameCycle(unit)) {
        UNITS_SetUsedSkill(unit, nullptr);
        PATH_ResetToPreviousType(unit->pDynamicPath);
        unit->dwFlags = (unit->dwFlags & ~0x40u) | 2u;
        UNITS_ChangeAnimMode(unit, DUNGEON_IsRoomInTown(UNITS_GetRoom(unit)) ? PLRMODE_TOWNNEUTRAL : PLRMODE_NEUTRAL);
    }
    auto* path = unit->pDynamicPath;
    path->nDirection = d2_realm_client_motion_step_facing(path->nDirection, path->nNewDirection,
        path->nDiffDirection, path->dwFlags);
    return true;
}

// A bounded natural Fire Bolt/Fireball start for a remote caster. The owning local client normally
// starts its skill before sending; local prediction and client missiles are separate paths.
static int apply_remote_projectile_start(const uint8_t* packet, int length) {
    struct Ops {
        bool isPlayerType(uint8_t type) const { return type == UNIT_PLAYER; }
        bool isNaturalProjectile(uint32_t skill) const { return is_natural_client_projectile(skill & 0xffffu); }
        D2UnitStrc* resolveRemoteCaster(uint32_t guid) const {
            auto it = std::find_if(g_players.begin(), g_players.end(), [guid](const auto& player) {
                return player.view.guid == guid;
            });
            if (it == g_players.end() || static_cast<int>(it - g_players.begin()) == g_selectedPlayer) return nullptr;
            return it->motionUnit;
        }
        bool casterCanStart(D2UnitStrc* unit) const {
            auto* room = unit ? UNITS_GetRoom(unit) : nullptr;
            return unit && unit->pDynamicPath && room && !DUNGEON_IsRoomInTown(room);
        }
        D2UnitStrc* resolveMonster(uint8_t type, uint32_t guid) const {
            // Bound retained relations to the monster lifetime cleanup implemented here.
            if (type != UNIT_MONSTER) return nullptr;
            auto* resolved = find_realm_client_unit(type, guid);
            return resolved && UNITS_GetRoom(resolved) ? resolved : nullptr;
        }
        int x(D2UnitStrc* unit) const { return UNITS_GetXPosition(unit); }
        int y(D2UnitStrc* unit) const { return UNITS_GetYPosition(unit); }
        bool skillCanStart(D2UnitStrc*, const D2RealmPlayerCast& cast) const {
            const int skillId = cast.opcode == 0x4d ? static_cast<int>(cast.skill & 0xffffu) : static_cast<int>(cast.skill);
            auto* txt = DATATBLS_GetSkillsTxtRecord(skillId);
            return txt && txt->wCltStFunc == 0 && txt->wCltDoFunc == 0 && txt->nAnim == PLRMODE_CAST;
        }
        bool prepareStart(D2UnitStrc* unit) const {
            if (!unit->pSkills) unit->pSkills = SKILLS_AllocSkillList(nullptr);
            if (!unit->pSkills) return false;
            PATH_ResetToPreviousType(unit->pDynamicPath);
            return true;
        }
        void setAim(D2UnitStrc* unit, int x, int y) const {
            D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, x, y);
        }
        void setTarget(D2UnitStrc* unit, D2UnitStrc* target) const {
            UNITS_SetTargetUnitForDynamicUnit(unit, target);
        }
        bool start(D2UnitStrc* unit, const D2RealmPlayerCast& cast,
                D2UnitStrc*, int targetX, int targetY) const {
            UNITS_UpdateDirectionAndSpeed(unit, targetX, targetY);
            const int skillId = cast.opcode == 0x4d ? static_cast<int>(cast.skill & 0xffffu) : static_cast<int>(cast.skill);
            D2SkillStrc* skill = nullptr;
            if (cast.opcode == 0x4c) {
                SKILLS_AssignSkill(unit, skillId, cast.skillLevel, FALSE, __FILE__, __LINE__);
                skill = SKILLS_GetSkillById(unit, skillId, -1);
            } else {
                // 0x4D byte +10 is hand selection. The canonical 0x94 skill list owns level.
                skill = unit->pSkills ? SKILLS_GetSkillById(unit, skillId, -1) : nullptr;
            }
            if (!skill) return false;
            UNITS_SetUsedSkill(unit, skill);
            unit->dwFlags &= ~0x40u;
            return UNITS_ChangeAnimMode(unit, SKILLS_GetSkillModeFromUnit(unit, skill)) != 0;
        }
    };
    return d2::realm::applyRemoteProjectileCastPacket<D2UnitStrc>(packet, length, Ops{});
}

// The selected player's unit-target action (native Client action 0x16 -> 6FB530C0) resolves and
// retains the canonical target CUnit. The relation supplies both dynamic-path aim and the
// canonical missile target; target removal clears the relation before freeing that CUnit.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_player_predict_right_cast_on_monster(uint32_t guid) {
    if (g_selectedPlayer < 0 || size_t(g_selectedPlayer) >= g_players.size()) return 0;
    auto* unit = g_players[g_selectedPlayer].motionUnit;
    auto targetIt = std::find_if(g_monsters.begin(), g_monsters.end(), [guid](const auto& monster) {
        return monster.view.guid == guid;
    });
    if (!unit || !unit->pDynamicPath || !UNITS_GetRoom(unit) || targetIt == g_monsters.end() ||
        !targetIt->motionUnit || !UNITS_GetRoom(targetIt->motionUnit)) return 0;
    auto* target = targetIt->motionUnit;
    // 1.13c D2Client+0x1B7F0 a5440: a hover monster in MONMODE_DEATH/DEAD is not a unit target; the cast falls to the ground branch.
    if (target->dwAnimMode == MONMODE_DEATH || target->dwAnimMode == MONMODE_DEAD) return 0;
    auto* skill = UNITS_GetRightSkill(unit);
    if (!skill || !is_natural_client_projectile(SKILLS_GetSkillIdFromSkill(skill, __FILE__, __LINE__)) ||
        skill->nOwnerGUID != -1) return -1;
    auto* previous = UNITS_GetUsedSkill(unit);
    if (previous && !is_natural_client_projectile(SKILLS_GetSkillIdFromSkill(previous, __FILE__, __LINE__))) return -1;
    if (previous) {
        const int flags = SKILLS_GetFlags(previous);
        if ((flags & 1) || !(unit->dwFlags & 0x40u)) {
            if (flags & 1) SKILLS_SetFlags(previous, flags | 2);
            create_client_projectile(unit, previous);
        }
        UNITS_SetUsedSkill(unit, nullptr);
    }
    if (DUNGEON_IsRoomInTown(UNITS_GetRoom(unit)) || unit->dwAnimMode == PLRMODE_DEATH ||
        unit->dwAnimMode == PLRMODE_DEAD || SKILLS_GetUseState(unit, skill) != 0) return 0;
    auto* txt = skill->pSkillsTxt;
    if (!txt || txt->wCltStFunc != 0 || txt->wCltDoFunc != 0 || txt->nAnim != PLRMODE_CAST) return -1;
    PATH_ResetToPreviousType(unit->pDynamicPath);
    const int targetX = UNITS_GetXPosition(target), targetY = UNITS_GetYPosition(target);
    if (!d2::realm::applyCastTargetRelation(0x4c, target, targetX, targetY,
            [unit](int x, int y) { D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, x, y); },
            [unit](D2UnitStrc* relation) { UNITS_SetTargetUnitForDynamicUnit(unit, relation); })) return 0;
    UNITS_UpdateDirectionAndSpeed(unit, targetX, targetY);
    UNITS_SetUsedSkill(unit, skill);
    unit->dwFlags &= ~0x40u;
    UNITS_ChangeAnimMode(unit, SKILLS_GetSkillModeFromUnit(unit, skill));
    return 1;
}

// Apply the server's targeted-skill start to a remote player's canonical CUnit.
static int apply_remote_unit_skill_start(const uint8_t* packet, int length) {
    D2RealmPlayerCast cast{};
    if (!d2_realm_d2gs_parse_player_cast(packet, length, &cast)) return -1;
    struct Ops {
        D2UnitStrc* resolvePlayer(uint32_t guid) const {
            for (const auto& player : g_players)
                if (player.view.guid == guid) return player.motionUnit;
            return nullptr;
        }
        bool isRemotePlayer(D2UnitStrc* unit, uint32_t guid) const {
            const auto it = std::find_if(g_players.begin(), g_players.end(), [guid, unit](const auto& player) {
                return player.view.guid == guid && player.motionUnit == unit;
            });
            return it != g_players.end() && static_cast<int>(it - g_players.begin()) != g_selectedPlayer;
        }
        D2UnitStrc* resolveTarget(uint8_t type, uint32_t guid) const {
            if (type == UNIT_PLAYER) {
                for (const auto& player : g_players)
                    if (player.view.guid == guid) return player.motionUnit;
            } else if (type == UNIT_MONSTER) {
                for (const auto& monster : g_monsters)
                    if (monster.view.guid == guid) return monster.motionUnit;
            }
            return nullptr;
        }
        bool hasCanonicalIdentity(D2UnitStrc* unit, uint32_t guid, uint8_t type) const {
            return unit && unit->dwUnitType == type && unit->dwUnitId == guid;
        }
        bool bothUnitsInActiveRooms(D2UnitStrc* caster, D2UnitStrc* target) const {
            return UNITS_GetRoom(caster) && UNITS_GetRoom(target);
        }
        bool casterCanStart(D2UnitStrc* unit) const {
            auto* room = UNITS_GetRoom(unit);
            return unit->pDynamicPath && room && !DUNGEON_IsRoomInTown(room) &&
                unit->dwAnimMode != PLRMODE_DEATH && unit->dwAnimMode != PLRMODE_DEAD;
        }
        bool skillSupported(uint32_t id) const {
            auto* txt = DATATBLS_GetSkillsTxtRecord(static_cast<int>(id));
            return is_natural_client_projectile(static_cast<int>(id)) && txt &&
                txt->wCltStFunc == 0 && txt->wCltDoFunc == 0 && txt->nAnim == PLRMODE_CAST;
        }
        bool ensureSkillList(D2UnitStrc* unit) const {
            if (!unit->pSkills) unit->pSkills = SKILLS_AllocSkillList(nullptr);
            return unit->pSkills != nullptr;
        }
        void assignNaturalSkill(D2UnitStrc* unit, uint32_t id, uint8_t level) const {
            SKILLS_AssignSkill(unit, static_cast<int>(id), level, FALSE, __FILE__, __LINE__);
        }
        D2SkillStrc* findSkill(D2UnitStrc* unit, uint32_t id) const {
            return SKILLS_GetSkillById(unit, static_cast<int>(id), -1);
        }
        int x(D2UnitStrc* unit) const { return UNITS_GetXPosition(unit); }
        int y(D2UnitStrc* unit) const { return UNITS_GetYPosition(unit); }
        void resetPath(D2UnitStrc* unit) const { PATH_ResetToPreviousType(unit->pDynamicPath); }
        void setPathTarget(D2UnitStrc* unit, int x, int y) const {
            D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, x, y);
        }
        void updateFacing(D2UnitStrc* unit, int x, int y) const { UNITS_UpdateDirectionAndSpeed(unit, x, y); }
        void setTargetUnit(D2UnitStrc* unit, D2UnitStrc* target) const {
            UNITS_SetTargetUnitForDynamicUnit(unit, target);
        }
        void setUsedSkill(D2UnitStrc* unit, D2SkillStrc* skill) const { UNITS_SetUsedSkill(unit, skill); }
        void clearCastEventFlag(D2UnitStrc* unit) const { unit->dwFlags &= ~0x40u; }
        bool startSkillMode(D2UnitStrc* unit, D2SkillStrc* skill) const {
            return UNITS_ChangeAnimMode(unit, SKILLS_GetSkillModeFromUnit(unit, skill)) != 0;
        }
    } ops;
    return d2::realm::applyRemoteUnitSkillStart(cast, ops);
}

// Permanent native gate readback for selected cast failures; no state mutation.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_player_right_cast_state(int field) {
    if (g_selectedPlayer < 0 || size_t(g_selectedPlayer) >= g_players.size()) return -1;
    auto* unit = g_players[g_selectedPlayer].motionUnit;
    if (!unit) return -1;
    auto* skill = UNITS_GetRightSkill(unit);
    switch (field) {
    case 0: return skill ? SKILLS_GetUseState(unit, skill) : -1;
    case 1: return skill ? SKILLS_GetSkillLevel(unit, skill, TRUE) : -1;
    case 2: return skill ? SKILLS_GetSkillIdFromSkill(skill, __FILE__, __LINE__) : -1;
    case 3: return skill ? skill->nOwnerGUID : -1;
    case 4: return UNITS_GetRoom(unit) ? DUNGEON_IsRoomInTown(UNITS_GetRoom(unit)) : -1;
    case 5: return skill ? SKILLS_GetFlags(skill) : -1;
    case 6: return unit->dwAnimMode;
    case 7: return STATLIST_UnitGetStatValue(unit, STAT_MANA, 0);
    case 8: return STATLIST_UnitGetStatValue(unit, STAT_LEVEL, 0);
    case 9: return unit->pInventory != nullptr;
    case 10: return unit->pInventory ? unit->pInventory->dwSignature : 0;
    case 11: return unit->pInventory ? unit->pInventory->dwOwnerGuid : -1;
    case 12: return unit->pInventory && unit->pInventory->pOwner == unit;
    default: return -1;
    }
}

// Selected-player natural Fire Bolt/Fireball action precedes packet serialization (6FADA7C0).
// -1 is unsupported, 0 is refused, 1 is an accepted canonical client action.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_player_predict_right_cast(int x, int y) {
    if (g_selectedPlayer < 0 || size_t(g_selectedPlayer) >= g_players.size()) return 0;
    auto* unit = g_players[g_selectedPlayer].motionUnit;
    if (!unit || !unit->pDynamicPath || !UNITS_GetRoom(unit) || x < 0 || y < 0 || x > 65535 || y > 65535) return 0;
    auto* skill = UNITS_GetRightSkill(unit);
    if (!skill || !is_natural_client_projectile(SKILLS_GetSkillIdFromSkill(skill, __FILE__, __LINE__)) ||
        skill->nOwnerGUID != -1) return -1;
    auto* previous = UNITS_GetUsedSkill(unit);
    if (previous && !is_natural_client_projectile(SKILLS_GetSkillIdFromSkill(previous, __FILE__, __LINE__))) return -1;
    // 6FAC97D0 completes a pending old action before 6FAC9030 checks the new one.
    if (previous) {
        const int flags = SKILLS_GetFlags(previous);
        if ((flags & 1) || !(unit->dwFlags & 0x40u)) {
            if (flags & 1) SKILLS_SetFlags(previous, flags | 2);
            create_client_projectile(unit, previous);
        }
        UNITS_SetUsedSkill(unit, nullptr);
    }
    if (DUNGEON_IsRoomInTown(UNITS_GetRoom(unit)) || unit->dwAnimMode == PLRMODE_DEATH ||
        unit->dwAnimMode == PLRMODE_DEAD || SKILLS_GetUseState(unit, skill) != 0) return 0;
    auto* txt = skill->pSkillsTxt;
    if (!txt || txt->wCltStFunc != 0 || txt->wCltDoFunc != 0 || txt->nAnim != PLRMODE_CAST) return -1;
    PATH_ResetToPreviousType(unit->pDynamicPath);
    D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, x, y);
    UNITS_UpdateDirectionAndSpeed(unit, x, y);
    UNITS_SetTargetUnitForDynamicUnit(unit, nullptr);
    UNITS_SetUsedSkill(unit, skill);
    unit->dwFlags &= ~0x40u;
    // Same-mode starts are accepted while preserving the native animation clock.
    UNITS_ChangeAnimMode(unit, SKILLS_GetSkillModeFromUnit(unit, skill));
    return 1;
}

static int apply_player_skill_update(const uint8_t* packet, int length) {
    if (length != 12) return -1;
    if ((packet[0] == 0x21 && packet[1] != UNIT_PLAYER) || (packet[0] == 0x22 && packet[11])) return 0;
    const uint32_t guid = uint32_t(packet[3]) | (uint32_t(packet[4]) << 8) |
        (uint32_t(packet[5]) << 16) | (uint32_t(packet[6]) << 24);
    const int id = packet[7] | (int(packet[8]) << 8);
    D2UnitStrc* unit = nullptr;
    for (auto& player : g_players) if (player.view.guid == guid) { unit = player.motionUnit; break; }
    if (!unit) return 0;
    if (packet[0] == 0x21) {
        if (!DATATBLS_GetSkillsTxtRecord(id)) return 0;
        SKILLS_AssignSkill(unit, id, packet[9], packet[2], __FILE__, __LINE__);
    } else {
        auto* skill = SKILLS_GetSkillById(unit, id, -1);
        if (!skill) return 0;
        SKILLS_SetQuantity(skill, packet[9]);
    }
    return 1;
}

EMSCRIPTEN_KEEPALIVE int d2_realm_client_player_skill_value(uint32_t guid, int id, int quantity) {
    for (auto& player : g_players) if (player.view.guid == guid && player.motionUnit) {
        auto* skill = SKILLS_GetSkillById(player.motionUnit, id, -1);
        if (skill) return quantity ? SKILLS_GetQuantity(skill) : SKILLS_GetSkillLevel(player.motionUnit, skill, FALSE);
    }
    return -1;
}

// Native 1D/E/F select the current player; 20 resolves an existing player GUID.
// Values are already in the stat's wire units (resource fractions included).
static int apply_player_stat(const uint8_t* packet, int length) {
    const auto read = [](const uint8_t* p, int n) {
        uint32_t value = 0;
        for (int i = 0; i < n; ++i) value |= uint32_t(p[i]) << (i * 8);
        return value;
    };
    D2UnitStrc* unit = nullptr;
    int stat = 0;
    uint32_t value = 0;
    if (packet[0] == 0x20) {
        if (length != 10) return -1;
        const uint32_t guid = read(packet + 1, 4);
        for (auto& player : g_players) if (player.view.guid == guid) { unit = player.motionUnit; break; }
        stat = packet[5]; value = read(packet + 6, 4);
    } else {
        const int bytes = packet[0] == 0x1d ? 1 : packet[0] == 0x1e ? 2 : 4;
        if (length != bytes + 2) return -1;
        if (g_selectedPlayer >= 0 && size_t(g_selectedPlayer) < g_players.size())
            unit = g_players[g_selectedPlayer].motionUnit;
        stat = packet[1]; value = read(packet + 2, bytes);
    }
    if (!unit) return 0;
    STATLIST_SetUnitStat(unit, stat, static_cast<int32_t>(value), 0);
    return 1;
}

EMSCRIPTEN_KEEPALIVE int d2_realm_client_player_stat(uint32_t guid, int stat) {
    if (stat < 0 || stat > 255) return 0;
    for (auto& player : g_players) if (player.view.guid == guid && player.motionUnit)
        return STATLIST_UnitGetStatValue(player.motionUnit, stat, 0);
    return 0;
}

// 0x15 is a room/location reassignment, not a path prediction. Only the exact assigned player
// (unit type 0 and matching server GUID) changes this record; other units remain out of scope.
static int reassign_player(const uint8_t* packet, int length) {
    D2RealmPlayerReassign update{};
    if (d2_realm_d2gs_parse_player_reassign(packet, length, &update) != 11) return -1;
    if (update.unitType != 0) return 0;
    int target = -1;
    for (size_t i = 0; i < g_players.size(); ++i) if (g_players[i].view.guid == update.guid) { target = static_cast<int>(i); break; }
    if (target < 0) return 0;
    const bool selected = target == g_selectedPlayer;
    D2RealmClientPlayerEntry& entry = g_players[target];
    D2RealmClientPlayerState& player = selected ? g_player : entry.view;
    uint32_t level = 0, roomX = 0, roomY = 0, unusedInitSeed = 0;
    if (!d2_drlg_client_player_context(update.x, update.y, 0,
            &level, &roomX, &roomY, &unusedInitSeed)) return -2;
    player.x = update.x;
    player.y = update.y;
    if (!level) {
        player.clientRoomActive = 0;
        player.clientRoomLevel = player.clientRoomTileX = player.clientRoomTileY = 0;
    }
    player.reassignValue = update.value;
    player.reassignments++;
    refresh_player_room_level(player, selected);
    if (selected) entry.view = g_player;
    // 0x15 rebuilds/repositions the path even when the resolved room pointer is unchanged;
    // retaining a same-room route would let a later tick overwrite this server coordinate.
    if (!ensure_player_motion(entry, true)) return -3;
    if (selected) store_selected_player();
    return 1;
}

// Decoded bytes can end between server packets. Buffer only that suffix, then frame with the
// engine's pinned 1.13c size table on the next call. Unknown opcodes fail closed; unrelated valid
// packets are skipped without changing this player's state.
static uint32_t skill_u32(const uint8_t* p) {
    return uint32_t(p[0]) | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}

// Native 0x94 and 0x23 both look up an existing player GUID. Selection requires the
// actual CUnit skill+owner entry; do not mirror arbitrary server IDs into a parallel state.
static int apply_player_skills(const uint8_t* packet, int length) {
    const bool list = packet[0] == 0x94;
    if ((list && length != 6 + 3 * packet[1]) || (!list && length != 13)) return -1;
    if (!list && packet[1] != UNIT_PLAYER) return 0;
    const uint32_t guid = skill_u32(packet + 2);
    for (auto& player : g_players) {
        if (player.view.guid != guid) continue;
        if (!ensure_player_unit(player)) return -1;
        auto* unit = player.motionUnit;
        if (list) {
            if (!unit->pSkills) unit->pSkills = SKILLS_AllocSkillList(nullptr);
            if (!unit->pSkills) return -1;
            for (int row = 0; row < packet[1]; ++row) {
                const uint8_t* p = packet + 6 + row * 3;
                const int skill = p[0] | (int(p[1]) << 8);
                if (!DATATBLS_GetSkillsTxtRecord(skill)) return -1;
                SKILLS_AssignSkill(unit, skill, p[2], FALSE, __FILE__, __LINE__);
            }
        } else {
            const int skill = packet[7] | (int(packet[8]) << 8);
            if (!DATATBLS_GetSkillsTxtRecord(skill)) return -1;
            if (packet[6]) SKILLS_SetLeftActiveSkill(unit, skill, static_cast<int32_t>(skill_u32(packet + 9)));
            else SKILLS_SetRightActiveSkill(unit, skill, static_cast<int32_t>(skill_u32(packet + 9)));
        }
        return 1;
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE int d2_realm_client_player_feed(const uint8_t* bytes, int length) {
    if ((!bytes && length) || length < 0 || length > kPendingCapacity - g_pendingLength) return reject_stream(-1);
    if (length) {
        std::memcpy(g_pending + g_pendingLength, bytes, static_cast<size_t>(length));
        g_pendingLength += length;
    }
    int offset = 0;
    int applied = 0;
    while (offset < g_pendingLength) {
        const int size = d2_realm_d2gs_packet_size(g_pending + offset, g_pendingLength - offset);
        if (size == 0) break;
        if (size < 0 || size > g_pendingLength - offset) return reject_stream(-2);
        const uint8_t* packet = g_pending + offset;
        if (packet[0] == 0x26) {
            D2RealmGameChat parsed{};
            if (d2_realm_d2gs_parse_gamechat(packet, size, &parsed) != size) return reject_stream(-2);
            g_gameChat = parsed;
            ++g_gameChatCount;
            ++applied;
        } else if (packet[0] == 0x59) {
            const int result = assign_player(packet, size);
            if (result < 0) return reject_stream(-3);
            applied += result;
        } else if (packet[0] == 0x94 || packet[0] == 0x23) {
            const int result = apply_player_skills(packet, size);
            if (result < 0) return reject_stream(-2);
            applied += result;
        } else if ((packet[0] >= 0x1d && packet[0] <= 0x1f) || packet[0] == 0x20) {
            const int result = apply_player_stat(packet, size);
            if (result < 0) return reject_stream(-2);
            applied += result;
        } else if (packet[0] == 0x21 || packet[0] == 0x22) {
            const int result = apply_player_skill_update(packet, size);
            if (result < 0) return reject_stream(-2);
            applied += result;
        } else if (packet[0] == 0x4c || packet[0] == 0x4d) {
            const int result = packet[0] == 0x4c
                ? apply_remote_unit_skill_start(packet, size)
                : apply_remote_projectile_start(packet, size);
            if (result < 0) return reject_stream(-2);
            applied += result;
        } else if (packet[0] == 0x0d) {
            const int result = apply_player_stop(packet, size);
            if (result < 0) return reject_stream(-2);
            applied += result;
        } else if (packet[0] == 0x0f) {
            const int result = apply_player_move(packet, size);
            if (result < 0) return reject_stream(-2);
            applied += result;
        } else if (packet[0] == 0x9d) {
            const int result = apply_owned_item(packet, size);
            if (result < 0) return reject_stream(-4);
            applied += result;
        } else if (packet[0] == 0x0b) {
            const int result = select_player(packet, size);
            if (result < 0) return reject_stream(-2);
        } else if (packet[0] == 0xa7 || packet[0] == 0xa8 || packet[0] == 0xa9 || packet[0] == 0xaa) {
            const int result = apply_state_packet(packet, size);
            if (result < 0) return reject_stream(-4);
            applied += result;
        } else if (packet[0] == 0xac) {
            if (!apply_monster_assign(packet, size)) return reject_stream(-4);
            applied++;
        } else if (packet[0] == 0x69 || packet[0] == 0x6a) {
            const int result = apply_monster_death_mode(packet, size);
            if (result < 0) return reject_stream(-4);
            applied += result;
        } else if (packet[0] == 0x0a) {
            if (packet[1] == 0) {
                const int result = remove_player(packet, size);
                if (result < 0) return reject_stream(-2);
            } else if (packet[1] == 1) {
                if (!remove_monster(packet, size)) return reject_stream(-4);
                applied++;
            }
        } else if (packet[0] == 0x67) {
            const int result = apply_monster_move(packet, size);
            if (result < 0) return reject_stream(-4);
            applied += result;
        } else if (packet[0] == 0x15) {
            const int result = reassign_player(packet, size);
            if (result < 0) return reject_stream(-2);
            applied += result;
        } else if (packet[0] == 0x03) {
            // Native 1.13c LoadAct allocates a bClient act with the map seed. Packet act is
            // zero-based; the DRLG adapter takes the existing 1-based vector convention.
            const uint32_t seed = static_cast<uint32_t>(packet[2]) |
                (static_cast<uint32_t>(packet[3]) << 8) |
                (static_cast<uint32_t>(packet[4]) << 16) |
                (static_cast<uint32_t>(packet[5]) << 24);
            const int townLevel = static_cast<int>(packet[6]) | (static_cast<int>(packet[7]) << 8);
            destroy_all_monster_motion();
            destroy_all_player_motion();
            if (!d2_drlg_init() || !d2_drlg_client_act_begin(seed, static_cast<int>(g_clientDifficulty), packet[1] + 1, townLevel)) return reject_stream(-3);
            // The former act's room pointer is no longer live after LoadAct. Keep the CUnit seed,
            // but require a new assignment/reassignment to bind its current active-room identity.
            for (auto& entry : g_players) {
                entry.view.clientRoomActive = 0;
                entry.view.clientRoomLevel = entry.view.clientRoomTileX = entry.view.clientRoomTileY = 0;
            }
            g_player.clientRoomActive = 0;
            g_player.clientRoomLevel = g_player.clientRoomTileX = g_player.clientRoomTileY = 0;
            g_monsters.clear();
            g_monsterViews.clear();
            clear_monster_death_mode_events();
            store_selected_player();
            g_world.hasLoadAct = 1;
            g_world.act = packet[1];
            g_world.mapSeed = seed;
            g_world.area = static_cast<uint32_t>(packet[6]) |
                (static_cast<uint32_t>(packet[7]) << 8);
            g_world.rawLoadActTail = static_cast<uint32_t>(packet[8]) |
                (static_cast<uint32_t>(packet[9]) << 8) |
                (static_cast<uint32_t>(packet[10]) << 16) |
                (static_cast<uint32_t>(packet[11]) << 24);
            g_world.loadCompleteEvent = 0;
            g_world.hasMapReveal = 0;
            g_world.mapLevel = g_world.mapX = g_world.mapY = 0;
            g_world.loadActCount++;
            g_world.difficulty = g_clientDifficulty;
        } else if (packet[0] == 0x04) {
            if (g_selectedPlayer < 0 || !g_player.active) return reject_stream(-3); // 1.13c handler needs its current player for post-load refresh.
            g_world.loadCompleteEvent = 1;
            g_world.unloadCompleteEvent = 0;
        } else if (packet[0] == 0x05) {
            // Native UnloadComplete closes loader resources and changes loader flags; it preserves
            // the active client act, room graph, assigned units, and their path resources.
            g_world.loadCompleteEvent = 0;
            g_world.unloadCompleteEvent = 1;
        } else if (packet[0] == 0x07) {
            const uint32_t x = static_cast<uint32_t>(packet[1]) | (static_cast<uint32_t>(packet[2]) << 8);
            const uint32_t y = static_cast<uint32_t>(packet[3]) | (static_cast<uint32_t>(packet[4]) << 8);
            const uint32_t level = packet[5];
            const int hasPlayer = g_player.active && g_player.clientRoomActive;
            if (!g_world.hasLoadAct || !d2_drlg_client_map_reveal(static_cast<int>(level), static_cast<int>(x), static_cast<int>(y),
                hasPlayer, static_cast<int>(g_player.clientRoomLevel), static_cast<int>(g_player.clientRoomTileX), static_cast<int>(g_player.clientRoomTileY))) return reject_stream(-3);
            // PlayerAssign can precede the first active-room event. Re-resolve its location after
            // this MapReveal has built the room, then preserve that selection on other levels.
            refresh_all_player_motion_rooms();
            for (auto& monster : g_monsters) create_monster_motion(monster);
            g_world.hasMapReveal = 1;
            const uint32_t renderLevel = static_cast<uint32_t>(d2_drlg_client_render_level());
            if (renderLevel == level || !g_world.mapLevel) {
                g_world.mapX = x;
                g_world.mapY = y;
            }
            g_world.mapLevel = renderLevel;
            g_world.mapRevealCount++;
            g_world.mapRevision++;
        } else if (packet[0] == 0x08) {
            const uint32_t x = static_cast<uint32_t>(packet[1]) | (static_cast<uint32_t>(packet[2]) << 8);
            const uint32_t y = static_cast<uint32_t>(packet[3]) | (static_cast<uint32_t>(packet[4]) << 8);
            const uint32_t level = packet[5];
            const int hasPlayer = g_player.active && g_player.clientRoomActive;
            // UnsetClientIsInSight decrements the target room's sight count and propagates exact
            // reference-count changes through its near-room graph. Free only carriers whose own
            // active room will lose every status; paths in retained rooms keep their route.
            for (auto& monster : g_monsters) {
                D2UnitStrc* unit = monster.motionUnit;
                D2ActiveRoomStrc* room = unit && unit->pDynamicPath ? unit->pDynamicPath->pRoom : nullptr;
                if (room && d2_drlg_client_room_freed_by_hide(static_cast<int>(level), static_cast<int>(x),
                        static_cast<int>(y), hasPlayer, static_cast<int>(g_player.clientRoomLevel),
                        static_cast<int>(g_player.clientRoomTileX), static_cast<int>(g_player.clientRoomTileY), room))
                    destroy_monster_motion(monster);
            }
            for (size_t i = 0; i < g_clientMissiles.size();) {
                auto* unit = g_clientMissiles[i].unit;
                auto* room = unit->pDynamicPath ? unit->pDynamicPath->pRoom : nullptr;
                if (room && d2_drlg_client_room_freed_by_hide(static_cast<int>(level), static_cast<int>(x),
                        static_cast<int>(y), hasPlayer, static_cast<int>(g_player.clientRoomLevel),
                        static_cast<int>(g_player.clientRoomTileX), static_cast<int>(g_player.clientRoomTileY), room)) {
                    record_client_missile_event(6, unit);
                    free_client_missile(unit);
                    g_clientMissiles.erase(g_clientMissiles.begin() + i);
                } else ++i;
            }
            for (auto& player : g_players) {
                D2UnitStrc* unit = player.motionUnit;
                D2ActiveRoomStrc* room = unit && unit->pDynamicPath ? unit->pDynamicPath->pRoom : nullptr;
                if (room && d2_drlg_client_room_freed_by_hide(static_cast<int>(level), static_cast<int>(x),
                        static_cast<int>(y), hasPlayer, static_cast<int>(g_player.clientRoomLevel),
                        static_cast<int>(g_player.clientRoomTileX), static_cast<int>(g_player.clientRoomTileY), room))
                    detach_player_motion_path(player);
            }
            if (!g_world.hasLoadAct || !d2_drlg_client_map_hide(static_cast<int>(level), static_cast<int>(x), static_cast<int>(y),
                hasPlayer, static_cast<int>(g_player.clientRoomLevel), static_cast<int>(g_player.clientRoomTileX), static_cast<int>(g_player.clientRoomTileY))) return reject_stream(-3);
            refresh_all_player_motion_rooms();
            for (auto& monster : g_monsters) create_monster_motion(monster);
            g_world.mapHideCount++;
            g_world.mapRevision++;
            g_world.hasMapReveal = d2_drlg_client_tile_count() > 0;
            if (!g_world.hasMapReveal) g_world.mapLevel = g_world.mapX = g_world.mapY = 0;
            else g_world.mapLevel = static_cast<uint32_t>(d2_drlg_client_render_level());
        }
        offset += size;
    }
    const int remaining = g_pendingLength - offset;
    if (offset && remaining) std::memmove(g_pending, g_pending + offset, static_cast<size_t>(remaining));
    g_pendingLength = remaining;
    return applied;
}

EMSCRIPTEN_KEEPALIVE int d2_realm_client_player_pending() { return g_pendingLength; }

struct RealmClientMissileView {
    uint32_t version, size, id, classId, mode, framePrecise, frameCountPrecise, animSpeed;
    uint32_t direction, trans, preciseX, preciseY, level, ownerGuid, skill, remaining;
    uint32_t initSeed, seedLow, seedHigh;
};
static_assert(sizeof(RealmClientMissileView) == 76);
static std::vector<RealmClientMissileView> g_clientMissileViews;
EMSCRIPTEN_KEEPALIVE int d2_realm_client_missile_event_count() { return static_cast<int>(g_clientMissileEvents.size()); }
EMSCRIPTEN_KEEPALIVE const RealmMissileEvent* d2_realm_client_missile_events() { return g_clientMissileEvents.data(); }
// Consumes class-62 terminal hit/expiry sound rows only. Record words are
// {version,tick,missileId,classId,event,soundRow}; other missile sounds remain unsupported.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_take_missile_sounds(int32_t* out, int cap) {
    if (!out || cap <= 0) return 0;
    const int count = std::min(cap, static_cast<int>(g_clientMissileSoundEvents.size()));
    for (int i = 0; i < count; ++i) {
        const auto& event = g_clientMissileSoundEvents[static_cast<size_t>(i)];
        const int offset = i * 6;
        out[offset] = static_cast<int32_t>(event.version);
        out[offset + 1] = static_cast<int32_t>(event.tick);
        out[offset + 2] = static_cast<int32_t>(event.missileId);
        out[offset + 3] = static_cast<int32_t>(event.classId);
        out[offset + 4] = static_cast<int32_t>(event.event);
        out[offset + 5] = static_cast<int32_t>(event.soundRow);
    }
    g_clientMissileSoundEvents.erase(g_clientMissileSoundEvents.begin(),
        g_clientMissileSoundEvents.begin() + count);
    return count;
}
EMSCRIPTEN_KEEPALIVE int d2_realm_client_missile_count() { return static_cast<int>(g_clientMissiles.size()); }
EMSCRIPTEN_KEEPALIVE const RealmClientMissileView* d2_realm_client_missiles() {
    g_clientMissileViews.clear();
    for (const auto& entry : g_clientMissiles) {
        const auto* unit = entry.unit;
        auto* path = unit->pDynamicPath;
        const auto* txt = DATATBLS_GetMissilesTxtRecord(unit->dwClassId);
        g_clientMissileViews.push_back({1, static_cast<uint32_t>(sizeof(RealmClientMissileView)), unit->dwUnitId,
            static_cast<uint32_t>(unit->dwClassId), unit->dwAnimMode, static_cast<uint32_t>(unit->nSeqCurrentFramePrecise),
            static_cast<uint32_t>(unit->dwFrameCountPrecise), static_cast<uint32_t>(unit->wAnimSpeed), static_cast<uint32_t>(PATH_GetDirection(path)),
            txt ? txt->nTrans : 0u, static_cast<uint32_t>(PATH_GetPrecisionX(path)),
            static_cast<uint32_t>(PATH_GetPrecisionY(path)), entry.level,
            unit->pMissileData->dwOwnerGUID, static_cast<uint32_t>(MISSILE_GetSkill(entry.unit)),
            static_cast<uint32_t>(MISSILE_GetCurrentFrame(entry.unit)), unit->dwInitSeed,
            unit->pSeed.nLowSeed, unit->pSeed.nHighSeed});
    }
    return g_clientMissileViews.data();
}

EMSCRIPTEN_KEEPALIVE int d2_realm_client_monster_count() {
    return static_cast<int>(g_monsters.size());
}

// Narrow readback for native D2GS state-packet verification. State data remains on the same
// canonical CUnit used by rendering; this does not add a parallel state projection or widen its ABI.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_unit_state(uint8_t unitType, uint32_t guid, int state) {
    D2UnitStrc* unit = find_realm_client_unit(unitType, guid);
    if (!unit || state < 0 || !sgptDataTables || state >= sgptDataTables->nStatesTxtRecordCount) return -1;
    return STATES_CheckState(unit, state) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int d2_realm_client_unit_state_stat(uint8_t unitType, uint32_t guid,
        int state, int statId, uint16_t layer) {
    D2UnitStrc* unit = find_realm_client_unit(unitType, guid);
    if (!unit || state < 0 || statId < 0 || !sgptDataTables ||
        state >= sgptDataTables->nStatesTxtRecordCount || statId >= sgptDataTables->nItemStatCostTxtRecordCount)
        return 0;
    D2StatListStrc* list = STATLIST_GetStatListFromUnitAndState(unit, state);
    return list ? STATLIST_GetStatValue(list, statId, layer) : 0;
}

// Focused lifecycle readback for the canonical GUID roster; zero means no matching live CUnit.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_unit_resources_readback(uint8_t unitType, uint32_t guid) {
    return d2_realm_client_unit_resources_status(find_realm_client_unit(unitType, guid));
}

// Read one equipped slot from the canonical server-owned player's real inventory. This is a
// bounded evidence interface; it does not synthesize an inventory item in JS.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_owned_item_readback_by_guid(uint32_t ownerGuid,
        int bodyLoc, D2RealmOwnedItemView* out) {
    if (!out) return 0;
    D2UnitStrc* owner = find_realm_client_unit(UNIT_PLAYER, ownerGuid);
    return d2_realm_client_owned_item_readback(owner, bodyLoc, out);
}

// Advance path-owned monster CUnits once for each shell game frame. Renderer getters remain pure.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_motion_tick() {
    ++g_clientMissileTick;
    int moving = 0;
    advance_client_missiles();
    for (size_t i = 0; i < g_players.size(); ++i) {
        auto& player = g_players[i];
        if (!player.motionUnit || !player.motionUnit->pDynamicPath) continue;
        const int result = advance_client_projectile_cast(player.motionUnit) ? 0 :
            d2_realm_client_motion_advance_unit(player.motionUnit);
        if (result > 0) {
            moving++;
            refresh_player_motion_view(player, static_cast<int>(i) == g_selectedPlayer);
        }
    }
    for (auto& monster : g_monsters) {
        if (!monster.motionUnit || !monster.motionUnit->pDynamicPath) continue;
        monster.lastTickResult = d2_realm_client_motion_advance_unit(monster.motionUnit);
        if (monster.lastTickResult > 0) moving++;
        refresh_monster_motion_view(monster);
    }
    return moving;
}

EMSCRIPTEN_KEEPALIVE const D2RealmClientMonsterView* d2_realm_client_monsters() {
    g_monsterViews.clear();
    g_monsterViews.reserve(g_monsters.size());
    for (const auto& monster : g_monsters) g_monsterViews.push_back(monster.view);
    return g_monsterViews.data();
}
EMSCRIPTEN_KEEPALIVE int d2_realm_client_monster_death_mode_event_count() {
    return static_cast<int>(g_monsterDeathModeEvents.size());
}
EMSCRIPTEN_KEEPALIVE const d2::realm::MonsterDeathModeEvent* d2_realm_client_monster_death_mode_events() {
    g_monsterDeathModeEventView = g_monsterDeathModeEvents;
    g_monsterDeathModeEvents.clear();
    return g_monsterDeathModeEventView.data();
}
EMSCRIPTEN_KEEPALIVE int d2_realm_client_monster_aux(uint32_t index, D2RealmClientMonsterAux* out) {
    if (!out || index >= g_monsters.size()) return 0;
    const auto& monster = g_monsters[index];
    *out = {};
    out->version = 2;
    out->size = sizeof(*out);
    out->guid = monster.view.guid;
    out->hasExtra31 = monster.hasExtra31 ? 1u : 0u;
    out->extra31 = monster.extra31;
    out->hasStats = monster.hasStats ? 1u : 0u;
    out->statCount = static_cast<uint32_t>(monster.stats.size());
    if (!monster.stats.empty()) {
        out->firstStatId = monster.stats.front().statId;
        out->firstStatParameter = monster.stats.front().parameter;
        out->firstStatValue = monster.stats.front().value;
    }
    out->unitFlags = monster.motionUnit ? monster.motionUnit->dwFlags : 0;
    return 1;
}
EMSCRIPTEN_KEEPALIVE int d2_realm_client_monster_motion_status(uint32_t index, D2RealmClientMotionStatus* out) {
    if (!out) return 0;
    *out = {};
    if (index >= g_monsters.size()) return 0;
    const auto& monster = g_monsters[index];
    out->version = 1;
    out->size = sizeof(*out);
    out->guid = monster.view.guid;
    out->mode = monster.view.mode;
    out->framePrecise = monster.view.framePrecise;
    out->frameCountPrecise = monster.view.frameCountPrecise;
    out->animSpeed = monster.view.animSpeed;
    out->lastPlanResult = monster.lastPlanResult;
    out->lastTickResult = monster.lastTickResult;
    const D2UnitStrc* unit = monster.motionUnit;
    const D2DynamicPathStrc* path = unit ? unit->pDynamicPath : nullptr;
    if (path) {
        out->attached = 1;
        out->pathFlags = path->dwFlags;
        out->pathType = path->dwPathType;
        out->pathPoints = path->dwPathPoints;
        out->currentPoint = path->dwCurrentPointIdx;
        out->startX = path->tGameCoords.wPosX;
        out->startY = path->tGameCoords.wPosY;
        out->targetX = path->tTargetCoord.X;
        out->targetY = path->tTargetCoord.Y;
    }
    return 1;
}
EMSCRIPTEN_KEEPALIVE int d2_realm_client_monster_motion_status_v2(uint32_t index, D2RealmClientMotionStatusV2* out) {
    if (!out) return 0;
    *out = {};
    if (!d2_realm_client_monster_motion_status(index, &out->base)) return 0;
    out->base.version = 2;
    out->base.size = sizeof(*out);
    const D2UnitStrc* unit = g_monsters[index].motionUnit;
    const D2DynamicPathStrc* path = unit ? unit->pDynamicPath : nullptr;
    const D2DrlgRoomStrc* room = path && path->pRoom ? path->pRoom->pDrlgRoom : nullptr;
    if (room) {
        out->roomLevel = room->pLevel ? static_cast<uint32_t>(room->pLevel->nLevelId) : 0;
        out->roomTileX = static_cast<uint32_t>(room->nTileXPos);
        out->roomTileY = static_cast<uint32_t>(room->nTileYPos);
        out->roomTileWidth = static_cast<uint32_t>(room->nTileWidth);
        out->roomTileHeight = static_cast<uint32_t>(room->nTileHeight);
        out->clientSightRefs = room->wRoomsInList[ROOMSTATUS_CLIENT_IN_SIGHT];
    }
    return 1;
}
EMSCRIPTEN_KEEPALIVE int d2_realm_client_monster_motion_status_v3(uint32_t index, D2RealmClientMotionStatusV3* out) {
    if (!out) return 0;
    *out = {};
    if (!d2_realm_client_monster_motion_status_v2(index, &out->base)) return 0;
    out->base.base.version = 3;
    out->base.base.size = sizeof(*out);
    const D2RealmClientMonster* monster = &g_monsters[index];
    const D2DynamicPathStrc* path = monster->motionUnit ? monster->motionUnit->pDynamicPath : nullptr;
    if (path) {
        out->currentDirection = path->nDirection;
        out->targetDirection = path->nNewDirection;
        out->directionDelta = path->nDiffDirection;
        out->directionFlags = path->dwFlags;
    }
    return 1;
}
// Collision probe with the exact pattern/mask the assigned monster path planner uses. Temporarily
// remove/restore the carrier's own footprint, matching #10334's collision preflight.
// Returns -1 outside its loaded active-room chain, 0 clear, or 1 colliding.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_monster_motion_collision(uint32_t index, int x, int y) {
    if (index >= g_monsters.size()) return -1;
    const D2UnitStrc* unit = g_monsters[index].motionUnit;
    const D2DynamicPathStrc* path = unit ? unit->pDynamicPath : nullptr;
    if (!path || !path->pRoom) return -1;
    PATH_RemoveCollisionFootprintForUnit(const_cast<D2UnitStrc*>(unit), FALSE);
    D2ActiveRoomStrc* room = COLLISION_GetRoomBySubTileCoordinates(path->pRoom, x, y);
    const int collision = room ? (COLLISION_CheckAnyCollisionWithPattern(room, x, y, path->dwCollisionPattern,
        path->nMoveTestCollisionMask) ? 1 : 0) : -1;
    PATH_AddCollisionFootprintForUnit(const_cast<D2UnitStrc*>(unit));
    return collision;
}
EMSCRIPTEN_KEEPALIVE const D2RealmClientPlayerState* d2_realm_client_player_state() { return &g_player; }
EMSCRIPTEN_KEEPALIVE const D2RealmGameChat* d2_realm_client_game_chat() { return &g_gameChat; }
EMSCRIPTEN_KEEPALIVE uint32_t d2_realm_client_game_chat_count() { return g_gameChatCount; }
EMSCRIPTEN_KEEPALIVE uint32_t* d2_realm_client_player_unit_seed() {
    return g_player.active && g_player.cunitRngValid ? &g_player.cunitRngLow : nullptr;
}
// Derived from the selected canonical CUnit, so 0x0B, removal and replacement cannot
// leave stale hand selection behind. Existing player/render ABI remains unchanged.
EMSCRIPTEN_KEEPALIVE const int32_t* d2_realm_client_selected_skills() {
    static int32_t result[4];
    result[0] = result[1] = result[2] = result[3] = -1;
    if (g_selectedPlayer < 0 || static_cast<size_t>(g_selectedPlayer) >= g_players.size()) return result;
    auto* unit = g_players[g_selectedPlayer].motionUnit;
    if (!unit || !unit->pSkills) return result;
    auto* left = UNITS_GetLeftSkill(unit);
    auto* right = UNITS_GetRightSkill(unit);
    if (left) { result[0] = SKILLS_GetSkillIdFromSkill(left, __FILE__, __LINE__); result[2] = SKILLS_GetOwnerGUIDFromSkill(left); }
    if (right) { result[1] = SKILLS_GetSkillIdFromSkill(right, __FILE__, __LINE__); result[3] = SKILLS_GetOwnerGUIDFromSkill(right); }
    return result;
}

// Same eight-byte row as d2_session_skill_choices, but sourced from the GUID-matched
// realm player's canonical CUnit. The realm popup follows the live realm player view,
// including its roster skills, room level, usability and class skill metadata.
struct D2RealmClientSkillChoice {
    uint16_t id;
    int8_t listRow;
    int8_t charClass;
    uint16_t iconCel;
    uint8_t flags;
    uint8_t level;
};
static_assert(sizeof(D2RealmClientSkillChoice) == 8, "realm skill choice row is 8 bytes");
EMSCRIPTEN_KEEPALIVE uint32_t d2_realm_client_skill_choices(uint32_t guid, D2RealmClientSkillChoice* out, uint32_t cap) {
    if (!out || !cap) return 0;
    auto player = std::find_if(g_players.begin(), g_players.end(), [guid](const auto& entry) {
        return entry.view.active && entry.view.guid == guid;
    });
    if (player == g_players.end()) return 0;
    auto* unit = player->motionUnit;
    if (!unit || !unit->pSkills) return 0;
    const bool inTown = DUNGEON_IsTownLevelId(static_cast<int>(player->view.clientRoomLevel));
    uint32_t n = 0;
    for (D2SkillStrc* skill = SKILLS_GetFirstSkillFromSkillList(unit->pSkills); skill && n < cap;
         skill = SKILLS_GetNextSkill(skill)) {
        D2SkillsTxt* txt = SKILLS_GetSkillsTxtRecordFromSkill(skill);
        if (!txt) continue;
        D2SkillDescTxt* desc = DATATBLS_GetSkillDescTxtRecord(txt->wSkillDesc);
        auto& row = out[n++];
        row.id = static_cast<uint16_t>(SKILLS_GetSkillIdFromSkill(skill, __FILE__, __LINE__));
        row.listRow = desc ? static_cast<int8_t>(desc->nListRow) : -1;
        row.charClass = txt->nCharClass;
        row.iconCel = desc ? desc->nIconCel : 0;
        // bit 4 as d2_session_skill_choices: every non-usable state is red except an aura (retail D2Client+0x3DA60 gives it a non-red class).
        const int useState = SKILLS_GetUseState(unit, skill);
        row.flags = (txt->dwFlags[0] & SKILLSFLAG_PASSIVE ? 1 : 0) |
            (txt->dwFlags[0] & SKILLSFLAG_LEFTSKILL ? 2 : 0) |
            (useState != SKILLUSTATE_USABLE && useState != SKILLUSTATE_AURA ? 4 : 0) |
            (inTown && !(txt->dwFlags[0] & SKFLAG_INTOWN) ? 8 : 0) |
            (useState == SKILLUSTATE_AURA ? 16 : 0);   // bit 16 = aura, drawn with colour shift 4 (D2Client+0x3DA60)
        row.level = static_cast<uint8_t>(SKILLS_GetSkillLevel(unit, skill, TRUE));
    }
    return n;
}

EMSCRIPTEN_KEEPALIVE int d2_realm_client_player_used_skill_at(int index) {
    if (index < 0 || static_cast<size_t>(index) >= g_players.size()) return -1;
    auto* unit = g_players[index].motionUnit;
    if (!unit || !unit->pSkills) return -1;
    auto* skill = UNITS_GetUsedSkill(unit);
    return skill ? SKILLS_GetSkillIdFromSkill(skill, __FILE__, __LINE__) : -1;
}

EMSCRIPTEN_KEEPALIVE int d2_realm_client_player_count() { return static_cast<int>(g_players.size()); }
EMSCRIPTEN_KEEPALIVE const D2RealmClientPlayerState* d2_realm_client_player_at(int index) {
    return index >= 0 && static_cast<size_t>(index) < g_players.size() ? &g_players[index].view : nullptr;
}
// Read live render state from the same canonical CUnit whose DynamicPath the realm feed and
// 25 Hz tick mutate. The 92-byte historical assignment view stays ABI-compatible.
EMSCRIPTEN_KEEPALIVE const D2RealmClientPlayerEntry::MotionView* d2_realm_client_player_motion_at(int index) {
    if (index < 0 || static_cast<size_t>(index) >= g_players.size()) return nullptr;
    auto& entry = g_players[static_cast<size_t>(index)];
    auto& out = entry.motionView;
    out.active = 0;
    out.mode = out.direction = out.framePrecise = out.frameCountPrecise = out.animSpeed = 0;
    out.positionHintX = out.positionHintY = 0;
    out.seqMode = out.seqFrame = 0;
    const D2UnitStrc* unit = entry.motionUnit;
    if (!unit || !unit->pDynamicPath) return &out;
    out.active = 1;
    out.mode = unit->dwAnimMode;
    out.direction = PATH_GetDirection(unit->pDynamicPath);
    out.framePrecise = unit->nSeqCurrentFramePrecise;
    out.frameCountPrecise = unit->dwFrameCountPrecise;
    out.animSpeed = static_cast<uint32_t>(unit->wAnimSpeed);
    out.positionHintX = unit->nLastClickX;
    out.positionHintY = unit->nLastClickY;
    if (unit->dwAnimMode == PLRMODE_SEQUENCE) {
        out.seqMode = unit->dwSeqMode;
        out.seqFrame = static_cast<uint32_t>(unit->nSeqCurrentFramePrecise >> 8);
    }
    return &out;
}
EMSCRIPTEN_KEEPALIVE const D2RealmClientWorldState* d2_realm_client_world_state() { return &g_world; }

}  // extern "C"
