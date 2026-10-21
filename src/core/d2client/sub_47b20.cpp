// D2Client sub_47B20 (row s3503): close the unit-interaction dialog for unit id EDI.
// Derived from cached 1.13c decompile; callees not yet ported are seams in H.
#include <cstdint>
#include <cstring>
namespace d2client_47b20 {
using U=uint32_t; using I=int32_t; using P=uint8_t*;
struct H{
 P* player;            // DAT_6fbcbbfc (unit/handle; +0 mode, +0x14 = sub record)
 P* unitSlots;         // DAT_6fba808[128], chain via +0xE4 (idx 0x39)
 U* a3aba; U* a3b10; U* a9721;   // 6fbc3aba (u8) 6fbc3b10 (u32) 6fbc9721 (u8) -- see below
 void (*f46a60)(U); void (*send)(const uint8_t*); void (*f2cef0)();
 void (*free10660)(U); void (*f72790)(I,I,I); I (*o11090)(P,I);
 void (*f6a220)(); void (*f2a590)(I,P); void (*f24490)(P,I);
 void (*fatal)();
};
static H h;
static U rd(P p,U o){U v;std::memcpy(&v,p+o,4);return v;}
static void wr(P p,U o,U v){std::memcpy(p+o,&v,4);}

// 1.13c D2Client+0x47B20
void sub_47B20(U id){
 P pl=*h.player;
 if(pl&&rd(pl,0)==0&&rd(pl,0x14)){P r=(P)(uintptr_t)rd(pl,0x14);wr(r,0x150,0);wr(r,0x154,0);wr(r,0x158,0);wr(r,0x15c,0);}
 h.f46a60(id);
 P u=(P)(uintptr_t)h.unitSlots[id&0x7f];
 while(u&&rd(u,0xc)!=id)u=(P)(uintptr_t)rd(u,0xe4);
 if(u&&rd(u,0)!=1)h.fatal();      // Ordinal_10265(0x5a)+10024+_exit(-1)
 if(u)wr(u,0xc4,rd(u,0xc4)|2);
 const uint8_t pkt[5]={0x30,1,0,0,0}; h.send(pkt);
 h.f2cef0(); *(uint8_t*)h.a3aba=0;
 if(*h.a3b10){h.free10660(*h.a3b10);*h.a3b10=0;}
 *(uint8_t*)h.a9721=0; h.f72790(8,1,0);
 if(u){
  if(rd(u,0)==1&&rd(u,0x14)){P r=(P)(uintptr_t)rd(u,0x14);r[0x28]&=0xfe;}
  wr(u,0xc4,rd(u,0xc4)&0xffffffbf);
  if(h.o11090(u,1)){h.f6a220();h.f2a590(1,u);h.f24490((P)1,0);}
 }
}
}
