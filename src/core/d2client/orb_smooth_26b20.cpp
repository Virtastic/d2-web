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
// Retail x87 at 0x26C7E: fild(a); fidiv(span); fimul(delta); _ftol, with the 53-bit precision control the CRT
// startup sets (__controlfp(0x10000, 0x30000), D2Client+0xCB5D2). Each rounding is nearest-even to 53 bits, no float.
static unsigned __int128 round53(unsigned __int128 v){
 int n=0; for(unsigned __int128 t=v;t;t>>=1)++n;
 if(n<=53) return v;
 int d=n-53; unsigned __int128 q=v>>d, rem=v&((((unsigned __int128)1)<<d)-1), half=((unsigned __int128)1)<<(d-1);
 if(rem>half||(rem==half&&(q&1)))++q;
 return q<<d;}
static int32_t x87Scale(int32_t a,int32_t span,int32_t delta){  // trunc(round53(round53(a/span)*delta)); span in 7..15
 if(a==0||delta==0) return 0;
 bool neg=(a<0)!=(delta<0);
 unsigned __int128 ua=a<0?(unsigned __int128)(uint32_t)(-(int64_t)a):(unsigned __int128)a;
 unsigned __int128 mag=delta<0?(unsigned __int128)(uint32_t)(-(int64_t)delta):(unsigned __int128)delta;
 unsigned __int128 sp=(unsigned __int128)span, m=0; int k=0;
 for(;k<120;++k){ m=(ua<<k)/sp; if(m>=((unsigned __int128)1)<<52)break;}
 unsigned __int128 rem=(ua<<k)-m*sp;
 if(rem*2>sp||(rem*2==sp&&(m&1)))++m;  // q = m*2^-k, 53 bits
 unsigned __int128 p=round53(m*mag)>>k;
 return neg?int32_t(-(int64_t)(uint64_t)p):int32_t((uint64_t)p);}
// 1.13c D2Client+0x26B20
// stat is EAX: 6, 8 or 10; anything else returns 0.
I sub_26B20(Env& e,I stat){
 I cur=e.current();   // retail reads the current value (ord 10973) before it looks at the stat
 I n=stat==6?0:stat==8?1:stat==10?2:-1; if(n<0) return 0;
 I mx=e.max[n]();
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
  r=x87Scale(I(f-U(s.tick))+1,span,s.shown-s.prev)+s.prev;
 if(r<0) return 0;
 return r>mx?mx:r;
}
}
