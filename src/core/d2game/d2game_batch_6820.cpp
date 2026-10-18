// src/core/d2game/d2game_batch_6820.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): the MSVC 2003 stack probe and
// file-stream leaves, the UNIT hash-entry unlink, the four game unit-list sweeps, the inactive-unit
// compressor, the CRC32 core and one client-list lookup. Stage A batch lane: implementation + marker
// only, no spec, no vector.
//
// Register-only arguments (Ghidra's in_EAX/unaff_EDI/unaff_ESI/unaff_EBX, or arguments the ABI passed
// in a register and the callsite dropped) are explicit parameters here and are named in the comment at
// each function. A game record is opaque and read/written through retail byte offsets; unit records use
// the 1.10f D2UnitStrc offsets (reference/D2MOO/source/D2Common/include/Units/Units.h). The fatal path
// is Fog Ordinal 10265(id) -> Ordinal 10024(text) -> _exit(-1); only the exit is observable.
//
// 0x498B0 (CLIENTS_GetNameIfInGame) lives in d2game_batch_49850.cpp instead: that file already owns the
// 1.13c client-list globals and the neighbouring CLIENTS_GetClientIdByName.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <unistd.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// ---- MSVC 2003 CRT leaves ------------------------------------------------------------------------
// 1.13c D2Game+0x6820
// __chkstk: entered with the frame size in EAX and the return address at [esp]; it walks one page per
// 4 KB, touching each guard page, and re-pushes the return address below the new frame. Both the input
// and the result are ESP/EAX -- outside the C ABI -- so a host callee can only take the frame size and
// do nothing: the wasm stack is grown by the host runtime and has no guard page to touch.
void D2Game_ChkStk(uint32_t frameBytes)
{
    (void)frameBytes;
}

// 1.13c D2Game+0x2FD0
// __cdecl fopen wrapper: __fsopen(name, mode, _SH_DENYNO = 0x40). The decompile drops the FILE* return
// because the import's type is unknown, but callers 0x56250/0x53F10 read EAX; the host stream is the
// whole result and the share mode has no analogue, so the return is kept here.
// ponytail: the retail 32-bit FILE object is not created; this returns the host stdio stream that
// d2game_fwrite/d2game_fclose below consume.
void* D2Game_CrtFopen(const char* name, const char* mode)
{
    return std::fopen(name, mode);
}

// 1.13c D2Game+0x321F
// _fwrite: __lock_file(stream); count = __fwrite_lk(data, size, count, stream); FUN_6fc23261 unlocks
// the stream/whole lock table; return count. The stream lock is the per-FILE critical section, which
// the host stdio layer already holds for the duration of fwrite, and the _IOSTRG/_IOERR flag handling
// stays inside the host stream; only the returned element count is observable.
size_t D2Game_CrtFwrite(const void* data, size_t size, size_t count, FILE* stream)
{
    return std::fwrite(data, size, count, stream);
}

// 1.13c D2Game+0x32B7
// _fclose: result -1; a stream with _IOSTRG (0x40) set skips the close and only clears _flag, every
// other stream is locked and closed by __fclose_lk, then FUN_6fc23300 unlocks. Host stdio streams are
// never _IOSTRG, so the branch is unobservable; closing is the whole effect.
int D2Game_CrtFclose(FILE* stream)
{
    return std::fclose(stream);
}

