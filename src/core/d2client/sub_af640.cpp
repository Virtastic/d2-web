// Prior art: none. Derived from cached 1.13c decompile. Env-seam style.
#include <cstdint>
namespace d2client_af640 {
using U=uint32_t;
struct Env {
 U (*rd)(U addr); void (*wr)(U addr,U v);   // rd: 32-bit little-endian read (callers mask)
 U tblBase;                 // DAT 0x6fbba608 (RVA 0x10A608), 0x80 per entry
 U gPlayer;                 // DAT 0x6fbcbbfc (RVA 0x11BBFC)
 U gC3B8;                   // DAT 0x6fbcc3b8 (RVA 0x11C3B8)
 U gC81C;                   // DAT 0x6fbc981c (RVA 0x11C81C), tick deadline (written)
 U tick;                    // GetTickCount()
 U (*a4e20)(U id,U tbl,U idx);               // sub_A4E20
 void (*a7e20)(U unit,U p1,U p2,U p3);       // sub_A7E20
 U (*o10331)(U);  U (*o10826)(U); U (*o10459)(U);   // ordinals unverified
 void (*o10704)(U); void (*o11164)(U,U); U (*o10648)(U); void (*o11056)(U,U);
 U (*o10716)(U);
 void (*fatal)(U code);     // Ordinal_10265/10024 then _exit(-1)
};
// 1.13c D2Client+0xAF640  (__fastcall, ECX = packet)
void sub_AF640(Env& e,U pkt){
 U idx=e.rd(pkt+1)&0xFF;
 U a=e.rd(pkt+6)&0xFFFF, b=e.rd(pkt+8)&0xFFFF, c=e.rd(pkt+10)&0xFF;
 U u=e.a4e20(e.rd(pkt+2),e.tblBase+idx*0x80,idx);
 if(!u) return;
 e.a7e20(u,a,b,c);   // unit passed in EAX by callee convention
 U h=e.o10331(u);
 if(!h){ e.fatal(0x538); return; }
 e.wr(u+0x18,(e.o10459(e.o10826(h)))&0xFF);   // byte store at +0x18
 e.wr(u+0x1C,e.gC3B8);
 U w=e.rd(u+0x2C);
 if(w){
  U t=e.rd(u);
  if(t==0){ e.o10704(u); e.o11164(w,0x1c09); }
  else if(t==1){ if(!e.o10648(u)) e.o11056(w,0x100); }
 }
 if(u==e.gPlayer){
  if(e.o10716(e.o10331(u))){ e.fatal(0x81E); return; }
  if(e.gC81C<e.tick+3000) e.gC81C=e.tick+3000;
 }
}
}
