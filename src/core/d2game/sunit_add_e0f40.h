// 1.13c D2Game+0xE0F40 SUNIT_Add (EAX=pRoom, ECX=nY, EDX=pUnit, stack nX, pGame, a6; ret 0xc)
// Rule over an environment E (hookenv.h): every callee is one E::h(rva, ...) call that logs what the capture recorded
// and returns the pinned value, because none of them can run on a blob (they want a live memory pool, collision grid,
// path allocator and packed-pointer unit list). Pinned returns: 0xA556 O10826 level id (1), 0xA56E O10459 act,
// 0xA520 O10688 the objects.txt record the collision byte is read from, 0xDF9A0 the room/clients test, 0xA478 O10331
// UNITS_GetRoom. Fields go through E::rd32/rd8/wr32 on the retail-layout blob. The 1.13c tail (the unit-list insert)
// is the callee 0xE0310. Returns a6, which is what retail leaves in eax.
#pragma once
#include <cstdint>

namespace e0f40 {
// one recorded callee call, stack words s0..s3 (the recorder logs at most four)
template <class E> static inline uint32_t C(E& e, uint32_t rva, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0)
{ return e.h(rva, 0, 0, 0, 0, 0, s0, s1, s2, s3); }

template <class E> uint32_t Add(E& e, uint32_t pUnit, uint32_t nX, uint32_t nY, uint32_t pGame, uint32_t pRoom, uint32_t a6)
{
    const uint32_t act = C(e, 0xA56E, C(e, 0xA556, pRoom));       // DRLG_GetActNoFromLevelId(DUNGEON_GetLevelIdFromRoom(pRoom))
    e.wr32(pUnit + 0x18, act);                                    // pUnit->nAct
    e.wr32(pUnit + 0x1C, e.rd32(pGame + 0xBC + 4 * act));         // pUnit->pDrlgAct = pGame->pAct[nAct]

    const uint32_t type = e.rd32(pUnit), mode = e.rd32(pUnit + 0x10);
    switch (type)
    {
    case 0:  // player: dynamic path, then the player init
        C(e, 0xA66A, e.rd32(pGame + 0x1C), pRoom, nX, nY);        // O11003 PATH_AllocDynamicPath(pool, room, nX, nY, ...)
        C(e, 0x37E20, nX, nY);                                    // player init(pGame, pUnit, nX, nY)
        break;
    case 1:  // monster: dynamic path, velocity/position, then the clients test
        C(e, 0xA66A, e.rd32(pGame + 0x1C), pRoom, nX, nY);
        C(e, 0xAFAB0, pGame, nX, nY);                             // MONSTER_SetVelocityAndPosition(pGame, ..., nX, nY)
        if (C(e, 0xDF9A0)) C(e, 0xAF920);                         // MONSTER_UpdateAiCallbackEvent(pGame, pUnit)
        else C(e, 0xE0860, pGame);                                // the detach (1.10f sub_6FCBC300)
        break;
    case 2: {  // object: objects.txt record, static path, collision when the record says so, room add
        const uint32_t rec = C(e, 0xA520, e.rd32(pUnit + 4));     // O10688 objects.txt record for dwClassId
        C(e, 0xA5F2, pUnit, pRoom, nX, nY);                       // O10574 UNITS_InitializeStaticPath
        if (e.rd8(rec + 0x120 + mode)) C(e, 0xA69A, pUnit, pRoom, nX, nY);   // O10032 UNITS_BlockCollisionPath
        C(e, 0xA6FA, pUnit, pRoom);                               // UNITROOM_AddUnitToRoom
        break; }
    case 3:  // missile: the dynamic path and nothing else
        C(e, 0xA66A, e.rd32(pGame + 0x1C), pRoom, nX, nY);
        break;
    case 4:  // item: only on the ground (mode 3)
        if (mode == 3) {
            C(e, 0xA5F2, pUnit, pRoom, nX, nY);
            C(e, 0xA69A, pUnit, pRoom, nX, nY);
            if (pRoom) C(e, 0xA62E, pUnit, pRoom, 0);             // UNITROOM_AddUnitToRoomEx
        }
        break;
    case 5:  // tile: static path, collision, room add
        C(e, 0xA5F2, pUnit, pRoom, nX, nY);
        C(e, 0xA69A, pUnit, pRoom, nX, nY);
        C(e, 0xA6FA, pUnit, pRoom);
        break;
    default:  // any other type asserts (FOG_DisplayAssert) and calls exit(-1); the descriptor has no such tuple,
        abort();  // so fail loudly rather than invent a log entry for a call the capture never recorded
    }

    C(e, 0xE0310);                                                // the 1.13c unit-list insert (pGame in edx, pUnit in esi)
    if (C(e, 0xA478, pUnit)) C(e, 0xA4DE, pUnit);                 // UNITS_GetRoom -> UNITROOM_RefreshUnit
    if (!a6) C(e, 0xA6E8, pUnit, 1);                              // UNITS_SetCurrentRoomInvalid(pUnit, 1)
    return a6;
}
}  // namespace e0f40
