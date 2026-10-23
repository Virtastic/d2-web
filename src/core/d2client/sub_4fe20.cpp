// D2Client+0x4FE20: add a key node (0x74 bytes, key at +0, link at +8) at the head of a record's key list unless present. Seams: 9D630, 4FCE0, alloc (Ordinal_10042(0x74,0)).
#include <cstdint>
#include <cstring>
namespace d2client_4fe20 {
static uint32_t rd(const void* p,uint32_t o){uint32_t v;std::memcpy(&v,static_cast<const uint8_t*>(p)+o,4);return v;}
static void wr(void* p,uint32_t o,uint32_t v){std::memcpy(static_cast<uint8_t*>(p)+o,&v,4);}
typedef const uint8_t* (*Find9D630)(uint32_t ecx,int32_t id);
typedef uint32_t (*Find4FCE0)(uint32_t ecx,int32_t edx); // key from the caller's EAX, as 4FCE0 sees it
typedef uint8_t* (*Alloc)(uint32_t size,uint32_t zero);   // Ordinal_10042
// 1.13c D2Client+0x4FE20 (ecx=a, edx=b; eax=key, edx as seen after the 9D630 call)
void sub_4FE20(Find9D630 find,Find4FCE0 find4fce0,Alloc alloc,uint32_t a,int32_t b,int32_t key,int32_t edx){
 const uint8_t* u=find(a,b);
 if(!u) return;
 uint32_t headp=rd(u,0x34);
 if(!headp||edx==key) return;
 if(find4fce0(a,edx)!=0) return;
 uint8_t* n=alloc(0x74,0);
 wr(n,0,0); wr(n,4,0); wr(n,8,0);
 wr(n,0,(uint32_t)key);
 uint8_t* h=reinterpret_cast<uint8_t*>((uintptr_t)headp);
 wr(n,8,rd(h,0));
 wr(h,0,(uint32_t)(uintptr_t)n);
}
}
