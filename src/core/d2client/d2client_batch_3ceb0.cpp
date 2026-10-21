// Prior art used: ; no D2Client module in D2MOO. Imported-DLL calls go through Services.
#include <array>
#include <cstdint>
#include <cstring>
#include <string>
namespace d2client_batch_3ceb0 {
struct Services {
 virtual ~Services()=default;
 // list walk (Ordinals 10947 first, 10858 data, 10353 count, 10273 next)
 virtual void* first(void* list)=0; virtual int16_t* data(void* node)=0;
 virtual int32_t count(void* node)=0; virtual void* next(void* node)=0;
 virtual int32_t queryAC()=0; virtual void sendPacket(const uint8_t*)=0;   // 0xC42A/0x143E0 packet send
 virtual int32_t ord10601()=0; virtual void ord10537()=0;
 virtual void release(uint32_t h)=0; virtual void ord10043(int32_t,int32_t)=0;
 virtual int32_t textWidth()=0; virtual void drawText(int32_t,int32_t,int32_t)=0;
 virtual std::u16string label(void* ctx,int32_t)=0;
 virtual std::u16string str10003()=0; virtual void fab17f0()=0;
 virtual void draw10085(const std::u16string&,int32_t y,int32_t mode,int32_t)=0;
 virtual int32_t ord10787()=0; virtual int32_t ord10847(int32_t)=0; virtual uint32_t alloc6f6c0()=0;
 virtual std::u16string name10265(int32_t)=0; virtual void fatal(const std::u16string&)=0;
};
struct State {
 Services& api;
 int32_t a968[240]{}, aA58[240]{}, aB48[240]{}, aC38[240]{}; uint8_t b9d29{}; int32_t n9d28{};
 int32_t a9d30[16]{}, a9e20[16]{}, a9dd0[16]{}; int32_t g_cc420{};
 uint8_t handles[7][34]{}; uint32_t h_cc41c{}; uint16_t idx991c{};
 void* list_cbbfc{}; uint8_t mask10{}; // gdwBitMasks_exref[0x10] low byte (low confidence)
 uint8_t table2e0[0x480]{}; int32_t mouseX{}, mouseY{}; int32_t g_c985c{}, g_c97f8{};
 std::u16string textBuf;
};
template<class T> T rd(const void*p,unsigned o){T v;std::memcpy(&v,(const uint8_t*)p+o,sizeof v);return v;}
// shared body of 3CEB0 / 3CF00 / 3CF50 (identical): walk until node whose data short equals ebx; always 0
static uint32_t walk(State& s,void* eax,int32_t ebx){
 if(eax){ void* it=s.api.first(eax);
  while(it){ int16_t* d=s.api.data(it); if(d && s.api.count(it)>=1 && *d==ebx) break; it=s.api.next(it);} }
 return 0;
}
// 1.13c D2Client+0x3CEB0
uint32_t sub_3CEB0(State& s,void* eax,int32_t ebx){return walk(s,eax,ebx);}
// 1.13c D2Client+0x3CF00
uint32_t sub_3CF00(State& s,void* eax,int32_t ebx){return walk(s,eax,ebx);}
// 1.13c D2Client+0x3CF50
uint32_t sub_3CF50(State& s,void* eax,int32_t ebx){return walk(s,eax,ebx);}
// 1.13c D2Client+0x3CFA0
int32_t sub_3CFA0(State& s,int32_t eax,int32_t p2){
 int n=s.n9d28; s.a968[n]=eax; s.aA58[n]=p2; s.aB48[n]=s.api.ord10601(); s.api.ord10537();
 s.b9d29=0; s.n9d28=n+1; return eax-0x30;
}
static void pkt51(State& s,int32_t b,int32_t c){ uint8_t p[9]; p[0]='Q'; std::memcpy(p+1,&b,4); std::memcpy(p+5,&c,4); s.api.sendPacket(p);}
// 1.13c D2Client+0x3D080
void sub_3D080(State& s,int32_t eax,int32_t p1){
 if(eax<0x10 && s.a9d30[eax]==-1){
  int32_t v=s.aB48[p1]; s.a9d30[eax]=v; s.a9e20[eax]=s.g_cc420==0?0:1;
  int32_t t=s.aC38[p1]; s.a9dd0[eax]=t;
  pkt51(s,eax*0x10000+(s.a9e20[eax]!=0?0x8000:0)+v,t);
 }
}
// 1.13c D2Client+0x3D180 (slot p: rebind to the 48-px cell under the mouse, dedupe other slots)
void sub_3D180(State& s,int32_t p){
 for(int k=0;k<60;++k){
  if(s.mouseX>=s.a968[k] && s.mouseX<=s.a968[k]+0x30 && s.aA58[k]-0x30<=s.mouseY && s.mouseY<=s.aA58[k]){
   if(s.a9d30[p]!=-1) pkt51(s,p*0x10000+0xffff,-1);
   int32_t v=s.aB48[k]; s.a9d30[p]=v; s.a9e20[p]=s.g_cc420==0?0:1;
   int32_t t=s.aC38[k]; s.a9dd0[p]=t;
   pkt51(s,p*0x10000+(s.a9e20[p]!=0?0x8000:0)+v,t);
   int32_t m=0xffff;
   for(int i=0;i<16;++i,m+=0x10000)
    if(s.a9d30[p]==s.a9d30[i] && s.a9dd0[p]==s.a9dd0[i] && p!=i){ s.a9d30[i]=s.a9e20[i]=s.a9dd0[i]=-1; pkt51(s,m,-1); }
  }
 }
}
// 1.13c D2Client+0x3D320
void sub_3D320(State& s,const int16_t* p1,int32_t p2){
 for(int k=0;k<60;++k) if(s.aB48[k]==(int32_t)*p1){
  int32_t m=0xffff;
  for(int i=0;i<16;++i,m+=0x10000)
   if(s.a9d30[i]==s.aB48[k] && p2<1){ s.a9d30[i]=s.a9e20[i]=s.a9dd0[i]=-1; pkt51(s,m,-1); }
 }
}
// 1.13c D2Client+0x3D470
static void freeSlot(State& s,uint32_t h){ if(h){ s.api.release(h); s.api.ord10043(0x7a,0);} }
void sub_3D470(State& s){
 if(s.h_cc41c){ freeSlot(s,s.h_cc41c); s.h_cc41c=0; }
 uint8_t* e=s.handles[s.idx991c%7]; uint32_t h=rd<uint32_t>(e,0);
 if(h){ freeSlot(s,h); std::memset(e,0,4);} 
 for(auto& r:s.handles){ h=rd<uint32_t>(r,0); if(h){ freeSlot(s,h); std::memset(r,0,4);} }
}
// 1.13c D2Client+0x3D660
void sub_3D660(State& s){
 void* it=s.api.first(s.list_cbbfc);
 if(!it) s.api.fatal(s.api.name10265(0x47d));
 do{ int16_t* d=s.api.data(it);
  if(s.mask10 & rd<uint8_t>(d,8)) sub_3D320(s,d,s.api.count(it));
  it=s.api.next(it);
 }while(it);
}
// 1.13c D2Client+0x3D770 (low confidence: table stride/field overlap in decompile)
void sub_3D770(State& s,int32_t ebx,int32_t p2,int32_t p3){
 unsigned i=0, o=0x6; // retail base 0x6FBCA2E0, scan ptr starts at +6
 while(rd<int32_t>(s.table2e0,o-6)!=ebx || rd<int32_t>(s.table2e0,o)!=1){ o+=10; ++i; if(0x6fbca2e0u+o>0x6fbca759u) return; }
 if(rd<int32_t>(s.table2e0,4+i*20)==-1) return;
 std::u16string t=s.api.label(&s,1);
 if(s.api.textWidth()>0x28){ s.textBuf=t.substr(0,16);
  size_t n=s.textBuf.size(); // zero trailing chars until narrow enough
  do{ if(n) s.textBuf[--n]=0; } while(s.api.textWidth()>0x28 && n);
 }
 s.api.drawText(p2,p3,0);
}
// 1.13c D2Client+0x3D8A0
void sub_3D8A0(State& s,int32_t p1,int32_t p2,int32_t p3,int32_t p4,int32_t p5){
 bool out=(s.mouseX<p4)||(p4+0x30<s.mouseX)||(s.mouseY<p5-0x30)||(p5<s.mouseY);
 if(out) p2=1; else if(p2!=1) p2=0;
 if(!out){ // inside rect
  std::u16string a=s.api.str10003(); s.api.fab17f0(); s.api.fab17f0();
  std::u16string b=s.api.str10003(); b+=a; b+=s.api.str10003(); b+=s.api.str10003();
  std::string d=std::to_string(p3); for(char c:d.substr(0,9)) b+=(char16_t)c;
  b+=a; b+=s.api.str10003();
  s.api.draw10085(b,p5-0x32,p2,1);
 }
}
// 1.13c D2Client+0x3DA60
uint32_t sub_3DA60(State& s,int32_t esi){
 int32_t r=s.api.ord10787();
 if(r==0){ if(esi!=(int32_t)(intptr_t)s.list_cbbfc || s.g_c985c<=s.g_c97f8) return 0; }
 else if(r==6) return 4;
 return 1;
}
// 1.13c D2Client+0x3DAA0
uint32_t sub_3DAA0(State& s,const int16_t* eax){
 if(!eax) return rd<uint32_t>(s.handles[s.idx991c%7],0);
 int32_t i=s.api.ord10847(*eax);
 if(i>=0 && i<7){ uint8_t* e=s.handles[i]; if(!rd<uint32_t>(e,0)){ uint32_t v=s.api.alloc6f6c0(); std::memcpy(e,&v,4);} return rd<uint32_t>(e,0);}
 return s.h_cc41c;
}
}
