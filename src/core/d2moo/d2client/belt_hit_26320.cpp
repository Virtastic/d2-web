// Prior art: none (D2MOO has no D2Client). Derived from the 1.13c disassembly (the decompile misreads the slot byte as a register).
// Env-seam style: globals and the Ordinal_10991 info fill come in through Env.
#include <cstdint>
#include <cstring>
namespace d2client_belt_hit {
using I=int32_t; using U=uint32_t;
struct Env {
 I expanded;          // DAT_6fbcc46c
 I width,height;      // DAT_6fb8bc48, DAT_6fb8bc4c
 void (*fill)(U* buf);  // Ordinal_10991 into the 66-dword local: buf[1] low byte = entry count, rect words at fixed offsets
};
// 1.13c D2Client+0x26320
// ev: mouse event (u16 x at +0xc, u16 y at +0xe). Expanded belt: x0/y0 are the entry (count-4) words, x1/y1 are buf words 15 and 17.
U sub_26320(const Env& e,const uint8_t* ev){
 uint16_t ux,uy;std::memcpy(&ux,ev+0xc,2);std::memcpy(&uy,ev+0xe,2);I x=ux,y=uy;
 U b[66]={0};e.fill(b);
 if(e.expanded){
  const I k=2+4*(I(b[1]&0xff)-4);
  return I(b[k])<=x&&x<=I(b[15])&&I(b[k+2])<=y&&y<=I(b[17]);
 }
 const I h=e.height,half=e.width/2;
 return h-0x30<y&&half+0x17<=x&&x<=half+0x91&&h-0x27<=y&&y<=h-10;
}
}
