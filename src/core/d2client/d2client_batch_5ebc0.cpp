// Implementation batch (alpha lane): D2Client+0x5EBC0..0x5F6B0. Decompile-derived; not yet vectored.
// Imported-DLL ordinals / unrecovered helpers are the Svc virtuals. "REG?" = register argument the decompile
// shows only as in_EAX/unaff_*: a GUESS, so rows using it stay NAMED.
#include <cstdint>
#include <cstddef>
namespace d2client_batch_5ebc0 {
struct G {
 uint8_t* cc24c{}; int32_t cc250{};          // sound-record array (0x8e bytes each) and count
 int32_t cc268{}, cc26c{}, cc270{}, cc274{}, cc278{}, cc27c{};
 uint8_t pal[11]{};                          // DAT_6fbcc210..21a
 int32_t cc1b0{};                            // 1 => shifted draw
 uint16_t* cc1b4{};                          // 3x u16 output cursor
 void* cc1b8{}; int32_t cc1bc{};             // 0x2800-byte block list, running entry count
 int32_t* cc1c4{}; void* cc1c0{};            // header, node chain head
 int32_t cc1c8[4]{};                         // 4 handles
 int32_t cc3bc{};                            // *DAT_6fbcc3bc value
 uint8_t c9860{};
};
struct Svc {
 virtual ~Svc()=default;
 virtual void f5da80()=0; virtual void f5db30(int32_t idx,char* out80)=0; virtual void f5dae0()=0; virtual void f5cd10()=0;
 virtual int32_t f5ea90(int32_t a,int32_t b,int32_t c,int32_t d)=0; virtual int32_t fabd810(int32_t h,int32_t* out)=0;
 virtual int32_t o10102()=0; virtual int32_t o10105()=0; virtual void o10103()=0; virtual int32_t o10016()=0;
 virtual int32_t o10091()=0; virtual int32_t o10095()=0; virtual int32_t o10092()=0;
 virtual void fatal302()=0;                    // Ordinal_10265(0x302) + Ordinal_10024 + _exit(-1)
 virtual int32_t o10190(int r,int g,int b)=0;  // color index
 virtual void o10065(int32_t h)=0; virtual void free(int32_t tag,void* p)=0;
 virtual void o10010(int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t col,int32_t a)=0;
 virtual void* alloc(int32_t tag,int32_t n)=0; // Ordinal_10042
 virtual void o10112()=0; virtual void o10111(int32_t n,void* buf,int32_t z)=0;
};
static uint8_t* rec(G& g,int32_t i){ return (g.cc24c&&i>=0&&i<g.cc250)?g.cc24c+i*0x8e:nullptr; }
template<class T> static T& at(uint8_t* p,int off){ return *reinterpret_cast<T*>(p+off); } // unaligned in retail too

// 1.13c D2Client+0x5EBC0  (ecx = record index; eax = REG? "load" flag; stack = param_1)
void sub_5EBC0(Svc& s,G& g,int32_t idx,int32_t eax,int32_t p1){
 int32_t saved=g.cc268; uint8_t* r=rec(g,idx); if(!r) r=nullptr; // retail derefs null+off; caller guarantees non-null
 if(at<uint8_t>(r,0x81)!=0) return;
 int32_t st=at<int32_t>(r,0x86); if(st==2) return;
 if(st==1){ if(eax==0) return; s.f5da80(); return; }
 if(st!=0) return;
 char name[80]; s.f5db30(idx,name);
 if(s.o10102()==0){ at<uint8_t>(r,0x81)=1; return; }
 int32_t sz=s.o10105(); s.o10103();
 if((uint32_t)g.cc278<(uint32_t)(g.cc27c+sz) && s.f5ea90(p1,(g.cc27c-g.cc278)+sz,eax,p1)==0){ g.cc270=saved; return; }
 if(eax!=0){
  int32_t h=s.fabd810(s.o10016(),&at<int32_t>(r,0x70)); at<int32_t>(r,0x6c)=h;
  if(h&&at<int32_t>(r,0x70)){ g.cc27c+=at<int32_t>(r,0x70); s.f5dae0(); at<int32_t>(r,0x86)=2; s.f5cd10(); return; }
  s.fatal302(); return;
 }
 int32_t o=s.o10091(); at<int32_t>(r,0x82)=o;
 if(o){ int32_t n=s.o10095(); g.cc27c+=n; at<int32_t>(r,0x70)=n; s.f5dae0(); g.cc274++; at<int32_t>(r,0x86)=1; }
}
// 1.13c D2Client+0x5ED90
void sub_5ED90(Svc& s,G& g,int32_t now){
 if(!g.cc24c||!(now==0||(uint32_t)(now-g.cc26c)>0x18)) return;
 int32_t i=1;
 for(int32_t off=0x8e; off<0x65ae7; off+=0x8e,i++){
  uint8_t* r=g.cc24c+off; bool again=false;
  if(at<int32_t>(r,0x86)==0&&(at<int32_t>(r,0x8a)!=0||at<uint8_t>(r,0x4c)!=0)){
   if(now==0||(g.cc274<0xf&&(uint32_t)(now-g.cc270)>0xf9)){ sub_5EBC0(s,g,i,1,0 /*REG?*/); }
   again=true;
  } else again=true;
  if(again&&at<int32_t>(r,0x86)==1&&s.o10092()!=0) s.f5da80();
 }
 g.cc26c=now;
}
// 1.13c D2Client+0x5F090
void sub_5F090(Svc& s,G& g){
 static const uint8_t c[11][3]={{0,0,0xff},{0xff,0,0},{0xff,0,0xff},{0,0xff,0},{0x44,0x70,0x74},{0x48,0xa0,0x34},
  {0xf4,0xf4,0xf4},{0x80,0x80,0x80},{0xf4,0xf4,0},{0,0,0xff},{0,0xff,0}};
 for(int i=0;i<11;i++) g.pal[i]=(uint8_t)s.o10190(c[i][0],c[i][1],c[i][2]);
}
// 1.13c D2Client+0x5F180
void sub_5F180(Svc& s,G& g){
 for(int i=0;i<4;i++){ int32_t h=g.cc1c8[i]; if(h){ s.o10065(h); s.free(0x7a,nullptr); } g.cc1c8[i]=0; }
}
// 1.13c D2Client+0x5F1C0  (ecx = REG? x origin, eax = REG? y origin, stack = colour). 12 line segments from table 6fb82de8.
void sub_5F1C0(Svc& s,G& g,int32_t x,int32_t y,int32_t col,const int32_t tab[24]){
 if(g.cc1b0==1){ x-=1; y+=5; }
 for(int i=0;i<12;i++) s.o10010(x+tab[i*4+0]*2,y+tab[i*4+1]*2,x+tab[i*4+2]*2,y+tab[i*4+3]*2,col,0xff);
}
// 1.13c D2Client+0x5F240  (pushes a zeroed 0x1c-byte node on the chain at cc1c0; link at word 6)
struct Node { void* w[7]; };
void sub_5F240(Svc& s,G& g){
 Node* n=(Node*)s.alloc(0x5ed,0); for(auto& p:n->w) p=nullptr; n->w[6]=g.cc1c0; g.cc1c0=n;
}
// 1.13c D2Client+0x5F290  (edi = REG? destination array; param_1 selects which clear)
void sub_5F290(Svc& s,G& g,int32_t p1,int32_t* edi){
 uint32_t z[3]={0,0,0};
 if(p1!=0){ s.o10112(); s.o10111(4,&z[0],0); return; }
 edi[*g.cc1c4]=0; s.o10112(); s.o10111(400,&z[0],0);
}
// 1.13c D2Client+0x5F300  (frees the 0x2800-byte block chain; next ptr at +0x2800)
void sub_5F300(Svc& s,G& g){
 if(!g.cc1c4) return;
 for(uint8_t* b=(uint8_t*)g.cc1b8; b; ){ uint8_t* n=*(uint8_t**)(b+0x2800); s.free(0x51e,nullptr); b=n; }
 g.cc1b8=nullptr; g.cc1bc=0; g.cc1c4[2]=g.cc1c4[3]=g.cc1c4[4]=g.cc1c4[5]=0;
}
// 1.13c D2Client+0x5F360  (ecx = tree node: [0]=leaf flag,[1],[2] payload,[3]=child,[4]=sibling)
struct TNode { int32_t kind; int32_t a; int32_t b; TNode* child; TNode* sib; };
void sub_5F360(G& g,TNode* n){
 for(;n;n=n->sib){ sub_5F360(g,n->child);
  if(n->kind==0){ g.cc1b4[0]=(uint16_t)n->a; g.cc1b4[1]=(uint16_t)((uint32_t)n->a>>16); g.cc1b4[2]=(uint16_t)n->b; g.cc1b4+=3; } } // [1]=word at +6
}
// 1.13c D2Client+0x5F3B0  (ecx = node, edx = byte counter)
void sub_5F3B0(TNode* n,int32_t* cnt){
 for(;n;n=n->sib){ sub_5F3B0(n->child,cnt); if(n->kind==0) *cnt+=6; }
}
// 1.13c D2Client+0x5F3E0  map-file open: ordinals unattributed, flow only (REG?/stack args of 10107/10110/578/501 are guesses)
struct MapSvc {
 virtual ~MapSvc()=default;
 virtual void o10115()=0; virtual void o578(char*)=0; virtual void o10114()=0;
 virtual int32_t o10107(int32_t a,int32_t b,int32_t c,int32_t d,int32_t e)=0; virtual void o501()=0;
 virtual void o10110(int32_t n,int32_t* out3)=0; virtual void o10108()=0; virtual void o10109()=0;
 virtual void o10112(int32_t,int32_t)=0; virtual void o10111(int32_t,int32_t*,int32_t)=0;
 virtual void fmtMap(char* out)=0; virtual void fmtMa(char* out,int32_t n)=0; // "%s%s.map" / "%s%s.ma%d"
};
int32_t sub_5F3E0(MapSvc& s,G& g){
 char b[264]{},c[264]{},ma[24]{}; int32_t slot[4]{}; int32_t got0=0,got1=0; int32_t idx=0;
 s.o10115();
 bool direct=true;
 if(g.c9860!=0){ s.o578(c); s.o10114(); s.o578(b); if(s.o10107(0,0,0,0,0)!=-1){ direct=false; s.o501(); } }
 if(direct){ s.fmtMap(b); if(s.o10107(0,4,0x80,0,0)==-1) return -1; }
 s.o10110(0x18,slot); // 0x18-byte header read, 0xc-byte guard (got0/got1 REG? results)
 if(got0==0x18&&got1==0xc){
  int i=0; for(;i<4;i++) if(slot[i]==g.cc3bc) break;
  if(i==4){ slot[idx]=g.cc3bc; s.fmtMa(ma,idx); s.o10108(); s.o10112(0,0); s.o10111(0x18,slot,0); }
  else idx=i;
 } else { for(int i=0;i<4;i++){ s.fmtMa(ma,i); s.o10108(); } idx=0; s.o10112(0,0); s.o10111(0x18,slot,0); }
 s.o10109(); s.fmtMa(ma,idx);
 return s.o10107(0,0,4,0x80,0);
}
// 1.13c D2Client+0x5F6B0  (appends 20-byte entry slot; blocks of 0x2800 bytes = 512 entries, next ptr word 0xa00)
void sub_5F6B0(Svc& s,G& g){
 int32_t n=g.cc1bc; uint32_t slot=(uint32_t)n&0x1ff; int32_t blk=n>>9; g.cc1bc=n+1;
 uint32_t* prev=nullptr; uint32_t* b=(uint32_t*)g.cc1b8; int32_t k=0;
 while(b&&k<blk){ prev=b; b=(uint32_t*)(uintptr_t)b[0xa00]; k++; } // ponytail: 32-bit link word as in retail; wasm64 port widens
 if(!b){ b=(uint32_t*)s.alloc(0x35d,0); for(int i=0;i<0xa01;i++) b[i]=0; if(prev) prev[0xa00]=(uint32_t)(uintptr_t)b; else g.cc1b8=b; }
 uint32_t* e=b+slot*5; e[0]=e[1]=e[2]=e[3]=e[4]=0;
}
} // namespace d2client_batch_5ebc0
