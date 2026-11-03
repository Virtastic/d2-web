// Implementation batch (alpha lane): D2Client+0x52860..0x53720. Decompile-derived; not yet vectored.
// Imported-DLL ordinals and not-yet-recovered D2Client helpers are the Svc virtuals (named by ordinal/RVA).
// Register arguments the decompile does not show are marked "REG?" and are NOT guessed.
#include <cstdint>
#include <cstddef>
namespace d2client_batch_52860 {
struct Inner { uint32_t flags; int32_t f4,f8,fc,f10,f14,f18,f28,f2c,f30; };
struct Unit { int32_t type, id4, f8, id12, f10; uint32_t pad[3]; uint32_t seed[2]; uint32_t pad2[1]; int32_t path; // +0x2c (opaque handle here)
 uint32_t pad3[19]; Unit* next7c; };
static_assert(offsetof(Unit,id12)==0xc&&offsetof(Unit,f10)==0x10&&offsetof(Unit,seed)==0x20&&offsetof(Unit,path)==0x2c&&offsetof(Unit,next7c)==0x7c||sizeof(void*)==8,"unit layout");
struct Rec { uint32_t w[23]; }; // 0x5c-byte message record fed to FUN_6fb630f0
struct G {
 int32_t cc2e8{},a2134{},cc2ec{},d3478{},cc2e0{},cc2e4{},a212c{},a2130{},cc3b8{};
 int32_t c960c{},c9610{},c9618{},c961c{},c9624{},c9628{},c962c{},c9630{};
 uint8_t c9614{},c9615{},c9616{},c9617{}; int32_t c9620{},c9634{};
 Unit* cbbfc{};
};
struct Svc {
 virtual ~Svc()=default;
 virtual Unit* o10331(Unit*)=0; virtual Unit* o10754(Unit*)=0;   // 1-arg 10331 (52F10, 53320 tail)
 virtual Unit* o10331x(Unit*,int32_t,int32_t)=0;               // 3-arg 10331(unit,k,id) as 52860/529F0/530D0/53320 push it (disassembly)
 virtual int32_t pathX(int32_t)=0; virtual int32_t pathY(int32_t)=0; // Ordinal_10867 / 10750
 virtual Unit* f630f0(Rec&)=0;                                    // FUN_6fb630f0
 virtual void o10019(Unit*,int32_t)=0; virtual void o10637(Unit*,int32_t)=0;
 virtual Unit* spawn(Unit* edx,int32_t id,int32_t x,int32_t y,int32_t c,int32_t d)=0; // FUN_6fb5ff10 (EDX = unit, stack id,x,y,0,1)
 virtual void f45570(int32_t,int32_t,int32_t,int32_t,int32_t,int32_t)=0; // FUN_6faf5570
 virtual int32_t o10826(Unit*)=0; virtual int32_t o10055()=0; virtual int32_t o10265(int32_t)=0;
 virtual void o10024(const char*,int32_t)=0; [[noreturn]] virtual void exit_m1()=0;
 virtual void fmt10003(int32_t n,uint16_t* out512)=0;             // Ordinal_10003 + Unicode::sprintf(0x100,..)
 virtual void o10184()=0; virtual void o10028()=0; virtual void o10150(int32_t,int32_t,int32_t)=0;
 virtual void f59a20(uint32_t eax,uint32_t*,uint8_t*,uint8_t*,uint8_t*)=0; 
 virtual int32_t scaled10083(int32_t scale,uint32_t l4)=0; // 10083(scale) * (double)l4, then _ftol (0x7acc4); the x87 product is not expressible as a call
 virtual void o10561(Unit*,uint32_t out[4])=0; virtual int16_t o10482(Unit*,int32_t,int32_t,int32_t)=0;
 virtual int32_t rdPath(int32_t path,int32_t off)=0; virtual Unit* listHead(int32_t idx)=0; virtual Unit* nextNode(Unit*)=0; // path+off dword; DAT_6fbba808[idx]; node[0x39]
 virtual void f515c0(int32_t,int32_t,int32_t)=0; virtual void f77b00(Unit* eax)=0; virtual Unit* o10717(int32_t)=0;
};
static bool direct(int t){ return t==2||(t>3&&t<6); } // types read straight off the path block
// unit x / y (Ordinal_10867 / Ordinal_10750 for the non-direct types, path+0xc / +0x10 for types 2,4,5)
static int32_t X(Svc& s,Unit* u){ return direct(u->type)?s.rdPath(u->path,0xc):(u->path?s.pathX(u->path):0); }
static int32_t Y(Svc& s,Unit* u){ return direct(u->type)?s.rdPath(u->path,0x10):(u->path?s.pathY(u->path):0); }
static uint32_t H(Unit* u){ return (uint32_t)(uintptr_t)u; } // 32-bit handle stored in a message word
// 1.13c D2Client+0x52860  (ebx = unit)
void sub_52860(Svc& s,Unit* ebx){
 Unit* e=s.o10754(s.o10331x(ebx,2,0x173));
 int32_t x,y;
 if(e){ x=X(s,e); y=Y(s,e); } else { x=X(s,ebx)-0xf; y=Y(s,ebx)-0xf; }
 Rec r{}; r.w[0]=0x420; r.w[1]=H(ebx); r.w[2]=H(ebx); r.w[4]=0x123; r.w[7]=(uint32_t)x; r.w[8]=(uint32_t)y; r.w[12]=1; s.f630f0(r);
 if(ebx){ Rec q{}; q.w[0]=1; q.w[1]=H(ebx); q.w[5]=(uint32_t)x; q.w[6]=(uint32_t)y; q.w[11]=0; q.w[12]=1; q.w[4]=0x14c;
  Unit* p=s.f630f0(q); if(p) s.o10019(p,0); }
}
// 1.13c D2Client+0x529F0  (ebx = unit)
void sub_529F0(Svc& s,Unit* ebx){
 if(!ebx) return;
 int32_t k; switch(ebx->id4){ case 0x21c:k=0x1db;break; case 0x21d:k=0x1dc;break; case 0x21e:k=0x1da;break; default:return; }
 Unit* o=s.o10754(s.o10331x(ebx,2,k)); if(!o) return;
 Rec r{}; r.w[0]=0x420; r.w[1]=H(ebx); r.w[2]=H(ebx); r.w[4]=0x21d; r.w[7]=(uint32_t)X(s,o); r.w[8]=(uint32_t)Y(s,o); r.w[12]=1;
 Unit* p=s.f630f0(r); if(p) s.o10637(p,o->id12);
}
// 1.13c D2Client+0x52BD0  (esi = unit; EDX = esi at the 5ff10 call)
void sub_52BD0(Svc& s,Unit* esi){
 if(!esi) return;
 int32_t y=Y(s,esi),x=X(s,esi);
 Unit* n=s.spawn(esi,0x25a,x,y,0,1); if(n) s.o10637(n,0x19);
}
// 1.13c D2Client+0x52D90 (disassembly-checked: list walk over DAT_6fbba808[a2134&0x7f] via node[0x39], same compare/return rules)
void sub_52D90(Svc& s,G& g,const char* msg5a){
 int32_t t=s.o10055();
 if(g.cc2e8==0||t>=g.cc2e8) return;
 for(Unit* n=s.listHead(g.a2134&0x7f);n;n=s.nextNode(n)){
  if(n->id12!=g.a2134) continue;
  if(n->type!=1){ s.o10024(msg5a,s.o10265(0x5a)); s.exit_m1(); }
  if(n->f10!=0xc&&t<g.cc2e8-0x46) return;
  break;
 }
 uint16_t buf[512]{}; s.fmt10003(g.cc2e8-t,buf);
 s.o10184(); s.o10028(); s.o10150(0x177,9,1); s.o10184();
}
// 1.13c D2Client+0x52F10
void sub_52F10(Svc& s,G& g){
 uint32_t l4=0;
 if(g.cbbfc) s.o10331(g.cbbfc);
 s.f59a20(0,&l4,&g.c9614,&g.c9615,&g.c9616);
 int32_t c=g.c9628;
 if(c<g.c9618){ g.c9610=s.scaled10083(g.c962c,l4); g.c962c=(int32_t)(((int64_t)(int32_t)((uint32_t)c<<7))/g.c9618); }
 else if(c<g.c9624-g.c961c) g.c9610=0;
 else{
  g.c9610=s.scaled10083(g.c962c,l4);
  int32_t d=g.c961c-g.c9624; if(g.c961c==0) g.c961c=0x19;
  g.c962c=0x80-(int32_t)((int64_t)(int32_t)((uint32_t)(d+c)<<7)/g.c961c);
 }
 g.c9628=c+1;
 Unit* u=g.cbbfc?s.o10331(g.cbbfc):nullptr; int32_t v=u?s.o10826(u):0;
 if(g.c9624<g.c9628||v!=g.c9630){ // rep stos of 11 dwords from 0x6fbc960c
  g.c960c=0; g.c9610=0; g.c9614=g.c9615=g.c9616=g.c9617=0; g.c9618=0; g.c961c=0; g.c9620=0; g.c9624=0; g.c9628=0; g.c962c=0; g.c9630=0; g.c9634=0; }
}
// 1.13c D2Client+0x53070
void sub_53070(Svc& s){ s.f45570(0x1253,0,0xfa,0,0,0); Rec r{}; s.f630f0(r); }
// 1.13c D2Client+0x530D0  (esi = unit)
void sub_530D0(Svc& s,Unit* esi){
 Unit* o=s.o10754(s.o10331x(esi,2,0x182));
 int32_t x,y;
 if(o){ s.f45570(0x124f,0,0x15e,0,0,0); x=X(s,o); y=Y(s,o); } else { x=X(s,esi)-0xf; y=Y(s,esi)-0xf; }
 Rec r{}; r.w[0]=0x420; r.w[4]=0x170; r.w[1]=H(esi); r.w[2]=H(esi); r.w[7]=(uint32_t)x; r.w[8]=(uint32_t)y; r.w[12]=1;
 Unit* p=s.f630f0(r); if(p&&o) s.o10637(p,o->id12);
}
// 1.13c D2Client+0x532C0  (eax = unit)
void sub_532C0(Svc& s,G& g,Unit* eax){
 if(!eax) return;
 int32_t k=s.o10826(eax);
 if(k<0x83||k>0x84) g.d3478=0;
 else if(g.d3478==0&&g.cc2ec&&g.cbbfc){ s.f515c0(0x27f,0,1); g.d3478=1; }
}
// 1.13c D2Client+0x53320  (eax = unit); the tail call reaches 532C0 with EAX = o10331(cbbfc) or 0 when cbbfc is null
void sub_53320(Svc& s,G& g,Unit* eax,Inner* in /* *(*(eax+0x54)+0x30), null if absent */){
 Unit* o=s.o10754(s.o10331x(eax,3,0x271)); if(!o) return;
 s.f77b00(eax);
 if(eax&&in){
  in->f4=0; in->f8=0; in->fc=0x96000;
  in->f10=0; in->f14=0; in->f18=(int32_t)0xfffff000;
  in->f28=0; in->f2c=0; in->f30=5;
  in->flags&=0xffffffdfu;
 }
 s.o10019(o,1);
 Unit* t=g.cbbfc?s.o10331(g.cbbfc):nullptr;
 sub_532C0(s,g,t);
}
// 1.13c D2Client+0x53550  (stack arg = unit; ret 4)
void sub_53550(Svc& s,G& g,Unit* a){
 Unit* me=g.cbbfc; if(!me) return;
 Rec r{}; r.w[0]=0x2003; r.w[1]=H(me); r.w[4]=0x11f; r.w[11]=0; r.w[12]=1;
 uint32_t box[4]{}; s.o10561(a,box); // box = {x0,y0,w,h}: x=x0+rnd(w), y=y0+rnd(h)
 auto rnd=[&](uint32_t n)->uint32_t{ if((int32_t)n<1) return 0;
  uint64_t v=(uint64_t)me->seed[0]*0x6ac690c5ull+me->seed[1]; me->seed[0]=(uint32_t)v; me->seed[1]=(uint32_t)(v>>32);
  return (n&(n-1))==0?((n-1)&(uint32_t)v):(uint32_t)((uint32_t)v%n); };
 int32_t ok=0;
 for(int32_t i=0;i<0x19;){
  uint32_t x=box[0]+rnd(box[2]);
  uint32_t y=box[1]+rnd(box[3]);
  r.w[5]=x; r.w[6]=y;
  if(s.o10482(a,(int32_t)x,(int32_t)y,5)==0) s.f630f0(r); else ok--;
  ok++; i++;
  if(ok>=3) break;
 }
}
// 1.13c D2Client+0x536F0
void sub_536F0(Svc& s,G& g){
 for(Unit* u=s.o10717(g.cc3b8);u;u=u->next7c) if(s.o10826(u)==8) sub_53550(s,g,u);
}
// 1.13c D2Client+0x53720
void sub_53720(Svc& s,G& g){
 if(g.c960c) sub_52F10(s,g);
 if(g.cc2e0==0&&g.a212c!=-1&&(++g.a212c,g.a212c>0x1d)){ g.cc2e0=1; sub_536F0(s,g); }
 if(g.cc2e4&&g.a2130!=-1&&(++g.a2130,g.a2130>0x1d)){ g.cc2e4=0; g.a2130=-1; }
}
}