// ---- UNIT hash entry unlink ----------------------------------------------------------------------
// A link word is either a direct pointer (high bit clear) or a complement of one (high bit set); a
// direct link is an offset from the pair address plus the head's +0x04 base -- the same encoding the
// 1.10f D2HashTableStrc/D2HashItemStrc pair uses. The unlink writes the head into the resolved slot,
// writes the link into head+0x04, and clears both words of the pair.
static void d2game_unlink_pair(void* entry, uint32_t pairOff)
{
    const int32_t head = (int32_t)(intptr_t)rdptr(entry, pairOff);
    if (head == 0) return;
    const uint32_t link = rd32(entry, pairOff + 4);
    uint8_t* slot;
    if ((int32_t)link < 0) {
        slot = (uint8_t*)(uintptr_t)(uint32_t)~link;
    } else {
        slot = (uint8_t*)entry + pairOff + (link - rd32((const void*)(intptr_t)head, 4));
    }
    *(uint32_t*)(void*)slot = (uint32_t)head;
    wr32((void*)(intptr_t)head, 4, link);
    wr32(entry, pairOff, 0);
    wr32(entry, pairOff + 4, 0);
}

// 1.13c D2Game+0x2250
// Take EAX's record (explicit here) out of two intrusive chains: the pair at +0x04/+0x08, then the pair
// at +0x0C/+0x10. The second pair is examined only when the first pair's second word is nonzero.
void D2Game_UnlinkHashEntry(void* entry)
{
    if (rd32(entry, 8) == 0) return;
    d2game_unlink_pair(entry, 4);
    d2game_unlink_pair(entry, 0x0C);
}

// ---- game unit-list sweeps -----------------------------------------------------------------------
// Host seams for the callees whose implementations live outside this batch (address = retail RVA).
// SUNIT_RemoveUnit unlinks the unit from the hash bucket itself, which is why only the monster sweep
// below also stores the next pointer into the bucket head.
struct D2GameBatch10Seams {
    void (*removeUnit)(void* game, void* unit);   // 0xE20A0, SUNIT_RemoveUnit
};

// Game layout (1.13c): +0x1120 pUnitList[5][128] (row stride 0x200: players, monsters, objects,
// items, missiles), +0x1B20 pTileList. Unit layout (1.10f D2UnitStrc): +0xE4 pListNext.
static constexpr uint32_t kGameUnitList = 0x1120;
static constexpr uint32_t kGameTileList = 0x1B20;
static constexpr uint32_t kUnitListNext = 0xE4;
static constexpr uint32_t kUnitListRows = 0x80;

// 1.13c D2Game+0x38860
// PLAYER_RemoveAllPlayers: 0x80 buckets of unit list row 0 (game+0x1120); the next pointer is read
// before the unit is removed, and the bucket head is left alone because removal unlinks the unit.
void D2Game_RemoveAllPlayers(void* game, const D2GameBatch10Seams& s)
{
    for (uint32_t i = 0; i < kUnitListRows; ++i) {
        void* unit = rdptr(game, kGameUnitList + i * 4);
        while (unit != nullptr) {
            if (unit == nullptr) d2game_fatal(0xDF);   // retail's redundant null check
            void* next = rdptr(unit, kUnitListNext);
            s.removeUnit(game, unit);
            unit = next;
        }
    }
}

// 1.13c D2Game+0x6FEC0
// MISSILES_RemoveAll: the same sweep over unit list row 4 (game+0x1920).
void D2Game_RemoveAllMissiles(void* game, const D2GameBatch10Seams& s)
{
    for (uint32_t i = 0; i < kUnitListRows; ++i) {
        void* unit = rdptr(game, kGameUnitList + 4 * 0x200 + i * 4);
        while (unit != nullptr) {
            if (unit == nullptr) d2game_fatal(0xDF);
            void* next = rdptr(unit, kUnitListNext);
            s.removeUnit(game, unit);
            unit = next;
        }
    }
}

// 1.13c D2Game+0xB0640
// MONSTER_RemoveAll: unit list row 1 (game+0x1320), and here the next pointer is stored into the bucket
// head before removal (1.10f MONSTER_RemoveAll does the same).
void D2Game_RemoveAllMonsters(void* game, const D2GameBatch10Seams& s)
{
    for (uint32_t i = 0; i < kUnitListRows; ++i) {
        void** bucket = (void**)((uint8_t*)game + kGameUnitList + 0x200 + i * 4);
        void* unit = *bucket;
        while (unit != nullptr) {
            if (unit == nullptr) d2game_fatal(0xDF);
            void* next = rdptr(unit, kUnitListNext);
            *bucket = next;
            s.removeUnit(game, unit);
            unit = next;
        }
    }
}

