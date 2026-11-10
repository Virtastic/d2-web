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
#include <D2Items.h>
#include <D2QuestRecord.h>
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
#include "../d2client/inv_weapon_hands.h"
#include "../d2client/character_screen.h"
#include "d2gs_client_motion.h"
#include "d2gs_client_player_speed.h"
#include "d2gs_client_player_skills.h"
#include "d2gs_client_object.h"
#include "d2gs_client_snapshot.h"
#include "d2gs_client_target_relations.h"
#include "d2gs_client_owned_item.h"
#include "d2gs_client_game_packets.h"
#include "d2gs_client_unit_resources.h"
#include "client_fireball_hit.h"
#include "client_missile_sound.h"
#include "d2gs_client_player_motion.h"
#include "d2gs_client_player_death.h"
#include "d2gs_client_player_stop.h"
#include "d2gs_client_monster_death_events.h"
#include "cast_target_relation.h"
#include "client_skill_fx.h"
#include <DataTbls/OverlayTbls.h>
#include "d2gs_client_monster_death_clock.h"
#include "d2gs_client_pkt_route.h"
#include "d2gs_client_combat.h"
#include "d2gs_client_unit_fx.h"
#include "d2gs_client_quests.h"
#include "d2gs_client_warps.h"
#include "d2gs_client_state_stats.h"
#include "client_pets.h"
#include "client_monster_name.h"
#include "../d2client/d2client_pkt_env.h"
#include <D2Environment.h>
#include "../d2client/d2client_unit_ports.h"

extern "C" uint32_t d2c_port_af5f0(uint32_t val, const uint8_t* rec,
    uint32_t (*f2a750)(uint32_t ecx, uint32_t edx, uint32_t* blk, uint32_t one));

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
struct D2RealmClientObjectView {
    uint32_t unitType, guid, classId, x, y, mode, interact;
};
static_assert(sizeof(D2RealmClientObjectView) == 28);
std::vector<D2RealmClientObjectView> g_objects;
// The client units of the assigned objects, found by guid. They are kept apart from the player and monster
// lookup: an object's path is a static one, and that lookup's callers walk a dynamic path.
// Object activations (a shrine used by a player) not yet taken by the shell: the sound it plays at the player.
std::vector<D2gsClientObjectEvent> g_objectEvents;
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

static d2::realm::CorpseRoster g_corpses;

static void destroy_player_owned_items(D2UnitStrc* owner) {
    // Free item CUnits before the inventory/resource owner is released; INVENTORY_FreeInventory
    // only detaches items from the inventory.
    d2_realm_client_items_forget_owner(owner);
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
    d2_realm_client_items_reset();
    g_selectedPlayer = -1;
    g_corpses.clear();
    clear_player();
}

// The selected player's room, the room the client environment calls take.
D2ActiveRoomStrc* selected_player_room() {
    if (g_selectedPlayer < 0 || static_cast<size_t>(g_selectedPlayer) >= g_players.size()) return nullptr;
    D2UnitStrc* unit = g_players[g_selectedPlayer].motionUnit;
    return unit && unit->pDynamicPath ? unit->pDynamicPath->pRoom : nullptr;
}

uint32_t selected_player_guid() {
    if (g_selectedPlayer < 0 || static_cast<size_t>(g_selectedPlayer) >= g_players.size()) return 0xFFFFFFFFu;
    return g_players[g_selectedPlayer].view.guid;
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
void d2_client_lightmap_set_den_quest_avail(int);
int d2_drlg_init();
int d2_drlg_client_act_begin(uint32_t, int, int, int);
D2DrlgActStrc* d2_drlg_client_act();
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
// Object units live in the client's object table and on their room's unit list (d2gs_client_object.cpp).
static D2UnitStrc* find_object_unit(uint32_t guid) { return d2gs_client_object_find(guid); }
static void destroy_object_unit(uint32_t guid) { d2gs_client_object_destroy(d2gs_client_object_find(guid)); }
// Every caller has ended the act first, so the rooms, their unit lists and collision grids are already gone.
static void clear_objects() {
    while (D2UnitStrc* unit = d2gs_client_object_next(nullptr)) {
        d2gs_client_object_unplace(unit, false);
        d2gs_client_object_destroy(unit);
    }
    g_objects.clear();
    g_objectEvents.clear();
}
// An object assigned before its room was revealed takes the room, and its footprint, once it is there.
static void place_unroomed_objects() {
    for (D2UnitStrc* unit = d2gs_client_object_next(nullptr); unit; unit = d2gs_client_object_next(unit)) {
        if (UNITS_GetRoom(unit)) continue;
        const int x = UNITS_GetXPosition(unit), y = UNITS_GetYPosition(unit);
        if (D2ActiveRoomStrc* room = d2_drlg_client_active_room(x, y)) d2gs_client_object_place(unit, room, x, y);
    }
}
static void sync_object_view(const D2UnitStrc* unit) {
    for (auto& object : g_objects)
        if (object.unitType == UNIT_OBJECT && object.guid == unit->dwUnitId) { object.mode = unit->dwAnimMode; return; }
}

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
    d2_realm_client_unit_enter_mode(unit, mode, unit->dwUnitType == UNIT_PLAYER,
        d2::realm::monsterAnimationModeNeedsSetup(unit->dwAnimMode, mode, unit->dwFrameCountPrecise),
        [](D2UnitStrc* u) { STATLIST_DeactivateTemporaryStates(u); },
        [](D2UnitStrc* u) { UNITS_SetAnimStartFrame(u); });
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

// An assigned monster's life is the record's byte against a fixed maximum (1.13c D2Client+0xAE420).
// The local player's own hireling keeps the life its own records set.
static bool is_own_hireling(uint32_t guid);
static void apply_assigned_life(D2UnitStrc* unit, uint32_t lifeByte) {
    if (!unit || !unit->pStatListEx || is_own_hireling(unit->dwUnitId)) return;
    STATLIST_SetUnitStat(unit, 7, static_cast<int>(d2::realm::kAssignedMaxLife), 0);
    STATLIST_SetUnitStat(unit, 6, static_cast<int>(d2::realm::assignedLife(static_cast<uint8_t>(lifeByte))), 0);
}

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
        apply_assigned_life(unit, monster.view.hpPct);
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
    // The client's own player unit outlives an act change with everything it holds: the server does
    // not send the hero's items or stats again. Only its place in the old act's rooms goes.
    // GUESS: that the other players' units are freed here; the act-load handler's unit sweep is not read.
    // Settled by the decompile of the unit-table clear the 0x03 handler calls.
    for (size_t i = 0; i < g_players.size(); ++i) {
        if (static_cast<int>(i) == g_selectedPlayer) detach_player_motion_path(g_players[i]);
        else destroy_player_motion(g_players[i]);
    }
    d2_realm_client_items_forget_loose();
}

struct PlayerSkillOps {
    bool hasList(D2UnitStrc* unit) const { return unit->pSkills != nullptr; }
    bool allocList(D2UnitStrc* unit) const { return (unit->pSkills = SKILLS_AllocSkillList(nullptr)) != nullptr; }
    void initList(D2UnitStrc* unit) const { SKILLS_InitSkillList(unit); }
    D2SkillStrc* left(D2UnitStrc* unit) const { return UNITS_GetLeftSkill(unit); }
    D2SkillStrc* right(D2UnitStrc* unit) const { return UNITS_GetRightSkill(unit); }
};

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
    // The owner is never sent its own base velocity: the client sets it, as the server does for its unit.
    d2_realm_client_player_init_rates(player.motionUnit,
        [](D2UnitStrc* u, int32_t stat, int32_t value) { STATLIST_SetUnitStat(u, stat, value, 0); });
    // The same init gives the unit its skill list (Attack, the class's base skills, Attack in both
    // hands), so a hand can be read before, or without, the server's skill packets.
    if (sgptDataTables) d2_realm_client_player_init_skills(player.motionUnit, PlayerSkillOps{});
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
            apply_assigned_life(existing.motionUnit, monster.view.hpPct);
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

static void clear_game_packet_state();
static int reject_stream(int error) {
    clear_game_packet_state();
    clear_client_missiles();
    reset_monster_death_mode_events();
    destroy_all_monster_motion();
    clear_players();
    d2_drlg_act_end();
    clear_world();
    g_monsters.clear();
    d2::realm::clientPetsClear();
    g_monsterViews.clear();
    clear_objects();
    clear_game_chat();
    g_pendingLength = 0;
    return error;
}

static void clear_client_quest_record();
// What the server's quest packets (0x28, 0x29, 0x52, 0x5D) and tile-unit packets (0x09) have built.
static D2RealmClientQuests g_quests;
static D2RealmClientWarps g_warps;

EMSCRIPTEN_KEEPALIVE void d2_realm_client_player_reset() {
    clear_game_packet_state();
    clear_client_quest_record();
    d2_realm_client_quests_reset(g_quests);
    g_warps.list.clear();
    clear_client_missiles();
    reset_monster_death_mode_events();
    destroy_all_monster_motion();
    clear_players();
    d2_drlg_act_end();
    clear_world();
    g_monsters.clear();
    d2::realm::clientPetsClear();
    g_monsterViews.clear();
    clear_objects();
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
    } else if (unitType == UNIT_OBJECT) {
        return d2gs_client_object_find(guid);
    }
    return nullptr;
}

// The movement and mode packets (0x0C, 0x0F, 0x10, 0x67..0x6D) run through the retail client's
// own handlers (src/core/d2client/d2client_af*.cpp). The handlers read the record, build the
// request block and call two engine routines; this is the native side of those two calls: the
// unit placement (D2Client 0x2A1B0) and the action dispatch (D2Client 0x2A750 / 0x4DC70).
namespace {
D2cPktEnv g_pktEnv{};
d2::realm::PktRoute g_pktRoute{};
D2RealmClientMonster* g_pktMonster = nullptr;
D2UnitStrc* g_pktUnit = nullptr;
uint8_t g_pktOpcode = 0;
int g_pktResult = 0;
}

static uint32_t pkt_word(const uint32_t* l, uint32_t i) {
    return l && i < g_pktRoute.block ? l[i] : 0;
}

static uint32_t pkt_get_stat(uintptr_t unit, uint32_t stat, uint32_t layer) {
    D2UnitStrc* u = reinterpret_cast<D2UnitStrc*>(unit);
    if (!u || !u->pStatListEx) return 0;
    return static_cast<uint32_t>(STATLIST_UnitGetStatValue(u, static_cast<int>(stat), static_cast<uint16_t>(layer)));
}

static void pkt_set_stat(uintptr_t unit, uint32_t stat, uint32_t value, uint32_t layer) {
    D2UnitStrc* u = reinterpret_cast<D2UnitStrc*>(unit);
    if (!u || !u->pStatListEx) return;
    STATLIST_SetUnitStat(u, static_cast<int>(stat), static_cast<int>(value), static_cast<uint16_t>(layer));
}

static bool g_resyncPending = false;
static uint16_t g_resyncX = 0, g_resyncY = 0;

// 0x2A1B0: x in EAX, y the first stack word, the radius selector the second.
static void pkt_place(uint32_t x, uintptr_t unitBits, uint32_t y, uint32_t radiusSel, uint32_t, uint32_t) {
    D2UnitStrc* unit = reinterpret_cast<D2UnitStrc*>(unitBits);
    x &= 0xffff;
    y &= 0xffff;
    if (!unit) return;
    const bool isPlayer = unit->dwUnitType == UNIT_PLAYER;
    if (d2::realm::placeIgnored(unit->dwUnitType, unit->dwAnimMode, unit->dwFlags, x, y)) return;
    unit->nLastClickX = static_cast<uint16_t>(x);
    unit->nLastClickY = static_cast<uint16_t>(y);
    D2RealmClientPlayerEntry* player = nullptr;
    if (isPlayer)
        for (auto& candidate : g_players)
            if (candidate.motionUnit == unit) { player = &candidate; break; }
    const bool isLocal = player && g_selectedPlayer >= 0 &&
        player == &g_players[static_cast<size_t>(g_selectedPlayer)];
    // The monster view is found from the unit, not from the monster the record named: retail acts on the unit.
    D2RealmClientMonster* monster = nullptr;
    if (!isPlayer && unit->dwUnitType == UNIT_MONSTER)
        for (auto& candidate : g_monsters)
            if (candidate.motionUnit == unit) { monster = &candidate; break; }
    if (isPlayer && !player) return;
    if (!unit->pDynamicPath) {
        if (monster) { monster->view.x = x; monster->view.y = y; }
        return;
    }
    // The local player's tolerance adds ((ping + 0x32) >> 7). The ping is the host's own clock
    // difference, not a game value, and the realm layer does not sample it yet.
    // GUESS: localTerm 0 is right only for a ping under 78 ms; a capture with the ping pinned settles it.
    const int radius = d2::realm::placeRadius(unit->dwUnitType == UNIT_MONSTER, isLocal, unit->dwAnimMode, radiusSel, 0);
    const int curX = PATH_GetXPosition(unit->pDynamicPath), curY = PATH_GetYPosition(unit->pDynamicPath);
    if (std::abs(curX - static_cast<int>(x)) <= radius && std::abs(curY - static_cast<int>(y)) <= radius) {
        // Retail rule for a target inside the arrival radius: if the target shares a coordinate with the unit's
        // position, nothing happens. Otherwise two blocked-target tests run, the first on the unit's own two
        // record words and the second on the adjusted target; a hit on either keeps the path. With no hit the
        // target dispatch runs: a local player sends its own position back, another player takes a walk command,
        // any other unit takes a path command. Captured: a near target is not always kept (with a small radius
        // the check reaches both blocked-target tests and then the dispatch).
        // GUESS: the blocked-target test is a sprite-cell query this realm layer does not hold, so every near
        // target is treated as blocked here (keeps the path). Blocker: that sprite query, which needs the sprite
        // data this layer does not load; a fixture with the sprite data pinned settles it.
        return;
    }
    // Retail requires a room at the target.
    if (!d2_drlg_client_active_room(static_cast<int>(x), static_cast<int>(y))) return;
    if (isLocal) {
        // Local resync: retail sends the client's own position as a 5-byte packet 0x5F {x, y}
        // and does not move the unit.
        g_resyncPending = true;
        g_resyncX = static_cast<uint16_t>(curX);
        g_resyncY = static_cast<uint16_t>(curY);
        return;
    }
    if (isPlayer) {
        // Retail's walk dispatch for another player: for unit modes 2, 3 and 6 it first refreshes the path from
        // the unit's current path record (an absent record is fatal in retail), and it issues the walk command
        // whenever the target's y is nonzero, whatever the mode. Captured: for modes 2/3/6 the walk/run is
        // re-issued through the state-packet handler with an all-zero packet (one type for mode 3, another for
        // modes 2 and 6). Here the refresh is a detach and attach at the target keeping the facing.
        // GUESS: the walk command's effect (its move-kind dispatch and the path helpers it calls) is not ported,
        // so the re-issue is not modelled.
        player->view.x = x;
        player->view.y = y;
        ensure_player_motion(*player, true);
        return;
    }
    // Only a monster has a client motion to move; objects, missiles and items are not re-attached here.
    if (!monster) return;
    d2_realm_client_motion_detach(unit);
    d2_realm_client_motion_attach(unit, static_cast<int>(x), static_cast<int>(y));
}

// The resync the local player's far placement asks the page to send: packet 0x5F, x and y little-endian.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_take_resync(uint8_t* out) {
    if (!g_resyncPending || !out) return 0;
    g_resyncPending = false;
    out[0] = 0x5f;
    out[1] = static_cast<uint8_t>(g_resyncX); out[2] = static_cast<uint8_t>(g_resyncX >> 8);
    out[3] = static_cast<uint8_t>(g_resyncY); out[4] = static_cast<uint8_t>(g_resyncY >> 8);
    return 5;
}

// The request's target: a point, or for the unit-target handlers the (type, guid) of a unit.
static bool pkt_target(const uint32_t* l, int* x, int* y) {
    if (!g_pktRoute.unitTarget) {
        *x = static_cast<int>(pkt_word(l, 0));
        *y = static_cast<int>(pkt_word(l, 1));
        return true;
    }
    D2UnitStrc* target = find_realm_client_unit(static_cast<uint8_t>(pkt_word(l, 0)), pkt_word(l, 1));
    if (!target || !target->pDynamicPath) return false;
    *x = static_cast<int>(PATH_GetXPosition(target->pDynamicPath));
    *y = static_cast<int>(PATH_GetYPosition(target->pDynamicPath));
    return true;
}

static D2RealmClientPlayerEntry* player_entry_of(const D2UnitStrc* unit, bool* selected) {
    for (size_t i = 0; i < g_players.size(); ++i) {
        if (!unit || g_players[i].motionUnit != unit) continue;
        if (selected) *selected = static_cast<int>(i) == g_selectedPlayer;
        return &g_players[i];
    }
    return nullptr;
}

// Put a player's client unit on a cell at once (the placement the 0x15 reassignment also does).
static bool place_player_entry(D2RealmClientPlayerEntry& entry, bool selected, uint32_t x, uint32_t y) {
    D2RealmClientPlayerState& view = selected ? g_player : entry.view;
    view.x = x;
    view.y = y;
    refresh_player_room_level(view, selected);
    if (selected) entry.view = g_player;
    if (!ensure_player_motion(entry, true)) return false;
    if (selected) store_selected_player();
    return true;
}

// The hero stands up in town (see heroRevivesOnLife).
static void revive_hero_on_life(D2UnitStrc* unit, uint32_t life) {
    if (!unit || !d2::realm::heroRevivesOnLife(unit->dwAnimMode, life)) return;
    d2_realm_client_unit_set_mode(unit, d2::realm::kPlayerModeTownNeutral);
    unit->dwFlags &= ~d2::realm::kUnitFlagDead;
}

