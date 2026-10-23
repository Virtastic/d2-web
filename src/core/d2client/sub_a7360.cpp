// Prior art: none (D2MOO has no D2Client). Derived from cached 1.13c decompile.
// Env-seam style: callees and the fatal path come in through Env.
#include <cstdint>
namespace d2client_a7360 {
using U=uint32_t;
struct Env {
 U (*rd)(U addr);                         // read u32 from retail address space
 U (*a6500)(U unit);                      // ESI=unit; 1 inserted, 0 already present
 U (*a4e20)(U id,U row,U type);           // bucket lookup
 void (*a6ef0)(U row,U idx,U id,U type);  // re-link helper
 void (*a6d60)(U unit);                   // free unit
 void (*fatal)(U code);                   // Ordinal_10265(0x1c6) -> Ordinal_10024 -> _exit(-1)
};
static const U kTabA=0x6fbba608, kTabB=0x6fbb9a08; // type*0x200 bytes each
// 1.13c D2Client+0xA7360
// EAX=unit, ECX=param_1 (passed through to A6500, unused here). Returns unit or 0.
U sub_A7360(const Env& e,U unit){
 if(e.a6500(unit)) return unit;
 U type=e.rd(unit), id=e.rd(unit+12);
 U p=e.a4e20(id,kTabA+type*0x200,type);
 if(p==unit){ e.fatal(0x1c6); return 0; }
 if(p){
  U t=e.rd(p), pid=e.rd(p+12);
  U row=((e.rd(p+0xC4)>>0x15)&1)?kTabB+t*0x200:kTabA+t*0x200;
  e.a6ef0(row,pid&0x7f,pid,t);
  if(e.a6500(unit)) return unit;
 }
 e.a6d60(unit);
 return 0;
}
}
