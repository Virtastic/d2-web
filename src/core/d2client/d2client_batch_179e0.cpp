// Stage A: client monster decisions and room population. Prior art: ,
// D2MOO has no D2Client module; community pointer tables only identify imported contracts.
// Derived from 1.13c; instruction checks restore the register arguments, signed offsets,
// and class switch lost by Ghidra's 17F70 constant propagation. No parity verdict yet.
#include <cstdint>
#include <cstring>
#include <cstddef>

namespace d2client179e0 {
// All object references are explicit retail 32-bit addresses. The resolver maps these
// into native memory without assuming that a wasm64 pointer fits a retail field.
struct UnitPrefix {
    uint32_t type, classId, pool, id, mode, data, act, actData, seedLo, seedHi, initSeed, path;
};
static_assert(offsetof(UnitPrefix, mode) == 0x10);
static_assert(offsetof(UnitPrefix, data) == 0x14);
static_assert(offsetof(UnitPrefix, seedLo) == 0x20);
static_assert(offsetof(UnitPrefix, path) == 0x2c);
struct Bounds { uint32_t x, y, width, height; };
struct Spawn { uint32_t id, classId, x, y, zero0, zero1, active; };
static_assert(sizeof(Spawn) == 0x1c);
struct Env {
    uint8_t* (*memory)(uint32_t);
    uint32_t player;                    // global D2Client+0x11BBFC
    uint32_t tables;                    // *sgptDataTables
    uint32_t bitMask0;                  // gdwBitMasks[0]
    uint32_t* nextId;                   // global D2Client+0xDE834
    int32_t (*blocked)(uint32_t);       // 16CB0, EAX=unit
    void (*idle)();                     // 17270
    void (*blockedAction)(uint32_t);    // 17300, EDI=target
    int32_t (*action)(uint32_t,uint32_t,int32_t,int32_t,int32_t); // 17500 target,unit,mode,speed,ticks
    void (*mode)(int32_t,int32_t,int32_t); // 173E0 mode,speed,ticks
    void (*search)(uint32_t,uint32_t,uint32_t*,int32_t*); // 16C20 unit,predicate,out unit,out distance
    uint32_t (*flag)(uint32_t);         // 4C2C0, returns address of mutable flag
    void (*targetAction)(uint32_t,uint32_t,int32_t,int32_t); // 176A0 unit,target,0,6
    int32_t (*test10648)(uint32_t);
    int32_t (*stat10522)(uint32_t,uint32_t);
    void (*event)(uint32_t,uint32_t,uint32_t,uint32_t); // A6EF0
    uint32_t (*pathX)(uint32_t);         // Common#10867
    uint32_t (*pathY)(uint32_t);         // Common#10750
    void (*pathLimit)(uint32_t,int32_t); // Common#10168
    void (*move)(uint32_t,uint32_t,int32_t,int32_t); // 17390 EAX=x,ECX=y,stack speed,flags
    void (*pathMask)(uint32_t,uint32_t); // Common#11056
    uint32_t (*findTarget)(uint32_t,int32_t*); // 16D80
    void (*farAction)(uint32_t,int32_t); // 16FE0
    int32_t (*classIs)(uint32_t,int32_t); // 1760
    void (*special)(uint32_t,uint32_t); // RVA,unit (17710,17010,17800)
    void (*action17950)(uint32_t,uint32_t,uint32_t); // unit,target,distance
    void (*roomBounds)(uint32_t,Bounds*); // Common#10561
    int16_t (*collision)(uint32_t,uint32_t,uint32_t,int32_t,uint32_t); // Common#10573
    void (*spawn)(const Spawn*,int32_t,int32_t,int32_t,int32_t,int32_t); // A7780
    uint32_t (*room10826)(uint32_t);
    uint32_t (*level10014)(uint32_t);
    int32_t (*index10601)(uint32_t);
    uint32_t (*get10331)(uint32_t);
    uint32_t (*get10596)(uint32_t);
    void (*update10562)(uint32_t,uint32_t,uint32_t,uint32_t);
};
template<class T> T read(Env& e,uint32_t p) { T v; std::memcpy(&v,e.memory(p),sizeof v); return v; }
template<class T> void write(Env& e,uint32_t p,T v) { std::memcpy(e.memory(p),&v,sizeof v); }
uint32_t word(Env& e,uint32_t p,uint32_t o) { return read<uint32_t>(e,p+o); }
uint32_t step(Env& e,uint32_t p) {
    uint64_t s=uint64_t(word(e,p,0))*0x6ac690c5u+word(e,p,4);
    write(e,p,uint32_t(s)); write(e,p+4,uint32_t(s>>32)); return uint32_t(s);
}
uint32_t random(Env& e,uint32_t p,uint32_t n) {
    if (int32_t(n)<1) return 0;
    uint32_t v=step(e,p); return n&(n-1) ? v%n : v&(n-1);
}
uint32_t delayAddress(Env& e,uint32_t u) {
    if (!u || word(e,u,0)!=1) return 0;
    uint32_t p=word(e,u,0x14); return p ? p+0x30 : 0;
}
void delay(Env& e,uint32_t u,int32_t ticks) { uint32_t p=delayAddress(e,u); if(p) write(e,p,ticks); }
uint32_t coord(Env& e,uint32_t u,bool y) {
    int32_t t=int32_t(word(e,u,0)); uint32_t p=word(e,u,0x2c);
    if(t==2 || (t>=4 && t<=5)) return word(e,p,y?0x10:0xc);
    return p ? (y?e.pathY(p):e.pathX(p)) : 0;
}
}
using d2client179e0::Env;