// 1.13c D2Client+0x19830 selectors 7, 8 and 9: a player dies, lies dead, or stands again.
// Returns -1 when the selector is not one of the three.
static int apply_player_life_action(D2UnitStrc* unit, uint32_t selector, int x, int y, uint32_t hitClass) {
    bool selected = false;
    D2RealmClientPlayerEntry* const entry = player_entry_of(unit, &selected);
    if (!entry) return -1;
    const bool wasDead = d2::realm::playerIsDead(unit->dwFlags, unit->dwAnimMode);
    const bool attached = unit->pDynamicPath != nullptr;
    const int32_t dx = attached ? x - static_cast<int>(PATH_GetXPosition(unit->pDynamicPath)) : 0;
    const int32_t dy = attached ? y - static_cast<int>(PATH_GetYPosition(unit->pDynamicPath)) : 0;
    const bool inTown = attached && DUNGEON_IsRoomInTown(UNITS_GetRoom(unit));
    d2::realm::PlayerLifeAction action{};
    if (!d2::realm::playerLifeAction(static_cast<uint8_t>(selector), selected, inTown, wasDead, dx, dy, &action))
        return -1;
    if (action.storeHitClass) unit->dwLastHitClass = hitClass;
    if (action.clearDead) unit->dwFlags = d2::realm::playerFlagsAfterRevive(unit->dwFlags);
    if (d2_realm_client_unit_set_mode(unit, static_cast<uint32_t>(action.mode)) < 0) return 0;
    if (action.markDead) unit->dwFlags = d2::realm::playerFlagsAfterDeath(unit->dwFlags);
    switch (action.move) {
    case d2::realm::PlayerLifeMove::Place:
        place_player_entry(*entry, selected, static_cast<uint32_t>(x), static_cast<uint32_t>(y));
        break;
    case d2::realm::PlayerLifeMove::Walk:
    case d2::realm::PlayerLifeMove::Run:
        if (attached) d2_realm_client_motion_apply_player_move(unit,
            action.move == d2::realm::PlayerLifeMove::Run ? 0x17 : 0x01, 0,
            static_cast<uint16_t>(x), static_cast<uint16_t>(y));
        break;
    default: break;
    }
    return 1;
}
static void emit_hit_overlay(D2UnitStrc* unit, bool struckNow = false);
static void record_unit_hit(D2UnitStrc* unit);

static int pkt_player_action(D2UnitStrc* unit, uint32_t code, const uint32_t* l) {
    if (code >= 7 && code <= 9) {
        const int result = apply_player_life_action(unit, code, static_cast<int>(pkt_word(l, 0)),
            static_cast<int>(pkt_word(l, 1)), pkt_word(l, 2));
        return result > 0 ? 1 : 0;
    }
    if (code == 1 || code == 0x17) {
        int x = 0, y = 0;
        if (!pkt_target(l, &x, &y)) return 0;
        return d2_realm_client_motion_apply_player_move(unit, static_cast<uint8_t>(code), 0,
            static_cast<uint16_t>(x), static_cast<uint16_t>(y)) > 0 ? 1 : 0;
    }
    if (code == 6) {
        unit->dwLastHitClass = pkt_word(l, 2);
        const uint32_t previous = unit->dwAnimMode;
        if (d2_realm_client_unit_set_mode(unit, PLRMODE_GETHIT) <= 0) return 0;
        if (previous != unit->dwAnimMode) emit_hit_overlay(unit);
        return 1;
    }
    if (code == d2::realm::kHitAction) {
        unit->dwLastHitClass = d2::realm::playerHitClass(pkt_word(l, 2));
        record_unit_hit(unit);
        return 1;
    }
    return 0;
}

static int pkt_monster_mode(D2RealmClientMonster& monster, uint32_t code, uint32_t mode) {
    D2UnitStrc* const unit = monster.motionUnit;
    const uint32_t previousMode = unit->dwAnimMode;
    if (d2_realm_client_unit_set_mode(unit, mode) < 0) return 0;
    if (previousMode != unit->dwAnimMode) emit_hit_overlay(unit);
    // The old 0x69/0x6A projection's event queue stays the view the page reads.
    if (g_pktOpcode != 0x69 && g_pktOpcode != 0x6a) return 1;
    if (!d2::realm::appendMonsterDeathModeEvent(g_monsterDeathModeEvents,
            g_nextMonsterDeathModeSequence, monster.view.guid, g_pktOpcode, code, previousMode, mode)) return 0;
    ++g_nextMonsterDeathModeSequence;
    return 1;
}

static int pkt_monster_action(D2RealmClientMonster& monster, uint32_t code, const uint32_t* l) {
    D2UnitStrc* const unit = monster.motionUnit;
    using d2::realm::MonsterAction;
    switch (d2::realm::monsterAction(code)) {
    case MonsterAction::Move: {
        int x = 0, y = 0;
        if (!unit->pDynamicPath || !pkt_target(l, &x, &y)) return 0;
        const int points = d2_realm_client_motion_set_target(unit, x, y);
        monster.lastPlanResult = points;
        if (points > 0) {
            d2_realm_client_motion_set_mode(unit, code == 1 ? MONMODE_WALK : MONMODE_RUN);
            return 1;
        }
        const uint32_t fallback = d2_realm_client_motion_refusal_mode(unit->dwUnitType, unit->dwAnimMode);
        if (fallback != unit->dwAnimMode) d2_realm_client_motion_set_mode(unit, fallback);
        return 0;
    }
    case MonsterAction::GetHit: {
        const uint32_t hit = d2::realm::getHitLife(pkt_word(l, 2));
        const uintptr_t bits = reinterpret_cast<uintptr_t>(unit);
        if (hit && (pkt_get_stat(bits, 6, 0) >> 8) != hit) pkt_set_stat(bits, 6, hit << 8, 0);
        unit->dwLastHitClass = pkt_word(l, 6);
        return pkt_monster_mode(monster, code, MONMODE_GETHIT);
    }
    case MonsterAction::Hit: {
        const auto hit = d2::realm::monsterHit(pkt_word(l, 0), pkt_word(l, 1));
        if (hit.setLife) pkt_set_stat(reinterpret_cast<uintptr_t>(unit), 6, hit.life << 8, 0);
        unit->dwLastHitClass = hit.hitClass;
        record_unit_hit(unit);
        return 1;
    }
    case MonsterAction::Death: {
        // A monster whose death spawns its minion is replaced by the spawn, not played out here.
        D2MonStatsTxt* stats = DATATBLS_GetMonStatsTxtRecord(static_cast<int>(monster.view.classId));
        const int minion = stats ? stats->wMinion[0] : -1;
        if (stats && stats->nSplEndDeath == 1 && minion >= 0 &&
            sgptDataTables && minion < sgptDataTables->nMonStatsTxtRecordCount) return 0;
        unit->dwLastHitClass = pkt_word(l, 6);
        pkt_set_stat(reinterpret_cast<uintptr_t>(unit), 6, 0, 0);
        return pkt_monster_mode(monster, code, MONMODE_DEATH);
    }
    case MonsterAction::Dead:
        // Izual's DEAD from his sequence mode is his own transition.
        if (monster.view.classId == 0x11c && unit->dwAnimMode == MONMODE_SEQUENCE) return 0;
        return pkt_monster_mode(monster, code, MONMODE_DEAD);
    case MonsterAction::Stop: {
        const uint32_t hp = pkt_word(l, 2);
        monster.view.hpPct = hp;
        if (hp) pkt_set_stat(reinterpret_cast<uintptr_t>(unit), 6, hp << 8, 0);
        const int x = static_cast<int>(pkt_word(l, 0)), y = static_cast<int>(pkt_word(l, 1));
        if (unit->pDynamicPath) {
            const int dx = std::abs(static_cast<int>(PATH_GetXPosition(unit->pDynamicPath)) - x);
            const int dy = std::abs(static_cast<int>(PATH_GetYPosition(unit->pDynamicPath)) - y);
            if ((dx > 2 || dy > 2) && d2_realm_client_motion_set_target(unit, x, y) > 0) {
                d2_realm_client_motion_set_mode(unit, MONMODE_WALK);
                return 1;
            }
        }
        if (unit->dwAnimMode != MONMODE_DEAD && unit->dwAnimMode != MONMODE_DEATH)
            d2_realm_client_unit_set_mode(unit, MONMODE_NEUTRAL);
        return 1;
    }
    case MonsterAction::AttackAt: {
        // Aim at the spot when it is set and is not the unit's own, then take the code's mode.
        if (!unit->pDynamicPath) return 0;
        PATH_ResetToPreviousType(unit->pDynamicPath);
        UNITS_SetUsedSkill(unit, nullptr);
        UNITS_SetTargetUnitForDynamicUnit(unit, nullptr);
        const int x = static_cast<int>(pkt_word(l, 0)), y = static_cast<int>(pkt_word(l, 1));
        if ((x || y) && (x != UNITS_GetXPosition(unit) || y != UNITS_GetYPosition(unit)))
            UNITS_UpdateDirectionAndSpeed(unit, x, y);
        unit->dwFlags &= ~0x40u;
        return pkt_monster_mode(monster, code, static_cast<uint32_t>(d2::realm::monsterActionMode(code)));
    }
    case MonsterAction::AttackOn: {
        // The target unit becomes the path's target and the monster faces it; an unknown target
        // clears the relation and the mode is still set.
        if (!unit->pDynamicPath) return 0;
        PATH_ResetToPreviousType(unit->pDynamicPath);
        UNITS_SetUsedSkill(unit, nullptr);
        D2UnitStrc* target = find_realm_client_unit(static_cast<uint8_t>(pkt_word(l, 0)), pkt_word(l, 1));
        if (target && (target == unit || !target->pDynamicPath)) target = nullptr;
        UNITS_SetTargetUnitForDynamicUnit(unit, target);
        if (target) UNITS_UpdateDirectionAndSpeed(unit, UNITS_GetXPosition(target), UNITS_GetYPosition(target));
        unit->dwFlags &= ~0x40u;
        return pkt_monster_mode(monster, code, static_cast<uint32_t>(d2::realm::monsterActionMode(code)));
    }
    case MonsterAction::Unhandled:
        break;
    }
    return 0;
}

// 0x2A750: ECX = action code, EDX = unit, the request block on the stack.
static void pkt_action(uint32_t code, uintptr_t unitBits, const uint32_t* l) {
    D2UnitStrc* unit = reinterpret_cast<D2UnitStrc*>(unitBits);
    code &= 0xff;
    g_pktResult = 0;
    if (!unit) return;
    if (unit->dwUnitType == UNIT_PLAYER) {
        g_pktResult = pkt_player_action(unit, code, l);
    } else if (unit->dwUnitType == UNIT_MONSTER && g_pktMonster && g_pktMonster->motionUnit == unit) {
        g_pktResult = pkt_monster_action(*g_pktMonster, code, l);
        refresh_monster_motion_view(*g_pktMonster);
    }
}

// The 0x0C handler takes the 0x2A750 call with the unit as a 32-bit register; the unit is the
// one the record named, held here.
static uint32_t pkt_action_af5f0(uint32_t code, uint32_t, uint32_t* l, uint32_t) {
    pkt_action(code, reinterpret_cast<uintptr_t>(g_pktUnit), l);
    return static_cast<uint32_t>(g_pktResult);
}

// One record: < 0 malformed, else the count of units it changed.
static int apply_retail_unit_packet(const uint8_t* packet, int length) {
    const d2::realm::PktRoute route = d2::realm::pktRoute(packet[0]);
    if (route.handler == d2::realm::PktHandler::None || length != route.length) return -1;
    uint32_t guid = 0;
    std::memcpy(&guid, packet + route.guidOffset, sizeof guid);
    D2RealmClientMonster* monster = nullptr;
    D2UnitStrc* unit = nullptr;
    const uint8_t unitType = route.typed ? packet[1] : static_cast<uint8_t>(UNIT_MONSTER);
    if (unitType == UNIT_MONSTER) {
        for (auto& candidate : g_monsters)
            if (candidate.view.guid == guid) { monster = &candidate; break; }
        if (!monster || !create_monster_motion(*monster)) return 0;
        unit = monster->motionUnit;
    } else {
        unit = find_realm_client_unit(unitType, guid);
    }
    if (!unit) return 0;
    g_pktEnv.f2A750 = pkt_action;
    g_pktEnv.f2A1B0 = pkt_place;
    g_pktEnv.getStat = pkt_get_stat;
    g_pktEnv.setStat = pkt_set_stat;
    g_pktRoute = route;
    g_pktMonster = monster;
    g_pktUnit = unit;
    g_pktOpcode = packet[0];
    g_pktResult = 0;
    const uintptr_t bits = reinterpret_cast<uintptr_t>(unit);
    using d2::realm::PktHandler;
    switch (route.handler) {
    case PktHandler::AF5F0: d2c_port_af5f0(0, packet, pkt_action_af5f0); break;
    case PktHandler::AF510: d2c_af510(&g_pktEnv, bits, packet); break;
    case PktHandler::AF4C0: d2c_af4c0(&g_pktEnv, bits, packet); break;
    case PktHandler::AF460: d2c_af460(&g_pktEnv, bits, packet); break;
    case PktHandler::AF3F0: d2c_af3f0(&g_pktEnv, bits, packet); break;
    case PktHandler::AF390: d2c_af390(&g_pktEnv, bits, packet); break;
    case PktHandler::AF340: d2c_af340(&g_pktEnv, bits, packet); break;
    case PktHandler::AF7C0: d2c_af7c0(&g_pktEnv, bits, packet); break;
    case PktHandler::AF750: d2c_af750(&g_pktEnv, bits, packet); break;
    case PktHandler::AF2B0: d2c_af2b0(&g_pktEnv, bits, packet); break;
    case PktHandler::None: break;
    }
    g_pktMonster = nullptr;
    g_pktUnit = nullptr;
    return g_pktResult;
}


// Objects the server assigns (0x51 reaches the creator at D2Client+0xA75C0). The record is what the creator is
// handed; its mode follows the object's client unit.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_object_count() { return static_cast<int>(g_objects.size()); }
EMSCRIPTEN_KEEPALIVE const D2RealmClientObjectView* d2_realm_client_objects() { return g_objects.data(); }

// The object units themselves, for the scene: one record of 14 words per unit, in the object table's order,
// {id, class, x, y, mode, 8.8 cel clock, 8.8 cel count, 8.8 step, flags (1 selectable, 2 in a revealed room),
// interact byte, overlay count, overlay rows x3 (-1 unused)}. Returns the count written.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_object_units(int32_t* out, int cap) {
    int n = 0;
    for (D2UnitStrc* unit = d2gs_client_object_next(nullptr); unit && out && n < cap; unit = d2gs_client_object_next(unit), ++n) {
        int32_t* r = out + n * 14;
        r[0] = static_cast<int32_t>(unit->dwUnitId); r[1] = unit->dwClassId;
        r[2] = UNITS_GetXPosition(unit); r[3] = UNITS_GetYPosition(unit);
        r[4] = static_cast<int32_t>(unit->dwAnimMode); r[5] = unit->nSeqCurrentFramePrecise;
        r[6] = static_cast<int32_t>(unit->dwFrameCountPrecise); r[7] = unit->wAnimSpeed;
        r[8] = ((unit->dwFlags & UNITFLAG_TARGETABLE) ? 1 : 0) | (UNITS_GetRoom(unit) ? 2 : 0);
        r[9] = unit->pObjectData->InteractType;
        r[11] = r[12] = r[13] = -1;
        r[10] = d2gs_client_object_overlays(unit, r + 11);
    }
    return n;
}
// Takes the pending object activations: {version, object id, player id, shrine kind, sound row, effect} each.
// The sound plays at the player. Effect 1 has turned the player's name around; 2..4 (the fire, exploding and
// poison shrines' missiles) are reported and not made: the realm missile maker creates skill projectiles only.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_take_object_events(int32_t* out, int cap) {
    if (!out || cap <= 0) return 0;
    const int count = std::min(cap, static_cast<int>(g_objectEvents.size()));
    for (int i = 0; i < count; ++i) {
        const auto& event = g_objectEvents[static_cast<size_t>(i)];
        int32_t* r = out + i * 6;
        r[0] = 1; r[1] = static_cast<int32_t>(event.objectGuid); r[2] = static_cast<int32_t>(event.playerGuid);
        r[3] = static_cast<int32_t>(event.shrine); r[4] = static_cast<int32_t>(event.sound); r[5] = static_cast<int32_t>(event.effect);
    }
    g_objectEvents.erase(g_objectEvents.begin(), g_objectEvents.begin() + count);
    return count;
}
// 0x0E and 0x4D aimed at an object. The activating player is looked up among the players this client knows.
static int apply_object_packet(const uint8_t* packet, int length) {
    D2gsClientObjectEvent event = {};
    const int result = d2gs_client_object_packet(packet, length,
        [](uint32_t guid) -> D2UnitStrc* { return find_realm_client_unit(UNIT_PLAYER, guid); }, &event);
    if (result == 1) {
        uint32_t guid = 0;
        std::memcpy(&guid, packet + 2, sizeof guid);
        sync_object_view(d2gs_client_object_find(guid));
    } else if (result == 2) {
        // The player unit here carries no name of its own: the name the client shows is the player record's.
        if (event.effect == kObjectEffectReverseName)
            for (auto& player : g_players)
                if (player.view.active && player.view.guid == event.playerGuid)
                    d2gs_client_object_reverse_name(reinterpret_cast<char*>(player.view.name));
        if (g_objectEvents.size() == 64) g_objectEvents.erase(g_objectEvents.begin());
        g_objectEvents.push_back(event);
    }
    return result < 0 ? -1 : result ? 1 : 0;
}

