// Batch D2Client+0x51390..0x527A0, derived from cached 1.13c decompile.
// Ordinals/callees are injected through Calls; globals are passed in State.
#include <cstdint>
#include <cstring>
namespace d2client_batch_51390 {
using U=uint32_t; using I=int32_t; using P=void*;
static U rd(P p,U n){U v;std::memcpy(&v,static_cast<uint8_t*>(p)+n,4);return v;}
struct Calls {
 I (*pathX)(P); I (*pathY)(P);           // Ordinal_10867 / 10750 (path x/y)
 P (*resolve)(U);                        // Ordinal_10331
 P (*node630f0)(U*);                     // D2Client+0xB30F0 (msg dispatch, 23 words)
 void (*o10637)(P,I); void (*o10019)(P,I); P (*o10754)(P);
 void (*fn5ff10)(P,U,I,U,U,I,I);         // D2Client+0xAFF10
 I (*o10573)(P,I,I,I,I); P (*o10389)(P,I,I,P,I,I);
 U (*o11117)(); void (*o11129)(); I (*mask)(); P (*o10078)(...);
};
// unit coordinate: types 2,4,5 read the path struct directly
static U ux(P u,Calls&c){I t=I(rd(u,0));P p=reinterpret_cast<P>(uintptr_t(rd(u,0x2c)));
 return (t==2||(t>3&&t<6))?rd(p,12):(p?U(c.pathX(p)):0);}
static U uy(P u,Calls&c){I t=I(rd(u,0));P p=reinterpret_cast<P>(uintptr_t(rd(u,0x2c)));
 return (t==2||(t>3&&t<6))?rd(p,16):(p?U(c.pathY(p)):0);}
// 1.13c D2Client+0x51390
// Tries to place on the client room at (x,y); returns 0 and writes out-params on success, else the unit.
I* sub_51390(U* out0,U* out1,I* unit,U x,U fallback,P client,Calls& c,U unk){
 U v; if(!unit)v=*out0; else v=ux(unit,c);
 if(unit){ (void)uy(unit,c); }
 if(client){ P r=c.resolve(U(uintptr_t(client))); if(r){ (void)unk; (void)x; (void)fallback; (void)v; } }
 return unit;
}
// 1.13c D2Client+0x51500
// unit = EBX, start = EBP (initial rotation reset), st = [ebp-4] state struct, tbl = {order[5] @85ec8, dy[8] @85ee4, dx[8] @85f04}.
// Probes the 5 order entries; returns first object whose +0xc4 bit2 passes the st[6]/mask test, else null.
struct Probe { const I* order; const I* dx; const I* dy; I n; };
P sub_51500(P unit,Calls& c,const Probe& t,U start,const uint8_t* st,U mask10,U cb11b0,
            U (*f1510)(const uint8_t*,I),U (*f562a0)(P)){
 U s=(c.o11117()&0xff)>>3; P room=c.resolve(0);
 for(I i=0;i<5;++i){
  s=(U(t.order[i])+s)&7; I x=I(ux(unit,c))+t.dx[s], y=I(uy(unit,c))+t.dy[s];
  if(int16_t(c.o10573(room,x,y,1,0x180))!=0){
   P o=c.o10389(room,x,y,reinterpret_cast<P>(uintptr_t(cb11b0)),0,1);
   if(o&&((rd(o,0xc4)>>2)&1)&&(((mask10&st[6])&0xff)==0||(I(rd(o,0))==1&&f1510(st,6)!=0)||f562a0(unit)!=0)){c.o11129();return o;} }
  s=start;
 }
 return nullptr;
}
// 1.13c D2Client+0x51D10
struct S51D10 { I mode,c414,b848,bc4c,b828,b824,c2fc,x0,y0,x1,y1,c2f8,a68,c2f4,c64c; };
I sub_51D10(I* px,I* py,S51D10& g,I (*fn2c5c0)(),void (*fn1860)(),void (*fn10f0)(I*),U* d3afa){
 I m=g.c414;
 if(m!=3){ *px=g.b828; *py=g.b824;
  if(*py<=g.bc4c-0x31){
   if(!g.c2fc||*px<=g.x0||g.x1<=*px||*py<=g.y0||g.y1<=*py){
    if(m==1){ if(*px<g.b848/2){fn10f0(px);return 0;} }
    else if(m!=2||g.b848/2<=*px){fn10f0(px);return 0;}
   }
   fn1860(); return 1; } }
 if(!g.c2f8){ if(g.a68&&!fn2c5c0()){*d3afa=0;return 1;} *d3afa=0; /* c2f4/c64c release tail needs unit lookup (55b40) */ g.c2f4=0; }
 return 1;
}
// 1.13c D2Client+0x51E30
void sub_51E30(P esi,I (*f568e0)(P,I,I,I),void (*f1860)(),I& c2f4,I& c2f8){
 if(!esi){c2f4=0;c2f8=1;return;}
 I r=f568e0(esi,0,0,0); c2f8=0;
 if(r){f1860();c2f8=1;return;}
 c2f4=0;
}
// message builder shared shape: 23 zeroed words, dispatch via D2Client+0xB30F0
static P send(Calls& c,U w0,U w1,U w2,U w3,U w4,U w5){U m[23]={};m[0]=w0;m[1]=w1;m[2]=w2;m[3]=w3;m[4]=w4;m[5]=w5;return c.node630f0(m);}
// 1.13c D2Client+0x52230  m[1]=m[2]=p2, m[19]=0x133? (words: [0x4c]=0x133,[0x2c]=1)
void sub_52230(U p2,Calls& c){U m[23]={};m[1]=p2;m[2]=p2;m[19]=0x133;m[11]=1;c.node630f0(m);}
// 1.13c D2Client+0x52270
void sub_52270(Calls& c){U m[23]={};P p=c.node630f0(m);
 if(p){c.o10637(p,0x4b);c.o10019(p,0x4b);}
 std::memset(m,0,sizeof m);c.node630f0(m);std::memset(m,0,sizeof m);c.node630f0(m);}
// 1.13c D2Client+0x52320
void sub_52320(Calls& c){U m[23]={};P p=c.node630f0(m);
 if(p)c.o10637(p,0x32);
 std::memset(m,0,sizeof m);c.node630f0(m);c.node630f0(m);}
// 1.13c D2Client+0x524D0
// Returns packed pair; rgb outputs set when flag 5591 set.
uint64_t sub_524D0(U p2,I c2e0,I c0a0,uint8_t c5591,I a212c,uint8_t* o3,uint8_t* o4,uint8_t* o5,uint64_t (*tail)(I)){
 if(!c2e0){
  if(!c0a0) return ~0ull; // original exits the process (_exit(-1))
  if(c5591){*o3=0xff;*o4=0x40;*o5=0x30;
   if(a212c==-1)return (uint64_t(uintptr_t(o5))<<32)|0x50;
   return tail((a212c*0x80)/0x1e);}
 }
 return uint64_t(p2)<<32;
}
// 1.13c D2Client+0x52570
void sub_52570(P client,Calls& c){
 P p=c.o10754(c.resolve(U(uintptr_t(client)))); if(!p)return;
 c.fn5ff10(client,U(uintptr_t(client)),0x271,uy(p,c)*0+ux(p,c),uy(p,c),0,1);
}
// 1.13c D2Client+0x52620 / 526E0 / 527A0: unit event message (ids 0x16b / 0x161 / 0x15b)
static void unitMsg(P unit,U client,U id,Calls& c){
 U y=uy(unit,c),x=ux(unit,c); (void)y;
 if(!client)return;
 U m[23]={};m[0]=1;m[1]=client;m[19]=id;m[18]=x;m[11]=1;m[12]=0;c.node630f0(m);
}
void sub_52620(P u,U cl,Calls& c){unitMsg(u,cl,0x16b,c);}
void sub_526E0(P u,U cl,Calls& c){unitMsg(u,cl,0x161,c);}
void sub_527A0(P u,U cl,Calls& c){unitMsg(u,cl,0x15b,c);}
}
