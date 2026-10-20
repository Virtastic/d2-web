// Implementation batch (alpha lane): D2Client+0x52860..0x53720. Decompile-derived; not yet vectored.
// Imported-DLL ordinals and not-yet-recovered D2Client helpers are the Svc virtuals (named by ordinal/RVA).
// Register arguments the decompile does not show are marked "REG?" and are NOT guessed.
#include <cstdint>
namespace d2client_batch_52860 {
struct Inner { uint32_t flags; int32_t f4,f8,fc,f10,f14,f18,f28,f2c,f30; };
struct Unit { int32_t type, id4, f8, id12, f10; uint32_t pad[5]; uint32_t seed[2]; uint32_t pad2[2]; int32_t path; // +0x2c (opaque handle here)
 Unit* next7c; };
struct Rec { uint32_t w[23]; }; // 0x5c-byte message record fed to FUN_6fb630f0
struct G {
 int32_t cc2e8{},a2134{},cc2ec{},d3478{},cc2e0{},cc2e4{},a212c{},a2130{},cc3b8{};
 int32_t c960c{},c9610{},c9618{},c961c{},c9624{},c9628{},c962c{},c9630{};
 uint8_t c9614{},c9615{},c9616{};
 Unit* cbbfc{};
};
struct Svc {
 virtual ~Svc()=default;
 virtual Unit* o10331(Unit*)=0; virtual Unit* o10754(Unit*)=0;   // REG?/stack arg of 10331 unrecovered in 52860/529F0/530D0/53320
 virtual int32_t pathX(int32_t)=0; virtual int32_t pathY(int32_t)=0; // Ordinal_10867 / 10750
 virtual Unit* f630f0(Rec&)=0;                                    // FUN_6fb630f0
 virtual void o10019(Unit*,int32_t)=0; virtual void o10637(Unit*,int32_t)=0;
 virtual Unit* spawn(int32_t ecx,Unit*,int32_t id,int32_t a,int32_t b,int32_t c,int32_t d)=0; // FUN_6fb5ff10
 virtual void f45570(int32_t,int32_t,int32_t,int32_t,int32_t,int32_t)=0; // FUN_6faf5570
 virtual int32_t o10826(Unit*)=0; virtual int32_t o10055()=0; virtual int32_t o10265(int32_t)=0;
 virtual void o10024(const char*,int32_t)=0; [[noreturn]] virtual void exit_m1()=0;
 virtual void fmt10003(int32_t n,uint16_t* out512)=0;             // Ordinal_10003 + Unicode::sprintf(0x100,..)
 virtual void o10184()=0; virtual void o10028()=0; virtual void o10150(int32_t,int32_t,int32_t)=0;
 virtual void f59a20(uint32_t*,uint8_t*,uint8_t*,uint8_t*)=0; virtual void o10083(int32_t)=0; virtual int32_t f7acc4()=0;
 virtual void o10561(Unit*,uint32_t out[4])=0; virtual int16_t o10482(int32_t,int32_t,int32_t,int32_t)=0;
 virtual void f515c0()=0; virtual void f77b00()=0; virtual Unit* o10717(int32_t)=0;
};
static bool direct(int t){ return t==2||(t>3&&t<6); } // types read straight off the path block
// 1.13c D2Client+0x52860  (ebx = unit)
void sub_52860(Svc& s,Unit* ebx){
 Unit* o=s.o10754(s.o10331(/*REG? arg*/ebx));
 Unit* u=o?o:ebx; // both arms issue the same 10867/10750 pair (results discarded in the decompile)
 if(!direct(u->type)&&u->path){ s.pathX(u->path); s.pathY(u->path); }
 Rec r{}; s.f630f0(r); // REG? inputs to f630f0 beyond the zeroed record
 if(ebx){ Rec q{}; q.w[0]=1; Unit* p=s.f630f0(q); if(p) s.o10019(p,0); }
}
// 1.13c D2Client+0x529F0  (ebx = unit)
void sub_529F0(Svc& s,Unit* ebx){
 if(!ebx||!(ebx->id4==0x21c||ebx->id4==0x21d||ebx->id4==0x21e)) return;
 Unit* o=s.o10754(s.o10331(/*REG? arg*/ebx)); if(!o) return;
 if(!direct(o->type)&&o->path){ s.pathX(o->path); s.pathY(o->path); }
 Rec r{}; Unit* p=s.f630f0(r); // REG? record contents set by caller regs
 if(p) s.o10637(p,o->id12);
}
// 1.13c D2Client+0x52BD0  (esi = unit; ecx = REG? passed through to spawn)
void sub_52BD0(Svc& s,int32_t ecx,Unit* esi){
 if(!esi) return;
 int32_t y=0,x=0;
 // types 2,4,5 read path+0x10 / +0xc directly; REG? raw path block address is esi->path
 if(direct(esi->type)) y=/*[path+0x10]*/0; else if(esi->path) y=s.pathY(esi->path);
 if(direct(esi->type)) x=/*[path+0xc]*/0; else if(esi->path) x=s.pathX(esi->path);
 Unit* n=s.spawn(ecx,esi,0x25a,x,y,0,1); if(n) s.o10637(n,0x19);
}
// 1.13c D2Client+0x52D90
void sub_52D90(Svc& s,G& g,const char* msg5a){
 int32_t t=s.o10055();
 if(g.cc2e8==0||t>=g.cc2e8) return;
 // list head DAT_6fbba808[a2134&0x7f], next at +0xe4 (idx 0x39); walk collapsed by the Svc seam below
 // ponytail: node walk (id==a2134, type!=1 => fatal, state!=0xc && t<cc2e8-0x46 => return) left to caller-supplied lookup
 (void)msg5a;
 uint16_t buf[512]{}; s.fmt10003(g.cc2e8-t,buf);
 s.o10184(); s.o10028(); s.o10150(0x177,9,1); s.o10184();
}
// 1.13c D2Client+0x52F10  (ecx = REG? passed through to f59a20)
void sub_52F10(Svc& s,G& g){
 uint32_t l4=0;
 if(g.cbbfc) s.o10331(g.cbbfc);
 s.f59a20(&l4,&g.c9614,&g.c9615,&g.c9616);
 if(g.c9628<g.c9618){ s.o10083(g.c962c); g.c9610=s.f7acc4(); g.c962c=(int32_t)(((int64_t)g.c9628<<7)/g.c9618); }
 else if(g.c9628<g.c9624-g.c961c) g.c9610=0;
 else{
  s.o10083(g.c962c); g.c9610=s.f7acc4();
  int32_t d=g.c961c-g.c9624; if(g.c961c==0) g.c961c=0x19;
  g.c962c=0x80-((d+g.c9628)*0x80)/g.c961c;
 }
 g.c9628++;
 Unit* u=g.cbbfc?s.o10331(g.cbbfc):nullptr; int32_t v=u?s.o10826(u):0;
 if(g.c9624<g.c9628||v!=g.c9630){ g.c960c=0; /* clears 11 dwords 0x6fbc960c..+0x2c; fields above overlap */ g.c9610=0; g.c9614=g.c9615=g.c9616=0; g.c9618=g.c961c=g.c9624=g.c9628=g.c962c=g.c9630=0; }
}
// 1.13c D2Client+0x53070
void sub_53070(Svc& s){ s.f45570(0x1253,0,0xfa,0,0,0); Rec r{}; s.f630f0(r); }
// 1.13c D2Client+0x530D0  (esi = unit)
void sub_530D0(Svc& s,Unit* esi){
 Unit* o=s.o10754(s.o10331(/*REG? arg*/esi));
 Unit* u=o?o:esi;
 if(o) s.f45570(0x124f,0,0x15e,0,0,0);
 if(!direct(u->type)&&u->path){ s.pathX(u->path); s.pathY(u->path); }
 Rec r{}; r.w[0]=0x420; /* REG? 0x182 stored first then overwritten */ Unit* p=s.f630f0(r);
 if(p&&o) s.o10637(p,o->id12);
}
// 1.13c D2Client+0x532C0  (eax = unit)
void sub_532C0(Svc& s,G& g,Unit* eax){
 if(!eax) return;
 int32_t k=s.o10826(eax);
 if(k<0x83||k>0x84) g.d3478=0;
 else if(g.d3478==0&&g.cc2ec&&g.cbbfc){ s.f515c0(); g.d3478=1; }
}
// 1.13c D2Client+0x53320  (eax = unit)
void sub_53320(Svc& s,G& g,Unit* eax,Inner* in /* *(*(eax+0x54)+0x30), null if absent */){
 Unit* o=s.o10754(s.o10331(/*REG? arg*/eax)); if(!o) return;
 s.f77b00();
 if(eax&&in){
  in->f4=0; in->f8=0; in->fc=0x96000;
  in->f10=0; in->f14=0; in->f18=(int32_t)0xfffff000;
  in->f28=0; in->f2c=0; in->f30=5;
  in->flags&=0xffffffdfu;
 }
 s.o10019(o,1);
 if(g.cbbfc) s.o10331(g.cbbfc);
 sub_532C0(s,g,eax); // REG? eax for 532C0 is the 10331 result (or unchanged eax when cbbfc==0)
}
// 1.13c D2Client+0x53550  (stack arg = unit)
void sub_53550(Svc& s,G& g,Unit* a){
 Unit* me=g.cbbfc; if(!me) return;
 Rec r{}; r.w[0]=0x2003; r.w[1]=(uint32_t)me->id4 /* REG? truncated handle */;
 r.w[18]=0; r.w[19]=1; /* local_30, local_2c */ r.w[16]=0x11f; /* local_4c */
 uint32_t box[4]{}; s.o10561(a,box); // box[0]=w, box[1]=x0?, box[2]=y0?, box[3]=h -- REG? order of stack vs out slots unrecovered
 int32_t ok=0;
 for(int i=0;i<=0x18&&ok<3;i++){
  auto rnd=[&](uint32_t n)->uint32_t{ if((int32_t)n<1) return 0;
   uint64_t x=(uint64_t)me->seed[0]*0x6ac690c5ull+me->seed[1]; me->seed[0]=(uint32_t)x; me->seed[1]=(uint32_t)(x>>32);
   return (n&(n-1))==0?((n-1)&(uint32_t)x):(uint32_t)((uint32_t)x%n); };
  r.w[3]=rnd(box[0])+box[1]; // local_5c[3]
  r.w[16]=box[2]+rnd(box[3]);
  if(s.o10482(0,r.w[3],r.w[16],5)==0) s.f630f0(r); else ok--; // REG? first arg uStack_4 unrecovered
  ok++;
 }
}
// 1.13c D2Client+0x536F0
void sub_536F0(Svc& s,G& g){
 for(Unit* u=s.o10717(g.cc3b8);u;u=u->next7c) if(s.o10826(u)==8) sub_53550(s,g,u);
}
// 1.13c D2Client+0x53720  (ecx = REG? passed to 52F10)
void sub_53720(Svc& s,G& g){
 if(g.c960c) sub_52F10(s,g);
 if(g.cc2e0==0&&g.a212c!=-1&&(++g.a212c,g.a212c>0x1d)){ g.cc2e0=1; sub_536F0(s,g); }
 if(g.cc2e4&&g.a2130!=-1&&(++g.a2130,g.a2130>0x1d)){ g.cc2e4=0; g.a2130=-1; }
}
}