// 0x51: D2Client+0xADF40 hands its fields to the creator (0xA75C0).
static void object_create_cb(uint32_t kind, uint32_t guid, uint32_t classId, uint32_t x, uint32_t y,
        uint32_t, uint32_t mode, uint32_t interact) {
    if (kind == UNIT_OBJECT) {
        destroy_object_unit(guid);
        if (D2UnitStrc* unit = d2gs_client_object_create(guid, classId, mode, static_cast<uint8_t>(interact))) {
            d2gs_client_object_place(unit, d2_drlg_client_active_room(static_cast<int>(x), static_cast<int>(y)),
                static_cast<int>(x), static_cast<int>(y));
        }
    }
    for (auto& object : g_objects)
        if (object.unitType == kind && object.guid == guid) {
            object = { kind, guid, classId, x, y, mode, interact };
            return;
        }
    g_objects.push_back({ kind, guid, classId, x, y, mode, interact });
}

// 0x0A: D2Client+0xACA40 hands (row, bucket, guid, type) to the unit-table remove (0xA6EF0). The
// removal applies to the projection that owns the type.
static const uint8_t* g_removePacket = nullptr;
static int g_removeLength = 0;
static int g_removeResult = 0;
static void game_packet_unit_removed(uint32_t guid);
static void unit_remove_cb(uintptr_t, uint32_t, uint32_t guid, uint32_t type) {
    if (type == UNIT_PLAYER) {
        g_removeResult = remove_player(g_removePacket, g_removeLength) < 0 ? -2 : 0;
    } else if (type == UNIT_MONSTER) {
        if (!remove_monster(g_removePacket, g_removeLength)) g_removeResult = -4;
        else { g_removeResult = 1; game_packet_unit_removed(guid); }
    } else if (type == UNIT_OBJECT) {
        destroy_object_unit(guid);
        g_objects.erase(std::remove_if(g_objects.begin(), g_objects.end(), [guid](const auto& object) {
            return object.guid == guid; }), g_objects.end());
    } else if (type == UNIT_ITEM) {
        g_removeResult = d2_realm_client_item_remove(guid);
    } else if (type == UNIT_TILE) {
        d2_realm_client_warps_remove(g_warps, guid);
    }
}

// 0x0E and 0x17: the unit-queue handlers (D2Client+0xAF560 / +0xAF240) resolve the named unit and hand
// the request to the action dispatch. An object target takes its mode change (selector 3) and its activation
// (selector 0x15) on its own client unit.
// The 0x17 handler reads the u16 at the address that the word at +8 names, and the same for the word at +10; the two
// loaded u16s are the pair it passes on. So those words are client-memory addresses, not packet data: below 0x10000 the
// read is unmapped and the client faults. No server sender of 0x17 is in the client's table of well-formed packets.
// GUESS: 0 stands in for both reads; the native capture cannot run that branch.
static uint16_t unit_queue_rd16(uint32_t) { return 0; }
static int apply_unit_queue_packet(const uint8_t* packet, int length) {
    const bool is0e = packet[0] == 0x0e;
    if (length != 12) return -1;
    uint32_t guid = 0;
    std::memcpy(&guid, packet + 2, sizeof guid);
    D2RealmClientMonster* monster = nullptr;
    D2UnitStrc* unit = nullptr;
    if (packet[1] == UNIT_OBJECT) return is0e ? apply_object_packet(packet, length) : 0;
    if (packet[1] == UNIT_MONSTER) {
        for (auto& candidate : g_monsters)
            if (candidate.view.guid == guid) { monster = &candidate; break; }
        if (!monster || !create_monster_motion(*monster)) return 0;
        unit = monster->motionUnit;
    } else {
        unit = find_realm_client_unit(packet[1], guid);
    }
    if (!unit) return 0;
    g_pktEnv.f2A750 = pkt_action;
    g_pktRoute = {};
    g_pktMonster = monster;
    g_pktUnit = unit;
    g_pktOpcode = packet[0];
    g_pktResult = 0;
    if (is0e) {
        d2c_af560(&g_pktEnv, reinterpret_cast<uintptr_t>(unit), packet);
    } else {
        D2cPlayerEnv env{ &g_pktEnv, 0, unit_queue_rd16, [] {} };
        if (g_selectedPlayer >= 0 && size_t(g_selectedPlayer) < g_players.size())
            env.localPlayer = reinterpret_cast<uintptr_t>(g_players[g_selectedPlayer].motionUnit);
        d2c_af240(&env, reinterpret_cast<uintptr_t>(unit), packet);
    }
    g_pktMonster = nullptr;
    g_pktUnit = nullptr;
    return g_pktResult;
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
    if (stop.unitType == UNIT_PLAYER && stop.action >= 7 && stop.action <= 9) {
        D2UnitStrc* const player = find_realm_client_unit(stop.unitType, stop.guid);
        if (!player) return 0;
        return apply_player_life_action(player, stop.action, stop.x, stop.y, stop.hitClass) > 0 ? 1 : 0;
    }
    const int mode = d2::realm::playerStopModeFromAction(stop.action);
    if (mode < 0) return 0;
    D2UnitStrc* const unit = find_realm_client_unit(stop.unitType, stop.guid);
    if (!unit) return 0;
    // Selector 6 first writes the record's third field into the unit's last-hit-class slot
    // (D2Client+0x19830: mov [ebx+0xB0], the zero-extended packet byte at +11), then sets the mode.
    unit->dwLastHitClass = stop.hitClass;
    const uint32_t previous = unit->dwAnimMode;
    if (d2_realm_client_unit_set_mode(unit, static_cast<uint32_t>(mode)) < 0) return -1;
    if (previous != unit->dwAnimMode) emit_hit_overlay(unit);
    return 1;
}

// 0x9C and 0x9D: every item record goes to the one item runtime, which seats the item on the unit
// the record names (the selected player's own unit for 0x9C).
static int apply_item_record(const uint8_t* packet, int length) {
    D2UnitStrc* hero = g_selectedPlayer >= 0 && size_t(g_selectedPlayer) < g_players.size()
        ? g_players[g_selectedPlayer].motionUnit : nullptr;
    D2UnitStrc* owner = nullptr;
    if (packet[0] == 0x9d) {
        D2RealmOwnedItemPacket owned{};
        if (d2_realm_d2gs_parse_owned_item(packet, length, &owned) != length) return -1;
        if (owned.ownerType == UNIT_PLAYER) owner = find_realm_client_unit(owned.ownerType, owned.ownerGuid);
    }
    return d2_realm_client_item_packet(hero, owner, packet, length);
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
        if (update.hasStats &&
            !d2gs_client_apply_state_stats(unit, update.state, update.stats.data(), update.stats.size()))
            return -1;
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

// What a unit's skill start, hit or server record plays, for the page to sound and draw: the one
// queue of d2gs_client_unit_fx.h (its kinds are listed there).
static d2::realm::UnitFxQueue g_unitFxEvents;
// The one-shot overlays still running, so a start can tell that a unit already carries a row.
struct LiveUnitOverlay { uint32_t unitType, unitGuid, row, endTick; };
static std::vector<LiveUnitOverlay> g_unitOverlays;

static void record_unit_fx(uint32_t kind, D2UnitStrc* unit, uint32_t row, uint32_t delay, uint32_t skill) {
    g_unitFxEvents.push({1, g_clientMissileTick, kind, static_cast<uint32_t>(unit->dwUnitType),
        unit->dwUnitId, row, delay, skill});
}

static bool unit_carries_overlay(D2UnitStrc* unit, uint32_t row) {
    g_unitOverlays.erase(std::remove_if(g_unitOverlays.begin(), g_unitOverlays.end(),
        [](const LiveUnitOverlay& o) { return o.endTick <= g_clientMissileTick; }), g_unitOverlays.end());
    return std::any_of(g_unitOverlays.begin(), g_unitOverlays.end(), [unit, row](const LiveUnitOverlay& o) {
        return o.unitType == static_cast<uint32_t>(unit->dwUnitType) && o.unitGuid == unit->dwUnitId && o.row == row;
    });
}

// A one-shot overlay on a unit (1.13c D2Client+0x6B300). The caller has bounds-checked the row.
// NOT PORTED: the start's own bookkeeping for rows below 0x8C (+0x69660) is not read.
static void start_unit_overlay(D2UnitStrc* unit, int row, uint32_t skill) {
    auto* txt = DATATBLS_GetOverlayTxtRecord(row);
    if (!txt) return;
    record_unit_fx(2, unit, static_cast<uint32_t>(row), 0, skill);
    g_unitOverlays.push_back({static_cast<uint32_t>(unit->dwUnitType), unit->dwUnitId, static_cast<uint32_t>(row),
        d2::realm::overlayEndTick(g_clientMissileTick, txt->dwFrames, txt->dwAnimRate)});
}

// A unit that has just entered death, get hit or knockback plays the overlay its last hit class
// names (1.13c D2Client+0x2A590). The hit action names the knockback mode itself whatever the
// unit is doing (`struckNow`).
static void emit_hit_overlay(D2UnitStrc* unit, bool struckNow) {
    if (!unit) return;
    const uint32_t mode = unit->dwAnimMode;
    const bool struck = struckNow || (unit->dwUnitType == UNIT_PLAYER
        ? mode == PLRMODE_DEATH || mode == PLRMODE_GETHIT || mode == PLRMODE_KNOCKBACK
        : unit->dwUnitType == UNIT_MONSTER &&
            (mode == MONMODE_DEATH || mode == MONMODE_GETHIT || mode == MONMODE_KNOCKBACK));
    if (!struck) return;
    const int row = d2::realm::hitOverlayRow(unit->dwLastHitClass);
    if (row <= 0 || row >= sgptDataTables->nOverlayTxtRecordCount) return;
    start_unit_overlay(unit, row, d2::realm::kUnitFxSkillHit);
}

// The hit action (0x13) of the unit request: the unit keeps its mode, plays its hit class's
// overlay and is reported struck so the page sounds the impact.
// NOT PORTED: the monster's blood splatter (+0x4C500), its flag bit from the life byte's top bit
// and the player's placement at the record's two bytes.
static void record_unit_hit(D2UnitStrc* unit) {
    emit_hit_overlay(unit, true);
    record_unit_fx(d2::realm::kUnitFxHit, unit, unit->dwLastHitClass, 0, d2::realm::kUnitFxSkillHit);
}

// Consumes the queued unit effects, eight words each:
// {version, tick, kind, unitType, unitGuid, row, delay, skill}.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_take_unit_fx(int32_t* out, int cap) {
    return g_unitFxEvents.take(out, cap);
}

// A unit's life, maximum life (both in 1/256) and last hit class into out[0..2]; 0 when the
// client holds no such unit.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_unit_life(int type, uint32_t guid, uint32_t* out) {
    if (!out || (type != UNIT_PLAYER && type != UNIT_MONSTER)) return 0;
    auto* unit = find_realm_client_unit(static_cast<uint8_t>(type), guid);
    if (!unit || !unit->pStatListEx) return 0;
    out[0] = static_cast<uint32_t>(STATLIST_UnitGetStatValue(unit, 6, 0));
    out[1] = static_cast<uint32_t>(STATLIST_UnitGetStatValue(unit, 7, 0));
    out[2] = unit->dwLastHitClass;
    return 1;
}

EMSCRIPTEN_KEEPALIVE uint32_t d2_realm_client_unit_fx_tick() { return g_clientMissileTick; }

// The hit class a unit was last struck with (the server's hit records set it), or 0.
EMSCRIPTEN_KEEPALIVE uint32_t d2_realm_client_unit_hit_class(int type, uint32_t guid) {
    if (type != UNIT_PLAYER && type != UNIT_MONSTER) return 0;
    auto* unit = find_realm_client_unit(static_cast<uint8_t>(type), guid);
    return unit ? unit->dwLastHitClass : 0;
}
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
    g_unitFxEvents.clear();
    g_unitOverlays.clear();
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

// The client's per-frame player update (1.13c D2Client+0x1C150) reads a per-mode table of
// (class, frame kind, end kind). These are the modes whose end kind is 2 or 3: get hit, both
// attacks, block, cast, throw, kick and the four skill modes. Each ends itself when its frame
// cycle ends; the client does not wait for the server. Sequence (18) has the same end kind; its
// frames advance through the sequence the used skill started.
static bool player_mode_ends_itself(uint32_t mode) {
    switch (mode) {
    case PLRMODE_GETHIT: case PLRMODE_ATTACK1: case PLRMODE_ATTACK2: case PLRMODE_BLOCK:
    case PLRMODE_CAST: case PLRMODE_THROW: case PLRMODE_KICK: case PLRMODE_SPECIAL1:
    case PLRMODE_SPECIAL2: case PLRMODE_SPECIAL3: case PLRMODE_SPECIAL4: case PLRMODE_SEQUENCE: return true;
    default: return false;
    }
}

