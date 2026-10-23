// Prior art: none (D2MOO has no D2Client). Derived from cached 1.13c decompile.
// Env-seam style: memory reads and the fatal-error path come in through Env.
#include <cstdint>
namespace d2client_a4e20 {
using U=uint32_t;
struct Env {
 U (*rd)(U addr);          // read u32 from retail address space
 void (*fatal)(void);      // Ordinal_10265(0x5a) -> Ordinal_10024(msg) -> _exit(-1)
};
// 1.13c D2Client+0xA4E20
// ECX=id (param_1), EAX=bucket index, EDX=table base (param_2), stack=type (param_3).
// Returns the node address or 0.
U sub_A4E20(const Env& e,U id,U idx,U table,U type){
 U n=e.rd(table+idx*4);
 while(n){
  if(e.rd(n+0xC)==id){ if(e.rd(n)==type) return n; e.fatal(); return 0; }
  n=e.rd(n+0xE4);
 }
 return 0;
}
}
