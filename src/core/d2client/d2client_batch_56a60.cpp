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
 virtual I o10511(Unit*)=0; virtual I o11086(Unit*,I,I)=0; virtual void o10554(I,void*)=0;
 virtual void o10748(I,I)=0; virtual void o10331(Unit*)=0; virtual I f1400()=0; virtual void o10565(I,void*,I,I,I)=0;
 virtual U o10397(I)=0; virtual void o10137(I,I)=0; virtual I o10786(Unit*,I,I=0)=0; virtual void o10542(Unit*,I)=0;
 virtual I unitX(Unit*)=0; virtual I unitY(Unit*)=0;     // type in {2,4,5}: path+0x10 / +0xc; else ord 10750 / 10867 or 0
 virtual I o10331r(Unit*,I,I,I,I)=0; virtual void o11073(I)=0;
 virtual const uint8_t* f1260()=0;     // FUN_6fab1260 -> record, byte +0x109 used
 virtual void o10542r(Unit*,I)=0;      // Ordinal_10542 (args REG?)
 virtual void fatal(I line)=0;         // Ordinal_10265/10024(&DAT_6fb8359f) + _exit(-1)
 virtual void f0b0()=0;                // FUN_6fada0b0
 virtual I o10392(I)=0;
 virtual void* o10392p(I)=0;
 virtual void o10168(I,I)=0; virtual void o10441(I,I)=0; virtual void o10558()=0; virtual void o10970()=0;
 virtual void o10243(Unit*,I)=0; virtual I f1380(Unit*)=0; virtual void o10936()=0; virtual void o10614()=0;
 virtual void o10595(Unit*)=0; virtual I o10826(I)=0; virtual void o10459(I)=0; virtual void f1660(Unit*)=0;
 virtual void f1630(Unit*)=0; virtual void f60450()=0; virtual I o10589(Unit*,I,I)=0; virtual I o10509(I)=0;
 virtual I o10075(Unit*,I,I)=0; virtual void o10565u(I)=0;
 virtual I f51540()=0; virtual I f5ff10(Unit*,I)=0;     // 0x57540 lookups; f5ff10 args: (unit,0x14f.. code,y,x,p2,p3)
 virtual I o10630()=0; virtual I o10980(I)=0; virtual void f6c0()=0;
};