// 1.13c D2Game+0xE21F0
// LEVEL_RemoveAllUnits' tile half: game+0x1B20 is a single pTileList head walked through +0xE4; the head
// is cleared after the walk. The decompile fatal-checks the game pointer first (0x12E).
void D2Game_RemoveAllTiles(void* game, const D2GameBatch10Seams& s)
{
    if (game == nullptr) d2game_fatal(0x12E);
    void* tile = rdptr(game, kGameTileList);
    while (tile != nullptr) {
        void* next = rdptr(tile, kUnitListNext);
        s.removeUnit(game, tile);
        tile = next;
    }
    wr32(game, kGameTileList, 0);
}

// ---- inactive-unit compressor --------------------------------------------------------------------
// 0x209B0 is the non-monster half of the 1.10f SUNITINACTIVE_CompressUnitIfNeeded: type 1 (monster)
// leaves for 0x40760, everything else follows the 1.10f else-branch shape. The Fog/Storm ordinals are
// host seams in retail order; names follow the 1.10f body (reference/D2MOO/source/D2Game/src/UNIT/
// SUnitInactive.cpp:573). Event nodes: +0x00 type (ENDANIM = 1), +0x24 next.
struct D2GameBatch10UnitSeams {
    uint32_t (*getRoom)(void* unit);                        // Ordinal 10331, unit in ESI
    uint32_t (*getLevelIdFromRoom)(uint32_t room);          // Ordinal 10826
    int32_t  (*getSaveMonsters)(uint32_t levelId);          // Ordinal 10929
    void     (*compressMonster)(void* game, void* unit);    // 0x40760, the monster half
    int32_t  (*checkPlayerBody)(void* unit);                // Ordinal 10494, STATES_CheckState(STATE_PLAYERBODY)
    const void* (*getObjectRecord)(uint32_t classId);       // Ordinal 10688, DATATBLS_GetObjectsTxtRecord
    uint32_t (*checkSparklyChest)(void* unit);              // 0xDF6C0, D2GAME_CheckIfSparklyChest
    void     (*deleteEvent)(void* game, void* event);       // 0x8E6F0, event in EAX, game in EDI
    void     (*compressInactiveUnit)(void* game, void* unit); // 0x404E0
    void     (*updatePath)(void* unit);                     // Ordinal 10650, UNITROOM_UpdatePath
    void     (*removeUnit)(void* game, void* unit);         // 0xE20A0
};

// Unit layout (1.10f D2UnitStrc): +0x00 dwUnitType, +0x04 dwClassId, +0x10 dwAnimMode, +0xC4 dwFlags,
// +0xC8 dwFlagEx, +0xDC pTimer (event chain). Object table record: +0x173 nRestore, +0x174
// nRestoreVirgins.
static constexpr uint32_t kUnitType = 0x00;
static constexpr uint32_t kUnitClassId = 0x04;
static constexpr uint32_t kUnitAnimMode = 0x10;
static constexpr uint32_t kUnitFlags = 0xC4;
static constexpr uint32_t kUnitFlagEx = 0xC8;
static constexpr uint32_t kUnitEvents = 0xDC;
static constexpr uint32_t kEventType = 0x00;
static constexpr uint32_t kEventNext = 0x24;
static constexpr uint32_t kEventTypeEndAnim = 1;
static constexpr uint32_t kUnitFlagIsResurrect = 0x2000000;
static constexpr uint32_t kUnitFlagExHasBeenDeleted = 0x100;

