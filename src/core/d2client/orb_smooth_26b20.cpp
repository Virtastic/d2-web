// Orb value smoothing. Env-seam: globals and the unit getters are passed in.
#include <cstdint>
namespace d2client_orb {
using I=int32_t; using U=uint32_t;
struct Slot { I shown,tick,prev,prevTick; };            // 0x6FBCA794 + 16*n
struct Env {
 U frame;                                               // 0x6FBC97F8
 Slot slot[3];                                          // life, mana, stamina
 I (*current)(); I (*max[3])();                         // ord 10973; 10913/10186/10578
};
// 1.13c D2Client+0x26B20
// stat is EAX: 6, 8 or 10; anything else returns 0. ponytail: retail multiplies in x87
// (fild/fidiv/fimul/ftol); integer (d+1)*delta/range can differ by 1 only on exact-integer ties.
I sub_26B20(Env& e,I stat){
 I n=stat==6?0:stat==8?1:stat==10?2:-1; if(n<0) return 0;
 I cur=e.current(), mx=e.max[n]();
 if(!mx) return 0;
 U f=e.frame; Slot& s=e.slot[n];
 if(f<2){s.prev=s.shown=cur;}
 if(n==0&&cur<s.shown){s.prev=s.shown=cur;s.tick=I(f);s.prevTick=I(f);}
 if(s.shown!=cur){
  I d=s.shown-cur; if(d<0)d=-d;
  if(d<(mx+((mx>>31)&7))>>3){s.prevTick=s.tick; s.prev=(cur==mx)?cur:s.shown;}
  else{s.prev=cur;s.prevTick=I(f);}
  s.shown=cur;s.tick=I(f);
 }
 I span=s.tick-s.prevTick; span=span<7?7:span>15?15:span;
 I r=cur;
 if(cur>0&&I(f-U(s.tick))<span&&s.shown!=s.prev)
  r=I((int64_t(I(f-U(s.tick))+1)*(s.shown-s.prev))/span)+s.prev;
 if(r<0) return 0;
 return r>mx?mx:r;
}
}
