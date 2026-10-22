// D2Client options-panel per-frame draw (entry table, stride 0x550). Derived from cached 1.13c decompile.
// Ordinals/globals arrive as callbacks and a state struct. Unrecovered args marked GUESS.
#include <cstdint>
#include <cstring>
namespace d2client_optpanel65230 {
using U=uint32_t; using I=int32_t;
struct St {            // globals
 const I*hdr;          // DAT_6fbcc05c: [0]=count [1]=rowH [2]=top [3]=titleY
 uint8_t*tbl;          // DAT_6fbcc060 entries 0x550 stride: +0 kind +4 hide +8 outY +0x110/+0x11c cb +0x53c flag
 I scrH;               // DAT_6fb8bc4c
 I scrW;               // DAT_6fb8bc48
 I lastW,lastH;        // DAT_6fbd3454/58 (in/out)
 I curW,curH;          // DAT_6fbcb824/28
 I mode;               // DAT_6fbcc068
 I sel;                // DAT_6fbcc058 (in/out)
 U anim;               // DAT_6fbcc064 (in/out)
 I off7c;              // DAT_6fbcc07c
 U cel;                // DAT_6fbcb9c0
 U tick,lastTick;      // GetTickCount, DAT_6fbd3404 (in/out)
};
struct Imp {
 I(*f64080)();                       // sub_64080 hitTest
 void(*f64fd0)(I);                   // sub_64FD0
 I(*o10227)();                       // Ordinal_10227 (visibility)
 void(*o10138)(I,I,I,I);             // Ordinal_10138 (draw row)
 void(*f63bf0)(I,void*,I,I);         // layout_63bf0 consumer
 void(*o10041)(const I*,I,I,I,I,I);  // Ordinal_10041 (cel draw)
 void(*fatal)(U);
};
static I rd(const uint8_t*p,U o){I v;std::memcpy(&v,p+o,4);return v;}
static void wr(uint8_t*p,U o,I v){std::memcpy(p+o,&v,4);}

// 1.13c D2Client+0x65230
void draw_65230(St&s,const Imp&f){
 if(!s.hdr){ f.fatal(0x6be); return; }
 if(!s.tbl){ f.fatal(0x6bf); return; }
 I n=s.hdr[0], rowH=s.hdr[1], top=s.hdr[2];
 I base=s.scrH-0x50;
 if(s.lastW!=s.curW||s.lastH!=s.curH){
  s.lastW=s.curW; s.lastH=s.curH;
  if(s.mode==0){ I r=f.f64080(); if(r!=-1) s.sel=r; } else f.f64fd0(s.curH);
 }
 I vis=0;
 for(I i=0;i<n;i++){
  uint8_t*e=s.tbl+(U)i*0x550;
  wr(e,8,0);
  if(rd(e,4)==0||f.o10227()!=0){
   // cb@+0x110 / +0x11c are per-entry callbacks (state query / pre-draw); host-supplied via table
   typedef I(*Cb)(); Cb c1,c2; std::memcpy(&c1,e+0x110,sizeof c1); std::memcpy(&c2,e+0x11c,sizeof c2);
   bool b=false; if(!c1) b=true; else if(c1()!=0) b=true; // bVar4 set when absent or nonzero
   I st=(b?1:0)*4+1;
   if(c2) c2();
   I y=rowH*vis+(base/2-(rowH*n)/2);
   I out=top+y; wr(e,8,y);
   switch(rd(e,0)){
    case 1: f.o10138(out,0,st,-1); f.o10138(out,2,st,-1); break;
    case 2: if(rd(e,0x53c)!=0) f.o10138(out,0,st,-1);
            f.f63bf0(y,e,st,rd(e,0x53c)==0); break;
    case -1: case 0: f.o10138(out,1,st,-1); break;
   }
   vis++;
  }
 }
 I cel[18]={0}; cel[13]=(I)s.cel;   // GUESS: second Ordinal_10041 arg is a distinct stack copy; same fields assumed
 cel[0]=s.anim?(I)(8-s.anim):0;
 I y=rd(s.tbl+(U)s.sel*0x550,8)+s.hdr[3];
 f.o10041(cel,s.scrW/2-s.off7c-0xf9,y,-1,5,0);
 f.o10041(cel,s.scrW/2+0xf9,y,-1,5,0);
 if(s.tick-s.lastTick>0x32){
  U a=(s.anim+1)&0x80000007u;
  if((I)a<0) a=((a-1)|0xfffffff8u)+1;
  s.anim=a; s.lastTick=s.tick;
 }
}
}
