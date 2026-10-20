// Implementation batch (RE-DONE lane). Derived from cached 1.13c decompile; ordinals and
// globals are injected so the logic is native-testable. Register args noted per function.
#include <cstdint>
#include <cstring>
namespace d2client_b801f0 {
using U=uint32_t; using I=int32_t; using P=void*;
static U rd(P p,U n){U v;std::memcpy(&v,static_cast<uint8_t*>(p)+n,4);return v;}
static void wr(P p,U n,U v){std::memcpy(static_cast<uint8_t*>(p)+n,&v,4);}
struct G { // D2Client globals by RVA-less VA (6fb.. image)
 U g6fbcbf6c=0,g6fbc393c=0,g6fbc38c0=0,g6fb91380=0; // 801F0
 P h3904[4]{}; P h3890[4]{};                         // 80220 handle arrays
 U g6fbcbf80=0,g6fbcbf84=0,g6fbcbf88=0; P g6fbcbbfc=nullptr; // 812F0
 U g6fbcbf78=0; P g6fbcbf68=nullptr;                 // 81470
};
struct Ops {
 void (*destroy)(P);            // Ordinal_10065
 void (*fatal)(U);              // Ordinal_10265+10024+_exit
 void (*err7a)(I,I);            // Ordinal_10043
 void (*f80ca0)();              // 0x80CA0
 void (*o10063)();              // Ordinal_10063
 void (*f814f0)();              // 0x814F0
};
// 1.13c D2Client+0x801F0
void sub_801F0(G& g,U p){ if(!g.g6fbcbf6c){g.g6fbcbf6c=1;g.g6fbc393c=1;g.g6fbc38c0=0;g.g6fb91380=p;} }
// 1.13c D2Client+0x80220
void sub_80220(G& g,Ops& o){
 for(P& h:g.h3904) if(h){o.destroy(h);o.err7a(0x7a,0);h=nullptr;}
 for(P& h:g.h3890) if(h){o.destroy(h);o.err7a(0x7a,0);h=nullptr;}
}
// 1.13c D2Client+0x812F0 (EAX = in)
void sub_812F0(G& g,Ops& o,U in){
 g.g6fbcbf80=in;
 if(in&&!g.g6fbcbf84){ g.g6fbcbf88=2; if(!g.g6fbcbbfc){o.fatal(0x137);return;} o.f80ca0(); }
}
// 1.13c D2Client+0x81470 (ESI = esi)
void sub_81470(G& g,Ops& o,I param1,U esi){
 if(g.g6fbcbf6c&&esi) g.g6fbcbf6c=0;
 if(g.g6fbcbf80&&!esi) g.g6fbcbf80=0;
 if(esi!=g.g6fbcbf78||param1){ o.o10063(); if(rd(g.g6fbcbf68,0x114)){o.fatal(0x15b);return;} }
 g.g6fbcbf78=esi;
}
// 1.13c D2Client+0x81720 (ESI = esi)
void sub_81720(Ops& o,U esi){ o.o10063(); o.o10063(); if(esi) o.f814f0(); }

struct Ops2 {
 void (*fatal)(U); P (*o10871)(); void (*o10945)(); void (*o11111)(); void (*o11108)(P); void (*o10819)();
 P (*o11139)(); void (*o10608)(); I (*o11041)();
 U (*o10973)(); P (*o10828)(); U (*o10304)(P); U (*o10601)(P,const char*,I); P (*o10507)();
 P (*o10244)(); P (*o11144)(P); P (*o11139b)(U,I); I (*o10744)(P,P);
 I (*o10195)(); I (*o10494)(P,I);
 P (*o11013)(I,I,I,U,U); void (*o10785)(P,I); void (*o10911)(P,const void*);
 void (*o11090)();
};
// 1.13c D2Client+0x81AA0 (ESI = esi)
void sub_81AA0(Ops2& o,U esi){
 if(!esi){o.fatal(0xcc2);return;}
 P r=o.o10871(); if(r){o.o10945();o.o11111();o.o11108(r);o.o10819();}
}
// 1.13c D2Client+0x81B40 (EBX = ebx)
bool sub_81B40(Ops2& o,U ebx){
 if(!ebx) return false;
 if(o.o11139()) o.o10608();
 return o.o11041()!=0;
}
// 1.13c D2Client+0x81B90 (ESI = out, 7 words)
void sub_81B90(Ops2& o,P out){
 wr(out,0,o.o10973()); wr(out,4,o.o10973()); wr(out,8,o.o10973());
 P a=o.o10828();
 if(!a){wr(out,24,0xffffffffu);wr(out,16,0);}
 else{wr(out,24,o.o10304(a));wr(out,16,o.o10601(a,"..\\Source\\D2Client\\UNIT\\Item.cpp",0x155));}
 P b=o.o10507();
 if(b){wr(out,20,o.o10304(b));wr(out,12,o.o10601(b,"..\\Source\\D2Client\\UNIT\\Item.cpp",0x161));}
 else{wr(out,20,0xffffffffu);wr(out,12,0);}
}
// 1.13c D2Client+0x81C30 (EAX = a, EBX = ebx)
P sub_81C30(Ops2& o,P a,P ebx){
 P r=o.o10244();
 if(r&&o.o11144(nullptr)){
  U u=rd(a,0x60);
  P v=o.o11139b(u,4);
  if(v==ebx) v=o.o11139b(u,5);
  if(v){ P w=o.o11144(nullptr); return o.o10744(v,w)?r:nullptr; }
  return nullptr;
 }
 return r;
}
// 1.13c D2Client+0x81CB0
U sub_81CB0(Ops2& o,P g6fbcbbfc){
 if(o.o10195()) return 1;
 return (g6fbcbbfc&&o.o10494(g6fbcbbfc,0x36))?1:0;
}
// 1.13c D2Client+0x81CE0 (EAX = in)
void sub_81CE0(Ops2& o,P in,const void* lab81a90){
 P r=o.o10871();
 if(!r){
  U a=0xffffffffu,b=6;
  if(in){a=rd(in,12);b=rd(in,0);}
  P h=o.o11013(0,2,0xc,b,a); o.o10785(h,0x36); o.o10911(h,lab81a90);
 } else o.o10785(r,0x36);
}
// 1.13c D2Client+0x81D30 (EAX = mode, ESI = unit)
void sub_81D30(Ops2& o,I mode,P unit,I p2){
 if(mode==2){ o.o11090();
  if(unit){ U f=rd(unit,0xc4); wr(unit,0xc4,p2?(f|2):(f&~2u)); } }
}
}