// One frame of a self-ending player mode: at the action frame the used skill's "do" runs once
// (flag 0x40 marks it done), the frame advances without wrapping, and at the end of the cycle the
// used skill is dropped, the path returns to its previous type and the unit goes back to neutral.
// PARTIAL: the "do" step is the client missile of a plain projectile skill only; the other
// skills' client do-functions (the Skills row's cltdofunc table) are not run here.
static bool advance_client_player_action(D2UnitStrc* unit) {
    if (!unit || unit->dwUnitType != UNIT_PLAYER || !player_mode_ends_itself(unit->dwAnimMode)) return false;
    auto* skill = UNITS_GetUsedSkill(unit);
    if (!(unit->dwFlags & 0x40u) && unit->nActionFrame >= 1 && unit->nActionFrame <= 3) {
        if (skill && skill->pSkillsTxt && !(skill->dwFlags & 1u) && unit->dwAnimMode == PLRMODE_CAST &&
                is_natural_client_projectile(skill->pSkillsTxt->nSkillId)) {
            record_client_missile_event(7, unit);
            create_client_projectile(unit, skill);
        }
        unit->dwFlags |= 0x40u; // Marked even when nothing was emitted.
    }
    // A sequence is never restarted here: its count running out is its end.
    if (!unit->dwFrameCountPrecise && unit->dwAnimMode != PLRMODE_SEQUENCE) UNITS_SetAnimStartFrame(unit);
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

// The mode change of an action (1.13c D2Client+0x2A6C0): the "do" mark is cleared and, when the
// mode really changes, the struck overlay is considered. The mode's own sound is the page's, keyed
// on the mode change it reads back.
static void client_set_mode(D2UnitStrc* unit, int mode) {
    unit->dwFlags &= ~0x40u;
    const uint32_t previous = unit->dwAnimMode;
    if (unit->dwUnitType == UNIT_MONSTER) d2_realm_client_unit_set_mode(unit, static_cast<uint32_t>(mode));
    else UNITS_ChangeAnimMode(unit, mode);
    if (previous != unit->dwAnimMode) emit_hit_overlay(unit);
}

// An action that could not start leaves the unit standing (1.13c D2Client+0x19710 for a player,
// the stop action for a monster). The dead stay as they are.
static void client_stop_unit(D2UnitStrc* unit) {
    unit->dwFlags &= ~0x40u;
    if (unit->dwUnitType == UNIT_PLAYER) {
        if (unit->dwAnimMode == PLRMODE_DEATH || unit->dwAnimMode == PLRMODE_DEAD) return;
        auto* room = UNITS_GetRoom(unit);
        client_set_mode(unit, room && DUNGEON_IsRoomInTown(room) ? PLRMODE_TOWNNEUTRAL : PLRMODE_NEUTRAL);
    } else if (unit->dwUnitType == UNIT_MONSTER) {
        if (unit->dwAnimMode != MONMODE_DEATH && unit->dwAnimMode != MONMODE_DEAD) client_set_mode(unit, MONMODE_NEUTRAL);
    }
}

// The hit class of a unit's own swing: the weapon in a player's hand (0 unarmed), a monster's
// own from its MonStats2 row.
static uint32_t client_weapon_hit_class(D2UnitStrc* unit) {
    if (unit->dwUnitType == UNIT_PLAYER) {
        auto* weapon = unit->pInventory ? D2Common_10434(unit, FALSE) : nullptr;
        return weapon ? ITEMS_GetHitClassFromItem(weapon) : 0;
    }
    if (unit->dwUnitType == UNIT_MONSTER) {
        auto* stats2 = UNITS_GetMonStats2TxtRecordFromMonsterId(unit->dwClassId);
        return stats2 ? stats2->nHitClass : 0;
    }
    return 0;
}

// The previous action is completed before a new one starts (1.13c D2Client+0x197D0): a used
// skill that has not done its "do" yet does it now, and the used skill is dropped.
// PARTIAL: the "do" is the client missile of a plain projectile skill only.
static void complete_previous_action(D2UnitStrc* unit) {
    auto* previous = UNITS_GetUsedSkill(unit);
    if (!previous) return;
    const int flags = SKILLS_GetFlags(previous);
    if (unit->dwUnitType == UNIT_PLAYER &&
            is_natural_client_projectile(SKILLS_GetSkillIdFromSkill(previous, __FILE__, __LINE__)) &&
            unit->dwAnimMode == PLRMODE_CAST && ((flags & 1) || !(unit->dwFlags & 0x40u))) {
        if (flags & 1) SKILLS_SetFlags(previous, flags | 2);
        create_client_projectile(unit, previous);
    }
    UNITS_SetUsedSkill(unit, nullptr);
}

// One skill start for every unit: the selected player's own click, another player's and a
// monster's from the server (1.13c D2Client+0xA22B0 at a spot, +0xA30C0 on a unit, both ending
// in +0xA1CF0). The caller has completed the previous action and reset the path.
struct ClientSkillStart {
    int id = 0;
    int level = 0;                 // the server's; unused for the selected player
    D2SkillStrc* skill = nullptr;  // the selected player's own skill of the hand
    D2UnitStrc* target = nullptr;
    int x = 0, y = 0;
    bool onUnit = false;
    bool local = false;
};
// 1 started, 0 not.
//  - aim: a spot becomes the path's target and the unit turns to it; a unit becomes the path's
//    target unit and is faced. A start on a unit that is not there stops the unit;
//  - a dead monster is a target only for a skill that takes corpses, when the skill searches
//    for its enemy by position;
//  - another unit's skill is assigned at the level the server sent and becomes its used skill;
//  - in town a skill without the InTown flag drops the used skill and stops the unit;
//  - the mode is the skill's own (a player's plain Attack takes Attack 1); a skill without a
//    mode leaves a player or monster in mode 1. A Sequence mode starts the skill's sequence;
//  - the start's sounds and overlay follow the Skills row (client_skill_fx.h).
// GUESS (level clamp): retail clamps another unit's level through +0x17B0, not read; the server's
// level is taken as sent. Settled by decompiling +0x17B0.
// GUESS (use state): the use-state test (mana, charges, quantity) is applied to the selected
// player only; this client does not hold other units' mana. Settled by a capture of +0xA1CF0 on
// a remote unit with zero mana.
// GUESS (repeat): the "already repeating" test that lets a Repeat skill keep its mode is taken as
// failing, so the mode is always set. Settled by decompiling D2Common ordinal 10494's use here.
// GUESS (second attack): retail's +0xA1260 picks Attack 2 for a player's plain Attack from the
// weapons in hand and a 50% roll on the unit's seed; every swing here is Attack 1. Settled by a
// function capture of +0xA1260 over one-handed, two-handed and dual-wield inventories.
// NOT PORTED: the line-of-sight test of rows with a range class, the gate +0xA5B90, the
// pre-aim call +0xA0660 and the 53 client start functions (the row's cltstfunc; each is taken as
// succeeding, so a start that retail would refuse there still plays its sounds). The client
// missile (+0xA0F60) is created at the action frame for plain projectile skills only.
static int client_skill_start(D2UnitStrc* unit, const ClientSkillStart& s) {
    if (!unit || !unit->pDynamicPath) return 0;
    if (s.onUnit && !s.target) {
        client_stop_unit(unit);
        return 0;
    }
    if (!d2::realm::applyCastTargetRelation(s.onUnit ? 0x4c : 0x4d, s.target, s.x, s.y,
            [unit](int tx, int ty) { D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, tx, ty); },
            [unit](D2UnitStrc* relation) { UNITS_SetTargetUnitForDynamicUnit(unit, relation); })) return 0;
    if (s.x != UNITS_GetXPosition(unit) || s.y != UNITS_GetYPosition(unit)) UNITS_UpdateDirectionAndSpeed(unit, s.x, s.y);

    auto* txt = DATATBLS_GetSkillsTxtRecord(s.id);
    if (!txt) return 0;
    const uint32_t rowFlags = txt->dwFlags[0];
    if (s.target && s.target->dwUnitType == UNIT_MONSTER && s.target->dwAnimMode == MONMODE_DEAD &&
        (rowFlags & static_cast<uint32_t>(SKILLSFLAG_SEARCHENEMYXY)) &&
        !(rowFlags & static_cast<uint32_t>(SKILLSFLAG_TARGETCORPSE))) return 0;

    D2SkillStrc* skill = s.skill;
    if (!s.local) {
        if (!unit->pSkills) unit->pSkills = SKILLS_AllocSkillList(nullptr);
        if (!unit->pSkills) return 0;
        SKILLS_AssignSkill(unit, s.id, s.level, FALSE, __FILE__, __LINE__);
        skill = SKILLS_GetSkillById(unit, s.id, -1);
    }
    if (!skill || !skill->pSkillsTxt) return 0;
    if (s.local && SKILLS_GetUseState(unit, skill) != 0) return 0;
    UNITS_SetUsedSkill(unit, skill);

    auto* room = UNITS_GetRoom(unit);
    if (!(rowFlags & static_cast<uint32_t>(SKILLSFLAG_INTOWN)) && room && DUNGEON_IsRoomInTown(room)) {
        UNITS_SetUsedSkill(unit, nullptr);
        client_stop_unit(unit);
        return 0;
    }

    const bool isPlayer = unit->dwUnitType == UNIT_PLAYER;
    const int mode = isPlayer && s.id == 0 ? PLRMODE_ATTACK1 : SKILLS_GetSkillModeFromUnit(unit, skill);
    if (mode != 0 || isPlayer || unit->dwUnitType == UNIT_MONSTER) client_set_mode(unit, mode ? mode : 1);

    d2::realm::SkillFxRow row;
    row.stSoundDelay = (rowFlags & static_cast<uint32_t>(SKILLSFLAG_STSOUNDDELAY)) != 0;
    row.weaponSound = (rowFlags & static_cast<uint32_t>(SKILLSFLAG_WEAPONSND)) != 0;
    row.stSound = static_cast<int16_t>(txt->wStSound);
    row.stSoundClass = static_cast<int16_t>(txt->nStSoundClass);
    row.castOverlay = static_cast<int16_t>(txt->wCastOverlay);
    row.itemCastSound = static_cast<int16_t>(txt->wItemCastSound);
    row.itemCastOverlay = static_cast<int16_t>(txt->wItemCastOverlay);
    row.charClass = static_cast<int8_t>(txt->nCharClass);
    d2::realm::SkillFxUnit actor;
    actor.isPlayer = isPlayer;
    actor.classId = unit->dwClassId;
    actor.hitClass = client_weapon_hit_class(unit);
    actor.animSpeed = static_cast<uint32_t>(static_cast<int32_t>(unit->wAnimSpeed));
    const auto plan = d2::realm::skillStartFx(row, actor, skill->nOwnerGUID != -1,
        sgptDataTables->nOverlayTxtRecordCount);
    for (int i = 0; i < plan.sounds; ++i)
        record_unit_fx(static_cast<uint32_t>(plan.sound[i].kind), unit, static_cast<uint32_t>(plan.sound[i].row),
            static_cast<uint32_t>(plan.sound[i].delay), static_cast<uint32_t>(s.id));
    if (plan.overlay > 0 && !unit_carries_overlay(unit, static_cast<uint32_t>(plan.overlay)))
        start_unit_overlay(unit, plan.overlay, static_cast<uint32_t>(s.id));
    return 1;
}

static D2UnitStrc* selected_player_unit() {
    if (g_selectedPlayer < 0 || size_t(g_selectedPlayer) >= g_players.size()) return nullptr;
    auto* unit = g_players[g_selectedPlayer].motionUnit;
    return unit && unit->pDynamicPath && UNITS_GetRoom(unit) ? unit : nullptr;
}

// The selected player starts a skill itself before the packet is sent; the packet goes out only
// when this start succeeded (1.13c D2Client+0x2A7C0: gate, local action, then send). Any skill of
// either hand takes this path: at a spot (client action 0x15) or on a unit (0x16). 1 started, 0 refused.
//  - gate (+0x29F30): a used skill whose row lacks Interrupt refuses a new action unless the
//    unit stands in a neutral mode;
//  - the previous action is completed first, then the start is the one every unit takes
//    (client_skill_start).
// GUESS (order of the use-state test): it is made here before the unit is aimed, so a skill the
// player cannot pay for leaves him as he is; retail makes it after the aim. Settled by a capture
// of +0x19830 with action 0x15 and zero mana.
static int local_skill_start(D2UnitStrc* unit, D2SkillStrc* skill, D2UnitStrc* target, int x, int y) {
    if (!unit || !skill || !skill->pSkillsTxt) return 0;
    if (unit->dwAnimMode == PLRMODE_DEATH || unit->dwAnimMode == PLRMODE_DEAD) return 0;
    const bool neutral = unit->dwAnimMode == PLRMODE_NEUTRAL || unit->dwAnimMode == PLRMODE_TOWNNEUTRAL;
    auto* previous = UNITS_GetUsedSkill(unit);
    if (previous && previous->pSkillsTxt && !neutral &&
        !(previous->pSkillsTxt->dwFlags[0] & static_cast<uint32_t>(SKILLSFLAG_INTERRUPT))) return 0;
    complete_previous_action(unit);
    if (SKILLS_GetUseState(unit, skill) != 0) return 0;
    PATH_ResetToPreviousType(unit->pDynamicPath);
    ClientSkillStart start;
    start.id = SKILLS_GetSkillIdFromSkill(skill, __FILE__, __LINE__);
    start.skill = skill;
    start.target = target;
    start.x = x;
    start.y = y;
    start.onUnit = target != nullptr;
    start.local = true;
    return client_skill_start(unit, start);
}

static D2SkillStrc* hand_skill(D2UnitStrc* unit, int hand) {
    return d2_realm_client_hand_skill(unit, hand, PlayerSkillOps{});
}

// The left (hand 0) or right (hand 1) skill at a spot. 1 started, 0 refused.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_local_skill_at(int hand, int x, int y) {
    auto* unit = selected_player_unit();
    if (!unit || x < 0 || y < 0 || x > 65535 || y > 65535) return 0;
    return local_skill_start(unit, hand_skill(unit, hand), nullptr, x, y);
}

// The left or right skill on a monster or player. 1 started, 0 refused (also a dying or dead
// target, which the click treats as ground).
EMSCRIPTEN_KEEPALIVE int d2_realm_client_local_skill_on_unit(int hand, int type, uint32_t guid) {
    auto* unit = selected_player_unit();
    if (!unit || (type != UNIT_MONSTER && type != UNIT_PLAYER)) return 0;
    auto* target = find_realm_client_unit(static_cast<uint8_t>(type), guid);
    if (!target || target == unit || !target->pDynamicPath || !UNITS_GetRoom(target)) return 0;
    if (type == UNIT_MONSTER && (target->dwAnimMode == MONMODE_DEATH || target->dwAnimMode == MONMODE_DEAD)) return 0;
    return local_skill_start(unit, hand_skill(unit, hand), target,
        UNITS_GetXPosition(target), UNITS_GetYPosition(target));
}

// What a click with a hand's skill does (1.13c D2Client+0x1B7F0 and, for a monster, +0x1B700).
// The skill's range class comes from its Skills row through the engine (none, hand to hand,
// ranged, either by the weapon in hand, or 4).
//  - on a unit: 0 = walk to it first, when the class is hand to hand or 4 and the unit is out of
//    melee range (the engine's size-aware test with the "target is moving" bonus); the skill is
//    then used on arrival. 1 = use the skill on the unit now.
//  - on the ground (type < 0): 2 = the right button with a hand to hand skill walks there
//    instead; 1 = use the skill at the spot.
// -1 when there is no local player, skill or target unit.
// NOT PORTED: a Skills row whose word at +0x2E is 0x13 takes its own branch (+0x1B1D0) when out
// of range; here it follows the rule above.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_skill_plan(int hand, int type, uint32_t guid) {
    auto* unit = selected_player_unit();
    auto* skill = unit ? hand_skill(unit, hand) : nullptr;
    if (!skill) return -1;
    const int kind = D2Common_11016(unit, skill);
    if (type < 0) return hand && kind == 1 ? 2 : 1;
    if (type != UNIT_MONSTER && type != UNIT_PLAYER) return -1;
    auto* target = find_realm_client_unit(static_cast<uint8_t>(type), guid);
    if (!target || !target->pDynamicPath || !UNITS_GetRoom(target)) return -1;
    if (kind != 1 && kind != 4) return 1;
    return UNITS_IsInMeleeRange(unit, target, UNITS_IsInMovingMode(target)) ? 1 : 0;
}

// The right skill on a monster, kept for callers of the older entry.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_player_predict_right_cast_on_monster(uint32_t guid) {
    return d2_realm_client_local_skill_on_unit(1, UNIT_MONSTER, guid);
}

// The local player's ground click starts its own route at once. The game server never echoes the
// owner's walk/run (0x0F/0x10 go only to the other clients; the owner is corrected by 0x15), so a
// client that waits for the echo never moves. Same target/route branch as a remote 0x0F:
// action 0x01 walk, 0x17 run. Returns 1 when a route started, 0 when refused or no local unit.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_local_move(int x, int y, int run) {
    if (g_selectedPlayer < 0 || size_t(g_selectedPlayer) >= g_players.size()) return 0;
    auto* unit = g_players[g_selectedPlayer].motionUnit;
    if (!unit || !unit->pDynamicPath || !UNITS_GetRoom(unit)) return 0;
    if (unit->dwAnimMode == PLRMODE_DEATH || unit->dwAnimMode == PLRMODE_DEAD) return 0;
    return d2_realm_client_motion_apply_player_move(unit, run ? 0x17 : 0x01, 0,
        static_cast<uint16_t>(x), static_cast<uint16_t>(y)) > 0 ? 1 : 0;
}

// Retail's far branch of a click on a unit issues a walk-to-unit command and sends the interact
// packet; the pursuit itself is not ported, so this routes to the unit's current position through the ground-walk path. A click on a unit starts the same route a ground click does, aimed at the unit's own
// position (the server's walk-to-entity pursues the target; the local route only has to start
// at once). 1 started, 0 refused, -1 when the unit is not one the client keeps a CUnit for.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_local_move_to_unit(int type, uint32_t guid, int run) {
    if (g_selectedPlayer < 0 || size_t(g_selectedPlayer) >= g_players.size()) return -1;
    auto* unit = g_players[g_selectedPlayer].motionUnit;
    if (!unit || !unit->pDynamicPath || !UNITS_GetRoom(unit)) return -1;
    if (type == UNIT_OBJECT) {
        // GUESS: the route is aimed at the object's own cell, which its footprint may block; a refused plan
        // reports "no client unit" so the walk request still goes out and the server's reply moves the hero.
        // What settles it: the client's walk-to-unit pursuit for an object target.
        D2UnitStrc* object = find_object_unit(guid);
        if (!object || !UNITS_GetRoom(object)) return -1;
        if (unit->dwAnimMode == PLRMODE_DEATH || unit->dwAnimMode == PLRMODE_DEAD) return 0;
        return d2_realm_client_motion_apply_player_move(unit, run ? 0x17 : 0x01, 0,
            static_cast<uint16_t>(UNITS_GetXPosition(object)), static_cast<uint16_t>(UNITS_GetYPosition(object))) > 0 ? 1 : -1;
    }
    auto* target = find_realm_client_unit(static_cast<uint8_t>(type), guid);
    if (!target || !target->pDynamicPath) return -1;
    if (unit->dwAnimMode == PLRMODE_DEATH || unit->dwAnimMode == PLRMODE_DEAD) return 0;
    return d2_realm_client_motion_apply_player_move(unit, run ? 0x17 : 0x01, 0,
        static_cast<uint16_t>(PATH_GetXPosition(target->pDynamicPath)),
        static_cast<uint16_t>(PATH_GetYPosition(target->pDynamicPath))) > 0 ? 1 : 0;
}

// Whether the selected player can use a unit now. Retail's "walk first" test (1.13c
// D2Client+0x29E80) for a monster or player is the engine's melee-range test with the "target is
// moving" bonus: in range when the distance is under 1, or within the player's melee range plus
// one plus the bonus with nothing blocking the way; a tentacle monster also passes at plus 8.
// Objects: retail's test has no melee rule for an object. It is usable now only when the footprint
// reach (D2Common #10594) is nonzero and the collision test between the two (D2Common #10324, mask
// 0x804) is zero; the object table's operate-range column is not read.
// GUESS (items, tiles): they take the melee rule above. The realm keeps no CUnit for them, so they
// stay -1 and the page falls back to its tile distance; that fallback is not retail.
// Settled by a function capture of +0x29E80 over item and tile targets with the melee inputs pinned.
// -1 for a unit the client keeps no CUnit for (ground items, an object outside the active rooms) or no local player.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_can_interact(int type, uint32_t guid) {
    if (g_selectedPlayer < 0 || size_t(g_selectedPlayer) >= g_players.size()) return -1;
    auto* unit = g_players[g_selectedPlayer].motionUnit;
    if (!unit || !unit->pDynamicPath) return -1;
    if (type == UNIT_OBJECT) {
        D2UnitStrc* object = find_object_unit(guid);
        if (!object || !UNITS_GetRoom(object) || !UNITS_GetRoom(unit)) return -1;
        return UNITS_IsObjectInInteractRange(unit, object) &&
            !UNITS_TestCollisionBetweenInteractingUnits(unit, object, 0x804) ? 1 : 0;
    }
    if (type != UNIT_MONSTER && type != UNIT_PLAYER) return -1;
    auto* target = find_realm_client_unit(static_cast<uint8_t>(type), guid);
    if (!target || !target->pDynamicPath) return -1;
    return UNITS_IsInMeleeRange(unit, target, UNITS_IsInMovingMode(target)) ? 1 : 0;
}

// 1 once the selected player has ended the action it was walking or casting (back in a
// neutral mode, or dead); 0 while that action still runs; -1 with no local unit.
// GUESS (pending-action words): retail keeps the pending action in two D2Client words. 0x11C2F4 is the
// pending target record: 0x51860 sets it to the clicked unit (or 0 to cancel) and records its type at
// 0x11964C and its guid at 0x119638; 0x51AE0 and 0x51D10 clear it when the path resolves. 0x11C2F8 is a
// busy latch: the refusal paths 0x2AE30, 0x2AEA0 and 0x2AFB0 set it (and clear 0x11C2F4), and 0x51860,
// 0x51E80, 0x51AE0, 0x51D10 do nothing while it is set. Here "done" is the anim mode, which is not that
// state machine; the realm does not read or write either word.
// capture: d2client/51ae0 <a walk to a monster, a stop, and a cast; 0x11C2F4 and 0x11C2F8 and the two
// pending-record words recorded per frame>.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_local_action_done() {
    if (g_selectedPlayer < 0 || size_t(g_selectedPlayer) >= g_players.size()) return -1;
    auto* unit = g_players[g_selectedPlayer].motionUnit;
    if (!unit) return -1;
    switch (unit->dwAnimMode) {
    case PLRMODE_NEUTRAL: case PLRMODE_TOWNNEUTRAL: case PLRMODE_DEATH: case PLRMODE_DEAD: return 1;
    default: return 0;
    }
}

