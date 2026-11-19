// Implementation batch D2Client+0xAD4B0..0xAEBE0 (packet thunks, unit-table lookups, packet dispatch).
// Decompile-derived; unrecovered callees are function pointers on Env, register args are explicit params.
// Pointers are uintptr_t; wire-packet fields are read at their byte offsets with memcpy.
#include <cstdint>
#include <cstring>
#include "d2client_batch_ad4b0.h"
namespace d2client_batch_ad4b0 {
static const uint32_t NEXT_OFF=0x39*4; // puVar1[0x39] link in unit node (original 32-bit layout)
static uintptr_t rdp(uintptr_t a){ uintptr_t v; std::memcpy(&v,(const void*)a,sizeof v); return v; }
static uint32_t r32(uintptr_t a){ uint32_t v; std::memcpy(&v,(const void*)a,4); return v; }
static uint16_t r16(uintptr_t a){ uint16_t v; std::memcpy(&v,(const void*)a,2); return v; }

// 1.13c D2Client+0xAD4B0
void sub_AD4B0(Env& e,uint32_t a){ uint8_t l[16]={0}; std::memcpy(l,(const void*)(uintptr_t)a,15); e.f62E60((uint32_t)(uintptr_t)l); }   // ECX = packet: its 15 bytes copied to a stack local, EAX = &local

// 1.13c D2Client+0xAD4F0
uint32_t sub_AD4F0(Env& e,uintptr_t p){ uint8_t l[7]; std::memcpy(l,(const void*)p,7); return e.f4C3C0(p,l); }   // EAX is the callee's

// 1.13c D2Client+0xAD9D0
void sub_AD9D0(Env& e,uintptr_t p){
 uint8_t n=*(const uint8_t*)(p+3);
 if(!n){ e.fatal(0x855); return; }
 uintptr_t b=p+4;
 for(int i=0;i<n;i++,b+=9){
  uint8_t type=*(const uint8_t*)b; uint32_t id=r32(b+1); uint16_t arg=r16(b+7);
  // the unit table and the node link (+0xE4) are 32-bit words of the retail image: 4-byte slots, 4-byte loads
  uintptr_t slot=(uintptr_t)e.ga(0xFBA608)+((uintptr_t)type*0x80+(id&0x7f))*4;
  for(uintptr_t u=r32(slot);u;u=r32(u+NEXT_OFF)){
   if(r32(u+12)==id){ if(r32(u)!=type){ e.fatal(0x5a); return; } e.f1A1B0(arg,0,0); break; }
  }
 }
}

// 1.13c D2Client+0xADEF0
void sub_ADEF0(Env& e,uintptr_t p){ uint8_t l[12]={0}; std::memcpy(l,(const void*)p,10); e.f194E0(l); }   // ECX = packet: its 10 bytes copied to a stack local, EAX = &local

// 1.13c D2Client+0xAE080
void sub_AE080(Env& e,const uint8_t* src){
 uint8_t l[0xFC]; std::memcpy(l,src,sizeof l);
 switch(l[1]){
 case 0: e.f34BB0(l); return; case 1: e.f34AF0(l); return; case 2: e.f34990(l); return;
 case 3: e.f36810(l); return; case 4: e.f34750(l); return;
 case 5: case 6: case 7: case 8: case 9: case 0x11: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
  e.fatal(0xf6d); return;
 case 10: e.f32320(l); return; case 0xb: case 0xc: e.f34080(l); return;
 case 0xd: e.f35AA0(l); return; case 0xe: e.f33AD0(l); return;
 case 0xf: e.f32C60(l); return; case 0x10: e.f32E10(l); return; case 0x12: e.f339E0(l); return;
 }
}

// 1.13c D2Client+0xAEBE0 (ECX = packet; checked against the disassembly: the 16-byte local is filled by D2Client+0xAC1B0)
void sub_AEBE0(Env& e,const uint8_t* pkt){
 uint8_t l[16]={0};
 e.fAC1B0(pkt,l);
 uintptr_t o=rdp((uintptr_t)e.ga(0x11BBFC));
 if(!o) return;
 const uint32_t life=(uint32_t)r16((uintptr_t)l+1)<<8;
 e.o10887(o,6,life,0);
 e.o10887(o,8,(uint32_t)r16((uintptr_t)l+3)<<8,0);
 e.o10887(o,10,(uint32_t)r16((uintptr_t)l+5)<<8,0);
 uint32_t dx=l[11],dy=l[12];
 if(dx>0x80) dx-=0x100;
 if(dy>0x80) dy-=0x100;
 const uint32_t d9=r32((uintptr_t)l+9),d7=r32((uintptr_t)l+7);
 e.f2A1B0(d9,0,(uint16_t)(d7+dx),(uint16_t)(dy+d9));
 if(life!=0 && r32(o+0x10)==0x11){ e.f6A6C0(o); e.f56210(o); }
}
} // namespace
