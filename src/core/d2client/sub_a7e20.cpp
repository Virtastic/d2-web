// Prior art: none. Derived from cached 1.13c decompile. Env-seam style.
#include <cstdint>
namespace d2client_a7e20 {
using U=uint32_t;
struct Env {
 U (*rd)(U addr); void (*wr)(U addr,U v);
 U gPlayer;                 // DAT 0x6fbcbbfc (RVA 0x11BBFC)
 U gC2F8, gC2F4, gC204;     // DAT 0x6fbcc2f8/f4/204 (RVA 0x11C2F8/F4/204)
 U (*a5a60)(U p1);          // sub_A5A60
 U (*o10331)();  U (*o10511)();  U (*o10858)();   // ordinals unverified
 U (*o10826)(U); char (*o10884)(U); void (*o10083)(U); U (*o10014)(U);
 void (*o10562)(U,U,U,U); void (*o10065)(); void (*o10024)(U);
 U (*o10533)(U); U (*o10589)(); void (*o10509)(U,U*,U,U,U); U (*o10075)(U);
 U bitmask18;               // gdwBitMasks[0x18]
 void (*a10ea0)(); void (*a81720)(); void (*a7d50)();
 void (*fatal)(U code);     // Ordinal_10265/10024 then _exit(-1)
};
// 1.13c D2Client+0xA7E20
// unit pointer in EAX; stack: p1, p2, p3.
void sub_A7E20(Env& e,U unit,U p1,U p2,U p3){
 U pl=e.gPlayer;
 if(unit==pl){ e.gC2F8=0; e.gC2F4=0; }
 U r=e.a5a60(p1);
 U w=e.rd(unit+0x2C);
 if(!w){ e.fatal(0x166); return; }
 if(p1&&p2&&!r){ e.fatal(0x168); return; }
 if(e.rd(unit+0xC4)&0x10000) return;
 U t=e.rd(unit), k=e.rd(unit+0x10);
 if(t==0){ if(k==0||k==0x11) return; }
 else if(t==1){ if(k==0||k==0xC) return; }
 if(unit==pl){
  U a=e.o10331();
  if(a){
   U b=e.o10511(), c=b?e.o10858():0;
   if((!b||!c||!(e.bitmask18&(e.rd(c+8)&0xFF)))&&p3){
    if(t!=0){ e.fatal(0x17D); return; }
    e.a10ea0();
   }
  }
  if(r){
   e.o10562(r,p1,p2,a!=r);
   char ch=e.o10884(e.o10826(r));
   e.o10083(ch==0);
   if(a){
    U i3=e.o10014(e.o10826(a)), i4=e.o10014(e.o10826(r));
    if((e.rd(i3+2)&0xFF)!=(e.rd(i4+2)&0xFF)){ e.o10065(); e.o10024(e.rd(i4+2)&0xFF); }
   }
  }
 }
 if(!e.o10533(w)){
  U rec[3]={pl,p1,p2};
  e.o10509(r,rec,e.o10589(),0x1c09,1);
  if(!e.o10075(w)){ e.fatal(0x1A9); return; }
 }
 e.gC204=2;
 if(unit==pl) e.a81720();
 e.a7d50();
}
}