// The server starts a skill on a unit: 0x4C on a target unit, 0x4D at a spot (1.13c
// D2Client+0xAF1D0 and +0xAF160, client actions 0x16 and 0x15). A player and a monster both take
// the start the selected player takes; the selected player has already started his own.
// A dead caster is left alone (a safety of this port, not read from retail).
// NOT PORTED: a monster whose skill mode is Sequence runs +0x4D5B0 first, not read.
static int apply_skill_start_packet(const uint8_t* packet, int length) {
    d2::realm::SkillStartRequest request;
    if (!d2::realm::decodeSkillStart(packet, length, &request)) return -1;
    ClientSkillStart start;
    start.id = static_cast<int>(request.skill & 0xffffu);
    start.level = request.level;
    start.onUnit = request.onUnit;
    start.x = request.x;
    start.y = request.y;
    if (request.onUnit) {
        auto* target = find_realm_client_unit(request.targetType, request.targetGuid);
        if (target && target->pDynamicPath && UNITS_GetRoom(target)) {
            start.target = target;
            start.x = UNITS_GetXPosition(target);
            start.y = UNITS_GetYPosition(target);
        }
    }
    if (request.unitType == UNIT_PLAYER) {
        const auto it = std::find_if(g_players.begin(), g_players.end(), [&request](const auto& player) {
            return player.view.guid == request.guid;
        });
        if (it == g_players.end() || static_cast<int>(it - g_players.begin()) == g_selectedPlayer) return 0;
        D2UnitStrc* unit = it->motionUnit;
        if (!unit || !unit->pDynamicPath || !UNITS_GetRoom(unit)) return 0;
        if (unit->dwAnimMode == PLRMODE_DEATH || unit->dwAnimMode == PLRMODE_DEAD) return 0;
        if (start.target == unit) start.target = nullptr;
        complete_previous_action(unit);
        PATH_ResetToPreviousType(unit->pDynamicPath);
        if (client_skill_start(unit, start)) return 1;
        client_stop_unit(unit);
        return 0;
    }
    if (request.unitType == UNIT_MONSTER) {
        const auto it = std::find_if(g_monsters.begin(), g_monsters.end(), [&request](const auto& monster) {
            return monster.view.guid == request.guid;
        });
        if (it == g_monsters.end()) return 0;
        D2UnitStrc* unit = it->motionUnit;
        if (!unit || !unit->pDynamicPath || !UNITS_GetRoom(unit)) return 0;
        if (unit->dwAnimMode == MONMODE_DEATH || unit->dwAnimMode == MONMODE_DEAD) return 0;
        if (start.target == unit) start.target = nullptr;
        PATH_ResetToPreviousType(unit->pDynamicPath);
        const int started = client_skill_start(unit, start);
        refresh_monster_motion_view(*it);
        return started;
    }
    return 0;
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

// The right skill at a spot, kept for callers of the older entry.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_player_predict_right_cast(int x, int y) {
    return d2_realm_client_local_skill_at(1, x, y);
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

// 1.13c D2Client+0xAEDF0 (0x19..0x1F) and +0xAED80 (0x20). 0x19 adds the byte to gold (stat 14); 0x1A and
// 0x1B add a byte or word to experience (stat 13); 0x1C sets experience to the dword; 0x1D..0x1F set the
// stat named by byte 1 to a byte, word or dword at byte 2. All act on the current player. 0x20 sets a
// stat on the player unit whose GUID it names. Values are in the stat's wire units.
// Retail effect of a missing hero: the stat packet logs a fixed message and exits the client, so nothing is
// written and no stat-change event runs. Here a missing selected record is a negative return, which rejects the
// stream and clears the realm state (the nearest equivalent). Open difference: retail keys this on the hero
// unit being null; a selected record with a null motion unit is the same condition (its unit is made when the
// player is assigned) and is rejected the same way (-1). The 0x20 form on a GUID without a record is a no-op.
// Differs from retail: the stat-change event that follows each 0x19..0x1F set (stat id and new value) is not run
// here. Its effects are client-side only and never write the stat list: stats 0 and 2 re-check the equipped
// items; stat 12 (level) re-checks the owner's equipped items and, for the hero, its inventory and equipped items
// again; hit points at a nonzero value on a dead unit run two dead-unit callees. The browser runs them through
// d2_client_stat_update_actions (play/assets/stat-update-watch.mjs), which the test
// test/d2client-aeb10-stat-update.test.mjs pins to the 1.13c switch.
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
        static const int8_t kSize[] = { 2, 2, 3, 5, 3, 4, 6 };   // 0x19..0x1F
        if (packet[0] < 0x19 || packet[0] > 0x1f || length != kSize[packet[0] - 0x19]) return -1;
        if (g_selectedPlayer < 0 || size_t(g_selectedPlayer) >= g_players.size()) return -1;
        unit = g_players[g_selectedPlayer].motionUnit;
        if (!unit) return -1;   // retail: a null hero is fatal, handled like a missing selected record
        switch (packet[0]) {
        case 0x19: stat = 14; value = STATLIST_UnitGetStatValue(unit, 14, 0) + read(packet + 1, 1); break;
        case 0x1a: stat = 13; value = STATLIST_UnitGetStatValue(unit, 13, 0) + read(packet + 1, 1); break;
        case 0x1b: stat = 13; value = STATLIST_UnitGetStatValue(unit, 13, 0) + read(packet + 1, 2); break;
        case 0x1c: stat = 13; value = read(packet + 1, 4); break;
        default: stat = packet[1]; value = read(packet + 2, packet[0] == 0x1d ? 1 : packet[0] == 0x1e ? 2 : 4); break;
        }
    }
    if (!unit) return 0;
    STATLIST_SetUnitStat(unit, stat, static_cast<int32_t>(value), 0);
    if (packet[0] >= 0x1d && packet[0] <= 0x1f && stat == 6) revive_hero_on_life(unit, value);
    // A changed strength, dexterity or level can make a worn item usable or unusable.
    if (stat == STAT_STRENGTH || stat == STAT_DEXTERITY || stat == STAT_LEVEL)
        d2_realm_client_owned_item_reconcile(unit);
    return 1;
}

// 1.13c D2Client+0xAEBE0 (0x95, 13 bytes) and +0xAECA0 (0x18, 15 bytes): after the opcode byte the packet
// is a little-endian bit stream whose first three 15-bit fields are life, mana and stamina (the readers at
// +0xAC1B0 / +0xAC240); the handlers store each into the current player's stat list (stats 6, 8, 10) in the
// stat's 8.8 form, i.e. shifted left by 8.
static int apply_life_mana(const uint8_t* packet, int length) {
    if (length != (packet[0] == 0x95 ? 13 : 15)) return -1;
    if (g_selectedPlayer < 0 || size_t(g_selectedPlayer) >= g_players.size()) return 0;
    D2UnitStrc* unit = g_players[g_selectedPlayer].motionUnit;
    if (!unit) return 0;
    uint64_t bits = 0;
    for (int i = 0; i < 6; ++i) bits |= uint64_t(packet[1 + i]) << (i * 8);
    STATLIST_SetUnitStat(unit, 6, static_cast<int32_t>((bits & 0x7fff) << 8), 0);
    STATLIST_SetUnitStat(unit, 8, static_cast<int32_t>(((bits >> 15) & 0x7fff) << 8), 0);
    STATLIST_SetUnitStat(unit, 10, static_cast<int32_t>(((bits >> 30) & 0x7fff) << 8), 0);
    revive_hero_on_life(unit, static_cast<uint32_t>(bits & 0x7fff));
    return 1;
}

// 0x8E: the server names a corpse and its owner (flag set), or says the corpse is gone.
static int apply_corpse_assign(const uint8_t* packet, int length) {
    d2::realm::CorpsePacket record{};
    if (!d2::realm::decodeCorpsePacket(packet, length, 0x8e, &record)) return -1;
    return g_corpses.apply(record) ? 1 : 0;
}

// 0x74: a corpse was made this frame (see corpseDropAction).
static int apply_corpse_drop(const uint8_t* packet, int length) {
    d2::realm::CorpsePacket record{};
    if (!d2::realm::decodeCorpsePacket(packet, length, 0x74, &record)) return -1;
    D2UnitStrc* const owner = find_realm_client_unit(UNIT_PLAYER, record.owner);
    bool ownerIsHero = false;
    D2RealmClientPlayerEntry* const ownerEntry = player_entry_of(owner, &ownerIsHero);
    D2UnitStrc* const corpse = find_realm_client_unit(UNIT_PLAYER, record.corpse);
    const auto action = d2::realm::corpseDropAction(record, owner != nullptr && ownerEntry != nullptr,
        owner && d2::realm::playerIsDead(owner->dwFlags, owner->dwAnimMode), ownerIsHero, corpse != nullptr);
    if (!action.act) return 0;
    if (action.ownerFalls) apply_player_life_action(owner, 8, 0, 0, owner->dwLastHitClass);
    if (action.placeCorpseOnHero && owner->pDynamicPath) {
        bool corpseSelected = false;
        if (D2RealmClientPlayerEntry* const corpseEntry = player_entry_of(corpse, &corpseSelected)) {
            corpseEntry->motionDirection = PATH_GetDirection(owner->pDynamicPath);
            place_player_entry(*corpseEntry, corpseSelected,
                static_cast<uint32_t>(PATH_GetXPosition(owner->pDynamicPath)),
                static_cast<uint32_t>(PATH_GetYPosition(owner->pDynamicPath)));
        }
    }
    if (action.stripWornItems && owner->pInventory && owner->pInventory->dwSignature == D2C_InventoryHeader) {
        D2UnitStrc* item = INVENTORY_GetFirstItem(owner->pInventory);
        while (item) {
            D2UnitStrc* const next = INVENTORY_GetNextItem(item);
            if (INVENTORY_GetItemNodePage(item) == d2::realm::kInventoryNodePageBody) {
                INVENTORY_RemoveItem(item);
                d2_realm_client_owned_item_destroy(item);
            }
            item = next;
        }
    }
    return 1;
}

// The corpses the server has named: { version 1, count, then count pairs of owner GUID, corpse GUID }.
EMSCRIPTEN_KEEPALIVE const uint32_t* d2_realm_client_corpses() {
    static std::vector<uint32_t> out;
    out.assign(2 + g_corpses.entries.size() * 2, 0);
    out[0] = 1;
    out[1] = static_cast<uint32_t>(g_corpses.entries.size());
    for (size_t i = 0; i < g_corpses.entries.size(); ++i) {
        out[2 + i * 2] = g_corpses.entries[i].owner;
        out[3 + i * 2] = g_corpses.entries[i].corpse;
    }
    return out.data();
}

// The selected player's life, mana and stamina with their maxima (stat list values, 8.8 fixed point):
// { version 1, size 32, life, lifeMax, mana, manaMax, stamina, staminaMax }; zeros until a player is selected.
EMSCRIPTEN_KEEPALIVE const uint32_t* d2_realm_client_resources() {
    static uint32_t out[8];
    std::memset(out, 0, sizeof out);
    out[0] = 1; out[1] = 32;
    if (g_selectedPlayer >= 0 && size_t(g_selectedPlayer) < g_players.size()) {
        D2UnitStrc* unit = g_players[g_selectedPlayer].motionUnit;
        if (unit) for (int i = 0; i < 6; ++i) out[2 + i] = static_cast<uint32_t>(STATLIST_UnitGetStatValue(unit, 6 + i, 0));
    }
    return out;
}

static D2UnitStrc* selected_player_stat_unit() {
    return g_selectedPlayer >= 0 && size_t(g_selectedPlayer) < g_players.size()
        ? g_players[g_selectedPlayer].motionUnit : nullptr;
}

// The selected player's character-screen numbers, by the same computation the single-player panel
// reads (d2CharacterScreen): 16 int32. `expansion` is the character's Lord of Destruction flag.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_character_screen(int expansion, int32_t* out) {
    return d2CharacterScreen(selected_player_stat_unit(), expansion ? 1 : 0, static_cast<int>(g_clientDifficulty), out);
}

// A stat of the selected player with (base 0) or without (base 1) the worn items' and states' bonuses.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_hero_stat(int stat, int layer, int base) {
    D2UnitStrc* unit = selected_player_stat_unit();
    if (!unit || stat < 0 || stat > 511) return 0;
    return base ? STATLIST_GetUnitBaseStat(unit, stat, static_cast<uint16_t>(layer))
                : STATLIST_UnitGetStatValue(unit, stat, static_cast<uint16_t>(layer));
}

// The Belts.txt row of the selected player's worn belt; 2 (the four-box row) with none.
EMSCRIPTEN_KEEPALIVE uint32_t d2_realm_client_belt_index() {
    D2UnitStrc* unit = selected_player_stat_unit();
    D2UnitStrc* worn = unit && unit->pInventory ? INVENTORY_GetItemFromBodyLoc(unit->pInventory, BODYLOC_BELT) : nullptr;
    return worn && worn->dwUnitType == UNIT_ITEM ? static_cast<uint32_t>(UNITS_GetBeltType(worn)) : 2;
}

// The items a player holds (guid 0: the selected player), 12 words each; see d2_realm_client_unit_items.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_items(uint32_t ownerGuid, uint32_t* out, int capacity) {
    return d2_realm_client_unit_items(ownerGuid ? find_realm_client_unit(UNIT_PLAYER, ownerGuid)
        : selected_player_unit(), out, capacity);
}

// The item-box colour tests of one held item against the selected player.
EMSCRIPTEN_KEEPALIVE uint32_t d2_realm_client_item_box_state(uint32_t guid) {
    return d2_realm_client_unit_item_box_state(selected_player_unit(), guid);
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
    // 1.13c D2Client+0xA7E20 leaves a dead unit where it is.
    if (entry.motionUnit && d2::realm::reassignRefusedForDead(entry.motionUnit->dwFlags,
            entry.motionUnit->dwAnimMode)) return 0;
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

// Game flags (0x01), the NPC speech marks (0x8A, 0x28) and the pet list with the hireling's stats (0x81,
// 0x7F, 0x9E..0xA2): the state lives in d2gs_client_game_packets.h, the unit lookups are done here.
static d2::realm::GameInfo g_gameInfo;
static d2::realm::NpcAlertState g_npcAlerts;
static d2::realm::PetState g_pets;

// GUESS: the assignment's hireling test (D2Common ordinal 11104) is read as "a hireling"; the
// pet list's own kind and owner decide here.
static bool is_own_hireling(uint32_t guid) {
    for (const auto& pet : g_pets.pets)
        if (pet.guid == guid) return pet.kind == d2::realm::kPetKindHireling && pet.owner == g_player.guid;
    return false;
}

static void clear_game_packet_state() {
    g_gameInfo = {};
    g_npcAlerts = {};
    g_pets = {};
}

static const D2RealmClientMonster* game_packet_monster(uint32_t guid) {
    for (const auto& monster : g_monsters) if (monster.view.guid == guid) return &monster;
    return nullptr;
}

static bool game_packet_class_is_npc(uint32_t classId) {
    const D2MonStatsTxt* stats = DATATBLS_GetMonStatsTxtRecord(static_cast<int>(classId));
    return stats && (stats->dwMonStatsFlags & MONSTATSFLAG_NPC);
}

static void game_packet_unit_removed(uint32_t guid) {
    d2::realm::npcAlertUnitRemoved(g_npcAlerts, guid);
    d2::realm::petUnitRemoved(g_pets, guid);
}

// Returns 1 when the packet changed client state, 0 when the retail handler would have left it alone.
static int apply_game_state_packet(const uint8_t* packet, int size) {
    using namespace d2::realm;
    switch (packet[0]) {
        case 0x01:
            if (!gameInfoApply(g_gameInfo, packet, size)) return 0;
            // The one difficulty the client keeps. The game has three; a byte naming none of them would
            // send the next act load to tables that do not exist, so the kept one stands.
            if (g_gameInfo.difficulty <= 2) g_clientDifficulty = g_gameInfo.difficulty;
            return 1;
        case 0x8a: {
            // Only monsters carry the mark here; the other unit kinds are not drawn with one.
            const D2RealmClientMonster* unit = size == 6 && packet[1] == UNIT_MONSTER ? game_packet_monster(gamePktU32(packet + 2)) : nullptr;
            return npcAlertApply(g_npcAlerts, packet, size, unit != nullptr, unit ? unit->view.classId : 0) == NpcAlertResult::Marked;
        }
        case 0x28: {
            bool found = false;
            if (size == 103 && packet[1] == UNIT_MONSTER) found = game_packet_monster(gamePktU32(packet + 2)) != nullptr;
            else if (size == 103 && packet[1] == UNIT_PLAYER) {
                for (const auto& player : g_players) if (player.view.guid == gamePktU32(packet + 2)) found = true;
            }
            return npcInteractReplyApply(g_npcAlerts, packet, size, found, game_packet_class_is_npc) > 0;
        }
        case 0x81: return petAssignApply(g_pets, packet, size);
        case 0x7f: return size == 10 && petLifeApply(g_pets, packet, size, game_packet_monster(gamePktU32(packet + 4)) != nullptr);
        default: return size >= 7 && petStatApply(g_pets, packet, size, game_packet_monster(gamePktU32(packet + 2)) != nullptr);
    }
}

// The game flags the server's 0x01 set. out[0..4] = difficulty, game flags, expansion, ladder, and the
// same in the save-flag layout a local game's client flags use. Returns 0 until the packet has arrived.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_game_info(uint32_t* out) {
    if (!out || !g_gameInfo.received) return 0;
    out[0] = g_gameInfo.difficulty; out[1] = g_gameInfo.gameFlags; out[2] = g_gameInfo.expansion; out[3] = g_gameInfo.ladder;
    out[4] = d2::realm::gameInfoSaveFlags(g_gameInfo);
    return 1;
}

