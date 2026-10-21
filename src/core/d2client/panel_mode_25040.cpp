// Derived from cached 1.13c code; ESI=object, EDI=mode, stack arg=on. Unnamed ordinals are callbacks.
#include <cstdint>
#include <cstring>
namespace d2client_panel_mode {
using U=uint32_t; using P=void*;
static U rd(const void* p,U n){U v;std::memcpy(&v,static_cast<const uint8_t*>(p)+n,4);return v;}
static void wr(void* p,U n,U v){std::memcpy(static_cast<uint8_t*>(p)+n,&v,4);}
static uint8_t rb(const void* p,U n){return static_cast<const uint8_t*>(p)[n];}
struct Calls {
 void (*fatal)();   // Ordinal_10265/10024 + _exit(-1) on mode > 7
 void (*a11090)(); void (*f6a220)(); void (*f1660)(P); void (*f1630)(P);
 void (*a10331)(); void (*a10032)(); void (*a10408)();
};
// 1.13c D2Client+0x25040
void sub_25040(P esi,U mode,U on,Calls& c){
 P cfg=reinterpret_cast<P>(static_cast<uintptr_t>(rd(esi,0x14)));
 P data=reinterpret_cast<P>(static_cast<uintptr_t>(rd(cfg,0)));
 U cur=rd(esi,0x10), fl=rd(esi,0xc4);
 if(mode>7){c.fatal();return;}
 auto closeCur=[&]{c.f1660(esi);c.f1630(esi);c.a10331();c.a10032();};
 if(mode==0){
  if(cur==2&&rd(data,0xe4)!=0x100){
   wr(esi,0x44,rb(data,0x129));c.a11090();c.f6a220();fl&=~2u;wr(esi,0xc4,fl);
   closeCur();goto done;
  }
 }else if(mode==2&&cur==0&&rd(data,0xdc)!=0x100){
  c.a11090();c.f6a220();wr(esi,0xc4,rd(esi,0xc4)&~2u);
  wr(esi,0x44,rb(data,299));goto done;
 }
 wr(esi,0xc4,on?rd(esi,0xc4)|2u:rd(esi,0xc4)&~2u);
 cur=rd(esi,0x10);
 if(mode!=cur){
  if(mode==4||mode==2||mode==5){if(cur!=5&&cur!=2&&cur!=4)c.a10408();}
  else if(mode==0&&cur!=0)closeCur();
  c.a11090();c.f6a220();
 }
 wr(esi,0x44,rb(data,0x129+mode));
done:
 U f=rd(esi,0xc4);
 wr(esi,0xc4,rb(data,0x118+rd(esi,0x10))?f&~0x20u:f|0x20u);
}
}
