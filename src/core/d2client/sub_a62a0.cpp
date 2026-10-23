// D2Client+0xA62A0: relation/visibility predicate between two client records (b in EAX, a on stack, retn 4).
// Seams: callees and the node-list head (VA 0x6FBCC4D4, passed as env.head).
#include <cstdint>
#include <cstring>
namespace d2client_a62a0 {
static uint32_t rd(const void* p,uint32_t o){uint32_t v;std::memcpy(&v,static_cast<const uint8_t*>(p)+o,4);return v;}
static const uint8_t* P(uint32_t v){return reinterpret_cast<const uint8_t*>((uintptr_t)v);}
struct Env {
 uint32_t head;                                              // dword at 0x6FBCC4D4 (list of nodes, next at +0x30)
 uint32_t (*ord10331)(const uint8_t* a);                     // D2Client+0xC1F0 thunk (ordinal 10331)
 uint32_t (*ord10057)(uint32_t v);                           // D2Client+0xC208 thunk (ordinal 10057)
 uint32_t (*ord10666)(const uint8_t* a);                     // D2Client+0xC6A6 thunk (ordinal 10666)
 uint32_t (*f9dc30)(int32_t eax,int32_t edx);                // D2Client+0x9DC30
 int32_t  (*f216a0)(uint32_t ecx);                           // D2Client+0x216A0
 uint32_t (*fa51f0)(const uint8_t* eax);                     // D2Client+0xA51F0
 uint32_t (*fa5440)(const uint8_t* eax);                     // D2Client+0xA5440
 uint32_t (*f1510)(int32_t eax,int32_t edx);                 // D2Client+0x1510
 uint32_t (*f4fd90)(uint32_t ecx,int32_t edx,uint32_t mask,int32_t esi); // D2Client+0x4FD90; ecx unverified (a, may be clobbered by 9DC30 on the 5440 path)
};
// Remap (type,id) through the node list: node[8]==id and node[0xc]!=-1 -> (0,node[0xc]).
static void remap(const Env& e,uint32_t& t,int32_t& id){
 if(t!=1) return;
 for(uint32_t n=e.head;n;n=rd(P(n),0x30)) if((int32_t)rd(P(n),8)==id){ int32_t m=(int32_t)rd(P(n),0xc); if(m!=-1){t=0;id=m;} break; }
}
// 1.13c D2Client+0xA62A0
uint32_t sub_A62A0(const Env& e,const uint8_t* b,const uint8_t* a){
 uint32_t ta=6,tb=6; int32_t ia=-1,ib=-1;
 if(a){ta=rd(a,0);ia=(int32_t)rd(a,0xc);}
 if(b){tb=rd(b,0);ib=(int32_t)rd(b,0xc);}
 if(e.ord10057(e.ord10331(a))){
  if(!b||rd(b,0)!=0||(!(rd(b,0xc4)&0x10000)&&rd(b,0x10)!=0&&rd(b,0x10)!=0x11)||e.f9dc30(ia,(int32_t)rd(b,0xc))!=0){
   if(!a||rd(a,0)!=0||!b||rd(b,0)!=1||e.f216a0(rd(b,0xc))==-1||e.fa51f0(a)!=0) return 1;
  }
  return 0;
 }
 remap(e,ta,ia);
 if(a&&((rd(a,0xc8)>>10)&1)&&rd(b,0x94)==0){ia=(int32_t)rd(a,0x98);ta=0;} // retail derefs b unchecked here
 remap(e,tb,ib);
 if(b&&((rd(b,0xc8)>>10)&1)&&rd(b,0x94)==0){ib=(int32_t)rd(b,0x98);tb=0;}
 if(ta==tb&&ia==ib) return 0;
 if(ta==0&&tb==0){
  if(b&&rd(b,0)==0&&e.fa5440(b)&&e.f9dc30(ia,(int32_t)rd(b,0xc))!=0) return 1;
  return e.f4fd90((uint32_t)(uintptr_t)a,ia,8,ib);
 }
 if(tb==1){
  uint32_t k=b?rd(b,4):0xFFFFFFFFu;
  if(e.f1510((int32_t)k,4)!=0) return 1;
  if(e.f1510((int32_t)k,5)!=0) return 0;
 }
 if(ta==0&&(tb==4||tb==2)) return 1;
 return e.ord10666(a)==0;
}
}
