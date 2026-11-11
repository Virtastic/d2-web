// D2Client debug-overlay / hireable-label helpers, decompile-derived. Imports go through Services;
// ordinals named by role. Float constants at 0x87530/34/38 are read as 1/1024, 2^32, 100.0 (unverified).
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include "d2client_batch_5a5e0.h"
namespace d2client_batch_5a5e0 {
static double kf(int32_t v){ double f=(double)v; if(v<0) f+=4294967296.0; return f*(1.0/1024.0); }
static void emit(Services& s,int x,int y,const char* fmt,...) __attribute__((format(printf,4,5)));
static void emit(Services& s,int x,int y,const char* fmt,...){
 char b[1024]; va_list a; va_start(a,fmt); vsnprintf(b,sizeof b,fmt,a); va_end(a); s.drawText(x,y,b); }
// 1.13c D2Client+0x5A5E0
void toggleDebug(Services& s,State& st){
 st.fpsEnable=(st.fpsEnable==0); st.detail=0;
 if(!st.fpsEnable) return;
 if(uint32_t* p=s.cacheStats()){ for(unsigned o:{1,2,4,5,9,10,11,12,13,14,0x12,0x13,0x14,0x18,0x19,0x1A}) p[o]=0; }
 s.cacheReset();
}
// 1.13c D2Client+0x5A680
void cacheOverlay(Services& s){
 int32_t a,b,c,d; s.tileStats(a,b,c,d);
 if(c>0){
  double pct=(double)b*100.0/(double)c;
  emit(s,10,0x30,"Tile Cache: (%.2fK) %3.2f%%, Misses: %d, %d percent utilized\n",(double)kf(a),(double)pct,b,d);
 }
 uint32_t* p=s.cacheStats(); if(!p) return;
 int32_t* q=(int32_t*)p;
 if(q[0]) emit(s,10,0x3c,"Floor Cache: (%.2fK) Misses: %d\n",(double)kf(q[0]),q[2]);
 if(q[3]) emit(s,10,0x48,"Subtile Cache: (%.2fK) Misses: %d\n",(double)kf(q[3]),q[5]);
 const char* n[3]={"1","2","3"};
 for(int i=0;i<3;i++) if(q[6+i]) emit(s,10,0x54+12*i,"Sprite Cache %s: (%.2fK) Misses: %d, Accesses Per Frame: %d\n",n[i],(double)kf(q[6+i]),q[12+i],q[18+i]);
}
// 1.13c D2Client+0x5A9A0
void frameOverlay(Services& s,State& st){
 uint32_t t=s.tick();
 if(st.fpsEnable && !st.fpsFlag){ st.fpsSkipBase=0; st.fpsFlag=t; }
 if(t-st.fpsLast<0x3e9){ st.fpsNow++; st.fpsSkipBase++; }
 else { st.fpsAcc=st.fpsNow; st.fpsNow=0; st.fpsLast=t; }
 if(!st.fpsEnable){ st.fpsFlag=st.fpsEnable; return; }
 emit(s,10,2,"%d FPS  %d Skip",st.fpsAcc,st.skip);
 emit(s,0xa0,2,"Ping: %d",st.ping);
 uint32_t tp,ap,tf,af; s.memStatus(tp,ap,tf,af);
 emit(s,10,0x10,"Phys:%d/%dm",(tp-ap>>20)+1,(tp>>20)+1);
 emit(s,0xa0,0x10,"Page:%d/%dm",(tf-af>>20)+1,(tf>>20)+1);
 static const char* v[]={"Unknown","Software","Software","DirectDraw","Glide","OpenGL","Direct3D","RAVE"};
 uint32_t m=s.videoMode(); emit(s,500,2,"Video: %s",v[m-1<7?m:0]);
 if(st.detail==0){
  emit(s,10,0x78,"GFX Memory->%i",s.gfxMem());
  emit(s,10,0x84,"COF Memory->%i of %i",st.cacheHandles[0],st.cacheHandles[1]);
  // one vararg is passed in 1.13c; the second %i has no argument in the decompile (passed as 0 here)
  emit(s,10,0x90,"Sprite Cache Memory->%i of %i",s.spriteCacheStat(),0);
  emit(s,10,0x9c,"Async Files->%i",s.asyncFiles());
  emit(s,10,0xa8,"CelData Cache Memory->%i of %i",s.celCacheStat(),0);
  cacheOverlay(s);
 }
}
// 1.13c D2Client+0x5AC00
void hirelingPopupInit(Services& s,State& st){
 st.hireFlag=0; s.setFlag(0x23,0,0);
 if(!s.regQuery("Diablo II","PopupHireling")) st.popupHireling=1;
}
// 1.13c D2Client+0x5AC70 is d2client_5ac70.cpp; the callers reach it through the label seam (ESI = buffer, stack a0 a1 a2)
void drawLabel(Services& s,uint16_t* buf,int32_t a0,int32_t a1,int32_t a2){ s.label(buf,a0,a1,a2); }
// 1.13c D2Client+0x5ADD0 lives in d2client_5add0.cpp (over the header's Services)
// 1.13c D2Client+0x5AEC0
void drawAsciiLabel(Services& s,const char* a,int32_t b,int32_t c){
 uint16_t w[32]={}; for(int i=0;a[i]&&i<31;i++) w[i]=(uint8_t)a[i];
 drawLabel(s,w,b,c,0);
}
// 1.13c D2Client+0x5AF10 (index in EAX; row stride 0xE0, name id at +0xC)
void drawHireableName(Services& s,State& st,int32_t idx,int32_t b,int32_t c,const uint8_t* rows,int32_t count){
 if(idx<0||idx>=count||!rows) return;
 int16_t id; std::memcpy(&id,rows+idx*0xe0+0xc,2);
 if(id==0||id==0x1506) return;
 const uint16_t* t=s.stringById(id); uint16_t w[64]={};
 for(int i=0;t&&t[i]&&i<63;i++) w[i]=t[i];
 drawLabel(s,w,b,c,0); (void)st;
}
// 1.13c D2Client+0x5AFA0
void freeHireables(Services& s,State& st){
 for(int i=0;i<7;i++) if(st.cacheHandles[i]){ s.freeEntry(st.cacheHandles[i]); st.cacheHandles[i]=0; }
 if(st.hireTable){
  for(int i=0;i<st.hireMax*5;i++) if(st.hireTable[i]) s.freeEntry(st.hireTable[i]);
  s.freeBuf(); st.hireTable=nullptr;
 }
}
}