// Delete every ENDANIM node of the unit's event chain; the loop reads the next pointer before the node
// goes away. Type 1 in retail's switch on the object/player bodies.
static void d2game_deleteEndAnimEvents(void* game, void* unit, const D2GameBatch10UnitSeams& s)
{
    void* event = rdptr(unit, kUnitEvents);
    while (event != nullptr) {
        const uint8_t type = rd8(event, kEventType);
        void* next = rdptr(event, kEventNext);
        if (type == kEventTypeEndAnim) s.deleteEvent(game, event);
        event = next;
    }
}

// 1.13c D2Game+0x209B0
void D2Game_CompressUnitIfNeeded(void* game, void* unit, const D2GameBatch10UnitSeams& s)
{
    const uint32_t type = rd32(unit, kUnitType);
    const uint32_t classId = rd32(unit, kUnitClassId);
    if (type == 1) {                                  // UNIT_MONSTER: the 1.13c split-off half
        s.compressMonster(game, unit);
        return;
    }
    const uint32_t room = s.getRoom(unit);            // ordinal 10331
    const int32_t saveMonsters = s.getSaveMonsters(s.getLevelIdFromRoom(room));   // 10826 -> 10929
    bool compress = saveMonsters != 0 || (rd32(unit, kUnitFlags) & kUnitFlagIsResurrect) != 0;
    switch (type) {
    case 0:                                           // UNIT_PLAYER
        if (s.checkPlayerBody(unit) != 0) {           // ordinal 10494
            d2game_deleteEndAnimEvents(game, unit, s);
            wr32(unit, kUnitFlagEx, rd32(unit, kUnitFlagEx) | kUnitFlagExHasBeenDeleted);
            s.compressInactiveUnit(game, unit);       // 0x404E0
            s.updatePath(unit);                       // ordinal 10650
            return;
        }
        break;
    case 2: {                                         // UNIT_OBJECT
        if (classId == 0x3B || classId == 0x3C) {     // town portal, permanent town portal
            d2game_deleteEndAnimEvents(game, unit, s);
            wr32(unit, kUnitFlagEx, rd32(unit, kUnitFlagEx) | kUnitFlagExHasBeenDeleted);
            s.compressInactiveUnit(game, unit);
            s.updatePath(unit);
            return;
        }
        const void* record = s.getObjectRecord(classId);      // ordinal 10688
        if (rd8(record, 0x173) == 0 || (s.checkSparklyChest(unit) & 2) != 0) compress = false;
        if (rd8(record, 0x174) != 0 && rd32(unit, kUnitAnimMode) != 0) compress = false;
        d2game_deleteEndAnimEvents(game, unit, s);
        // Retail also stores the game pointer into the word that the remove test below reads; the test
        // is "type != 4" and this path's type is 2, so that store cannot change the outcome.
        break;
    }
    case 4:                                           // UNIT_ITEM
    case 5:                                           // UNIT_TILE
        // The case-4/5 edge jumps past the `if (bCompress)` guard, so the compress is unconditional.
        s.compressInactiveUnit(game, unit);
        break;
    default:                                          // anything else skips both
        break;
    }
    if (compress && type != 4 && type != 5) s.compressInactiveUnit(game, unit);
    if (type != 4) s.removeUnit(game, unit);
}

// ---- CRC32 core ----------------------------------------------------------------------------------
// 1.13c D2Game+0xB470
// CRC-32 over size bytes: crc = 0xFFFFFFFF; per byte crc = (crc >> 8) ^ tbl[(byte ^ crc) & 0xFF]; the
// return is the complement. The retail table at 0x6FD1AA78 is the standard polynomial 0xEDB88320 table,
// reproduced here bitwise so no 1 KB module global is restated. A zero size is fatal (0x40).
uint32_t D2Game_Crc32(const uint8_t* data, int32_t size)
{
    if (size == 0) d2game_fatal(0x40);
    uint32_t crc = 0xFFFFFFFFu;
    do {
        crc ^= *data++;
        for (int k = 0; k < 8; ++k) crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
        --size;
    } while (size != 0);
    return ~crc;
}
