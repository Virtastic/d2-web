// Stage A, prior art: , D2MOO Units/Path/Dungeon types.
// D2MOO has no D2Client implementation. Read against cached 1.13c bodies and
// instructions: register arguments, two predicate output pointers, and RNG limit
// 2 are recovered from assembly. Native objects retain native pointers; the
// retail room +0x74 member maps semantically to D2ActiveRoomStrc::pUnitFirst
// (the existing D2MOO room layout is from 1.10f). No serialized image is cast here.
#include <cstddef>
#include <cstdint>
#include "../d2moo/include/Units/Units.h"
#include "../d2moo/include/Path/Path.h"
#include "../d2moo/include/Drlg/D2DrlgDrlg.h"

namespace d2client16c20 {
static_assert(offsetof(D2UnitStrc, pDynamicPath) == 0x2c + 3*(sizeof(void*)-4));
static_assert(offsetof(D2UnitStrc, dwAnimMode) == 0x10 + sizeof(void*)-4);
static_assert(offsetof(D2UnitStrc, pSeed) == 0x20 + 3*(sizeof(void*)-4));
static_assert(offsetof(D2StaticPathStrc, tGameCoords) == 0xc + sizeof(void*)-4);
static_assert(sizeof(D2CoordStrc) == 8);
struct Action { int32_t x, y, flags, speed, unknown, limit, extra; };
static_assert(sizeof(Action)==0x1c);
using Predicate = int32_t (*)(D2UnitStrc*, D2UnitStrc*, D2UnitStrc**, int32_t*);
struct Ops {
    D2ActiveRoomStrc* (*room)(D2UnitStrc*); // Common#10331
    void (*adjacent)(D2ActiveRoomStrc*, D2ActiveRoomStrc***, int32_t*); // #10383
    int32_t (*pathX)(D2DynamicPathStrc*); // #10867
    int32_t (*pathY)(D2DynamicPathStrc*); // #10750
    int32_t (*distance)(D2UnitStrc*,D2UnitStrc*); // #10522
    void (*action)(int32_t,D2UnitStrc*,const Action*); // Client+0x4DC70
};
struct State {
    D2UnitStrc* players[8]; // Client+0x11B800
    D2UnitStrc* player;    // Client+0x11BBFC
};
static int32_t coordinate(D2UnitStrc* u, bool y, const Ops& ops) {
    const int32_t type=static_cast<int32_t>(u->dwUnitType);
    if(type==2 || (type>3 && type<6))
        return y ? u->pStaticPath->tGameCoords.nY : u->pStaticPath->tGameCoords.nX;
    if(!u->pDynamicPath) return 0;
    return y ? ops.pathY(u->pDynamicPath) : ops.pathX(u->pDynamicPath);
}
}
using namespace d2client16c20;
// 1.13c D2Client+0x16C20
extern "C" int32_t d2_client_16c20(D2UnitStrc* source, Predicate predicate,
                                  D2UnitStrc** result, int32_t* distance, const Ops* ops) {
    auto* room=ops->room(source);
    if(!room) return 0;
    D2ActiveRoomStrc** rooms=nullptr; int32_t count=0;
    ops->adjacent(room,&rooms,&count);
    // Retail compares the count unsigned.
    for(uint32_t i=0;i<static_cast<uint32_t>(count);++i)
        for(auto* unit=rooms[i]->pUnitFirst;unit;unit=unit->pRoomNext) {
            const auto verdict=predicate(source,unit,result,distance);
            if(verdict) return verdict;
        }
    return 0;
}
// 1.13c D2Client+0x16CB0
extern "C" int32_t d2_client_16cb0(D2UnitStrc* unit,D2UnitStrc* other,const Ops* ops) {
    const auto x=coordinate(unit,false,*ops), y=coordinate(unit,true,*ops);
    const auto ox=coordinate(other,false,*ops), oy=coordinate(other,true,*ops);
    return x==ox && y==oy;
}
// 1.13c D2Client+0x16D80
extern "C" D2UnitStrc* d2_client_16d80(D2UnitStrc* source,int32_t* distance,
                                      const State* state,const Ops* ops) {
    D2UnitStrc* result=nullptr;
    for(auto* unit:state->players) {
        if(!unit || unit->dwAnimMode==0 || unit->dwAnimMode==12) continue;
        const auto value=ops->distance(source,unit);
        if(value<*distance) { result=unit; *distance=value; }
    }
    return result;
}
// 1.13c D2Client+0x16DD0
extern "C" int32_t d2_client_16dd0(D2UnitStrc* source,D2UnitStrc* candidate,
                                   D2UnitStrc** result,int32_t* distance,
                                   const State* state,const Ops* ops) {
    if(!candidate || candidate->dwUnitType>1) return 0;
    const auto mode=candidate->dwAnimMode;
    if(mode!=0 && mode!=(candidate->dwUnitType==0 ? 17u : 12u)) return 0;
    const auto value=ops->distance(source,candidate);
    if(value<3 || value>25 || ops->distance(candidate,state->player)<=7) return 0;
    if(SEED_RollLimitedRandomNumber(&source->pSeed,2)) {
        *distance=value; *result=candidate;
    }
    return 0;
}
// 1.13c D2Client+0x17270
extern "C" void d2_client_17270(D2UnitStrc* unit,const Ops* ops) {
    Action action{}; action.x=coordinate(unit,false,*ops); action.y=coordinate(unit,true,*ops);
    ops->action(7,unit,&action);
}
// 1.13c D2Client+0x17300
extern "C" void d2_client_17300(D2UnitStrc* unit,const Ops* ops) {
    Action action{}; action.x=coordinate(unit,false,*ops); action.y=coordinate(unit,true,*ops);
    ops->action(8,unit,&action);
}