// 1.13c D2Client+0x179E0
extern "C" void d2_client_179e0(uint32_t unit,uint32_t target,uint32_t distance,Env* env) {
    using namespace d2client179e0; auto& e=*env;
    if(!unit) return;
    uint32_t mode=word(e,unit,0x10);
    if(mode==0 || mode==12) return;
    if(distance<2 && e.blocked(unit)) { e.blockedAction(target); return; }
    if(mode!=1) return;
    if(distance<4) { e.action(target,unit,6,1,0); return; }
    uint32_t near=0; int32_t unused=0;
    if(distance>7) e.search(unit,0x16dd0,&near,&unused);
    uint32_t flag=e.flag(unit);
    if(flag) {
        if(near && distance>=8 && read<int32_t>(e,flag)==0) {
            e.targetAction(unit,near,0,6); write(e,flag,int32_t(1)); return;
        }
        if(read<int32_t>(e,flag)==0) {
            if(random(e,unit+0x20,100)>=75) { e.mode(5,1,0); return; }
            delay(e,unit,30); e.idle(); return;
        }
        if(random(e,unit+0x20,100)<20) write(e,flag,int32_t(0));
    }
    delay(e,unit,32); e.idle();
}

// 1.13c D2Client+0x17B60
extern "C" void d2_client_17b60(uint32_t unit,uint32_t target,Env* env) {
    using namespace d2client179e0; auto& e=*env;
    uint32_t mode=unit?word(e,unit,0x10):0;
    if(e.test10648(unit)) return;
    if(e.blocked(unit)) { e.blockedAction(target); return; }
    if(mode!=1) return;
    if(step(e,unit+0x20)%100<40) { delay(e,unit,20); e.idle(); }
    else e.mode(4,1,0);
}

// 1.13c D2Client+0x17C00
extern "C" void d2_client_17c00(uint32_t unit,Env* env) {
    using namespace d2client179e0; auto& e=*env;
    uint32_t player=e.player;
    if(!unit || word(e,unit,0x10)!=1 || word(e,unit,0)!=1) return;
    uint32_t data=word(e,unit,0x14); if(!data || !(data+0x34)) return;
    int32_t stat=e.stat10522(unit,player);
    if(read<int32_t>(e,data+0x34) && stat>25) {
        uint32_t id=word(e,unit,0xc); e.event(0x109c08,id&127,id,1); return;
    }
    write(e,data+0x34,int32_t(1));
    uint32_t first=step(e,unit+0x20)&1;
    uint32_t rx,ry;
    if(first) { rx=40; ry=random(e,unit+0x20,40); }
    else { rx=random(e,unit+0x20,40); ry=40; }
    if(step(e,unit+0x20)&1) rx=0u-rx;
    if(step(e,unit+0x20)&1) ry=0u-ry;
    uint32_t x=coord(e,player,false)+rx, y=coord(e,player,true)+ry;
    uint32_t ux=coord(e,unit,false), uy=coord(e,unit,true);
    e.pathLimit(word(e,unit,0x2c),90);
    e.move(3*x-2*ux,3*y-2*uy,1,0);
}

// 1.13c D2Client+0x17E10
extern "C" void d2_client_17e10(uint32_t unit,uint32_t target,uint32_t distance,Env* env) {
    using namespace d2client179e0; auto& e=*env;
    if(!unit || word(e,unit,0x10)!=1) return;
    if(distance>=5) { e.mode(9,1,0); return; }
    if(step(e,unit+0x20)%100<66) e.action(target,unit,8,1,30);
    else e.mode(12,1,30);
}

// 1.13c D2Client+0x17E80
extern "C" void d2_client_17e80(uint32_t unit,Env* env) {
    using namespace d2client179e0; auto& e=*env;
    if(!unit || word(e,unit,0x10)!=1) return;
    uint32_t target=0; int32_t distance=0x7fffffff;
    e.search(unit,0x16e50,&target,&distance);
    if(!target) return;
    uint32_t path=word(e,unit,0x2c); e.pathMask(path,0x3c01);
    if(distance<4) {
        e.pathMask(path,0xc01);
        if(!e.action(target,unit,4,12,120)) e.mode(4,12,120);
    } else if(step(e,unit+0x20)%100<30) e.mode(2,1,0);
    else delay(e,unit,5);
}

