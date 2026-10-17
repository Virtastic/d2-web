#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>
#include "../d2moo/include/Units/Units.h"
#include "../d2moo/include/DataTbls/LevelsTbls.h"

// 1.13c D2Client+0x226A0: the class 0..6 of the room record under a unit.
// eax = pUnit. Reads DAT_6fbcbbfc (0x11BBFC), DAT_6fbcc254 (0x11C254), DAT_6fbcc258 (0x11C258); writes none.
// The callees live behind the caller: the ops are the six D2Common import thunks of D2Client, 0x21ED0 (the
// room-record lookup, UNMAPPED) and, in-lined here, 0x21E80 (ours, pure_leaves_b7.cpp). Call order matters --
// it is the trace the vector records.
//
// The tree's wasm64 structs widen every pointer field ahead of an offset by sizeof(void*) - 4; the code
// keeps retail's 32-bit constants because the captured unit/path image IS the 32-bit one.
static_assert(offsetof(D2UnitStrc, dwUnitType) == 0x00, "unit+0x00 is dwUnitType");
static_assert(offsetof(D2UnitStrc, pDynamicPath) == 0x2C + 3 * (sizeof(void*) - 4), "unit+0x2C is the path");
static_assert(offsetof(D2StaticPathStrc, tGameCoords) == 0x0C + 1 * (sizeof(void*) - 4), "static path +0xC");
static_assert(offsetof(D2CoordStrc, nX) == 0 && offsetof(D2CoordStrc, nY) == 4, "tGameCoords x,y");
static_assert(offsetof(D2LevelsTxt, dwSoundEnv) == 0x21C, "levels.txt record +0x21C is the level index byte");
struct D2LevelClassRecord { uint8_t pad[0x1C]; int32_t cls; uint8_t pad2[0x38]; };   // 0x58-stride table
static_assert(offsetof(D2LevelClassRecord, cls) == 0x1C && sizeof(D2LevelClassRecord) == 0x58, "level class record");

enum {
    OP_ROOM       = 0xC1F0,   // ord 10331 UNITS_GetRoom: unit -> room, returns the room
    OP_ACT        = 0xC1DE,   // ord 10826 sub_3C000: room -> level
    OP_LEVELS     = 0xC214,   // ord 10014 DATATBLS_GetLevelsTxtRecord
    OP_PD_V1      = 0xC3BE,   // ord 10867 sub_34BB0 (a0 = pItemData)
    OP_PD_V2      = 0xC2EC,   // ord 10750 sub_34B80 (a0 = pItemData)
    OP_NEIGHBOURS = 0xC3DC,   // ord 10383 sub_3BA20: (room, &arr, &count)
    OP_TILE       = 0x21ED0,  // a0 = v2 (eax), a1 = v1 (ecx), a2 = room (edx) -> the room record, or 0
};
typedef uint32_t (*D2Client226A0Cb)(void* ctx, uint32_t op, uint32_t a0, uint32_t a1, uint32_t a2);

uint32_t D2Client_FlagClass18_6(const uint8_t* o);   // 0x21E80, src/core/d2client/pure_leaves_b7.cpp

static uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_226a0(uint32_t unit, uint32_t game, uint32_t levelTbl,
                                                         uint32_t levelCnt, D2Client226A0Cb cb, void* ctx) {
    const uint8_t* u = (const uint8_t*)(uintptr_t)unit;
    uint32_t lvl = 0;                                        // 0x226A8: game -> room -> level -> LevelsTxt[+0x21C]
    if (game) {
        uint32_t r1 = cb(ctx, OP_ROOM, game, 0, 0);          // 0x226B3
        if (r1) {
            uint32_t r2 = cb(ctx, OP_ACT, r1, 0, 0);         // 0x226BD
            if (r2) lvl = ((const uint8_t*)(uintptr_t)cb(ctx, OP_LEVELS, r2, 0, 0))[0x21C];   // 0x226D0
        }
    }
    uint32_t ebp = 1;                                        // 0x226F8
    if (levelTbl && (int32_t)lvl < (int32_t)levelCnt) {      // 0x226DF: the `jl` on a movzx byte is dead
        int32_t t = (int32_t)rd32((const uint8_t*)(uintptr_t)(levelTbl + lvl * 0x58), 0x1C);
        if (t > 0 && t < 7) ebp = (uint32_t)t;
    }
    uint32_t type = rd32(u, 0);                              // 0x2270D
    uint32_t pd = rd32(u, 0x2C);
    bool st = (type == 2 || (type > 3 && type < 6));         // 0x2270F: {2,4,5}
    uint32_t v1 = st ? rd32((const uint8_t*)(uintptr_t)pd, 0xC)                              // 0x22736
                     : pd ? cb(ctx, OP_PD_V1, pd, 0, 0) : 0;                                 // 0x22726 / 0x2272F
    uint32_t v2 = st ? rd32((const uint8_t*)(uintptr_t)pd, 0x10)                             // 0x22762
                     : pd ? cb(ctx, OP_PD_V2, pd, 0, 0) : 0;                                 // 0x22752 / 0x2275B
    uint32_t room = cb(ctx, OP_ROOM, unit, 0, 0);            // 0x22766
    if (!room) return 0;
    uint32_t rec = cb(ctx, OP_TILE, v2, v1, room);           // 0x2277F: eax=v2, ecx=v1, edx=room
    if (!rec) {
        uint32_t arr = 0; int32_t cnt = 0;                   // 0x22793: (room, &arr, &count)
        cb(ctx, OP_NEIGHBOURS, room, (uint32_t)(uintptr_t)&arr, (uint32_t)(uintptr_t)&cnt);
        if (cnt <= 0) return ebp;                            // 0x2279E signed
        const uint32_t* a = (const uint32_t*)(uintptr_t)arr;
        for (int32_t i = 0; i < cnt && !rec; ++i) rec = cb(ctx, OP_TILE, v2, v1, a[i]);   // 0x227A2..0x227BD
        if (!rec) return ebp;
    }
    uint32_t cls = D2Client_FlagClass18_6((const uint8_t*)(uintptr_t)rec);   // 0x227C9
    return cls ? cls : ebp;                                  // 0x227D0
}
