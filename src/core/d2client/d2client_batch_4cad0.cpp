// Implementation batch (alpha lane): D2Client+0x4CAD0..0x511F0. Decompile-derived; not yet vectored.
// Imported-DLL ordinals and not-yet-recovered D2Client helpers are the Svc virtuals (named by ordinal/RVA).
// Retail 32-bit pointers never appear; globals are fields of G.
#include <cstdint>
namespace d2client_batch_4cad0 {
struct Inner { uint32_t flags; int32_t f10,f14,f18,f28,f2c,f30; void* f3c; };
struct Node { Inner* p30; };
struct Unit { int32_t type; void* path; Node* node; uint32_t flags31; uint32_t flagsC8; uint32_t seed[2]; };
struct Frame { uint32_t w[18]; }; // 0x48-byte record: w[0]=active, w[13]=colour global, w[16]=0
struct G {
 int32_t cc414{},d2bc48{},d2bc4c{},cb828{},cb824{},cc300{},cc304{},cc308{},cc30c{},c9960{},cc418{},c995c{};
 int32_t aada4{},aad90{},aadb0{},aadd8{},aad84{},aad88{},aadac{},aad98{},aad9c{},d5d0{};
 uint8_t c9663{},c9721{}; uint8_t aad80[64]{}; int32_t ptr538Mode[64]{}; uint32_t table601c[5]{};
 struct Q { Q* next; int32_t id; int32_t v; }* queue{}; // list at DAT_6fbcc4d4: +8 id, +0x2c value, +0x30 next
 Unit* cbbfc{}; Unit* c9650{};
};
struct Svc {
 virtual ~Svc()=default;
 virtual int32_t gfxMode()=0;                         // Ordinal_10031
 virtual int32_t pathY(Unit*)=0; virtual int32_t pathX(Unit*)=0; // Ordinal_10750 / 10867
 virtual void* o10331(Unit*,int32_t,int32_t,int32_t)=0;
 virtual void o10870(void*)=0; virtual void o10468(void*)=0;
 virtual void o10671(Unit*,int32_t)=0; virtual void o11164(void*,int32_t)=0;
 virtual void f77b00()=0;
 virtual Unit* spawn(Unit*,int32_t id,int32_t a,int32_t b2,int32_t c,int32_t b)=0; // FUN_6fb5ff10
 virtual void o10210(Unit*)=0;
 [[noreturn]] virtual void fatal(int32_t)=0;
 virtual void o10520(void*,int32_t)=0; virtual void o10706(void*,int32_t)=0; virtual void o11129(Unit*,Unit*)=0;
 virtual void f55fb0()=0; virtual int32_t f1660(Unit*)=0; virtual int32_t f1630(Unit*)=0;
 virtual int32_t f4fe70()=0; virtual int32_t f4fec0(int32_t)=0; virtual int32_t f4ff10()=0;
 virtual void o10003(int32_t,int32_t,int32_t)=0; virtual void o10085()=0; virtual void o10028()=0;
 virtual void o10150()=0; virtual void o10041(const Frame&,int32_t x,int32_t y,int32_t,int32_t,int32_t)=0;
 virtual void* o10021(const Frame&)=0; virtual void o10060(void*)=0;
 virtual void* f46150()=0; virtual void f38a70()=0; virtual void f16360()=0; virtual void o511(void*)=0;
 virtual int32_t o10587(Unit*,int32_t,int32_t)=0; virtual void o10227()=0; virtual void f72790(int32_t,int32_t,int32_t)=0;
 virtual void f7b20()=0; virtual int32_t f55440()=0;
 virtual void fecc60(int32_t)=0; virtual void feccc0(int32_t)=0; virtual int32_t o10048()=0; virtual int32_t o10013()=0;
};
static Inner* inner(Unit* u){ return u->node? u->node->p30:nullptr; }
// Path coordinate selector shared by 4CAD0/4CDD0: types 2,4,5 read the path struct directly, others via ordinal.
struct PathRaw { int32_t c, y10; };
static int32_t cy(Svc& s,Unit* u,const PathRaw* raw){ int t=u->type; if(t==2||(t>3&&t<6)) return raw->y10; return u->path? s.pathY(u):0; }
static int32_t cx(Svc& s,Unit* u,const PathRaw* raw){ int t=u->type; if(t==2||(t>3&&t<6)) return raw->c;   return u->path? s.pathX(u):0; }
// 1.13c D2Client+0x4CAD0  (eax = event)
void sub_4CAD0(Svc& s,Unit* u,int32_t ev,const PathRaw* raw){
 if(ev==0xc){
  u->flags31&=0xfffffff1u;
  s.o10870(s.o10331(u,cx(s,u,raw),cy(s,u,raw),0x1000));
  s.o10870(s.o10331(u,cx(s,u,raw),cy(s,u,raw),0x100));
  s.o10671(u,5); s.o11164(u->path,0);
  Inner* i=inner(u);
  if(!i||!i->f3c){
   s.f77b00();
   if((i=inner(u))) i->flags=(i->flags&0xfffffffeu)|8;
   if((i=inner(u))) i->flags|=0x20;
   if((i=inner(u))){ i->f10=0; i->f14=0; i->f18=0x1000; }
   if((i=inner(u))){ i->f30=0xa0; i->f28=0; i->f2c=0; }
  }
 } else if(ev==0xe||ev==10){
  u->flags31|=0xe;
  s.o10468(s.o10331(u,cx(s,u,raw),cy(s,u,raw),0x1000));
  s.o10468(s.o10331(u,cx(s,u,raw),cy(s,u,raw),0x100));
  s.o10671(u,1); s.o11164(u->path,0x3c01);
  Inner* i=inner(u);
  if(u->node&&i&&i->f3c){
   i->flags=(i->flags&0xfffffffeu)|8;
   if((i=inner(u))) i->flags&=0xffffffdfu;
   if((i=inner(u))){ i->f10=0; i->f14=0; i->f18=(int32_t)0xffffc000; }
   if((i=inner(u))){ i->f30=0; i->f28=0; i->f2c=0; }
  }
 }
}
// 1.13c D2Client+0x4CDD0  (esi = unit)
void sub_4CDD0(Svc& s,G& g,Unit* u,const PathRaw* raw){
 uint64_t r=(uint64_t)u->seed[0]*0x6ac690c5ull+u->seed[1]; u->seed[0]=(uint32_t)r; u->seed[1]=(uint32_t)(r>>32);
 uint32_t k=(uint32_t)r%5; // 4CDD0 never sees k>4; retail's fatal(0x174) branch is dead
 Unit* n=s.spawn(u,(int32_t)g.table601c[k],cx(s,u,raw),cy(s,u,raw),0,1);
 if(n){ s.o10210(n); s.o10210(n); s.f77b00();
  if(n->node&&n->node->p30) n->node->p30->flags|=0x10;
  n->flagsC8|=0x40000; }
}
// 1.13c D2Client+0x4CEC0  (eax = msg)
void sub_4CEC0(Svc& s,G& g,Unit* u,int32_t msg,const PathRaw* raw){
 switch(msg){
 case 0x158:{ Unit* o=g.cbbfc; uint64_t r=(uint64_t)o->seed[0]*0x6ac690c5ull+o->seed[1]; o->seed[0]=(uint32_t)r; o->seed[1]=(uint32_t)(r>>32);
  s.o10520(u->path,1); s.o10706(u->path,(int32_t)((uint32_t)r&0x3f)); s.o10520(u->path,0); return; }
 case 0x96: g.c9650=u; return;
 case 0x9d: case 0x9e: case 0x13f: s.o10671(u,5); s.o11164(u->path,0); return;
 case 0x14b: if(g.cbbfc){ s.o11129(u,g.cbbfc); s.f55fb0(); } return;
 case 0x19d:{ int32_t y=s.f1660(u),x=s.f1630(u); Unit* n=s.spawn(u,0x188,x,y,0,1); if(n) s.o10210(n); return; }
 case 0x1f1: case 0x1f2: case 499: case 500: sub_4CDD0(s,g,u,raw); return;
 case 0x23a:{ int32_t y=s.f1660(u),x=s.f1630(u); s.spawn(u,0x265,x,y,0,1); return; }
 }
}
// 1.13c D2Client+0x4FF60  (ecx = param_1 = mouse x, eax = gfx mode, esi = mouse y)
int32_t sub_4FF60(G& g,int32_t mx,int32_t mode,int32_t my){
 if(mode==2){ if(mx<=g.d2bc48/2+0x28||mx>=g.d2bc48/2+0x4a) return 0; }
 else if(mx<0x29||mx>0x49) return 0;
 return (g.d2bc4c-0x8b<my&&my<g.d2bc4c-0x66)?1:0;
}
static void blank(Frame& f,G& g){ for(auto& w:f.w)w=0; f.w[13]=(uint32_t)g.cc300; }
// 1.13c D2Client+0x50020
void sub_50020(Svc& s,G& g){
 if(s.gfxMode()!=2) return;
 Frame f; blank(f,g); f.w[0]=2;
 if(s.f4fe70()&&!g.aada4){ s.o10003(g.d2bc4c-0x32,0,1); s.o10085(); }
 s.o10041(f,g.d2bc48/2+0xa3,g.d2bc4c-8,-1,5,0);
}
// 1.13c D2Client+0x50100
void sub_50100(Svc& s,G& g){
 if(s.gfxMode()!=2) return;
 Frame f; blank(f,g); f.w[0]=2;
 if(s.f4ff10()&&!g.aada4){ s.o10003(g.d2bc4c-0x32,0,1); s.o10085(); }
 s.o10041(f,g.d2bc48/2-0xc2,g.d2bc4c-8,-1,5,0);
}
// 1.13c D2Client+0x501F0.  Ordinal_10041 takes the Frame by value (retail pushes the stack record just built).
void sub_501F0(Svc& s,G& g){
 int32_t mode=g.cc414;
 if(s.gfxMode()!=2&&(mode==3||g.aad90||g.aadb0||(g.aadd8&&g.aad84))){ g.cc30c=0; return; }
 int32_t x=g.d2bc48; if(mode==1) x=g.d2bc48-g.d2bc48/2;
 Frame a; for(auto& w:a.w)w=0; a.w[13]=(uint32_t)g.cc304;
 s.o10060(s.o10021(a)); s.o10003(0,0,0); s.o10028();
 if(s.gfxMode()!=2){ s.o10003(g.d2bc4c-0x8e,0,0); s.o10150(); s.o10041(a,x-0x49,g.d2bc4c-0x69,-1,5,0); }
 Frame b; blank(b,g); int32_t on;
 if(s.gfxMode()==2) on=g.cc30c?s.f4fe70():0; else on=g.cc30c?s.f4fec0(g.cb828):0;
 b.w[0]=on?1:0;
 s.o10060(s.o10021(b)); s.o10003(0,0,0); s.o10028();
 if(s.gfxMode()==2){
  if(s.f4fe70()&&!g.aada4){ s.o10003(g.d2bc4c-0x32,0,1); s.o10085(); }
  s.o10041(b,g.d2bc48/2+0xa3,g.d2bc4c-8,-1,5,0); return; }
 s.o10041(b,x-0x46,g.d2bc4c-0x6d,-1,5,0);
}
// 1.13c D2Client+0x50430
void sub_50430(Svc& s,G& g){
 int32_t mode=g.cc414;
 if(s.gfxMode()!=2&&(mode==3||g.aad88||(g.aadd8&&(g.aad84||g.aad90))||g.aadb0)){ g.cc308=0; return; }
 int32_t x=(mode==2)?g.d2bc48/2+0x28:0x28;
 Frame a; for(auto& w:a.w)w=0; a.w[13]=(uint32_t)g.cc304;
 s.o10060(s.o10021(a)); s.o10003(0,0,0); s.o10028();
 if(s.gfxMode()!=2){ s.o10003(g.d2bc4c-0x8e,0,0); s.o10150(); s.o10041(a,x,g.d2bc4c-0x69,-1,5,0); }
 Frame b; blank(b,g); int32_t on;
 if(s.gfxMode()==2) on=g.cc308?s.f4ff10():0; else on=g.cc308?sub_4FF60(g,g.cb828,s.gfxMode(),g.cb824 /*esi = mouse Y, DAT_6fbcb824*/):0;
 b.w[0]=on?1:0;
 s.o10060(s.o10021(b)); s.o10003(0,0,0); s.o10028();
 if(s.gfxMode()!=2){ s.o10041(b,x+3,g.d2bc4c-0x6d,-1,5,0); return; }
 if(s.f4ff10()&&!g.aada4){ s.o10003(g.d2bc4c-0x32,0,1); s.o10085(); }
 s.o10041(b,g.d2bc48/2-0xc2,g.d2bc4c-8,-1,5,0);
}
struct Msg { uint16_t x,y; int32_t f18,f1c; };
// 1.13c D2Client+0x50750
void sub_50750(Svc& s,G& g,Msg* m){
 int32_t mode=s.gfxMode(), saved=g.cc414;
 if(mode==2&&g.aada4) return;
 int32_t hit=(s.gfxMode()==2)?s.f4ff10():sub_4FF60(g,m->x,s.gfxMode(),m->y);
 if(!hit){ m->f18=0; return; }
 g.cc308=1; s.f38a70(); s.f16360();
 if(saved!=2||!s.f46150()||g.c9663!=1){ m->f18=1; m->f1c=0; s.o511(m); }
}
// 1.13c D2Client+0x50E70
void sub_50E70(Svc& s,G& g){
 Unit* o=g.cbbfc;
 if(g.aadac) return;
 if(!g.aad98){
  int32_t r=s.o10587(o,4,0), snap=g.aad98;
  if(r&&!g.aad88){
   s.o10227();
   if(g.d5d0!=0x26) s.fatal(0x139b);
   bool bail=false;
   for(int32_t i=0;i<g.d5d0&&!bail;++i) if(g.aad80[i]==1) switch(g.ptr538Mode[i]){
    case 0: break; case 1: s.f72790(i,1,0); break;
    case 4: if(g.c9721) s.f7b20(); break;
    default: bail=true; }
   if(!bail){
    if(!g.cbbfc||(*(int32_t*)g.cbbfc==0&&!s.f55440())) g.aad98=1;
    if(snap==0){ if(g.aad98) s.feccc0(s.o10048()); } else if(!g.aad98) s.fecc60(s.o10048());
   }
  }
 } else {
  int32_t r=s.o10587(o,4,0), snap=g.aad98;
  if(!r){ s.o10227(); if(g.d5d0!=0x26) s.fatal(0x139b); g.aad98=0; if(snap) s.fecc60(s.o10048()); }
 }
 if(!g.aad9c){ if(s.o10587(o,5,0)>0&&!g.aad90) s.f72790(7,0,0); }
 else if(s.o10587(o,5,0)<1) s.f72790(7,1,0);
}
// 1.13c D2Client+0x510F0  (param_1 = &x, ebx = &y)
void sub_510F0(Svc& s,G& g,int32_t* x,int32_t* y){
 int32_t w=g.d2bc48;
 if(s.o10013()){
  *y-=*y/0x28;
  *x=((0x28-((*y+((*y>>31)&3))>>2))*(*x-w/2))/(w/2)+*x;
 }
 *x+=g.c9960-g.cc418; *y+=g.c995c-8;
}
// 1.13c D2Client+0x511F0  (this nonnull, eax = key struct with id at +0xc)
void sub_511F0(G& g,void* self,int32_t keyId,int32_t value){
 if(!self||!keyId) return;
 for(G::Q* q=g.queue;q;q=q->next) if(q->id==keyId){ q->v=value; return; }
 // retail falls through to a store at absolute address 8 (null-page write); not reproduced.
}
}
