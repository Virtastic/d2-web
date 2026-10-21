// Prior art used: none (D2MOO has no D2Client). Derived from cached 1.13c decompile.
#include <cstdint>
namespace d2client_45660 {
using I=int32_t; using U=uint32_t; using P=void*;
struct G { // D2Client globals, RVA = 0x6fb.... - 0x6fa00000 base
 U now;                // bbfc
 I c340,c34c,c348,c344,c350,c354,c35c,c360,c368,c36c,c370,c374,c358; // c3xx
 U tick97f8;           // c97f8
 P tbl254; I cnt258,lo25c,hi260; // table of 0x58-byte rows
 I* arr75b8;           // c75b8
};
struct Calls {
 P (*ord10331)(U); P (*ord10826)(P); P (*ord10014)(P); I (*ord10001)(); I (*ord11)();
 void (*f388b0)(); void (*f38a70)(I,I,I,I); I (*f55440)(); void (*f38930)(I);
 I (*f37d20)(U,I); I (*f37af0)(); I (*f37de0)(U,I); bool (*f38080)(U,I);
 uint8_t* (*f0da50)(); void (*ad3e20)();
};
static I rd(const uint8_t* p,U o){I v;__builtin_memcpy(&v,p+o,4);return v;}
// 1.13c D2Client+0x45660
void sub_45660(G& g,Calls& c){
 U t=g.now; if(!t) return;
 P u=c.ord10331(t); if(!u) return;
 I iv=(I)(intptr_t)c.ord10826(u); // handle kept as 32-bit id
 if(!iv) return;
 if(iv!=g.c34c){g.c340=(I)t;g.c34c=iv;}
 I r=0; U cls=0;
 if(P a=c.ord10331(g.now)) if(P b=c.ord10826(a)) cls=*(uint8_t*)((uint8_t*)c.ord10014(b)+0x21c);
 if(g.tbl254&&(I)cls<g.cnt258){ I* row=(I*)((uint8_t*)g.tbl254+cls*0x58); r=*row; if(r&&(r<g.lo25c||g.hi260<r)) r=0; }
 bool ok=c.ord10001()&&c.ord11();
 U dt=t-(U)g.c340;
 if(g.c348!=r&&(0x4a<dt||!ok||g.c348==0)){g.c350=(I)t;g.c348=r;}
 if(!ok) c.f388b0();
 if(g.c348==0) c.f388b0();
 I cur=g.c348; U ecx=g.tick97f8;
 if(g.c354){
  if(g.c360&&g.c35c<=(I)g.tick97f8){c.f38a70(0,0,0,0);g.c360=0;}
  cur=g.c348;
  if(g.c36c&&g.c368<=(I)g.tick97f8){ if(!c.f55440()) c.f38a70(0,0,0,0); g.c36c=0; cur=g.c348; }
  if((I)g.tick97f8<g.c370) return;
  g.c354=0; if(ok) c.f38930(g.c358);
 }
 if(cur&&ok&&!c.f37d20(ecx,cur)){
  I v=g.arr75b8[cur-g.lo25c];
  I m=(!c.f37af0()&&!v)?2:0;
  c.f388b0(); c.f38a70(0,0,m,v); g.c344=(I)t; cur=g.c348;
 }
 if(g.c34c!=g.c374&&0x3d<t-(U)g.c340){c.ad3e20();g.c374=g.c34c;cur=g.c348;}
 if(cur&&(U)(g.c350+0x7d)<t){
  uint8_t* d=c.f0da50(); I h=c.f37d20(ecx,cur);
  if(h&&c.f38080(ecx,h)){
   I k=c.f37de0(ecx,h), o=0, a=rd(d,0x54), b=rd(d,0x58), e=rd(d,0x5c);
   if(-1<k&&k<a)o=a; if(a<=k&&k<b)o=b; if(b<=k&&k<e)o=e;
   g.arr75b8[g.c348-g.lo25c]=o; g.c350=(I)t;
  }
 }
}
}
