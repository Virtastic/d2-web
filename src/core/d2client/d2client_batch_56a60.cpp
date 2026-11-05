// Implementation batch (alpha lane): D2Client+0x56A60..0x57860. Decompile-derived; not yet vectored.
// Imported ordinals and unrecovered helpers are Svc virtuals named by ordinal/RVA; args marked REG? are
// register-passed and unrecovered. 0x573A0 and 0x578F0 are not implemented (stack/register args garbled).
#include "d2client_batch_56a60.h"
#include <cstdint>
namespace d2client_batch_56a60 {

// old form of D2Client 56A60 (now d2client_56a60.cpp)
// (fastcall ECX=unit, EDX=idx; stack a4 passed to 11086; ret 4)
I sub_56A60(Svc& s,Unit* u,I idx,U a4){
 if(!s.row(idx)) return 0;
 I o=s.o10511(u); if(!o) return 0;
 I l1,l2; uint8_t l3[8];
 if(s.o11086(u,1,idx,a4,l3,&l2,&l1)<0) return 0;
 s.o10554(o,l2); s.o10748(o,l1); s.o10331(u); I f=s.f1400(); s.o10565(f,l2,l1,1,0x100);
 return 1;
}
// 1.13c D2Client+0x56B20
// (fastcall ECX=unit, EDX=idx; stack a4 passed to 10786; ret 4)
I sub_56B20(Svc& s,Unit* u,I idx,U a4){
 Row* r=s.row(idx); if(!r) return 0;
 I o=s.o10511(u); if(!o) return 0;
 I v;
 if(!(s.o10397(o)&2)) v=r->f138; else { s.o10137(o,0); v=r->f13c; }
 s.o10542(u,s.o10786(u,v,idx,a4)); return 1;
}
// old form of D2Client 56BB0 (now d2client_56bb0.cpp)
I sub_56BB0(Svc& s,Unit* u){
 if(u) u->flags&=0xfffffff1u;
 I x=s.unitX(u), y=s.unitY(u);
 s.o11073(s.o10331(u),x,y,5,0x1000); return 1;
}
// 1.13c D2Client+0x56DA0  (p1 = stack arg whose byte +0x11 is tested; esi = unit, words 0x11/0x12 = unit+0x44/+0x48)
void sub_56DA0(Svc& s,const uint8_t* p1,Unit* esi,I* w){
 if(esi && esi->type==1){
  const uint8_t* r=s.f1260(w[1]);
  if(r){
   if(p1[0x11]!=0x0e) w[0x11]=(I)r[0x109]*0x100-0x100;
   else { s.o10542r(esi,(I)r[0x109]-1); w[0x12]=((I)r[0x109]+1)*0x100; }
  }
  return;
 }
 s.o10542r(esi,8); w[0x12]=0x500;
}
// old form of D2Client 56E10 (now d2client_56e10.cpp)  (EAX = row idx, EBX = obj (+4 ent index, +0x2c handle), stack: a1 (passed to 10786), p2; ret 8)
I sub_56E10(Svc& s,const void* a1,I p2,I idx,const I* obj){
 Row* r=s.row(idx); if(!r||!obj) return 0;
 const uint8_t* e=(const uint8_t*)s.ent(obj[1]); if(!e) return 0;
 I v=s.o10786(a1,r->f138,idx,(U)p2);
 if(v<1) v=*(const I*)(e+0x3c)+p2-1;
 I n; if(v<2) n=1; else n=v<0xff?v:0xff;
 s.o10168(obj[0xb],n);
 s.o10441((void*)obj,(I)(((U)e[0x178]<<8)/(U)n)); s.o10558((void*)obj,n); s.o10970((void*)obj,n);
 return 1;
}
// old form of D2Client 56EE0 (now d2client_56ee0.cpp)  (fastcall ECX=unit, EDX=idx; stack a4 passed to 10786; ret 4)
I sub_56EE0(Svc& s,Unit* u,I idx,U a4){
 Row* r=s.row(idx); if(!r) return 0;
 I o=s.o10511(u); if(!o) return 0;
 s.f0b0(u);
 I handle=((const I*)u)[0xb];
 if(!handle) s.fatal(0x946);
 I* p=(I*)s.o10392p(handle); if(!p) return 0;
 U seed[2]; s.o10936(seed); s.o10614(seed,p[3]+((const I*)u)[3]);
 I a=s.o10786(u,r->f138,idx,a4);
 I b2=s.o10786(u,r->f13c,idx,a4);
 I n=s.f1380(seed,b2-a)+a; if(n<1) n=1;
 s.o10243(o,n); s.o10554(o,p[3]); s.o10748(o,p[0]);
 return 1;
}
// 1.13c D2Client+0x57000  (fastcall ECX=unit; handle = unit+0x2c)
I sub_57000(Svc& s,Unit* u){
 s.f0b0(u);
 I handle=((const I*)u)[0xb];
 if(!handle) s.fatal(0x946);
 if(!s.o10392(handle)) return 0;
 u->flags2&=0xfffbffffu;
 return 1;
}
// 1.13c D2Client+0x57200  (fastcall ECX=unit, EDX=idx; stack a4 passed to 60450; ret 4)
I sub_57200(Svc& s,Unit* u,I idx,U a4){
 Row* r=s.row(idx); if(!r) return 0;
 s.o10595(u); if(u) u->flags|=0xe;
 const uint8_t* rb=(const uint8_t*)r; int16_t w; __builtin_memcpy(&w,rb+0xea,2); I ebx=w;
 if(ebx>0 && ebx<s.entCount()){
  if((s.o10459(s.o10826(s.o10331(u)))&0xff)==1){ int16_t d; __builtin_memcpy(&d,rb+0xec,2); if(d>0 && d<s.entCount()) ebx=d; }
  I c60=s.f1660(u); I c30=s.f1630(u);
  s.f60450(ebx,c30,c60,idx,a4);
 }
 I x=s.unitX(u), y=s.unitY(u);
 const I xy[2]={x,y};
 I a=s.o10589(u,0x1c09,1);
 I room=s.o10331(u);
 I rr=s.o10509(room,xy,a); if(!rr) return 0;
 I handle=((const I*)u)[0xb]; if(!handle) return 0;
 if(!s.o10075(handle,u,rr,x,y)) return 0;
 I c60=s.f1660(u); I c30=s.f1630(u);
 s.o10565(s.o10331(u),c30,c60,1,0x1000);
 return 1;
}
// 1.13c D2Client+0x57540  (fastcall ECX=unit, EDX=p2; stack p3; ret 4). X = path+0xc, Y = path+0x10 (Y is read first).
I sub_57540(Svc& s,Unit* u,I p2,I p3){
 void* a=s.f51540(u,0x150,p2,p3);
 void* b=s.f51540(u,0x151,p2,p3);
 I y,x;
 y=s.unitY(u); x=s.unitX(u); void* o14f=s.f5ff10(u,0x14f,x,y,p2,p3);
 y=s.unitY(u); x=s.unitX(u); void* o151=s.f5ff10(u,0x151,x,y,p2,p3);
 y=s.unitY(u); x=s.unitX(u); void* o14e=s.f5ff10(u,0x14e,x,y,p2,p3);
 y=s.unitY(u); x=s.unitX(u); void* o14d=s.f5ff10(u,0x14d,x,y,p2,p3);
 void* hi[2]={o14f,o151};
 for(void* o:hi) if(I* g=s.gfxBlock(o)){ g[4]=0; g[5]=0; g[6]=0x7800; }
 void* lo[2]={a,b};
 for(void* o:lo) if(o){
  if(I* g=s.gfxBlock(o)){ g[1]=0; g[2]=0; g[3]=0xafc800; }
  if(I* g=s.gfxBlock(o)){ g[4]=0; g[5]=0; g[6]=(I)0xffff3800; }
 }
 if(o14e){ s.o10441(o14d,0x80); s.o10441(o14e,0x80); }
 return 1;
}
// 1.13c D2Client+0x57860  (fastcall ECX=unit; words 0x11/0x12 at +0x44/+0x48; flags at +0xc4; 10243 takes the 10630 handle)
I sub_57860(Svc& s,Unit* u,I* w){
 I h=s.o10630(); I n=s.o10980(h);
 I k=w[0x11]>>8;
 if(k==6){ s.o10542(u,7); w[0x12]=0x3200; }
 else if(k==0x16){
  if(n>3){ s.o10542(u,0x28); w[0x12]=0x1100; return 1; }
  s.o10243(h,n+1); return 1;
 } else if(k==0x1e){ s.f6c0(0xc); u->flags|=0x10000; return 1; }
 return 1;
}
}
