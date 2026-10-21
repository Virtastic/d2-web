// Prior art: none (D2Client has no D2MOO module). Derived from cached 1.13c decompile.
// Env-seam style: every Ordinal_/FUN_ callee comes in through Env; unit u is an opaque int32 array.
#include <cstdint>
namespace d2client_2b6e0 {
using I=int32_t; using U=uint32_t;
struct Env {
 void* (*o10511)();                 // Ordinal_10511 -> handle (0 = bail)
 void (*o10397)(void* h);
 I (*o10421)(void* u);              // Ordinal_10421 -> table ptr (0 = none)
 I (*o10577)(void* u);              // Ordinal_10577 -> count
 I (*tbl5)(I tbl,I i);              // byte at tbl+5 (per-entry flag), tbl is o10421 result
 void (*o10980)(void* h);
 I (*o10649)(void* h);
 void* (*f54e20)(I a,I slotBase,I u);   // FUN_6fb54e20; slotBase = &DAT_6fbba608 + u*0x80
 I (*o11138)(void* u,void* p,I z);
 void (*o10137)(void* h,I v);
 void (*o10542)(void* u,I v);
 I (*o10179)(void* h);
 void (*o10748)(void* h,I v);
 void* (*f2b680)(void* u);          // FUN_6fadb680
 void (*o10243)(void* h,I v);
 void (*o10554)(void* h,I v);
 void (*o10739)(I a,I b,I c,I d);
 I (*f1630)(void* p); I (*f1660)(void* p);
 void (*f1700)(); I (*f1740)();
 void (*f242b0)(void* u,I a);
};
// 1.13c D2Client+0x2B6E0
// u: int32 view of the unit (u[0]=type, u[4], u[0xb], u[0xe], u[0x12], u[0x31]; byte +0x4e).
// pw: the 32-bit value of the unit pointer (compared to 6 and to u[0xe]>>8 as the decompile does).
U sub_2B6E0(const Env& e,I* u,I pw,uint8_t b4e){
 void* h=e.o10511(); if(!h) return 0;
 e.o10397(h);
 I iVar9=0xf; if(e.o10421(u)) iVar9=e.o10577(u);
 I tbl=e.o10421(u); U f=7;
 if(tbl){ I n=e.o10577(u); for(I i=0;i<n;i++) if(e.tbl5(tbl,i)==0){ f=(U)(i-1); break; } }
 if(u==nullptr || ((u[0]!=0&&u[0]!=1)||b4e!=3)){
  void* p=nullptr; e.o10980(h); I t=e.o10649(h);
  if(pw!=6) p=e.f54e20(t,0x6fbba608+pw*0x80,pw);
  if(f&1){
   if(p && e.o11138(u,p,0)){ e.o10137(h,0); e.o10542(u,pw+1); u[0x12]=iVar9<<8; return 1; }
   if(!(f&2)){
    if(pw<=(u[0xe]>>8)){ e.o10542(u,0); u[0x12]=iVar9<<8; e.o10137(h,1); }
    u[0x31]&=(I)0xffffffbf;
    I r=e.o10179(h); if(r<2) r++; e.o10748(h,r); return 1;
   }
   I* q=(I*)e.f2b680(u);
   if(q && e.o11138(u,q,0)){ e.o10137(h,0); e.o10243(h,q[0]); e.o10554(h,q[3]); return 1; }
   e.o10137(h,0);
   if(u && u[0]==1){ e.f1700(); I r=e.f1740(); if(r==0x1b4||r==0x1b9) return 1; }
   u[0x12]=0x100; return 1;
  }
  e.o10137(h,0); e.o10243(h,0); e.o10554(h,0);
  u[0x12]=(iVar9-(u[0xe]>>8))*0x100;
  if(p||(p=e.f2b680(u))){ e.o10739(u[0xb],e.f1630(p),e.f1660(p),0); return 1; }
 } else e.f242b0(u,u[4]);
 return 0;
}
}