// The guids of the units carrying the speech mark. Returns the count written.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_npc_alerts(uint32_t* out, int cap) {
    int n = 0;
    for (const auto& alert : g_npcAlerts.alerts) { if (!out || n >= cap) break; out[n++] = alert.guid; }
    return n;
}

// The NPC whose menu is open (active != 0) is not marked by a later 0x8A; closing the menu clears it.
EMSCRIPTEN_KEEPALIVE void d2_realm_client_set_active_npc(int active, uint32_t guid) {
    g_npcAlerts.hasActiveNpc = active ? 1u : 0u;
    g_npcAlerts.activeNpc = active ? guid : 0u;
}

// The pet list, newest first: nine words per pet (class, kind, guid, owner, area, life percentage, seed,
// name id, unit present). Returns the count written.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_pets(uint32_t* out, int cap) {
    int n = 0;
    for (const auto& pet : g_pets.pets) {
        if (!out || n >= cap) break;
        uint32_t* o = out + n++ * 9;
        o[0] = pet.classId; o[1] = pet.kind; o[2] = pet.guid; o[3] = pet.owner; o[4] = pet.area; o[5] = pet.lifePct;
        o[6] = pet.seed; o[7] = pet.nameId; o[8] = game_packet_monster(pet.guid) ? 1u : 0u;
    }
    return n;
}

// A stat the server set on a pet unit; INT32_MIN when it never sent one.
EMSCRIPTEN_KEEPALIVE int32_t d2_realm_client_pet_stat(uint32_t guid, int32_t stat) {
    int32_t value = 0;
    return d2::realm::petStatGet(g_pets, guid, stat, &value) ? value : INT32_MIN;
}


// ---- Server packets --------------------------------------------------------------------------------------
// Every packet reaches its handler the way the retail client's does: the D2Net size rule frames it, the
// client's dispatch loop (d2c_ace20) looks its opcode up in the client's packet table, checks the size and
// calls the row's handler. A handler body is registered under the offset of the retail handler it stands
// for, so giving an opcode a handler is one row in kRealmPacketHandlers and no edit to the feed.
// A body returns the number of records it applied, or a negative code that rejects the stream.
extern "C++" {
#include "../d2client/cl_env.h"
#include "../d2client/server_packet_table.h"
}
extern "C" void d2c_ace20(const cl::Env& e, uint32_t base, uint32_t buf, int32_t len);
extern "C" uint32_t d2_realm_d2net_6750(const uint8_t* buf, uint32_t avail, uint32_t* out);

using RealmPacketBody = int (*)(const uint8_t* packet, int size);

static int pkt_game_chat(const uint8_t* packet, int size) {
    D2RealmGameChat parsed{};
    if (d2_realm_d2gs_parse_gamechat(packet, size, &parsed) != size) return -2;
    g_gameChat = parsed;
    ++g_gameChatCount;
    return 1;
}
static int pkt_remove_unit(const uint8_t* packet, int size) {
    if (size != 6) return -2;
    // The handler keeps my own hireling when the server drops it from view: it finds it in the pet list.
    d2::realm::clientPetImageHero(selected_player_guid());
    D2cRemoveEnv env{ &g_pktEnv, d2::realm::kPetHeroRecord, d2::realm::clientPetImageRead(d2::realm::kPetHeadSlot),
        [](uintptr_t address) { return d2::realm::clientPetImageRead(static_cast<uint32_t>(address)); }, unit_remove_cb };
    g_removePacket = packet; g_removeLength = size; g_removeResult = 0;
    d2c_aca40(&env, packet);
    return g_removeResult;
}
// 0x81 feeds two lists: the client's pet list, and the mercenary roster's own entry of the same packet.
static int pkt_pet_assign(const uint8_t* packet, int size) {
    if (!d2::realm::clientPetPacket81(packet, size)) return -2;
    apply_game_state_packet(packet, size);
    return 1;
}
// 0x09 assigns a unit by type, guid, class and position. The server sends it for tile units: the level exits.
static int pkt_tile_assign(const uint8_t* packet, int size) { return d2_realm_client_warps_on_assign(g_warps, packet, size) < 0 ? -2 : 0; }
static int pkt_quest_game_record(const uint8_t* packet, int size) { return d2_realm_client_quests_on_game_record(g_quests, packet, size) ? 0 : -2; }
static int pkt_quest_log_states(const uint8_t* packet, int size) { return d2_realm_client_quests_on_log_states(g_quests, packet, size) ? 0 : -2; }
static int pkt_quest_state(const uint8_t* packet, int size) { return d2_realm_client_quests_on_state(g_quests, packet, size) ? 0 : -2; }
static int pkt_pet_owner(const uint8_t* packet, int size) {
    return d2::realm::clientPetPacket7A(packet, size, selected_player_guid()) ? 1 : -2;
}
static int pkt_object_assign(const uint8_t* packet, int size) {
    if (size != 14) return -2;
    D2cObjectEnv env{ object_create_cb };
    d2c_adf40(&env, packet);
    return 1;
}
static int pkt_quest_available(const uint8_t* packet, int size) {
    // 1.13c D2Client+0xAC090: the server's quest-available record. The 37 bytes after the opcode (one per
    // quest id) are copied to g_QuestAvail, the table the monster light setter reads at [1] for Den of Evil.
    if (size != 38) return -2;
    d2_client_lightmap_set_den_quest_avail(packet[2]);
    return 1;
}
static int pkt_load_act(const uint8_t* packet, int /*size*/) {
    // Native 1.13c LoadAct allocates a bClient act with the map seed. Packet act is
    // zero-based; the DRLG adapter takes the existing 1-based vector convention.
    const uint32_t seed = static_cast<uint32_t>(packet[2]) |
        (static_cast<uint32_t>(packet[3]) << 8) |
        (static_cast<uint32_t>(packet[4]) << 16) |
        (static_cast<uint32_t>(packet[5]) << 24);
    const int townLevel = static_cast<int>(packet[6]) | (static_cast<int>(packet[7]) << 8);
    destroy_all_monster_motion();
    destroy_all_player_motion();
    if (!d2_drlg_init()) return -5;   // no game data mounted: the act cannot be built
    if (!d2_drlg_client_act_begin(seed, static_cast<int>(g_clientDifficulty), packet[1] + 1, townLevel)) return -3;
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
    clear_objects();
    g_warps.list.clear();
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
    return 0;
}
static int pkt_load_complete(const uint8_t* /*packet*/, int /*size*/) {
    if (g_selectedPlayer < 0 || !g_player.active) return -3; // 1.13c handler needs its current player for post-load refresh.
    g_world.loadCompleteEvent = 1;
    g_world.unloadCompleteEvent = 0;
    return 0;
}
static int pkt_unload_complete(const uint8_t* /*packet*/, int /*size*/) {
    // Native UnloadComplete closes loader resources and changes loader flags; it preserves
    // the active client act, room graph, assigned units, and their path resources.
    g_world.loadCompleteEvent = 0;
    g_world.unloadCompleteEvent = 1;
    return 0;
}
static int pkt_map_reveal(const uint8_t* packet, int /*size*/) {
    const uint32_t x = static_cast<uint32_t>(packet[1]) | (static_cast<uint32_t>(packet[2]) << 8);
    const uint32_t y = static_cast<uint32_t>(packet[3]) | (static_cast<uint32_t>(packet[4]) << 8);
    const uint32_t level = packet[5];
    const int hasPlayer = g_player.active && g_player.clientRoomActive;
    if (!g_world.hasLoadAct || !d2_drlg_client_map_reveal(static_cast<int>(level), static_cast<int>(x), static_cast<int>(y),
        hasPlayer, static_cast<int>(g_player.clientRoomLevel), static_cast<int>(g_player.clientRoomTileX), static_cast<int>(g_player.clientRoomTileY))) return -3;
    // PlayerAssign can precede the first active-room event. Re-resolve its location after
    // this MapReveal has built the room, then preserve that selection on other levels.
    refresh_all_player_motion_rooms();
    for (auto& monster : g_monsters) create_monster_motion(monster);
    place_unroomed_objects();
    g_world.hasMapReveal = 1;
    const uint32_t renderLevel = static_cast<uint32_t>(d2_drlg_client_render_level());
    if (renderLevel == level || !g_world.mapLevel) {
        g_world.mapX = x;
        g_world.mapY = y;
    }
    g_world.mapLevel = renderLevel;
    g_world.mapRevealCount++;
    g_world.mapRevision++;
    return 0;
}
static int pkt_environment(const uint8_t* packet, int /*size*/) {
    // Environment packet (client handler D2Client+0xAE9C0): cycle index, ticks, eclipse flag, applied to the
    // client act with the player's room. The retail handler then refreshes the day-dependent objects.
    const int32_t cycle = static_cast<int32_t>(static_cast<uint32_t>(packet[1]) | (static_cast<uint32_t>(packet[2]) << 8) |
        (static_cast<uint32_t>(packet[3]) << 16) | (static_cast<uint32_t>(packet[4]) << 24));
    const int32_t ticks = static_cast<int32_t>(static_cast<uint32_t>(packet[5]) | (static_cast<uint32_t>(packet[6]) << 8) |
        (static_cast<uint32_t>(packet[7]) << 16) | (static_cast<uint32_t>(packet[8]) << 24));
    // Retail applies it to the hero's act only (the act the hero's pDrlgAct names) with the hero's room.
    // cycle and ticks guards are defensive checks, not retail branches.
    D2DrlgActStrc* act = d2_drlg_client_act();
    D2UnitStrc* hero = nullptr;
    if (g_selectedPlayer >= 0 && size_t(g_selectedPlayer) < g_players.size())
        hero = g_players[g_selectedPlayer].motionUnit;
    if (hero && act && hero->pDrlgAct == act && UNITS_GetRoom(hero) &&
        cycle >= 0 && cycle < NUM_ENVIRONMENT_CYCLES && ticks >= 0)
        ENVIRONMENT_InitializeEnvironment(act, UNITS_GetRoom(hero), cycle, ticks, packet[9]);
    return 1;
}
static int pkt_map_hide(const uint8_t* packet, int /*size*/) {
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
    for (D2UnitStrc* unit = d2gs_client_object_next(nullptr); unit; unit = d2gs_client_object_next(unit)) {
        D2ActiveRoomStrc* room = UNITS_GetRoom(unit);
        if (room && d2_drlg_client_room_freed_by_hide(static_cast<int>(level), static_cast<int>(x),
                static_cast<int>(y), hasPlayer, static_cast<int>(g_player.clientRoomLevel),
                static_cast<int>(g_player.clientRoomTileX), static_cast<int>(g_player.clientRoomTileY), room))
            d2gs_client_object_unplace(unit);
    }
    if (!g_world.hasLoadAct || !d2_drlg_client_map_hide(static_cast<int>(level), static_cast<int>(x), static_cast<int>(y),
        hasPlayer, static_cast<int>(g_player.clientRoomLevel), static_cast<int>(g_player.clientRoomTileX), static_cast<int>(g_player.clientRoomTileY))) return -3;
    refresh_all_player_motion_rooms();
    for (auto& monster : g_monsters) create_monster_motion(monster);
    // A tile unit leaves with its room.
    d2_realm_client_warps_keep_in_rooms(g_warps, [](int wx, int wy) { return d2_drlg_client_active_room(wx, wy) != nullptr; });
    g_world.mapHideCount++;
    g_world.mapRevision++;
    g_world.hasMapReveal = d2_drlg_client_tile_count() > 0;
    if (!g_world.hasMapReveal) g_world.mapLevel = g_world.mapX = g_world.mapY = 0;
    else g_world.mapLevel = static_cast<uint32_t>(d2_drlg_client_render_level());
    return 0;
}
static int pkt_unit_request(const uint8_t* packet, int size) {
    const int result = apply_retail_unit_packet(packet, size);
    return result < 0 ? (d2::realm::pktRoute(packet[0]).typed ? -2 : -4) : result;
}
static int pkt_select_player(const uint8_t* packet, int size) { return select_player(packet, size) < 0 ? -2 : 0; }
static int pkt_monster_assign(const uint8_t* packet, int size) { return apply_monster_assign(packet, size) ? 1 : -4; }
// 0x11: a one-shot overlay on a unit (1.13c D2Client+0xADAA0); two rows play a sound row as well.
static int pkt_unit_overlay(const uint8_t* packet, int size) {
    d2::realm::UnitWordPacket p{};
    if (!d2::realm::decodeUnitWordPacket(0x11, packet, size, &p)) return -2;
    D2UnitStrc* unit = find_realm_client_unit(p.unitType, p.guid);
    if (!sgptDataTables || !d2::realm::unitOverlayAccepted(unit != nullptr, p.value,
            sgptDataTables->nOverlayTxtRecordCount)) return 0;
    start_unit_overlay(unit, p.value, d2::realm::kUnitFxSkillRecord);
    const int sound = d2::realm::unitOverlaySound(p.value);
    if (sound >= 0) record_unit_fx(d2::realm::kUnitFxSound, unit, static_cast<uint32_t>(sound), 0,
        d2::realm::kUnitFxSkillRecord);
    return 1;
}
// 0x2C: a unit's sound event (1.13c D2Client+0xAD740); the page resolves the event to a sound.
static int pkt_unit_sound(const uint8_t* packet, int size) {
    d2::realm::UnitWordPacket p{};
    if (!d2::realm::decodeUnitWordPacket(0x2c, packet, size, &p)) return -2;
    D2UnitStrc* unit = find_realm_client_unit(p.unitType, p.guid);
    if (!unit) return 0;
    record_unit_fx(d2::realm::kUnitFxUnitSound, unit, p.value, 0, unit->dwClassId);
    return 1;
}
// 0x4D on an object is the object's own packet; on any other unit it starts a skill at a spot.
static int pkt_skill_start_at(const uint8_t* packet, int size) {
    const int result = packet[1] == UNIT_OBJECT ? apply_object_packet(packet, size) : apply_skill_start_packet(packet, size);
    return result < 0 ? -2 : result;
}
// A body whose refusal rejects the stream with one fixed code.
extern "C++" {
template <int (*Apply)(const uint8_t*, int), int Code>
static int pkt_or(const uint8_t* packet, int size) { const int result = Apply(packet, size); return result < 0 ? Code : result; }
}

// Keyed by the retail handler each body stands for: the row's queued handler when it has one, else its
// arrival handler. A queued body runs on arrival here; this client keeps no per-unit packet queue.
struct RealmPacketHandler { uint32_t handler; RealmPacketBody body; };
static const RealmPacketHandler kRealmPacketHandlers[] = {
    { 0xafa90, apply_game_state_packet },                       // 0x01
    { 0xadb50, pkt_load_act },                                  // 0x03
    { 0xadfe0, pkt_load_complete },                             // 0x04
    { 0xacb70, pkt_unload_complete },                           // 0x05
    { 0xac440, pkt_map_reveal },                                // 0x07
    { 0xac3d0, pkt_map_hide },                                  // 0x08
    { 0xadf70, pkt_tile_assign },                               // 0x09
    { 0xaca40, pkt_remove_unit },                               // 0x0a
    { 0xad6e0, pkt_select_player },                             // 0x0b
    { 0xaf5f0, pkt_unit_request },                              // 0x0c
    { 0xaf590, pkt_or<apply_player_stop, -2> },                 // 0x0d
    { 0xaf560, pkt_or<apply_unit_queue_packet, -2> },           // 0x0e
    { 0xaf510, pkt_unit_request },                              // 0x0f
    { 0xaf4c0, pkt_unit_request },                              // 0x10
    { 0xadaa0, pkt_unit_overlay },                              // 0x11
    { 0xaf640, pkt_or<reassign_player, -2> },                   // 0x15
    { 0xaf240, pkt_or<apply_unit_queue_packet, -2> },           // 0x17
    { 0xaeca0, pkt_or<apply_life_mana, -2> },                   // 0x18
    { 0xaedf0, pkt_or<apply_player_stat, -2> },                 // 0x19..0x1f
    { 0xaed80, pkt_or<apply_player_stat, -2> },                 // 0x20
    { 0xac910, pkt_or<apply_player_skill_update, -2> },         // 0x21
    { 0xac800, pkt_or<apply_player_skill_update, -2> },         // 0x22
    { 0xac780, pkt_or<apply_player_skills, -2> },               // 0x23
    { 0xaf9f0, pkt_game_chat },                                 // 0x26
    { 0xafa70, apply_game_state_packet },                       // 0x28
    { 0xac330, pkt_quest_game_record },                         // 0x29
    { 0xad740, pkt_unit_sound },                                // 0x2c
    { 0xaf1d0, pkt_or<apply_skill_start_packet, -2> },          // 0x4c
    { 0xaf160, pkt_skill_start_at },                            // 0x4d
    { 0xadf40, pkt_object_assign },                             // 0x51
    { 0xad520, pkt_quest_log_states },                          // 0x52
    { 0xae9c0, pkt_environment },                               // 0x53
    { 0xadf20, pkt_or<assign_player, -3> },                     // 0x59
    { 0xaf8f0, pkt_quest_state },                               // 0x5d
    { 0xac090, pkt_quest_available },                           // 0x5e
    { 0xaf460, pkt_unit_request },                              // 0x67
    { 0xaf3f0, pkt_unit_request },                              // 0x68
    { 0xaf390, pkt_unit_request },                              // 0x69
    { 0xaf340, pkt_unit_request },                              // 0x6a
    { 0xaf7c0, pkt_unit_request },                              // 0x6b
    { 0xaf750, pkt_unit_request },                              // 0x6c
    { 0xaf2b0, pkt_unit_request },                              // 0x6d
    { 0xadef0, pkt_or<apply_corpse_drop, -2> },                 // 0x74
    { 0xac720, pkt_pet_owner },                                 // 0x7a
    { 0xad820, apply_game_state_packet },                       // 0x7f
    { 0xaeba0, pkt_pet_assign },                                // 0x81
    { 0xadb30, apply_game_state_packet },                       // 0x8a
    { 0xad130, pkt_or<apply_corpse_assign, -2> },               // 0x8e
    { 0xac880, pkt_or<apply_player_skills, -2> },               // 0x94
    { 0xaebe0, pkt_or<apply_life_mana, -2> },                   // 0x95
    { 0xae080, pkt_or<apply_item_record, -4> },                 // 0x9c
    { 0xae870, pkt_or<apply_item_record, -4> },                 // 0x9d
    { 0xaea40, apply_game_state_packet },                       // 0x9e..0xa2
    { 0xadeb0, pkt_or<apply_state_packet, -4> },                // 0xa7
    { 0xadd70, pkt_or<apply_state_packet, -4> },                // 0xa8
    { 0xadd30, pkt_or<apply_state_packet, -4> },                // 0xa9
    { 0xadb90, pkt_or<apply_state_packet, -4> },                // 0xaa
    { 0xae420, pkt_monster_assign },                            // 0xac
};

