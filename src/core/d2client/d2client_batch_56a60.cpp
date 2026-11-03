// Implementation batch (alpha lane): D2Client+0x56A60..0x57860. Decompile-derived; not yet vectored.
// Imported ordinals and unrecovered helpers are Svc virtuals named by ordinal/RVA; args marked REG? are
// register-passed and unrecovered. 0x573A0 and 0x578F0 are not implemented (stack/register args garbled).
#include <cstdint>
namespace d2client_batch_56a60 {
using I=int32_t; using U=uint32_t;
struct Unit { I type; I pad[0x30]; U flags; U flags2; };   // +0 type; +0xC4 flags (word 0x31); +0xC8 flags2
static_assert(__builtin_offsetof(Unit,flags)==0xc4 && __builtin_offsetof(Unit,flags2)==0xc8);
struct Row { I a[0x4e]; I f138, f13c; };                     // monster-row fields at +0x138/+0x13c only
struct Svc {
 virtual ~Svc()=default;
 virtual Row* row(I idx)=0;            // sgptDataTables+0xb98 table (0x23c stride), bound +0xba0; nullptr if out of range
 virtual const void* ent(I idx)=0;     // sgptDataTables+0xb64 table (0x1a4 stride), bound +0xb6c
 virtual I o10511(Unit*)=0; virtual I o11086(Unit*,I,I,U,void*,I*,I*)=0; virtual void o10554(I,I)=0;
 virtual void o10748(I,I)=0; virtual I o10331(Unit*)=0; virtual I f1400()=0; virtual void o10565(I,I,I,I,I)=0;
 virtual U o10397(I)=0; virtual void o10137(I,I)=0; virtual I o10786(const void*,I,I=0,U=0)=0; virtual void o10542(Unit*,I)=0;
 virtual I unitX(Unit*)=0; virtual I unitY(Unit*)=0;     // X = path+0xc (else ord 10867 / 0), Y = path+0x10 (else ord 10750 / 0); type in {2,4,5} reads the path directly
 virtual void o11073(I room,I x,I y,I a,I b)=0; virtual I entCount()=0;  // sgptDataTables+0xb6c
 virtual const uint8_t* f1260(I eaxUnitId)=0;     // FUN_6fab1260 -> record, byte +0x109 used
 virtual void o10542r(Unit*,I)=0;      // Ordinal_10542 (args REG?)
 virtual void fatal(I line)=0;         // Ordinal_10265/10024(&DAT_6fb8359f) + _exit(-1)
 virtual void f0b0(Unit* eaxUnit)=0;   // FUN_6fada0b0 (EAX = unit)
 virtual I o10392(I)=0;
 virtual void* o10392p(I)=0;
 virtual void o10168(I,I)=0; virtual void o10441(void*,I)=0; virtual void o10558(void*,I)=0; virtual void o10970(void*,I)=0;
 virtual void o10243(I,I)=0; virtual I f1380(U* seed,I esiRange)=0; virtual void o10936(U* seed)=0; virtual void o10614(U* seed,I edx)=0;
 virtual void o10595(Unit*)=0; virtual I o10826(I)=0; virtual I o10459(I)=0; virtual I f1660(Unit*)=0;
 virtual I f1630(Unit*)=0; virtual void f60450(I,I,I,I,U)=0; virtual I o10589(Unit*,I,I)=0;
 virtual I o10075(I handle,Unit*,I r,I x,I y)=0; virtual I o10509(I room,const I* xy,I a)=0;
 virtual void* f51540(Unit* eaxUnit,I code,I p2,I p3)=0; virtual void* f5ff10(Unit* edx,I code,I x,I y,I p2,I p3)=0;  // 0x57540 lookups (VA 6fb51540 / 6fb5ff10; both stdcall, p2/p3 on the stack)
 virtual I* gfxBlock(void* obj)=0;     // obj null/+0x54 null/(+0x54)->+0x30 null -> nullptr, else that block
 virtual I o10630()=0; virtual I o10980(I)=0; virtual void f6c0(I ediArg)=0;
};

// 1.13c D2Client+0x56A60
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
// 1.13c D2Client+0x56BB0
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
// 1.13c D2Client+0x56E10  (EAX = row idx, EBX = obj (+4 ent index, +0x2c handle), stack: a1 (passed to 10786), p2; ret 8)
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
// 1.13c D2Client+0x56EE0  (fastcall ECX=unit, EDX=idx; stack a4 passed to 10786; ret 4)
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