// 1.13c D2Client+0x56A60
I sub_56A60(Svc& s,Unit* u,I idx){
 if(!s.row(idx)) return 0;
 I o=s.o10511(u); if(!o) return 0;
 uint8_t b8[8],c4[4];
 if(s.o11086(u,1,idx)<0) return 0;
 s.o10554(o,b8); s.o10748(o,1); s.o10331(u); I f=s.f1400(); s.o10565(f,b8,o,1,0x100); (void)c4;
 return 1;
}
// 1.13c D2Client+0x56B20
I sub_56B20(Svc& s,Unit* u,I idx){
 Row* r=s.row(idx); if(!r) return 0;
 I o=s.o10511(u); if(!o) return 0;
 I v;
 if(!(s.o10397(o)&2)) v=r->f138; else { s.o10137(o,0); v=r->f13c; }
 s.o10542(u,s.o10786(u,v,idx)); return 1;
}
// 1.13c D2Client+0x56BB0
I sub_56BB0(Svc& s,Unit* u){
 if(u) u->flags&=0xfffffff1u;
 I x=s.unitX(u), y=s.unitY(u);
 s.o11073(s.o10331r(u,y,x,5,0x1000)); return 1;
}
// 1.13c D2Client+0x56DA0  (p1 = stack arg whose byte +0x11 is tested; esi = unit, words 0x11/0x12 = unit+0x44/+0x48)
void sub_56DA0(Svc& s,const uint8_t* p1,Unit* esi,I* w){
 if(esi && esi->type==1){
  const uint8_t* r=s.f1260();
  if(r){
   if(p1[0x11]!=0x0e) w[0x11]=(I)r[0x109]*0x100-0x100;
   else { s.o10542r(esi,0); w[0x12]=((I)r[0x109]+1)*0x100; }
  }
  return;
 }
 s.o10542r(esi,0); w[0x12]=0x500;
}
// 1.13c D2Client+0x56E10  (eax = row index REG?, ebx = obj with +4 ent index, +0x2c handle)
I sub_56E10(Svc& s,Unit* u,I p2,I idx,const I* obj,I handle){
 Row* r=s.row(idx); if(!r||!obj) return 0;
 const uint8_t* e=(const uint8_t*)s.ent(obj[1]); if(!e) return 0;
 I v=s.o10786(u,r->f138);
 if(v<1) v=*(const I*)(e+0x3c)-1+p2;
 if(v<2) v=1; else if(v>0xfe) v=0xff;
 s.o10168(handle,v); s.o10441(0,0); s.o10558(); s.o10970();
 return 1;
}
// 1.13c D2Client+0x56EE0  (handle = unit+0x2c)
I sub_56EE0(Svc& s,Unit* u,I idx,I handle){
 Row* r=s.row(idx); if(!r) return 0;
 if(!s.o10511(u)) return 0;
 s.f0b0();
 if(!handle) s.fatal(0x946-0x946+0); // REG? line constant not shown in this decompile
 I* p=(I*)s.o10392p(handle); if(!p) return 0;
 s.o10936(); s.o10614();
 I a=s.o10786(u,r->f138);
 s.o10786(u,r->f13c,idx);
 I n=s.f1380(u)+a; if(n<1) n=1;
 s.o10243(u,n); s.o10554(0,(void*)(intptr_t)p[3]); s.o10748(0,p[0]);
 return 1;
}
// 1.13c D2Client+0x57000
I sub_57000(Svc& s,Unit* u,I handle){
 s.f0b0();
 if(!handle) s.fatal(0x946);
 if(!s.o10392(handle)) return 0;
 u->flags2&=0xfffbffffu;
 return 1;
}
// 1.13c D2Client+0x57200  (+0x2a8 stand-in fields: row short +0xea)
I sub_57200(Svc& s,Unit* u,I idx,I sh,I handle,I nEnt){
 if(!s.row(idx)) return 0;
 s.o10595(u); if(u) u->flags|=0xe;
 if(sh>0 && sh<nEnt){ s.o10459(s.o10826(0)); s.f1660(u); s.f1630(u); s.f60450(); }
 s.unitY(u); s.unitX(u);
 I a=s.o10589(u,0x1c09,1);
 I o=s.o10509(0);
 if(o && handle && s.o10075(u,o,a)){ s.f1660(u); s.f1630(u); s.o10565u(s.o10331r(u,0,0,1,0x1000)); return 1; }
 return 0;
}
static void clr(I* g,I a,I b,I c,bool pos){ if(!g) return; if(pos){g[1]=0;g[2]=0;g[3]=c;} else {g[4]=0;g[5]=0;g[6]=c;} (void)a;(void)b; }
// 1.13c D2Client+0x57540  (p2,p3 REG?; zeros the 3-vector blocks at gfx+0x54->+0x30)
I sub_57540(Svc& s,Unit* u,I** gfx[4],I* h[4]){
 // gfx[i] = pointer chain (obj+0x54)->+0x30 block for the units found with codes 0x14f,0x151,0x14e,0x14d;
 // h[0]=f51540 #1, h[1]=f51540 #2 (REG? caller supplies resolved blocks, nullptr if absent/zero)
 I* b14f=(I*)gfx[0]; I* b151=(I*)gfx[1]; I* b14e=(I*)gfx[2];
 if(b14f){ b14f[4]=0; b14f[5]=0; b14f[6]=0x7800; }
 if(b151){ b151[4]=0; b151[5]=0; b151[6]=0x7800; }
 for(int i=0;i<2;i++) if(h[i]){ h[i][1]=0; h[i][2]=0; h[i][3]=0xafc800; h[i][4]=0; h[i][5]=0; h[i][6]=(I)0xffff3800; }
 if(b14e){ s.o10441(0,0x80); s.o10441(0,0x80); }
 (void)u;(void)clr; return 1;
}
// 1.13c D2Client+0x57860  (unit esi-less: param_1 words 0x11/0x12 at +0x44/+0x48; flags at +0xc4 word 0x31)
I sub_57860(Svc& s,Unit* u,I* w){
 I h=s.o10630(); I n=s.o10980(h);
 I k=w[0x11]>>8;
 if(k==6){ s.o10542(u,7); w[0x12]=0x3200; }
 else if(k==0x16){
  if(n>3){ s.o10542(u,0x28); w[0x12]=0x1100; return 1; }
  s.o10243(u,n+1); return 1;
 } else if(k==0x1e){ s.f6c0(); u->flags|=0x10000; return 1; }
 return 1;
}
}