static RealmPacketBody realm_packet_body(uint8_t opcode) {
    if (opcode >= d2::client::kServerPacketCount) return nullptr;
    const auto& row = d2::client::kServerPacketTable[opcode];
    const uint32_t handler = row.queued ? row.queued : row.handler;
    for (const auto& entry : kRealmPacketHandlers) if (entry.handler == handler) return entry.body;
    return nullptr;
}

// What the dispatch loop sees of the client module: the packet table, the packet under dispatch and the
// loop's own counters. The unit tables read as empty, so the loop queues nothing and the body does the work.
namespace {
constexpr uint32_t kDispatchModule = 0x6fab0000u, kDispatchPacket = 0x10000000u;
constexpr uint32_t kDispatchTable = 0xdde60, kDispatchBytes = 0x11c4dc, kDispatchPasses = 0x11c4e4, kDispatchBig = 0x11c4ec, kDispatchLast = 0x123414;
struct RealmDispatch {
    const uint8_t* packet;
    uint32_t size;
    int applied;
    int rejected;       // a body's negative code
    uint32_t fatal;     // the loop's own assertion line: opcode past the table, or a size the table disagrees with
};
uint32_t g_dispatchBytes, g_dispatchPasses, g_dispatchBig, g_dispatchLast;
uint32_t g_feedStall;   // why the last feed stopped short: 0, or (reason << 8) | opcode

uint32_t* dispatch_counter(uint32_t addr) {
    switch (addr - kDispatchModule) {
    case kDispatchBytes: return &g_dispatchBytes;
    case kDispatchPasses: return &g_dispatchPasses;
    case kDispatchBig: return &g_dispatchBig;
    case kDispatchLast: return &g_dispatchLast;
    default: return nullptr;
    }
}
uint32_t dispatch_read(void* ctx, uint32_t addr) {
    const auto& d = *static_cast<const RealmDispatch*>(ctx);
    if (addr - kDispatchPacket < d.size) {
        uint32_t value = 0;
        for (uint32_t i = 0, at = addr - kDispatchPacket; i < 4 && at + i < d.size; ++i) value |= static_cast<uint32_t>(d.packet[at + i]) << (8 * i);
        return value;
    }
    const uint32_t entry = addr - (kDispatchModule + kDispatchTable);
    if (entry < d2::client::kServerPacketCount * 12) {
        const auto& row = d2::client::kServerPacketTable[entry / 12];
        switch (entry % 12) {
        case 0: return row.handler ? kDispatchModule + row.handler : 0;
        case 4: return static_cast<uint32_t>(row.size);
        case 8: return row.queued ? kDispatchModule + row.queued : 0;
        default: return 0;
        }
    }
    if (const uint32_t* counter = dispatch_counter(addr)) return *counter;
    return 0;
}
void dispatch_write(void*, uint32_t addr, uint32_t value) {
    if (uint32_t* counter = dispatch_counter(addr)) *counter = value;
}
uint32_t dispatch_call(void* ctx, uint32_t rva, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                       uint32_t s0, uint32_t, uint32_t, uint32_t, uint32_t) {
    auto& d = *static_cast<RealmDispatch*>(ctx);
    switch (rva) {
    case 0xd246: {   // the packet size
        if (d.rejected || d.fatal) return 0;
        uint32_t size = 0;
        const uint32_t result = d2_realm_d2net_6750(d.packet, d.size, &size);
        *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(s0)) = size;
        return result;
    }
    case 0xbf92: d.fatal = s0; return 0;                       // the assertion line
    case 0xbf80: case 0x3664: return 0;                        // report and exit: the feed stops instead
    case 0xa4e20: case 0xac630: return 0;                      // unit lookup and per-unit queue: none kept
    case 0xac240: case 0xac1b0: case 0xac140: return 0;        // life/mana unpackers: the bodies decode
    default: break;
    }
    if (rva < kDispatchModule) return 0;
    if (RealmPacketBody body = realm_packet_body(d.packet[0])) {
        const int result = body(d.packet, static_cast<int>(d.size));
        if (result < 0) d.rejected = result; else d.applied += result;
    }
    return 0;
}
}  // namespace

// A packet the stream cannot frame (an opcode the size table does not know, or a size over the receive
// node) stops the feed where retail's splitter stops: the bytes stay pending, nothing after them is
// dispatched and the state built so far is kept. d2_realm_client_feed_stall names the opcode.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_player_feed(const uint8_t* bytes, int length) {
    if ((!bytes && length) || length < 0 || length > kPendingCapacity - g_pendingLength) return reject_stream(-1);
    if (length) {
        std::memcpy(g_pending + g_pendingLength, bytes, static_cast<size_t>(length));
        g_pendingLength += length;
    }
    const cl::Env env{ nullptr, dispatch_call, dispatch_read, dispatch_write, 0, 0 };
    int offset = 0;
    int applied = 0;
    g_feedStall = 0;
    while (offset < g_pendingLength) {
        const uint8_t* packet = g_pending + offset;
        const uint32_t available = static_cast<uint32_t>(g_pendingLength - offset);
        uint32_t entry = 0;
        const uint32_t size = d2_realm_d2net_6750(packet, available, &entry);
        if (size == 0) {   // more bytes needed, or an opcode with no size
            if (entry != 0xFFFFFFFFu) g_feedStall = 0x100u | packet[0];
            break;
        }
        if (size > 0x204) { g_feedStall = 0x200u | packet[0]; break; }
        if (size > available) break;
        if (packet[0] < d2::client::kServerPacketCount) {   // 0xaf and above are the transport's own
            RealmDispatch dispatch{ packet, size, 0, 0, 0 };
            cl::Env bound = env;
            bound.ctx = &dispatch;
            d2c_ace20(bound, kDispatchModule, kDispatchPacket, static_cast<int32_t>(size));
            if (dispatch.rejected) return reject_stream(dispatch.rejected);
            if (dispatch.fatal) { g_feedStall = 0x300u | packet[0]; break; }
            applied += dispatch.applied;
        }
        offset += static_cast<int>(size);
    }
    const int remaining = g_pendingLength - offset;
    if (offset && remaining) std::memmove(g_pending, g_pending + offset, static_cast<size_t>(remaining));
    g_pendingLength = remaining;
    return applied;
}

EMSCRIPTEN_KEEPALIVE uint32_t d2_realm_client_feed_stall() { return g_feedStall; }


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

// The unit's path position at 16.16 precision, as the single-player renderer reads it.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_unit_precise(uint8_t unitType, uint32_t guid, uint32_t* out) {
    D2UnitStrc* unit = find_realm_client_unit(unitType, guid);
    if (!out || !unit || !unit->pDynamicPath) return 0;
    out[0] = static_cast<uint32_t>(PATH_GetPrecisionX(unit->pDynamicPath));
    out[1] = static_cast<uint32_t>(PATH_GetPrecisionY(unit->pDynamicPath));
    return 1;
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

// What the client knows of one unit beyond its view: whether the hero may attack it, whose pet it is, and the
// name and second line its plate shows. The attack answer is given three ways because the page holds the
// roster: `selectable` where the hero stands with no player hostile, and the answer outside town with the
// roster's relation toward `rosterGuid` friendly (`enemy`) and hostile (`enemyIfHostile`).
struct D2RealmClientUnitInfo {
    uint32_t version;
    uint32_t size;
    uint32_t unitType;
    uint32_t guid;
    uint32_t selectable;
    uint32_t enemy;
    uint32_t enemyIfHostile;
    uint32_t rosterGuid;
    int32_t alignment;
    uint32_t isPet;
    uint32_t petType;
    uint32_t petOwner;
    uint32_t petDead;
    uint32_t petNameId;
    uint32_t typeFlags;
    int32_t superUnique;
    uint32_t nameSeed;
    uint32_t ownerGuid;
    uint8_t umods[12];
    char name[128];
    char line[256];
};
static_assert(offsetof(D2RealmClientUnitInfo, umods) == 72);
static_assert(offsetof(D2RealmClientUnitInfo, name) == 84);
static_assert(offsetof(D2RealmClientUnitInfo, line) == 212);
static_assert(sizeof(D2RealmClientUnitInfo) == 468);

static uint32_t monstats2_flag(uint32_t classId, uint32_t bit) {
    const D2MonStatsTxt* row = DATATBLS_GetMonStatsTxtRecord(static_cast<int>(classId));
    if (!row || !sgptDataTables->pMonStats2Txt) return 0;
    const int ex = static_cast<int16_t>(row->wMonStatsEx);
    if (ex < 0 || ex >= sgptDataTables->nMonStats2TxtRecordCount) return 0;
    return (sgptDataTables->pMonStats2Txt[ex].nFlags[(bit >> 3) & 3] >> (bit & 7)) & 1u;
}

EMSCRIPTEN_KEEPALIVE const D2RealmClientUnitInfo* d2_realm_client_unit_info(uint8_t unitType, uint32_t guid) {
    static D2RealmClientUnitInfo info;
    if (!sgptDataTables) return nullptr;
    d2::realm::RelationUnit target{}, hero{};
    D2UnitStrc* targetUnit = nullptr;
    const D2RealmClientMonster* monster = nullptr;
    if (unitType == UNIT_MONSTER) {
        for (const auto& candidate : g_monsters) if (candidate.view.guid == guid) { monster = &candidate; break; }
        if (!monster) return nullptr;
        targetUnit = monster->motionUnit;
        target = { true, UNIT_MONSTER, monster->view.classId, guid, monster->view.mode, targetUnit ? targetUnit->dwFlags : 0 };
    } else if (unitType == UNIT_PLAYER) {
        targetUnit = find_realm_client_unit(UNIT_PLAYER, guid);
        if (!targetUnit) return nullptr;
        target = { true, UNIT_PLAYER, static_cast<uint32_t>(targetUnit->dwClassId), guid, targetUnit->dwAnimMode, targetUnit->dwFlags };
    } else return nullptr;
    D2UnitStrc* heroUnit = nullptr;
    if (g_selectedPlayer >= 0 && static_cast<size_t>(g_selectedPlayer) < g_players.size())
        heroUnit = g_players[g_selectedPlayer].motionUnit;
    if (heroUnit)
        hero = { true, UNIT_PLAYER, static_cast<uint32_t>(heroUnit->dwClassId), selected_player_guid(), heroUnit->dwAnimMode, heroUnit->dwFlags };

    info = {};
    info.version = 1;
    info.size = sizeof(info);
    info.unitType = unitType;
    info.guid = guid;
    d2::realm::RelationFacts facts{};
    facts.aligned = heroUnit && targetUnit && STATLIST_AreUnitsAligned(heroUnit, targetUnit);
    facts.monStats2Flag = monstats2_flag;
    D2ActiveRoomStrc* room = selected_player_room();
    facts.inTown = room && DUNGEON_IsRoomInTown(room);
    info.selectable = d2::realm::clientUnitRelation(target, hero, facts);
    facts.inTown = false;
    info.enemy = d2::realm::clientUnitRelation(target, hero, facts);
    facts.rosterHostile = true;
    info.enemyIfHostile = d2::realm::clientUnitRelation(target, hero, facts);
    info.rosterGuid = d2::realm::clientRelationRosterGuid(target);
    info.alignment = targetUnit ? STATLIST_GetUnitAlignment(targetUnit) : 0;
    if (unitType != UNIT_MONSTER) return &info;

    if (const d2::realm::ClientPet* pet = d2::realm::clientPetFind(guid)) {
        info.isPet = 1;
        info.petType = pet->type;
        info.petOwner = pet->owner;
        info.petDead = pet->dead;
        info.petNameId = pet->nameId;
    }
    info.typeFlags = monster->view.typeFlags;
    info.superUnique = monster->superUniqueIndex;
    info.nameSeed = monster->nameId;
    info.ownerGuid = monster->ownerGuid;
    const size_t umodCount = std::min<size_t>(monster->nameBytes.size(), 9);
    for (size_t i = 0; i < umodCount; ++i) info.umods[i] = monster->nameBytes[i];
    D2UnitStrc classUnit{};
    classUnit.dwUnitType = UNIT_MONSTER;
    classUnit.dwClassId = static_cast<int>(monster->view.classId);
    d2::realm::MonsterNameInput in{};
    in.unit = &classUnit;
    in.classId = monster->view.classId;
    in.typeFlags = monster->view.typeFlags;
    in.nameSeed = monster->nameId;
    in.superUnique = monster->superUniqueIndex;
    in.umods = info.umods;
    in.umodCount = static_cast<uint32_t>(umodCount);
    in.hirelingNameId = d2::realm::clientPetHirelingNameId(guid);
    const std::string name = d2::realm::clientUtf8(d2::realm::clientMonsterName(in));
    const std::string line = d2::realm::clientUtf8(d2::realm::clientMonsterEnchantLine(in));
    std::strncpy(info.name, name.c_str(), sizeof(info.name) - 1);
    std::strncpy(info.line, line.c_str(), sizeof(info.line) - 1);
    return &info;
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

#include "../monspawn/d2client_light_in.h"
// The online client's light map: the same window, ambient, occlusion and falloff as the session's
// (d2_client_lightmap_compose), fed from the units this client holds -- the selected player, its monsters'
// CUnits and its objects (Objects.txt Lit<mode> is a diameter, position in subtiles *8 + 4).
// Objects settled: the radius (nLit[mode] >> 1) and position are the rule the session lightmap uses
// (monspawn_check.cpp, measured against retail's light captures). Monsters take the light setter's rule in the
// compose step (max of MonStats2 nLight and the item light, at most 18; on Den of Evil a radius of 3 while the
// Den quest byte is set). The byte is g_QuestAvail[1], the second byte after the opcode of server packet 0x5E (D2Client
// +0xAC090), fed to d2_client_lightmap_set_den_quest_avail when that packet arrives.
// Returns the session lightmap record (same layout as d2_session_lightmap), or null without a player/act.
EMSCRIPTEN_KEEPALIVE const void* d2_realm_client_lightmap(int ambientOverride, int occludeUnits) {
    D2DrlgActStrc* act = d2_drlg_client_act();
    if (!act || g_selectedPlayer < 0 || static_cast<size_t>(g_selectedPlayer) >= g_players.size()) return nullptr;
    D2UnitStrc* player = g_players[g_selectedPlayer].motionUnit;
    if (!player) return nullptr;
    static std::vector<D2ClientLightIn> in;
    in.clear();
    for (const auto& monster : g_monsters)
        if (monster.motionUnit) in.push_back({ monster.motionUnit, UNIT_MONSTER, static_cast<int32_t>(monster.view.classId), 0, 0, 0, monster.view.guid, nullptr });
    for (const auto& object : g_objects) {
        if (object.unitType != UNIT_OBJECT) continue;
        // The record's mode follows the unit, so a torch the server lights or puts out changes its radius; the
        // light is anchored in the object's own room, as the local game's is.
        const D2ObjectsTxt* txt = DATATBLS_GetObjectsTxtRecord(static_cast<int>(object.classId));
        const int radius = txt && object.mode < 8 ? txt->nLit[object.mode] >> 1 : 0;
        D2UnitStrc* unit = find_object_unit(object.guid);
        in.push_back({ nullptr, UNIT_OBJECT, static_cast<int32_t>(object.classId), static_cast<int32_t>(object.x) * 8 + 4,
            static_cast<int32_t>(object.y) * 8 + 4, radius, object.guid, unit ? UNITS_GetRoom(unit) : nullptr });
    }
    return d2_client_lightmap_compose(player, act, in.data(), static_cast<int>(in.size()), ambientOverride, occludeUnits);
}

// Current day/night state of the client act: period, intensity, ticks, rgb; 0 without an act.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_period_of_day(int32_t* out) {
    D2DrlgActStrc* act = d2_drlg_client_act();
    D2DrlgEnvironmentStrc* env = act ? DUNGEON_GetEnvironmentFromAct(act) : nullptr;
    if (!env || !out) return 0;
    out[0] = env->nPeriodOfDay; out[1] = env->nIntensity; out[2] = env->nTicks;
    out[3] = env->nRed | (env->nGreen << 8) | (env->nBlue << 16);
    return 1;
}

