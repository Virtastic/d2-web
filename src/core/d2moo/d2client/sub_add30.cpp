// Prior art: none (D2MOO has no D2Client). Derived from the 1.13c disassembly of D2Client+0xADD30.
// Env-seam style: callees come in through Env, with the registers and stack words retail passes.
#include <cstdint>
#include "sub_add30.h"
namespace d2client_add30 {
static const uint32_t kTbl=0x6fbba608; // D2Client+0xBA608, 0x200-byte buckets (idx<<9)
// 1.13c D2Client+0xADD30  (packet handler, ecx = packet: p[1]=index byte, p[2..5]=dword id, p[6]=state byte)
void sub_add30(const Env& e,const uint8_t* p){
 uint32_t idx=p[1]; uint32_t id; __builtin_memcpy(&id,p+2,4);
 uint32_t r=e.find(id&0x7f,id,kTbl+(idx<<9),idx);
 if(r){ uint32_t b=p[6]; e.f720B0(b,r); e.f73630(b,r); }
}
}
