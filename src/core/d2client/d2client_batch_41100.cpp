// Implementation batch (alpha lane): D2Client 0x41100..0x42240. Decompile-derived; no vectors yet.
// Imported DLL calls and register-passed args are Services; globals are named by RVA in comments.
#include <cstdint>
#include <cstring>
extern "C" void d2client_scatter_41100(void* unit, int32_t seedLow, int32_t cx, int32_t cy, int32_t r, int32_t a5, int32_t a6);
namespace d2client_batch_41100 {
struct Svc {
 virtual ~Svc()=default;
 virtual const uint8_t* row(int32_t i)=0;            // sgptDataTables +0xb98 base, +0xba0 count, stride 0x23c
 virtual int32_t unitVariant(void* u)=0;             // D2Client+0x40550
 virtual int32_t unitAnimLen(void* u)=0;             // D2Client+0x40450
 virtual bool coords(int32_t* out2)=0;               // D2Client+0xA1340
 virtual void* spawn(const uint32_t* rec)=0;         // D2Client+0xB30F0
 virtual void seed(uint32_t& hi,int32_t& count)=0;   // Ordinal_10614 / Ordinal_10331
 virtual bool probe(int32_t x,int32_t y)=0;          // D2Client+0x1400
 virtual void setFrame(void* o,int32_t f)=0; virtual void setSpeed(void* o,int32_t v)=0;
 virtual int32_t o10786(void* u,int32_t a,int32_t i,int32_t b)=0;
 virtual void* o10392(int32_t h)=0; virtual int32_t px(void* p)=0; virtual int32_t py(void* p)=0; // 0x1630/0x1660
 virtual void prep()=0;                              // D2Client+0x2A0B0
 [[noreturn]] virtual void fatal(int32_t code)=0;
 virtual void drawA(void*,int32_t,int32_t,int32_t,int32_t,int32_t)=0; // 0x40F00
 virtual void drawB(void*,int32_t,int32_t,int32_t,int32_t,int32_t)=0; // 0x41000
 virtual void drawC(void*,int32_t,int32_t,int32_t,int32_t,int32_t)=0; // 0x40DF0
};
template<class T> static T rd(const uint8_t* p,unsigned o){T v;std::memcpy(&v,p+o,sizeof v);return v;}

// 1.13c D2Client+0x41100 : see scatter_41100.cpp (real callees)
// 1.13c D2Client+0x41290
bool spawnEffect(Svc& s,uint32_t a,int32_t idx,int32_t b){
 const uint8_t* e=s.row(idx); if(!e) return false;
 int32_t xy[2]; if(!s.coords(xy)) return false;
 if(rd<int16_t>(e,0xea)<0||rd<int16_t>(e,0xec)<0) return false;
 s.o10786((void*)(uintptr_t)a,rd<int32_t>(e,100),idx,b);
 uint32_t rec[24]={0}; rec[0]=1; rec[7]=(uint32_t)b; rec[6]=(uint32_t)idx; // +0x1c, +0x18 (decomp)
 void* o=s.spawn(rec);
 if(o){ s.setFrame(o,rd<int16_t>(e,0xec)); s.setSpeed(o,xy[1]); }
 return true;
}

// 1.13c D2Client+0x413A0 : start use-animation; spawn rec type 0x20 when no current frame count
struct Svc2 : Svc {
 virtual int32_t o10630()=0; virtual int32_t o10980(int32_t)=0; virtual void o10243(int32_t,int32_t)=0;
 virtual bool o10494(void*,int32_t)=0; virtual void o10542(void*,int32_t)=0; virtual void sound(int32_t)=0;
};
bool startAnim(Svc2& s,uint8_t* u,int32_t idx){
 const uint8_t* e=s.row(idx); if(!e) return false;
 int32_t v=s.unitVariant(u); if(v<0) return false;
 int32_t h=s.o10630(); if(!h) return false;
 if(s.unitAnimLen(u)<=0) return false;
 if(u) { uint32_t f=rd<uint32_t>(u,0xc4)&0xffffffbfu; std::memcpy(u+0xc4,&f,4); }
 int32_t xy[2]; if(!s.coords(xy)) return false;
 int32_t a=xy[0], b=h;
 int32_t n=s.o10980(h);
 if(n<1){ uint32_t rec[24]={0}; rec[0]=0x20; rec[1]=rec[2]=(uint32_t)(uintptr_t)u; rec[4]=(uint32_t)h; rec[7]=(uint32_t)v; rec[11]=(uint32_t)idx;
  if(s.spawn(rec)) s.sound(0xf); b=0 /* unaff_EBX unset in decomp */; }
 else { a=n-1; b=h; }
 s.o10243(b,a);
 if(s.o10494(u,0xc)){ s.o10542(u,e[0x16]-1); uint32_t m=0x800; std::memcpy(u+0x48,&m,4); }
 return true;
}

// D2Client 0x414F0 (seam form; real: use_dispatch_414f0.cpp)
bool dispatchC(Svc& s,void* u,int32_t idx,int32_t b){
 if(!s.row(idx)) return false;
 int32_t v=s.unitVariant(u); if(v<0) return false;
 int32_t xy[2]; if(!s.coords(xy)) return false;
 if(!s.unitAnimLen(u)) return false;
 s.drawC(u,xy[1],xy[0],idx,b,v); return true;
}

// D2Client 0x415A0 / 0x41690 seam form (real 415A0: use_dispatch_414f0.cpp) (identical but for the draw callee: 0x40F00 / 0x41000)
static bool viaPath(Svc& s,uint8_t* u,int32_t idx,int32_t b,bool second){
 const uint8_t* e=s.row(idx); if(!e) return false;
 int32_t v=s.unitVariant(u); if(v<0) return false;
 s.prep();
 int32_t h=rd<int32_t>(u,0x2c); if(!h) s.fatal(0x946);
 void* p=s.o10392(h); if(!p) return false;
 int32_t len=s.unitAnimLen(u);
 if(len<1) len=s.o10786(u,rd<int32_t>(e,100),idx,b);
 int32_t x=s.px(p),y=s.py(p);
 if(second) s.drawB(u,v,x,y,b,len); else s.drawA(u,v,x,y,b,len);
 return true;
}
bool path415A0(Svc& s,uint8_t* u,int32_t i,int32_t b){return viaPath(s,u,i,b,false);}
bool path41690(Svc& s,uint8_t* u,int32_t i,int32_t b){return viaPath(s,u,i,b,true);}

// 1.13c D2Client+0x41780 (tail calls scatter 0x41100)
bool scatterVia(Svc& s,uint8_t* u,int32_t idx,int32_t b,int32_t r){
 const uint8_t* e=s.row(idx); if(!e) return false;
 int32_t v=s.unitVariant(u); if(v<0) return false;
 int32_t len=s.unitAnimLen(u);
 if(len<1) len=s.o10786(u,rd<int32_t>(e,100),idx,b);
 s.prep();
 int32_t h=rd<int32_t>(u,0x2c); if(!h) s.fatal(0x946);
 int32_t y2; void* p=s.o10392(h); int32_t xy[2];
 if(!p){ if(!s.coords(xy)) return false; y2=xy[0]; (void)len; }
 else { len=s.px(p); y2=s.py(p); }
 d2client_scatter_41100(u,len,len,y2,r,idx,b); (void)v; return true;
}

// 1.13c D2Client+0x41F40 : copy login fields out of a packet; ip string copied only if not "0"
struct Login{ char ip[0x40]; char name[0x40]; uint32_t a; uint16_t b; uint32_t c; };
void loginCopy(const uint8_t* pk,Login& g){
 if(!(pk[0x33]=='0'&&pk[0x34]==0)){ std::strncpy(g.ip,(const char*)pk+0x33,sizeof g.ip-1); g.ip[sizeof g.ip-1]=0; }
 std::strncpy(g.name,(const char*)pk+0x1b,sizeof g.name-1); g.name[sizeof g.name-1]=0;
 g.a=rd<uint32_t>(pk,0x15); g.b=rd<uint16_t>(pk,0x19); g.c=rd<uint32_t>(pk,0x225);
}

// 1.13c D2Client+0x42090 / 0x420E0 : globals 6fbcc3cc, 6fbc9898, 6fbc9890, 6fbc988c (RVAs per image base)
struct Disp{ int32_t sysMetric4b, cc3cc, mode, v90, v8c; };
bool dispA(const Disp& d){
 if(d.sysMetric4b&&d.cc3cc){ if(d.mode==1) return d.v90!=0; if(d.mode==2&&(d.v8c==4||d.v8c==5)) return true; }
 return false; }
bool dispB(const Disp& d){ return d.sysMetric4b&&d.cc3cc&&d.mode==1 ? d.v90==0 : false; }

// 1.13c D2Client+0x42240 : push 2 on queue when state(6fba4314)==5; count(6fbcc568)>=5 is fatal 0xa6
bool queuePush2(Svc& s,int32_t state,int32_t& cnt,int32_t q[5]){
 if(state!=5) return true;
 if(cnt>4) s.fatal(0xa6);
 q[cnt++]=2; return true;
}
}
