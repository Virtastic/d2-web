// Prior art: none (D2MOO has no D2Client). Derived from cached 1.13c decompile.
// Env-seam style: globals and the Ordinal_10991 rect fill come in through Env.
#include <cstdint>
#include <cstring>
namespace d2client_belt_hit {
using I=int32_t; using U=uint32_t;
struct Env {
 I expanded;          // DAT_6fbcc46c
 I width,height;      // DAT_6fb8bc48, DAT_6fb8bc4c
 void (*rect)(I slot,I* r); // Ordinal_10991 fill; r = x0,x1,y0,y1 for slot
};
// 1.13c D2Client+0x26320
// ev: mouse event (u16 x at +0xc, u16 y at +0xe); slot: DI (read from the unfilled-rect layout).
U sub_26320(const Env& e,const uint8_t* ev,uint8_t slot){
 uint16_t ux,uy;std::memcpy(&ux,ev+0xc,2);std::memcpy(&uy,ev+0xe,2);I x=ux,y=uy;
 I r[4]={0,0,0,0};e.rect(slot,r);
 if(!e.expanded) return e.height-0x30<y && e.width/2+0x17<=x && x<=e.width/2+0x91 && e.height-0x27<=y && y<=e.height-10;
 return r[0]<=x&&x<=r[1]&&r[2]<=y&&y<=r[3];
}
}
