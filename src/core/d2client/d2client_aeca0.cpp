// 1.13c D2Client+0xAECA0: life/mana/stamina + extra stats + position packet (0x18). ECX = packet; the 16-byte local is filled by D2Client+0xAC240
// (EAX = packet, ESI = local). Read off the disassembly: the older decompile-only version passed the first field in as a parameter.
#include <cstdint>
#include <cstring>
#include "d2client_batch_ad4b0.h"
namespace d2client_batch_ad4b0 {
static uintptr_t rdp(uintptr_t a){ uintptr_t v; std::memcpy(&v,(const void*)a,sizeof v); return v; }
static uint32_t r32(uintptr_t a){ uint32_t v; std::memcpy(&v,(const void*)a,4); return v; }
static uint16_t r16(uintptr_t a){ uint16_t v; std::memcpy(&v,(const void*)a,2); return v; }
// 1.13c D2Client+0xAECA0
void sub_AECA0(Env& e,const uint8_t* pkt){
 uint8_t l[16]={0};
 e.fAC240(pkt,l);
 const uintptr_t o=rdp((uintptr_t)e.ga(0x11BBFC));
 if(!o) return;
 const uint32_t life=(uint32_t)r16((uintptr_t)l+1)<<8;
 e.o10887(o,6,life,0);
 e.o10887(o,8,(uint32_t)r16((uintptr_t)l+3)<<8,0);
 e.o10887(o,10,(uint32_t)r16((uintptr_t)l+5)<<8,0);
 e.o10887(o,0x4a,l[7],0);
 e.o10887(o,0x1a,l[8],0);
 uint32_t dx=l[13],dy=l[14];
 if(dx>0x80) dx-=0x100;
 if(dy>0x80) dy-=0x100;
 const uint32_t d11=r32((uintptr_t)l+11),d9=r32((uintptr_t)l+9);
 e.f2A1B0(d11,0,(uint16_t)(d9+dx),(uint16_t)(dy+d11));
 if(life!=0 && r32(o+0x10)==0x11){ e.f6A6C0(o); e.f56210(o); }
}
} // namespace d2client_batch_ad4b0
