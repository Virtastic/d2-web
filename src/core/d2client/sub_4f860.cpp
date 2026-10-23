// D2Client sub_4F860: per-frame ambient/weather-sound updater (stdcall, one arg: now tick).
// Every callee and global goes through the seam; Env holds the D2Client globals it owns.
#include <cstdint>
namespace d2client_4f860 {
using U=uint32_t;
struct Env { // D2Client globals (RVA = VA-0x6fab0000)
 U bbfc,c280,c3b8,c254,c258; // read: game, enable, act-key, table base, table count
 U c310,c314,c318,c31c,c320,c324,c328,c32c; // read/write state
};
struct Services {
 virtual ~Services()=default;
 virtual U rd32(U addr)=0;
 virtual U game(U g)=0;            // +0xC1F0 (#10331)
 virtual U unit(U g)=0;            // +0xC1DE (#10826)
 virtual U act(U key)=0;           // +0xC7E4 (#10396)
 virtual U unitData(U u)=0;        // +0xC214 (#10014)
 virtual bool f80450()=0;          // +0x80450
 virtual bool weather(int32_t* scaled)=0; // 0x4F770 != const; *scaled = ftol(w*k)
 virtual void f88810(U a,U b)=0;
 virtual void f88770(U a,U b)=0;
 virtual void f88230(U eax,U self,U h,U a,U b)=0;
 virtual U f88a70()=0;
 virtual void f87ed0(U h,U v)=0;
 virtual U f87f00(U h)=0;
 virtual void f88bf0(U h)=0;
 virtual void f88ba0(U h,int32_t x,int32_t y,int32_t z)=0;
 virtual U rand(U n)=0;            // 0x89640
 virtual void f5cf90(U v)=0;
 virtual void f4f750()=0;
 virtual void f4f720(U v)=0;
};
// ponytail: retail reads an uninitialised slot as sound id when the table is absent; we use 0.
// 1.13c D2Client+0x4F860
void sub_4F860(U now,Env& g,Services& s){
 if(!g.c280||!g.bbfc) return;
 U gm=s.game(g.bbfc); if(!gm) return;
 U unit=s.unit(gm); if(!unit) return;
 U act=s.act(g.c3b8);
 U alt=(act<1||act>3)?1:0;
 U tbl=0,snd=0,ebx=0,key=0;
 if(g.bbfc){ U a=s.game(g.bbfc); U b=a?s.unit(a):0; if(b) key=s.rd32(s.unitData(b)+0x21c)&0xff; }
 if(g.c254&&(int32_t)key>=0&&(int32_t)key<(int32_t)g.c258){
  tbl=key*0x58+g.c254;
  if(alt){snd=s.rd32(tbl+0x10);ebx=s.rd32(tbl+8);} else {snd=s.rd32(tbl+0xc);ebx=s.rd32(tbl+4);}
 }
 U flag=s.f80450()?0x40:0;
 int32_t esi=0; { int32_t w; if(s.weather(&w)) esi=w; }
 U c31c=g.c31c;
 if(ebx!=c31c){
  U t4=tbl?s.rd32(tbl+4):0, t8=tbl?s.rd32(tbl+8):0;
  bool sw=(ebx==t4&&c31c==t8)||(ebx==t8&&c31c==t4);
  if(sw) s.f88230(0xfa,g.c314,g.c314,0,0);
  else { s.f88810(ebx,flag); s.f88770(snd,0); }
  if(ebx!=g.c31c){
   if(!ebx){ g.c314=0; g.c31c=0; }
   else {
    g.c314=s.f88a70();
    if(sw){ s.f87ed0(g.c314,0); s.f88230(0xfa,0,g.c314,0xff,0); }
    g.c31c=ebx;
   }
  }
 }
 if(!flag){ s.f88bf0(g.c318); g.c318=0; g.c320=0; }
 else {
  bool adj=true;
  if(!g.c318){
   if(!esi) adj=false;
   else { g.c318=s.f88a70(); if(!g.c318) adj=false; else s.f87ed0(g.c318,esi); }
  }
  if(adj&&g.c318){
   int32_t cur=(int32_t)s.f87f00(g.c318), n;
   if(esi<=cur){ n=cur-6; if(esi>n) n=esi; } else { n=cur+6; if(esi<n) n=esi; }
   if(n==0){ s.f88bf0(g.c318); g.c318=0; } else s.f87ed0(g.c318,n);
  }
  g.c320=flag;
 }
 s.f5cf90(key);
 if(snd){
  U c328=g.c328, c32c=g.c32c;
  if(snd!=g.c324){
   U v=s.rd32(tbl+0x14), q=v/3;
   U r=s.rand(2*q+1); c328=v+(r-q); g.c328=c328;
   c32c=now-s.rand(c328); g.c32c=c32c; g.c324=snd;
  }
  if(now-c32c>=c328){
   U h=s.f88a70();
   if(h){
    U x=s.rand(0x12d)+0x1c2;
    int32_t sg=(s.rand(2)?2:0)-1;
    int32_t y=(int32_t)s.rand(0xc9)-0x64;
    s.f88ba0(h,(int32_t)((U)sg*x),y,0);
    g.c32c=now;
    U v=s.rd32(tbl+0x14), q=v/3;
    g.c328=v+(s.rand(2*q+1)-q);
   }
  }
 }
 if(unit!=g.c310){
  U r1=s.rd32(tbl+0x1c), r2=s.rd32(tbl+0x20);
  s.f4f750();
  if(r1) s.f4f720(r1);
  if(r2) s.f4f720(r2);
  g.c310=unit;
 }
}
}
