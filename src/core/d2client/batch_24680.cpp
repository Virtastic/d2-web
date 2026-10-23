// Derived from cached 1.13c D2Client decompiles. Callees/ordinals are injected; globals live in G.
#include <cstdint>
#include <cstring>
namespace d2client_24680 {
using U=uint32_t; using I=int32_t; using P=void*;
static U rd(P p,U n){U v;std::memcpy(&v,static_cast<uint8_t*>(p)+n,4);return v;}
static void wr(P p,U n,U v){std::memcpy(static_cast<uint8_t*>(p)+n,&v,4);}
static U w(P s,U i){return rd(s,i*4);}
static void ww(P s,U i,U v){wr(s,i*4,v);}
struct G{ U mx,my,cx,cy; U f339d,f339e,f339f; U heldTime; I screenW,screenH; I mode,hasItem,cur,curA,curB,sel,x,y; };
struct Calls{
 P (*cfg)(); void (*fatal)(); void (*o10595)(); void (*o11090)(); U (*f1a220)(); U (*lcg)(uint64_t*); U (*f37e0)(P);
 U (*tick)(); U (*f1a0c0)(I); void (*f5280)(); void (*f1b300)(P,I,I,I,I); void (*f19660)(I);
 I (*o10174)(U,I,I); I (*o10522)(); void (*o10159)(U); I (*f9f30)(); void (*fa750)(I,P,I*); U (*f6e0)();
 U (*face070)(I,I); I (*o10750)(); I (*o10867)(); void (*f38a70)(P,I,I,I); U (*f56ef0)(U,U,U,I); P (*resolve)(U);
 U (*glob)(); P gp; U gpa;
};
static U lcg(P s){ uint64_t v=(uint64_t(w(s,8))<<32|w(s,9)); (void)v; uint64_t n=uint64_t(w(s,8))*0x6ac690c5ull+w(s,9); ww(s,8,U(n)); ww(s,9,U(n>>32)); return U(n);} // low word first (little endian)
// 1.13c D2Client+0x24680
U sub_24680(P s,Calls& c,G& g){
 P cfg=c.cfg(); U t=*(uint8_t*)((uint8_t*)cfg+0x1b4); if(t>0x12){c.fatal();return 0;}
 auto cell=[&](U off){ return U(*(uint8_t*)((uint8_t*)c.resolve(w(s,5))+off)); };
 auto d8=[&](U i){ return rd(c.resolve(w(s,5)),0xd8+i*4); };
 auto seed=[&]{ uint64_t n=uint64_t(w(s,8))*0x6ac690c5ull+w(s,9); ww(s,8,U(n)); ww(s,9,U(n>>32)); return U(n); };
 auto refresh=[&]{ c.o11090(); c.f1a220(); };
 switch(t){
 case 2: case 3:
  if(!w(s,0x32)){ U n=seed(); ww(s,0x35,n&0x3f); ww(s,0x32,1); }
  if((w(s,0x35)&0x3f)==0&&w(s,4)!=1){ ww(s,4,1); c.o10595(); }
  ww(s,0x35,w(s,0x35)+1);
  if(w(s,4)==2){ U n=seed(); ww(s,0x35,w(s,0x35)+(n&0x3f)); ww(s,4,0); c.o10595(); }
  if(t==3) c.f37e0(s);
  return 1;
 case 4: { U n=c.tick();
  if(w(s,0x35)<n){ ww(s,0x35,c.lcg((uint64_t*)((uint8_t*)s+32))+n); ww(s,0x11,cell(0x12e)); refresh(); }
  if(w(s,4)==3&&I(rd(c.resolve(w(s,5)),0xe4))-0x100<=I(w(s,0x11))){ ww(s,0x11,cell(0x129)); refresh(); }
  return c.f37e0(s)&0xffffff00; }
 case 5: { U n=c.tick();
  if(w(s,0x35)<n){ ww(s,0x35,c.lcg((uint64_t*)((uint8_t*)s+32))+n); ww(s,0x11,cell(0x12e)); seed(); refresh(); }
  U m=w(s,4); if((m==3||m==4)&&I(d8(m))-0x100<=I(w(s,0x11))){ ww(s,0x11,cell(0x129)); refresh(); }
  return c.f37e0(s)&0xffffff00; }
 case 6: { U n=c.tick();
  if(w(s,0x35)<n){ ww(s,0x35,c.lcg((uint64_t*)((uint8_t*)s+32))+n); ww(s,0x11,cell(0x12e)); refresh(); }
  if(w(s,4)>2&&I(w(s,0x11))==I(d8(w(s,4)))-0x100){ ww(s,0x11,cell(0x129)); refresh(); }
  return c.f37e0(s)&0xffffff00; }
 case 7: case 10: { U n=c.tick(); U wait=t==7?10000:3000;
  if(w(s,4)==0){ if(w(s,0x35)<n){ ww(s,0x35,n+wait); ww(s,0x11,cell(0x12a)); refresh(); } }
  else if(w(s,0x35)<n){ ww(s,0x35,c.lcg((uint64_t*)((uint8_t*)s+32))+n); refresh(); }
  return 1; }
 case 8: if(s&&w(s,4)&&!g.f339d){ g.f339d=1; c.f1a0c0(1); } return 1;
 case 0xc: if(s&&w(s,4)&&!g.f339e){ g.f339e=1; c.f1a0c0(0); c.f1a0c0(0); c.f1a0c0(0); } return 1;
 case 0xe: if(w(s,0x35)<c.tick()){ c.f5280(); ww(s,0x35,c.tick()+500); } return 1;
 case 0xf: if(w(s,0x35)<c.tick()){ if(w(s,4)==0){ c.f1b300(s,3,0,0,0); ww(s,0x35,c.tick()+500); } else { c.f19660(0x48); ww(s,0x35,c.tick()+500); } } return 1;
 case 0x10: if(w(s,4)==1&&w(s,0x11)==cell(0x12a)*0x100){ ww(s,4,2); ww(s,0x11,0); c.f1a220(); c.o10595(); } return 1;
 case 0x11: if(g.f339f&&!(c.f6e0()&0xff)){
   I k=I(w(s,0)); I a;
   if(k==2||(k>3&&k<6)) a=I(rd(c.resolve(w(s,0xb)),0x10)); else a=w(s,0xb)?c.o10750():0;
   if(k!=2&&(k<4||k>5)) c.face070(w(s,0xb)?c.o10867():0,a); else c.face070(I(rd(c.resolve(w(s,0xb)),0xc)),a); }
  return 1;
 case 0x12: if(w(s,0x35)<c.tick()){ if(seed()%100<10) c.f38a70(s,0,0,0); U n=seed(); ww(s,0x35,c.tick()+(n%0x3c)*1000); } return 1;
 case 9: { U n=c.tick(); U v=w(s,0x35);
   if(v){ if(n-v<0x3e9) return 1; if(c.o10522()>0x19) return c.f56ef0(0x6fbb9e08,w(s,3)&0x7f,w(s,3),2)&0xffffff00; }
   ww(s,0x35,n); return 1; }
 default: return 1; }
}
// 1.13c D2Client+0x266C0
struct S66{ I mode,hold,curA,curB,sel,selA; };
void sub_266C0(I idx,U arg,S66& st,I (*f6460)(),I (*o11017)(U),P (*o10455)(U),I (*f1630)(P),void (*o10333)(),P cur[2],I* b){
 I i=f6460(); if(i<0) return; I v=o11017(arg);
 if(st.mode==6||st.mode==8) return;
 if(v){ st.sel=i; cur[0]=cur[1]=nullptr; b[0]=1; return; }
 P p=o10455(arg); st.sel=i;
 if(!p){ b[0]=0; cur[0]=cur[1]=nullptr; return; }
 b[0]=1; if(p!=cur[1]){ I r=f1630(p); o10333(); cur[0]=cur[1]=p; b[1]=r+0xe; b[2]=i; }
 (void)idx;
}
// 1.13c D2Client+0x267B0
void sub_267B0(const G& g,void (*o10003)(I,I,I),void (*o10085)()){
 if(g.screenW/2-8<=I(g.my)&&I(g.my)<=g.screenW/2+5&&g.screenH-0x27<=I(g.mx)&&I(g.mx)<=g.screenH-0xd){ o10003(g.screenH-0x27,0,1); o10085(); }
}
// 1.13c D2Client+0x26820
void sub_26820(I sel,void (*f65e0)(),void (*o10333)(P),void (*o10014)(I,I),U arg,I base){
 f65e0(); if(sel!=2){ (void)arg; o10333(nullptr); o10014(base,sel); }
}
// 1.13c D2Client+0x26880
void sub_26880(const G& g,I pressed,I hot,void (*o10041)(const U*,I,I,I,I,I),U tail){
 U b[18]={0}; b[13]=tail; bool in=hot&&g.screenW/2-8<=I(g.my)&&I(g.my)<=g.screenW/2+5&&g.screenH-0x27<=I(g.mx)&&I(g.mx)<=g.screenH-0xd;
 b[0]=pressed?(in?3:2):(in?1:0);
 o10041(b,g.screenW/2-8,g.screenH-0x10,-1,5,0);
}
}
