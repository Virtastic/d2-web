// D2Client batch 0x40450: skill-record gates and effect spawns. Derived from cached 1.13c code.
// Ordinals and unresolved callees are host callbacks; sgptDataTables is a pinned view.
#include <cstdint>
#include <cstring>
#include "skill_gates_40450.h"
namespace d2client_b40450 {
using U=uint32_t; using I=int32_t; using P=void*;
static U rd(const void* p,U n){U v;std::memcpy(&v,static_cast<const uint8_t*>(p)+n,4);return v;}
static I rs(const void* p,U n){int16_t v;std::memcpy(&v,static_cast<const uint8_t*>(p)+n,2);return v;}
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
 void (*cast)(P,I,I);     // 0xA1020
 I (*castBegin)(I*);      // 0xA1340
 void (*castEnd)();       // 0xA1540
 I (*isDead)(P);          // Ordinal_10057(Ordinal_10331(x))
 U (*check562a0)(P);      // 0xA62A0
 U (*spawn)(U*);          // 0xB30F0 / 0x630F0 returns handle
 void (*setField)(P,U);   // Ordinal_10637
 void (*setPos)(P,U);     // Ordinal_10019
 U (*rand)();             // Ordinal_10513
};
static const uint8_t* rec(const Calls& c,I id){
 if(id<0||id>=c.dt.skillCount||!c.dt.skills)return nullptr; return c.dt.skills+id*0x23c; }
// Seam wrappers over the real functions in skill_gates_40450.cpp (the Calls table is no longer consulted).
I sub_40550(const Calls&,P unit,I id){ return d2_client_40550(static_cast<D2UnitStrc*>(unit),id); }
U sub_40450(const Calls&,I id){ return U(d2_client_40450(id,nullptr,0,0)); }
// 1.13c D2Client+0x40610
U sub_40610(const Calls& c,P u){
 P d=c.resolve(rd(u,0x2c)); if(!d){c.fatal(u);return 0;}
 I t=I(rd(d,0)); return t==0||t==1;
}
// 1.13c D2Client+0x40670
U sub_40670(const Calls& c,P u){
 P o=c.getOwner(u); if(!o)return 0;
 if(!c.resolve(rd(u,0x2c))){c.fatal(u);return 0;}
 I n=c.listCount(o); if(n-1>0){c.listSet(o,n-1); std::memset(static_cast<uint8_t*>(u)+0x44,0,4);} return 1;
}
// 1.13c D2Client+0x407C0 (extra arg is the caller's return-address slot, passed as ret)
U sub_407C0(const Calls& c,P u,I id,U ret){
 if(!rec(c,id))return 0; I s=sub_40550(c,u,id); if(s<0)return 0;
 if(!c.resolve(rd(u,0x2c))){c.fatal(u);return 0;}
 c.setSkill(s,ret); c.cast(u,s,-1); return 1;
}
// 1.13c D2Client+0x40880
U sub_40880(const Calls& c,P u,I id){
 if(!rec(c,id))return 0; if(sub_40550(c,u,id)<0)return 0;
 I l[2]; if(!c.castBegin(l))return 0; c.castEnd(); return 1;
}
// 1.13c D2Client+0x40910
U sub_40910(const Calls& c,P u,I id){
 if(!c.resolve(rd(u,0x2c))){c.fatal(u);return 0;}
 if(!rec(c,id)||sub_40550(c,u,id)<0)return 0;
 c.castEnd(); uint32_t f=rd(u,0xc4)|0x40; std::memcpy(static_cast<uint8_t*>(u)+0xc4,&f,4); return 1;
}
// 1.13c D2Client+0x409B0
bool sub_409B0(const Calls& c,P u){
 P d=c.resolve(rd(u,0x2c)); if(!d){c.fatal(u);return false;}
 if(c.isDead(u)||c.isDead(d))return false;
 I t=I(rd(d,0)); return t>=0&&t<2&&c.check562a0(u)!=0;
}
// 1.13c D2Client+0x40CA0: body is host-ordered Ordinal sequence; only the table gates are recovered here.
// ponytail: tail (Ordinals 10871/11013/10630...) left to caller via host hooks, add when a vector pins it.
bool sub_40CA0_gate(const Calls& c,I id){ return rec(c,id)!=nullptr; }
void sub_40DF0(const Calls&,I n,U p1,U p2,U p3,U p4,U p5,U p6,uint64_t seed){ d2_client_40df0(n,p1,p2,p3,p4,p5,p6,U(seed)); }
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
