// D2Client+0x4FDD0: set/clear mask bits on a key node. Seam: 9D630 passed in; node memory is writable.
#include <cstdint>
#include <cstring>
namespace d2client_4fdd0 {
static uint32_t rd(const void* p,uint32_t o){uint32_t v;std::memcpy(&v,static_cast<const uint8_t*>(p)+o,4);return v;}
typedef const uint8_t* (*Find9D630)(uint32_t ecx,int32_t id); // D2Client+0x9D630
// 1.13c D2Client+0x4FDD0 (ecx=a, edx=b, stack set; eax=mask, esi=key, edx as seen after the 9D630 call)
void sub_4FDD0(Find9D630 find,uint32_t a,int32_t b,int32_t set,uint32_t mask,int32_t esi,int32_t edx){
 const uint8_t* u=find(a,b);
 if(!u||!rd(u,0x34)||edx==esi) return;
 const uint8_t* head=reinterpret_cast<const uint8_t*>((uintptr_t)rd(u,0x34));
 for(uint32_t n=rd(head,0);n;n=rd(reinterpret_cast<const uint8_t*>((uintptr_t)n),8)){
  uint8_t* e=reinterpret_cast<uint8_t*>((uintptr_t)n);
  if((int32_t)rd(e,0)!=esi) continue;
  uint32_t f=rd(e,4); f=set?(f|mask):(f&~mask); std::memcpy(e+4,&f,4);
  return;
 }
}
}
