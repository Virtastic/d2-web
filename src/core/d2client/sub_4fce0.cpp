// D2Client+0x4FCE0: find key node in a client record's key list. Seam: 9D630 passed in.
#include <cstdint>
#include <cstring>
namespace d2client_4fce0 {
static uint32_t rd(const void* p,uint32_t o){uint32_t v;std::memcpy(&v,static_cast<const uint8_t*>(p)+o,4);return v;}
typedef const uint8_t* (*Find9D630)(uint32_t ecx,int32_t id); // D2Client+0x9D630
// 1.13c D2Client+0x4FCE0 (ecx=a, edx=b; esi=key, edx as seen after the 9D630 call). Returns node address or 0.
uint32_t sub_4FCE0(Find9D630 find,uint32_t a,int32_t b,int32_t esi,int32_t edx){
 const uint8_t* u=find(a,b);
 if(!u||!rd(u,0x34)||edx==esi) return 0;
 const uint8_t* head=reinterpret_cast<const uint8_t*>((uintptr_t)rd(u,0x34));
 for(uint32_t n=rd(head,0);n;n=rd(reinterpret_cast<const uint8_t*>((uintptr_t)n),8)){
  if((int32_t)rd(reinterpret_cast<const uint8_t*>((uintptr_t)n),0)==esi) return n;
 }
 return 0;
}
}
