// Prior art: none. Derived from cached 1.13c decompile. Env-seam style.
#include <cstdint>
namespace d2client_a7bf0 {
using U=uint32_t;
struct Env {
 U (*rd)(U addr); void (*wr)(U addr,U v);
 U gCounter;                         // DAT 0x6fb8e834 (wraps at 0xffffffff -> 0)
 U (*o11153)(U a,U type);            // Ordinal_11153(0,type) -> unit ptr
 void (*a7780)(const U* rec,U,U,U);  // sub_A7780
 void (*a7400)(U u,U id,U p1,U p4,U type); // sub_A7400
};
// 1.13c D2Client+0xA7BF0
// fastcall: ECX=p1, EDX=p2; stack: p3, type(p4).
void sub_A7BF0(Env& e,U p1,U p2,U p3,U type){
 if(++e.gCounter==0xffffffffu) e.gCounter=0;
 if(type==1){
  U rec[8]={e.gCounter,p1,p2,p3,0,0,0,type}; // local_1c..local_4
  e.a7780(rec,0,0,0); return;
 }
 U u=e.o11153(0,type);
 if(!u) return;
 e.wr(u+0xC4,e.rd(u+0xC4)|0x600000);
 e.a7400(u,e.gCounter,p1,type,type);
}
}
