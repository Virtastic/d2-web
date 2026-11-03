// D2Client batch 0x40450: skill-record gates and effect spawns. Derived from cached 1.13c code.
// Ordinals and unresolved callees are host callbacks; sgptDataTables is a pinned view.
#include <cstdint>
#include <cstring>
#include "skill_gates_40450.h"
namespace d2client_b40450 {
using U=uint32_t; using I=int32_t; using P=void*;
static U rd(const void* p,U n){U v;std::memcpy(&v,static_cast<const uint8_t*>(p)+n,4);return v;}
struct DT { uint8_t* skills; I skillCount; I c4; I bd4; I bc0; }; // +0xb98,+0xba0,+0xc4,+0xbd4,+0xbc0
struct Calls {
 DT dt;
 P (*unitData)(P);        // Ordinal_10871 / 10392 (unit+0x2c resolve)
 I (*skillLevel)(P,I,I);  // Ordinal_10680
 U (*fail)();             // Ordinal_10786
 void (*fatal)(P);        // 0x2A0B0 path + _exit
 P (*resolve)(U);         // Ordinal_10392
 P (*getOwner)(P);        // Ordinal_10511
 I (*listCount)(P);       // Ordinal_10980
 void (*listSet)(P,I);    // Ordinal_10243
 void (*setSkill)(I,U);   // Ordinal_11169
 void (*cast)(P tgt,P u,I s,I m,I id,U arg); // 0xB1020
 I (*castBegin)(I*,P);      // 0xB1340 (stack=&out0, edi=&out1, ebx=unit)
 void (*castEnd)(P u,I s,I id,U arg); // 0xB1540 (eax=unit)
 I (*isDead)(P);          // Ordinal_10057(Ordinal_10331(x))
 U (*check562a0)(P tgt,P u); // 0xB62A0 (eax=target, stack=unit)
 U (*spawn)(U*);          // 0xB30F0 / 0x630F0 returns handle
 void (*setField)(P,U);   // Ordinal_10637
 void (*setPos)(P,U);     // Ordinal_10019
 U (*rand)();             // Ordinal_10513
 // 0x40CA0 tail (all host callbacks; register args are the decompile's unaff_ESI / implicit)
 I (*o10494)(); void (*fx1b300)(P,I,I,I,I); void (*fx38a70)(I,I,I,I); P (*o10871)();
 P (*o11013)(I,I,I,I,I); P (*o10630)(); void (*o10807)(); void (*o10911)(P,U); void (*o10785)(P,I);
 void (*o10945)(); void (*o10542)(); void (*o11043)(P,I);
};
// retail: the 0x2A0B0 call, then fatal assert 0x946 only when unit+0x2c is ZERO; a non-zero id that resolves to null just fails the caller.
static P unitRes(const Calls& c,P u,bool& fatal){ U id=rd(u,0x2c); fatal=(id==0); if(fatal){c.fatal(u);return nullptr;} return c.resolve(id); }
static const uint8_t* rec(const Calls& c,I id){
 if(id<0||id>=c.dt.skillCount||!c.dt.skills)return nullptr; return c.dt.skills+id*0x23c; }
// Seam wrappers over the real functions in skill_gates_40450.cpp (the Calls table is no longer consulted).
I sub_40550(const Calls&,P unit,I id){ return d2_client_40550(static_cast<D2UnitStrc*>(unit),id); }
void sub_40DF0(const Calls&,I n,U p1,U p2,U p3,U p4,U p5,U p6){ d2_client_40df0(n,p1,p2,p3,p4,p5,p6); }
// 1.13c D2Client+0x40F00 and 0x41000: same loop over two 64-word tables (DAT_6fb862e0/e0 and 6fb86520/620)
static void ring(const Calls& c,U p1,U p2,U p3,U p4,I id,I step,const U* ta,const U* tb){
 U s[23]={}; s[0]=3; s[1]=p1; s[3]=p2; s[4]=p3; s[5]=p4; // 0x5c-based layout
 const uint8_t* r=rec(c,id);
 for(I i=0;i<0x100;i+=step*4){
  s[0x10]=ta[i/4]; s[0x0f]=tb[i/4];
  P h=reinterpret_cast<P>(uintptr_t(c.spawn(s)));
  c.setField(h,(r?rd(r,0x14c):0)+1);
 }
}
// 1.13c D2Client+0x40F00
void sub_40F00(const Calls& c,U p1,U p2,U p3,U p4,I id,I step,const U* t1,const U* t2){ ring(c,p1,p2,p3,p4,id,step,t1,t2); }
// dup of D2CLIENT+0x41000
void sub_41000(const Calls& c,U p1,U p2,U p3,U p4,I id,I step,const U* t1,const U* t2){ ring(c,p1,p2,p3,p4,id,step,t1,t2); }
}
