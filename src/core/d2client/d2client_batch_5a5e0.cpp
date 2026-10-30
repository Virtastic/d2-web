// D2Client debug-overlay / hireable-label helpers, decompile-derived. Imports go through Services;
// ordinals named by role. Float constants at 0x87530/34/38 are read as 1/1024, 2^32, 100.0 (unverified).
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <initializer_list>
namespace d2client_batch_5a5e0 {
struct Services {
 virtual ~Services()=default;
 virtual uint32_t* cacheStats()=0;           // Ordinal_10073: 0x1C-dword stats block or null
 virtual void cacheReset()=0;                // Ordinal_10064
 virtual void tileStats(int32_t& a,int32_t& b,int32_t& c,int32_t& d)=0; // Ordinal_10074
 virtual void drawText(int32_t x,int32_t y,const char* s)=0; // Ordinal_10050 (already formatted)
 virtual uint32_t tick()=0; virtual uint32_t videoMode()=0;
 virtual void memStatus(uint32_t& tp,uint32_t& ap,uint32_t& tf,uint32_t& af)=0;
 virtual int32_t gfxMem()=0; virtual int32_t spriteCacheStat()=0; // Ordinal_10032(Ordinal_10058())
 virtual int32_t asyncFiles()=0; virtual int32_t celCacheStat()=0; // Ordinal_10082(Ordinal_10010())
 virtual int32_t textWidth(const uint16_t* s)=0;   // Ordinal_10028 (current font)
 virtual void showText(const uint16_t* s,int32_t a,int32_t b,int32_t c)=0; // Ordinal_10150
 virtual void saveFont()=0; virtual void restoreFont()=0;                 // Ordinal_10184 pair
 virtual void setFlag(int32_t,int32_t,int32_t)=0; // FUN_72790
 virtual bool regQuery(const char* app,const char* key)=0; // Ordinal_423 (null result => default)
 virtual const char* stringById(int32_t id)=0;     // Ordinal_10003 (ascii view)
 virtual void freeEntry(int32_t h)=0; virtual void freeBuf()=0;
};
struct State {
 uint32_t fpsLast{},fpsNow{},fpsAcc{},fpsSkipBase{},fpsEnable{},fpsFlag{},detail{};
 int32_t ping{},skip{},cacheHandles[7]{},*hireTable{},hireCount{},hireMax{};
 uint32_t popupHireling{},hireFlag{};
 int32_t* dataTables{};
};
static float kf(int32_t v){ float f=(float)v; if(v<0) f+=4294967296.0f; return f*(1.0f/1024.0f); }
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
  float pct=(float)((double)d*100.0/(double)c);
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
// 1.13c D2Client+0x5AC70 (ESI = text buffer, 0x3E bytes into it is the end sentinel)
void drawLabel(Services& s,uint16_t* buf,int32_t b,int32_t c){
 s.saveFont(); uint16_t* e=buf+0x1f;
 while(s.textWidth(buf)>0x42){ *--e=0; }
 s.textWidth(buf); s.showText(buf,b,c,0); s.restoreFont();
}
// 1.13c D2Client+0x5ADD0
void drawNumber(Services& s,int32_t n,uint16_t* out){
 char a[32]; snprintf(a,sizeof a,"%d",n);
 uint16_t w[32]={}; for(int i=0;a[i]&&i<31;i++) w[i]=(uint8_t)a[i];
 size_t len=0; while(w[len]) len++;
 uint16_t* e=w+len; s.saveFont();
 while(s.textWidth(w)>0x2e){ *--e=0; }
 s.showText(w,0x37,0,0); s.restoreFont(); (void)out;
}
// 1.13c D2Client+0x5AEC0
void drawAsciiLabel(Services& s,const char* a,int32_t b,int32_t c){
 uint16_t w[32]={}; for(int i=0;a[i]&&i<31;i++) w[i]=(uint8_t)a[i];
 drawLabel(s,w,b,c);
}
// 1.13c D2Client+0x5AF10 (index in EAX; row stride 0xE0, name id at +0xC)
void drawHireableName(Services& s,State& st,int32_t idx,int32_t b,int32_t c,const uint8_t* rows,int32_t count){
 if(idx<0||idx>=count||!rows) return;
 int16_t id; std::memcpy(&id,rows+idx*0xe0+0xc,2);
 if(id==0||id==0x1506) return;
 const char* t=s.stringById(id); uint16_t w[32]={};
 for(int i=0;t&&t[i]&&i<31;i++) w[i]=(uint8_t)t[i];
 drawLabel(s,w,b,c); (void)st;
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
