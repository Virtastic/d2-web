// Prior art: none (D2MOO has no D2Client). Derived from cached 1.13c decompile.
// Env-seam style: globals and callees come in through Env.
#include <cstdint>
namespace d2client_2b1e0 {
using I=int32_t; using U=uint32_t;
struct Env {
 const I* cur;                 // DAT_6fbcbbfc (pointer compared with p)
 U tabA,tabB;                  // &DAT_6fbba608, &DAT_6fbb9a08 (addresses; stride 0x200 bytes per kind)
 void (*ord11093)(I* p,I v);   // Ordinal_11093
 void (*f77fb0)(I* p);         // 6fb77fb0
 void (*f1c130)();             // 6fb1c130
 void (*kind0)(I* p);          // 6facc150
 void (*kind1)();              // 6faff4a0
 void (*kind2)();              // 6fad5b40
 void (*kind3)();              // 6fb61af0
 void (*kind4)();              // 6fb32ea0
 I* (*f54e20)(I id,U tab,I kind); // 6fb54e20
 void (*f5bc20)(I* r);         // 6fb5bc20
 void (*f43e0)(const uint8_t* msg); // 6fac43e0, msg: 'K', u32 kind @1, u32 id @5 (packed)
 void (*f56ef0)(U tab,I lo7,I id,I kind); // 6fb56ef0
};
// 1.13c D2Client+0x2B1E0
void sub_2B1E0(const Env& e,I* p){
 if(e.cur==p||!p||!((U)p[0x31]>>0x17&1)){
  e.ord11093(p,0); e.f77fb0(p);
  I id,kind; U fl;
  if(!p){id=-1;kind=6;fl=0;} else {id=p[3];kind=p[0];fl=(U)p[0x31]>>0x15&1;}
  e.f1c130();
  switch(p[0]){case 0:e.kind0(p);break;case 1:e.kind1();break;case 2:e.kind2();break;case 3:e.kind3();break;case 4:e.kind4();}
  U tab=fl?e.tabB+(U)kind*0x200:e.tabA+(U)kind*0x200;
  I* r=e.f54e20(id,tab,kind);
  if(r) e.f5bc20(r);
 } else {
  if((U)p[0x32]>>5&1){
   uint8_t m[9]; m[0]='K';
   for(int i=0;i<4;i++){m[1+i]=(uint8_t)((U)p[0]>>(8*i));m[5+i]=(uint8_t)((U)p[3]>>(8*i));}
   e.f43e0(m);
   p[0x31]&=(I)0xff7fffff; p[0x32]&=(I)0xffffffdf;
  }
  if((U)p[0x31]>>0x15&1){ e.f56ef0(e.tabB+(U)p[0]*0x200,p[3]&0x7f,p[3],p[0]); return; }
 }
}
}