// Advance path-owned monster CUnits once for each shell game frame. Renderer getters remain pure.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_motion_tick() {
    ++g_clientMissileTick;
    int moving = 0;
    if (D2DrlgActStrc* act = d2_drlg_client_act()) ENVIRONMENT_UpdatePeriodOfDay(act, selected_player_room());
    advance_client_missiles();
    for (size_t i = 0; i < g_players.size(); ++i) {
        auto& player = g_players[i];
        if (!player.motionUnit || !player.motionUnit->pDynamicPath) continue;
        const int result = advance_client_player_action(player.motionUnit) ? 0 :
            d2_realm_client_motion_advance_unit(player.motionUnit);
        if (result > 0) {
            moving++;
            refresh_player_motion_view(player, static_cast<int>(i) == g_selectedPlayer);
        }
    }
    for (D2UnitStrc* object = d2gs_client_object_next(nullptr); object; object = d2gs_client_object_next(object)) {
        const uint32_t mode = object->dwAnimMode;
        d2gs_client_object_step(object);
        if (object->dwAnimMode != mode) sync_object_view(object);
    }
    for (auto& monster : g_monsters) {
        if (!monster.motionUnit || !monster.motionUnit->pDynamicPath) continue;
        // GUESS: a monster whose skill sequence has run out returns to neutral here; retail's
        // per-mode monster update (not read) owns that end. Settled by decompiling the Sequence
        // arm of the client's monster mode table.
        if (monster.motionUnit->dwAnimMode == MONMODE_SEQUENCE && monster.motionUnit->pAnimSeq &&
                UNITS_IsAtEndOfFrameCycle(monster.motionUnit)) {
            UNITS_SetUsedSkill(monster.motionUnit, nullptr);
            d2_realm_client_unit_set_mode(monster.motionUnit, MONMODE_NEUTRAL);
        }
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
        // bits 32/64: the bonus is positive / negative (the tree's level text is blue / red, D2Client+0x78820).
        const int bonus = SKILLS_GetBonusSkillLevelFromSkillId(unit, row.id);
        row.flags |= (bonus > 0 ? 32 : 0) | (bonus < 0 ? 64 : 0);
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

// A roster player in a Disguise state (Werewolf, Werebear) is drawn as the monster ConvertMode names, not
// as its class token. Fills the 96-byte monster animation record (the layout parseMonsterAnimations reads)
// and returns 1, or returns 0 when the unit is not converted to a monster.
struct RealmPlayerFormRecord {
    uint32_t id, classId, code, mode, framePrecise, frameCountPrecise, componentCodes[16];
    uint16_t animSpeed; uint8_t direction, flags; uint32_t baseWeaponClass;
};
static_assert(sizeof(RealmPlayerFormRecord) == 96, "monster animation record");
EMSCRIPTEN_KEEPALIVE int d2_realm_client_player_form_at(int index, RealmPlayerFormRecord* out) {
    if (!out || index < 0 || static_cast<size_t>(index) >= g_players.size()) return 0;
    const D2UnitStrc* unit = g_players[static_cast<size_t>(index)].motionUnit;
    if (!unit || !unit->pDynamicPath) return 0;
    int unitType = static_cast<int>(unit->dwUnitType), classId = static_cast<int>(unit->dwClassId);
    int mode = UNITS_GetAnimOrSeqMode(const_cast<D2UnitStrc*>(unit));
    D2COMMON_11013_ConvertMode(const_cast<D2UnitStrc*>(unit), &unitType, &classId, &mode, __FILE__, __LINE__);
    if (unitType != UNIT_MONSTER) return 0;
    const D2MonStatsTxt* mon = DATATBLS_GetMonStatsTxtRecord(classId);
    const D2MonStats2Txt* mon2 = UNITS_GetMonStats2TxtRecordFromMonsterId(classId);
    if (!mon) return 0;
    memset(out, 0, sizeof *out);
    out->id = unit->dwUnitId; out->classId = static_cast<uint32_t>(classId); out->code = mon->dwCode;
    out->mode = static_cast<uint32_t>(mode);
    out->framePrecise = static_cast<uint32_t>(unit->nSeqCurrentFramePrecise);
    out->frameCountPrecise = static_cast<uint32_t>(unit->dwFrameCountPrecise);
    out->animSpeed = static_cast<uint16_t>(unit->wAnimSpeed);
    out->direction = static_cast<uint8_t>(PATH_GetDirection(unit->pDynamicPath));
    out->baseWeaponClass = mon2 ? static_cast<uint32_t>(mon2->nBaseW) : 0;
    out->flags = (mon2 && (mon2->dwModeFlags & MONSTATS2MODEFLAG_DD)) ? 0x02 : 0;
    for (uint32_t c = 0; c < 16; ++c) out->componentCodes[c] = static_cast<uint32_t>(D2Common_11070(classId, c, 0));
    return 1;
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
// The single-player snapshot's shape (68 bytes, see d2gs_client_snapshot.h) from the game-server
// client state: the selected player's CUnit (position, life, mode, flags), the act/seed/difficulty
// the server sent, the live monster roster size. Empty until a player is assigned.
EMSCRIPTEN_KEEPALIVE const d2::realm::ClientSnapshot* d2_realm_client_snapshot() {
    static d2::realm::ClientSnapshot snapshot;
    d2::realm::ClientSnapshotInput in{};
    in.playerActive = g_player.active != 0;
    in.seed = g_world.mapSeed; in.frame = g_clientMissileTick;
    in.level = g_player.clientRoomActive ? g_player.clientRoomLevel : g_world.area;
    in.difficulty = g_clientDifficulty; in.act = g_world.act;
    in.classId = g_player.classId; in.guid = g_player.guid;
    in.x = static_cast<int32_t>(g_player.x); in.y = static_cast<int32_t>(g_player.y);
    in.mode = g_player.initialMode;
    in.monsters = static_cast<uint32_t>(g_monsters.size());
    if (g_selectedPlayer >= 0 && static_cast<size_t>(g_selectedPlayer) < g_players.size()) {
        const D2UnitStrc* unit = g_players[static_cast<size_t>(g_selectedPlayer)].motionUnit;
        if (unit && unit->pDynamicPath) {
            in.x = unit->pDynamicPath->tGameCoords.wPosX; in.y = unit->pDynamicPath->tGameCoords.wPosY;
            in.mode = unit->dwAnimMode; in.flags = unit->dwFlags;
            in.hp = STATLIST_UnitGetStatValue(const_cast<D2UnitStrc*>(unit), STAT_HITPOINTS, 0);
            in.maxHp = STATLIST_UnitGetStatValue(const_cast<D2UnitStrc*>(unit), STAT_MAXHP, 0);
        }
    }
    snapshot = d2::realm::build_client_snapshot(in);
    return &snapshot;
}

// The inventory panel's weapon boxes for the selected player's worn arms (the same rule the single-player panel
// reads): which empty-arm silhouettes and two-hander ghosts the active set shows. -1 with no player.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_weapon_hands() {
    if (g_selectedPlayer < 0 || static_cast<size_t>(g_selectedPlayer) >= g_players.size()) return -1;
    D2UnitStrc* player = g_players[static_cast<size_t>(g_selectedPlayer)].motionUnit;
    if (!player || !player->pInventory || player->pInventory->dwSignature != D2C_InventoryHeader) return -1;
    D2InventoryStrc* inv = player->pInventory;
    D2UnitStrc* r = INVENTORY_GetItemFromBodyLoc(inv, BODYLOC_RARM);
    D2UnitStrc* l = INVENTORY_GetItemFromBodyLoc(inv, BODYLOC_LARM);
    return d2client_weapon_hands::rule(r != nullptr, l != nullptr, INVENTORY_GetWieldType(player, inv),
                                       ITEMS_GetQuiverType(r), ITEMS_GetQuiverType(l));
}

// Transaction price: 1.13c D2Common+0x29D60 forwards to ITEMS_CalculateTransactionCost (D2Common+0x28E80).
// The quest flags it reads are the client's quest record: 96 bytes the server sends with its 0x28 packets
// (empty until the first arrives), the record retail passes as the fourth argument.
// Transaction types: 0 buy, 1 sell, 2 gamble, 3 repair (D2C_TransactionTypes).
// The item format version the server encodes stock and owned items with (owned decoder: format 96).
constexpr uint32_t kStockItemFormatVersion = 96;

static D2BitBufferStrc* client_quest_record() {
    static D2BitBufferStrc* record = nullptr;
    if (!record) record = QUESTRECORD_AllocRecord(nullptr);
    return record;
}

static void clear_client_quest_record() {
    D2BitBufferStrc* record = client_quest_record();
    if (record && record->pBuffer) std::memset(record->pBuffer, 0, 96);
}

EMSCRIPTEN_KEEPALIVE int d2_realm_client_set_quest_flags(const uint8_t* bytes, int length) {
    if (!bytes || length != 96) return 0;
    QUESTRECORD_CopyBufferToRecord(client_quest_record(), const_cast<uint8_t*>(bytes), 96, FALSE);
    d2_realm_client_quests_set_player(g_quests, bytes);
    return 1;
}

// The quest state the server's packets built (layout: d2_realm_client_quests_view). `logOpen` tells the
// state-change rule whether the quest log is on screen.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_quests(uint8_t* out, int cap, int logOpen) {
    g_quests.logOpen = logOpen ? 1 : 0;
    return d2_realm_client_quests_view(g_quests, out, cap);
}

// A room's place in its level's room list: the ordinal a local game's room and exit records carry.
static uint32_t client_room_ordinal(const D2DrlgRoomStrc* room) {
    uint32_t ordinal = 0;
    for (const D2DrlgRoomStrc* r = room && room->pLevel ? room->pLevel->pFirstRoomEx : nullptr; r; r = r->pDrlgRoomNext, ++ordinal)
        if (r == room) return ordinal;
    return UINT32_MAX;
}

// The client's active rooms: the rooms the server's map-add packets (0x07) put in sight, with their
// neighbours, less those its map-remove packets (0x08) freed. One 36-byte record each, the local game's room
// record: ordinal, level, subtile x, y, width, height, flags, 0, 0. Returns the count written.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_rooms(uint8_t* out, int cap) {
    D2DrlgActStrc* act = d2_drlg_client_act();
    if (!out || !act) return 0;
    int count = 0;
    for (D2ActiveRoomStrc* room = DUNGEON_GetRoomFromAct(act); room && (count + 1) * 36 <= cap; room = room->pRoomNext) {
        const D2DrlgRoomStrc* drlgRoom = room->pDrlgRoom;
        const uint32_t record[9] = { client_room_ordinal(drlgRoom),
            drlgRoom && drlgRoom->pLevel ? static_cast<uint32_t>(drlgRoom->pLevel->nLevelId) : 0u,
            static_cast<uint32_t>(room->tCoords.nSubtileX), static_cast<uint32_t>(room->tCoords.nSubtileY),
            static_cast<uint32_t>(room->tCoords.nSubtileWidth), static_cast<uint32_t>(room->tCoords.nSubtileHeight),
            room->dwFlags, 0u, 0u };
        std::memcpy(out + count * 36, record, 36);
        ++count;
    }
    return count;
}

// The level exits in the client's active rooms, one per tile unit the server assigned: a 68-byte record each,
// the local game's exit record (slot, room, source level, destination level, LvlWarp id, enabled, direction,
// x, y, select box x, y, dx, dy, exit walk x, y, offset x, y), then one uint32 guid per record. The room's own
// warp tile of that LvlWarp id names the record and the destination level. Returns the count written.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_warps(uint8_t* out, int cap) {
    if (!out) return 0;
    std::vector<uint32_t> guids;
    int count = 0;
    for (const D2RealmClientWarp& warp : g_warps.list) {
        if ((count + 1) * 72 > cap) break;
        D2ActiveRoomStrc* active = d2_drlg_client_active_room(warp.x, warp.y);
        const D2DrlgRoomStrc* room = active ? active->pDrlgRoom : nullptr;
        if (!room || !room->pLevel) continue;
        const D2LvlWarpTxt* txt = nullptr;
        int32_t destination = 0;
        for (const D2RoomTileStrc* tile = room->pRoomTiles; tile; tile = tile->pNext) {
            if (!tile->pLvlWarpTxtRecord || tile->pLvlWarpTxtRecord->dwLevelId != warp.warpId) continue;
            txt = tile->pLvlWarpTxtRecord;
            if (tile->pDrlgRoom && tile->pDrlgRoom->pLevel) destination = tile->pDrlgRoom->pLevel->nLevelId;
            break;
        }
        if (!txt) txt = DATATBLS_GetLvlWarpTxtRecordFromLevelIdAndDirection(static_cast<int>(warp.warpId), 'b');
        int32_t record[17] = { count, static_cast<int32_t>(client_room_ordinal(room)), room->pLevel->nLevelId, destination,
            static_cast<int32_t>(warp.warpId), 1, 0, warp.x, warp.y, 0, 0, 0, 0, 0, 0, 0, 0 };
        if (txt) {
            record[6] = static_cast<uint8_t>(txt->szDirection[0]);
            record[9] = static_cast<int32_t>(txt->dwSelectX); record[10] = static_cast<int32_t>(txt->dwSelectY);
            record[11] = static_cast<int32_t>(txt->dwSelectDX); record[12] = static_cast<int32_t>(txt->dwSelectDY);
            record[13] = static_cast<int32_t>(txt->dwExitWalkX); record[14] = static_cast<int32_t>(txt->dwExitWalkY);
            record[15] = static_cast<int32_t>(txt->dwOffsetX); record[16] = static_cast<int32_t>(txt->dwOffsetY);
        }
        std::memcpy(out + count * 68, record, 68);
        guids.push_back(warp.guid);
        ++count;
    }
    if (count) std::memcpy(out + count * 68, guids.data(), static_cast<size_t>(count) * 4);
    return count;
}

// A hire offer's stats for the selected player: the hireling the 0x4E offer (name id, seed) rolls, through the same
// MONSTERS_HirelingInit the server runs for the purchase (act from the name id, the game's difficulty). out[0..5] =
// level, hit points, defence, gold cost, hireling id, experience. Returns 0 with no player or no matching row.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_hire_stats(uint32_t mercName, uint32_t seed, int expansion, int32_t* out) {
    if (!out || g_selectedPlayer < 0 || static_cast<size_t>(g_selectedPlayer) >= g_players.size()) return 0;
    D2UnitStrc* player = g_players[g_selectedPlayer].motionUnit;
    if (!player) return 0;
    const BOOL exp = expansion ? TRUE : FALSE;
    D2HirelingInitStrc init = {};
    const int act = MONSTERS_GetActFromHirelingTxt(exp, 0, static_cast<uint16_t>(mercName));
    if (!MONSTERS_HirelingInit(exp, player, static_cast<int>(seed), act, static_cast<int>(g_clientDifficulty), &init)) return 0;
    out[0] = init.nLevel; out[1] = init.nHitpoints; out[2] = init.nDefense; out[3] = init.nGold; out[4] = init.nId; out[5] = init.nExperience;
    return 1;
}

static int realm_client_transaction_cost(D2UnitStrc* player, D2UnitStrc* item, uint32_t vendorId,
        uint32_t transType) {
    if (!player || !item || !item->pItemData || transType > 3) return -1;
    if (!DATATBLS_GetNpcTxtRecord(vendorId)) return -1;
    return ITEMS_CalculateTransactionCost(player, item, static_cast<D2C_Difficulties>(g_clientDifficulty),
        client_quest_record(), static_cast<int>(vendorId), static_cast<D2C_TransactionTypes>(transType));
}

// The price of an item the selected or any listed player owns (sell, repair, gamble, buy-back).
EMSCRIPTEN_KEEPALIVE int d2_realm_client_item_price(uint32_t itemGuid, uint32_t vendorId, uint32_t transType) {
    for (auto& player : g_players) {
        D2UnitStrc* owner = player.motionUnit;
        if (!owner || !owner->pInventory || owner->pInventory->dwSignature != D2C_InventoryHeader) continue;
        for (D2UnitStrc* item = INVENTORY_GetFirstItem(owner->pInventory); item;
                item = INVENTORY_GetNextItem(item)) {
            if (item->dwUnitType == UNIT_ITEM && item->dwUnitId == itemGuid)
                return realm_client_transaction_cost(owner, item, vendorId, transType);
        }
    }
    return -1;
}

// The price of a shop stock item (S2C 0x9C) for the selected player. The stock bits are the item bitstream
// itself (flags, format version, mode, then the page the item sits on), decoded with the owned-item format.
// The stock decode skips the equip-mode and body-location checks an equipped item needs.
// Returns -1 when the bits do not decode to exactly their length or the vendor/quest state is unknown.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_stock_price(const uint8_t* bytes, int length, uint32_t vendorId,
        uint32_t transType) {
    if (!bytes || length <= 0 || g_selectedPlayer < 0 || static_cast<size_t>(g_selectedPlayer) >= g_players.size())
        return -1;
    D2UnitStrc* player = g_players[g_selectedPlayer].motionUnit;
    if (!player) return -1;

    auto* item = UNITS_AllocUnit(nullptr, UNIT_ITEM);
    if (!item) return -1;
    ITEMS_AllocItemData(nullptr, item);
    ITEMS_InitItemSeed(item);
    UNITS_AllocStaticPath(item);
    if (!item->pItemData || !item->pStaticPath) {
        if (item->pItemData) ITEMS_FreeItemData(nullptr, item);
        if (item->pStaticPath) UNITS_FreeStaticPath(item);
        UNITS_FreeUnit(item);
        return -1;
    }
    UNITS_InitializeStaticPath(item, nullptr, 0, 0);

    int socketedItems = 0;
    BOOL failed = FALSE;
    const size_t consumed = ITEMS_DecodeItemFromBitstream(item, const_cast<uint8_t*>(bytes),
        static_cast<size_t>(length), FALSE, &socketedItems, kStockItemFormatVersion, &failed);
    int price = -1;
    if (!failed && consumed == static_cast<size_t>(length) && item->pItemData)
        price = realm_client_transaction_cost(player, item, vendorId, transType);
    STATLIST_FreeStatListEx(item);
    ITEMS_FreeItemData(nullptr, item);
    UNITS_FreeStaticPath(item);
    UNITS_FreeUnit(item);
    return price;
}

}  // extern "C"
