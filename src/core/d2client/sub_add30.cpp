// Prior art: none (D2MOO has no D2Client). Derived from cached 1.13c decompile.
// Env-seam style: callees come in through Env.
#include <cstdint>
namespace d2client_add30 {
struct Env {
 void* (*find)(int32_t a, uint32_t tbl, uint32_t idx); // D2Client+0xA4E20 (fastcall-ish; tbl=0x6fbba608+idx*0x80)
 void (*f720B0)(void* p);                              // D2Client+0x720B0
 void (*f73630)(void* p);                              // D2Client+0x73630
};
static const uint32_t kTbl=0x6fbba608; // D2Client+0xBA608, 0x80-byte stride
// 1.13c D2Client+0xADD30  (packet handler: p[1]=index byte, p[2..5]=dword id)
void sub_add30(const Env& e,const uint8_t* p){
 uint32_t idx=p[1]; int32_t id; __builtin_memcpy(&id,p+2,4);
 void* r=e.find(id,kTbl+idx*0x80,idx);
 if(r){ e.f720B0(r); e.f73630(r); }
}
}
