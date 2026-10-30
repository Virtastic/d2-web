// Derived from cached 1.13c code; ESI=unit, AX=x, stack word=y, param_1=mode, param_2/3=alt target. Ordinals are callbacks.
#include <cstdint>
#include <cstring>
namespace d2client_move_target {
using U=uint32_t; using P=void*;
static U rd(const void* p,U n){U v;std::memcpy(&v,static_cast<const uint8_t*>(p)+n,4);return v;}
static void wr16(void* p,U n,uint16_t v){std::memcpy(static_cast<uint8_t*>(p)+n,&v,2);}
struct Calls {
 U (*f1630)(P); U (*f1660)(P);       // unit x / y
 U (*a10331)();                      // result unused
 U (*a10769)(U,U,U,U);               // distance
 U (*a10651)(); U (*a11142)(); void (*a11087)(U,U);
 bool (*f1e860)(P,U);
 U (*f1400)(); U (*a11010)(U,U,U);   // (DAT_6fbcc3b8,x,y)
 void (*f49e0)(U,U); void (*f8e20)(U,U,U); void (*a10906)(U);
};
struct Globals { P player; U g9804; U cc3b8; };  // DAT_6fbcbbfc, DAT_6fbc9804, DAT_6fbcc3b8
// 1.13c D2Client+0x2A1B0
void sub_2A1B0(P esi,U x,U y,U mode,U p2,U p3,const Globals& g,Calls& c){
 x&=0xffff; y&=0xffff; // mode stays full 32-bit: retail compares param_1 as int; only the store and the nonzero gate use (short)
 if(!x||!static_cast<int16_t>(mode)) return;
 if(rd(esi,0xc4)&0x10000) return;
 U t=rd(esi,0x10), k=rd(esi,0);
 if(k==0){ if(t==0||t==0x11) return; }
 else if(k==1){ if(t==0||t==0xc) return; }
 wr16(esi,0x8c,static_cast<uint16_t>(x)); wr16(esi,0x8e,static_cast<uint16_t>(mode));
 c.a10331();
 U cx=c.f1630(esi), cy=c.f1660(esi);
 int32_t lim;
 if(mode==1) lim=10;
 else if(mode==2) lim=0;
 else if(esi==g.player){
  U b=(g.g9804+0x32)>>7;
  lim=t==1?b+3:t==3?b+7:b+5;
 }else{
  lim=15;
  if(k==1){ if(t>=3&&t<=5) lim=5; else if(t>=6&&t<=0xb) lim=7; }
 }
 int32_t dx=int32_t(x-cx), dy=int32_t(y-cy);
 if(dx<0)dx=-dx; if(dy<0)dy=-dy;
 bool near=dy<=lim&&dx<=lim;
 if(!near && mode==0 && int32_t(p2)>0){
  int32_t d=int32_t(c.a10769(cx,cy,x,y));
  if(d<100){ lim=d; near=int32_t(c.a10769(cx,cy,p2,p3))<lim; }
 }
 if(near){
  U r=c.a10651(); U q=c.a11142(); c.a11087(q,y);
  if(x==cx) return;
  if(y==cy) return;
  if(c.f1e860(esi,r)) return;
  if(c.f1e860(esi,p2)) return;
 }
 U v=c.f1400();
 if(v||(v=c.a11010(g.cc3b8,x,y))){
  if(esi==g.player){ U yy=c.f1660(esi), xx=c.f1630(esi); c.f49e0(xx,yy&0xffff); return; }
  if(k==0){ c.f8e20(v,x,y); return; }
  c.a10906(rd(esi,0x2c));
 }
}
}