// 1.13c D2Client+0x17F70
extern "C" void d2_client_17f70(uint32_t unit,Env* env) {
    using namespace d2client179e0; auto& e=*env;
    uint32_t p=delayAddress(e,unit); if(!p) return;
    int32_t ticks=read<int32_t>(e,p);
    if(ticks>0) { write(e,p,ticks-1); return; }
    int32_t distance=0x7fffffff;
    uint32_t target=e.findTarget(unit,&distance);
    if(!target) target=e.player;
    if(!target) { delay(e,unit,25); e.idle(); return; }
    if(distance>30) { e.farAction(unit,distance-15); e.idle(); return; }
    uint32_t cls=word(e,unit,4);
    if(e.classIs(cls,22)) { e.special(0x17710,unit); return; }
    // Sparse switch decoded from retail's byte-index table; omitted cases idle.
    switch(cls) {
    case 0x95: case 0x10c: d2_client_17e80(unit,env); return;
    case 0x97: case 0x10d: case 0x11b: case 0x153:
        d2_client_179e0(unit,target,uint32_t(distance),env); return;
    case 0x9d: case 0x9e: case 0x13f: d2_client_17c00(unit,env); return;
    case 0x9f: d2_client_17e10(unit,target,uint32_t(distance),env); return;
    case 0xe3: d2_client_17b60(unit,target,env); return;
    case 0x13e: e.action17950(unit,target,uint32_t(distance)); return;
    case 0x22c: e.special(0x17800,unit); return;
    case 0x23e: e.special(0x17010,unit); return;
    case 0x116: case 0x117: case 0x118: case 0x119: case 0x11a: return;
    default: e.idle(); return;
    }
}

// 1.13c D2Client+0x181A0
extern "C" void d2_client_181a0(uint32_t room,int32_t classId,uint32_t seed,Env* env) {
    using namespace d2client179e0; auto& e=*env;
    Bounds b; e.roomBounds(room,&b);
    if(classId<0 || classId>=int32_t(word(e,e.tables,0xa80))) return;
    uint32_t mon=word(e,e.tables,0xa78)+uint32_t(classId)*0x1a8;
    int32_t index=read<int16_t>(e,mon+0x18);
    if(index<0 || index>=int32_t(word(e,e.tables,0xa98))) return;
    uint32_t mon2=word(e,e.tables,0xa90)+uint32_t(index)*0x134; if(!mon2) return;
    int32_t size=read<int8_t>(e,mon2+8);
    for(int attempt=0;attempt<10;++attempt) {
        uint32_t x=b.x+random(e,seed,b.width-1), y=b.y+random(e,seed,b.height-1);
        if(e.collision(room,x,y,size,0x3f11)) continue;
        uint32_t id=++*e.nextId; if(id==0xffffffffu) id=*e.nextId=0;
        Spawn s{id,uint32_t(classId),x,y,0,0,1};
        e.spawn(&s,0,0,0,0,1); return;
    }
}

// 1.13c D2Client+0x18370
extern "C" void d2_client_18370(uint32_t room,Env* env) {
    using namespace d2client179e0; auto& e=*env;
    uint32_t r=e.room10826(room); if(!r) return;
    uint32_t level=e.level10014(r), seed=room?room+0x6c:0;
    for(uint32_t i=0;i<4;++i) {
        uint32_t p=level+0xcc+i*2;
        int32_t cls=read<int16_t>(e,p); if(cls<0) return;
        uint32_t chance=uint32_t(int32_t(read<int16_t>(e,p+8)));
        if(step(e,seed)%100>=chance) continue;
        uint32_t mon=cls<int32_t(word(e,e.tables,0xa80)) ? word(e,e.tables,0xa78)+uint32_t(cls)*0x1a8 : 0;
        uint32_t min=read<uint8_t>(e,mon+0x2f), max=read<uint8_t>(e,mon+0x30);
        uint32_t count=min+random(e,seed,max-min);
        int32_t mult=read<int16_t>(e,p+0x10); if(mult) count*=uint32_t(mult);
        for(int32_t left=int32_t(count);left>0;--left) d2_client_181a0(room,cls,seed,env);
    }
}

// 1.13c D2Client+0x18650
extern "C" uint32_t d2_client_18650(uint32_t unit,Env* env) {
    using namespace d2client179e0; auto& e=*env;
    if(!unit) return 0;
    int32_t i=e.index10601(unit);
    uint32_t row=(i>=0 && i<int32_t(word(e,e.tables,0xba0))) ? word(e,e.tables,0xb98)+uint32_t(i)*0x23c : 0;
    return (read<uint8_t>(e,row+5)&uint8_t(e.bitMask0)) ? 1u:0u;
}

// 1.13c D2Client+0x18920
extern "C" void d2_client_18920(uint32_t unit,Env* env) {
    using namespace d2client179e0; auto& e=*env;
    uint32_t x=coord(e,unit,false), y=coord(e,unit,true);
    uint32_t a=e.get10331(unit), b=e.get10596(unit);
    e.update10562(a,x,y,b);
}
