// Derived from cached 1.13c code. 1.13c D2Client+0x25400
#include <cstdint>
#include <cstring>
namespace d2client_25400 {
using U=uint32_t; using P=void*;
static int32_t rd(P p,U n){int32_t v;std::memcpy(&v,static_cast<uint8_t*>(p)+n,4);return v;}
static void wr(P p,U n,int32_t v){std::memcpy(static_cast<uint8_t*>(p)+n,&v,4);}
struct Calls { void (*fatal)(); void (*sub_251E0)(P); P (*deref)(int32_t); };
// obj in EAX. State 1: ramp obj+0x44 up by step toward target; state 3: ramp down to 0. At rest -> sub_251E0.
void sub_25400(P obj,Calls& c){
 int32_t st=rd(obj,0x10);
 if(st==1){
  P rec=c.deref(rd(obj,0x14));
  int32_t t=rd(rec,0xd8+4)-0x100, cur=rd(obj,0x44);
  if(cur!=t){ int16_t s; std::memcpy(&s,static_cast<uint8_t*>(obj)+0x4c,2);
   cur+=s; wr(obj,0x44,cur<t?cur:t); return; }
 } else if(st==3){
  int32_t cur=rd(obj,0x44);
  if(cur>0){ int16_t s; std::memcpy(&s,static_cast<uint8_t*>(obj)+0x4c,2);
   wr(obj,0x44,cur<s?0:cur-s); return; }
 } else { c.fatal(); return; }
 c.sub_251E0(obj);
}
}
