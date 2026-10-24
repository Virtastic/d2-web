// D2Client hireling-portrait / sound-slot helpers, decompile-derived (UI/Hireables.cpp). Imports go
// through Services, ordinals named by role. Native pointers stay native; record fields are read by offset.
#include <cstdint>
#include <cstring>
#include <cstdio>
namespace d2client_batch_5b120 {
template<class T> T rd(const void* p, unsigned o){ T v; std::memcpy(&v,(const uint8_t*)p+o,sizeof v); return v; }
// Hireling list node (stride 0x34+): +0 value +4 class +8 id +0xC unitId +0x1C life% +0x20 flag +0x30 next.
struct Services {
 virtual ~Services()=default;
 virtual const uint8_t* hireRec(int32_t i)=0;   // sgptDataTables+0xBE8 + i*0xE0, null if out of range (count at +0xBF0)
 virtual uint32_t bitMask14()=0;                // gdwBitMasks+0x14
 virtual const uint8_t* hireNode()=0;           // DAT_6fbcc4d4 list head (retail node bytes)
 virtual uint32_t loadSprite(const char* name)=0; // FUN_6fb6f6c0
 virtual uint32_t loadSlotIcon(int32_t i)=0;    // FUN_6fb6f6c0 for the 5 base slots
 virtual bool expansion()=0;                    // Ordinal_10227
 virtual uint32_t* allocCache(unsigned dwords)=0; // Ordinal_10045 (Hireables.cpp:0x96), zeroed by caller
 virtual uint32_t colour(int32_t r,int32_t g,int32_t b)=0; // Ordinal_10190
 virtual void fillRect(int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t col,int32_t mode)=0; // Ordinal_10014
 virtual const uint8_t* findPlayerRec(int32_t,int32_t)=0; // FUN_6fb4d630
 virtual uint32_t* iconSlots()=0;               // DAT_6fbc77c8[7]
 virtual uint32_t* cache()=0; virtual int32_t cacheCount()=0; // DAT_6fbc77c4, hire count
 virtual uint8_t* colourCache()=0;              // DAT_6fbcc288..8b (8b = initialised)
 virtual int32_t viewMode()=0;                  // DAT_6fbcc284
 virtual void drawName(const char* s,int32_t x,int32_t y)=0; // FUN_6fb0aec0
 virtual void drawIcon(void* cell,int32_t x,int32_t y,int32_t a,int32_t b,int32_t c)=0; // Ordinal_10041
 virtual void drawNameU(const void* uni,int32_t a,int32_t b,int32_t c)=0; // FUN_6fb0ac70
 virtual const void* playerName()=0;            // FUN_6fb55d90
 [[noreturn]] virtual void fatal(int32_t strId)=0; // Ordinal_10265 + 10024 + _exit(-1)
};
// 1.13c D2Client+0x5B120 ; class in eax, returns sprite handle (0 none)
uint32_t hireIcon(Services& s,int32_t cls,uint32_t* outFlags,const uint8_t* unit,uint32_t* outId){
 const uint8_t* rec=s.hireRec(cls);
 if(!rec) return 0;
 if(outFlags) *outFlags=rec[4]&s.bitMask14();
 const uint8_t* head=s.hireNode();
 uint32_t id=0xFFFFFFFFu;
 if(unit) for(const uint8_t* n=head;n;n=rd<const uint8_t*>(n,0x30))
  if(rd<uint32_t>(n,4)==(uint32_t)cls&&rd<uint32_t>(n,0xC)==rd<uint32_t>(unit,0xC)&&rd<uint32_t>(n,0x20)==0){id=rd<uint32_t>(n,8);break;}
 *outId=id;
 if(id==0xFFFFFFFFu) return 0;
 uint32_t want=0;
 for(const uint8_t* n=head;n;n=rd<const uint8_t*>(n,0x30)) if(rd<uint32_t>(n,8)==id){want=rd<uint32_t>(n,0);break;}
 int32_t k=0;
 for(int32_t i=1;i<5;i++) if(want==rd<uint16_t>(rec,0xB2+(i-1)*2)&&rec[0x2F+(i-1)*0x20]!=0){k=i;break;}
 uint32_t* slot=s.cache()+(k+cls*5);
 if(*slot==0){ char b[256]; snprintf(b,sizeof b,"hireables\\%s",(const char*)rec+k*0x20+0xF); *slot=s.loadSprite(b); }
 return *slot;
}
// 1.13c D2Client+0x5B260 : load 5 base icons, the expansion icons, allocate the per-hireling cache
void hireablesInit(Services& s){
 static const char* const names[3]={"Hireables\\NecromancerIcon","Hireables\\AssassinIcon","Hireables\\DruidIcon"};
 uint32_t* ic=s.iconSlots(); for(int i=0;i<7;i++) ic[i]=0;
 int i=0; for(;i<5;i++) ic[i]=s.loadSlotIcon(i);
 if(s.expansion()) for(int j=1;j<7&&j<3;j++) ic[i++]=s.loadSprite(names[j]); // names[1..2] non-empty, [3] empty ends loop
 unsigned n=(unsigned)s.cacheCount()*5; uint32_t* c=s.allocCache(n); for(unsigned k=0;k<n;k++) c[k]=0;
}
// 1.13c D2Client+0x5B530 : draw the owner's name (32-wchar bounded copy)
void hireName(Services& s,bool have,int32_t a,int32_t b){
 if(!have) return;
 uint16_t buf[32]={0}; const uint16_t* src=(const uint16_t*)s.playerName();
 for(int i=0;i<31&&src[i];i++) buf[i]=src[i];
 s.drawNameU(buf,a,b,0);
}
// 1.13c D2Client+0x5B5A0 : life bar (46px wide). lookupById selects the node list over the player record.
void lifeBar(Services& s,int32_t lookupById,int32_t id,int32_t x,int32_t unitId){
 int32_t life;
 if(lookupById==0){ const uint8_t* r=s.findPlayerRec(unitId,id); if(!r) return; life=rd<int32_t>(r,0x14); }
 else { const uint8_t* n=s.hireNode(); for(;;){ if(!n) return; if(rd<int32_t>(n,8)==id) break; n=rd<const uint8_t*>(n,0x30);} life=rd<int32_t>(n,0x1C); }
 if(!life) return;
 int32_t x1=x+0x2E; if(life<1) life=0; else if(life>99) life=100;
 int32_t mid=life*0x2E/100+x;
 uint8_t* c=s.colourCache();
 if(!c[3]){ c[3]=1; c[0]=(uint8_t)s.colour(0,0x80,0); c[1]=(uint8_t)s.colour(0xC0,0xC0,0); c[2]=(uint8_t)s.colour(0xFF,0,0); }
 uint8_t col=c[0]; if(life<0x32) col=c[1]; if(life<0x19) col=c[2];
 s.fillRect(x,0xE,mid,0x13,col,5); s.fillRect(mid,0xE,x1,0x13,0,5);
}
// 1.13c D2Client+0x5BA90 : draw icons+names for same-class hirelings that are not the viewed unit
struct Walk {
 virtual ~Walk()=default;
 virtual const uint8_t* firstUnit()=0;               // DAT_6fbcbc14
 virtual const uint8_t* nextUnit(const uint8_t*)=0;  // +0x80
 virtual uint32_t iconFor(uint32_t kind)=0;          // DAT_6fbc77c8.. per switch below
};
bool hirePortraits(Services& s,Walk& w,const uint8_t* viewed,int32_t* y,int32_t* x,int32_t* toggle){
 int16_t myCls=-1; int32_t myId=-1;
 if(viewed){ myId=rd<int32_t>(viewed,0xC); if(rd<int32_t>(viewed,0)==0){ const uint8_t* r=s.findPlayerRec(0,myId); myCls=r?rd<int16_t>(r,0x22):-1; } }
 if(myCls==-1||s.viewMode()!=0) return false;
 bool any=false; const uint8_t* head=w.firstUnit();
 for(const uint8_t* u=head;u;u=w.nextUnit(u)){
  int32_t cnt=0; for(const uint8_t* p=head;p;p=w.nextUnit(p)) if(rd<int16_t>(p,0x22)==rd<int16_t>(u,0x22)) cnt++;
  if(cnt<2||rd<int16_t>(u,0x22)!=myCls) continue;
  int32_t id=rd<int32_t>(u,0x10); if(id==myId) continue;
  uint32_t kind=rd<uint32_t>(u,0x1C); if(kind>6) continue;
  uint32_t icon=w.iconFor(kind);
  if(!icon) continue;
  lifeBar(s,id,id,*x,0); // retail passes (id,id,x): second arg is the node id, first the player-record id
  s.drawName((const char*)u+0x66,*x,*y);
  if(*toggle==0){*toggle=10;*y+=10;} else {*y-=*toggle;*toggle=0;}
  uint32_t cell[18]={0}; cell[12]=icon; // local_1c: icon handle at +0x2C of the 0x48-byte cell record
  s.drawIcon(cell,*x,0x3C,-1,1,0);
  *x+=0x38; any=true;
 }
 return any;
}
// 1.13c D2Client+0x5C5C0 / +0x5C7F0 use the global state through GlobalSvc below.
struct GlobalSvc {
 virtual ~GlobalSvc()=default;
 virtual int32_t viewMode()=0;                         // DAT_6fbcc284
 virtual int32_t* flags3()=0;                          // DAT_6fbcc2a0/98/94 (3 ints, cleared together)
 virtual const uint8_t* viewedUnit()=0;                // DAT_6fbcbbfc
 virtual const uint8_t* hireNode()=0;
 virtual bool isHireling(int32_t unitId)=0;            // FUN_6fb4d9d0
 virtual const uint8_t* unitBySlot(uint32_t)=0;        // DAT_6fbba808[id&0x7f] chain, next at +0xE4
 virtual bool unitCheck()=0;                           // FUN_6fb55440
 virtual bool expansion()=0;                           // Ordinal_10227
 virtual int32_t panelCount()=0;                       // DAT_6fb8d5d0, must be 0x26
 virtual uint8_t panelActive(uint32_t)=0;              // DAT_6fbaad80[i]
 virtual int32_t panelKind(uint32_t,unsigned off)=0;   // *(PTR_DAT_6fb8d538[i]+off)
 virtual void closePanel(uint32_t)=0;                  // FUN_6fb72790(i,1,0)
 virtual void kind4()=0;                               // FUN_6faf7b20 gated on DAT_6fbc9721
 virtual int32_t& openFlag()=0;                        // DAT_6fbaadcc
 virtual void fire(const void* tag,bool open)=0;       // FUN_6faeccc0 / 6faecc60 (Ordinal_10048 target)
 [[noreturn]] virtual void fatal(int32_t strId)=0;
};
static void panelSweep(GlobalSvc& g,unsigned off,bool& done){
 uint32_t i=0; done=false;
 for(;i<(uint32_t)g.panelCount();i++){
  if(g.panelActive(i)==1){ int32_t k=g.panelKind(i,off);
   if(k==1) g.closePanel(i); else if(k==4){ g.kind4(); } else if(k!=0) return; }
 }
 done=true;
}
void clearFlags(GlobalSvc& g){ int32_t* f=g.flags3(); f[0]=f[1]=f[2]=0; }
// 1.13c D2Client+0x5C5C0
void hirePanelTick(GlobalSvc& g){
 if(g.viewMode()==2){ clearFlags(g); return; }
 const uint8_t* v=g.viewedUnit(); int32_t id=v?rd<int32_t>(v,0xC):-1; bool own=false;
 for(const uint8_t* n=g.hireNode();n;n=rd<const uint8_t*>(n,0x30)) if(rd<int32_t>(n,0xC)==id){own=true;break;}
 bool hire=g.isHireling(id); bool wasOpen=g.openFlag()!=0;
 if(!((hire&&g.viewMode()!=1)||own)){
  clearFlags(g);
  if(wasOpen){ g.expansion(); if(g.panelCount()!=0x26) g.fatal(0x139B); g.openFlag()=0; g.fire(nullptr,false); }
  return;
 }
 bool clr=true;   // flags are cleared unless the slot chain walk dead-ends on a null unit
 if(v) for(const uint8_t* n=g.hireNode();n;n=rd<const uint8_t*>(n,0x30))
  if(rd<int32_t>(n,4)==7&&rd<int32_t>(n,0xC)==rd<int32_t>(v,0xC)&&rd<int32_t>(n,0x20)==0){
   uint32_t sid=rd<uint32_t>(n,8); if(sid==0xFFFFFFFFu) break;
   const uint8_t* u=g.unitBySlot(sid&0x7F);
   for(;;u=rd<const uint8_t*>(u,0xE4)){
    if(!u){ clr=false; break; }
    if(rd<uint32_t>(u,0xC)==sid){ if(rd<int32_t>(u,0)!=1) g.fatal(0x5A); clr=g.unitCheck(); break; }
   }
   break; }
 if(clr) clearFlags(g);
 if(g.openFlag()!=0) return;
 g.expansion(); if(g.panelCount()!=0x26) g.fatal(0x139B);
 bool done; panelSweep(g,0x4C,done); if(!done) return;
 if(!v||(rd<int32_t>(v,0)==0&&!g.unitCheck())) g.openFlag()=1; else return;
 g.fire(nullptr,true);
}
// 1.13c D2Client+0x5CD10 : decode a stream's wave header (ESI = stream; state at +0x86 must be 2)
// decode = D2sound #10025 via thunk 0xD2C4: (stream+0x6c, &hdrPtr in/out, fmt out) -> data ptr or 0; fmt = 16-byte WAVEFORMATEX head
using WaveDecode = uint32_t(*)(uint32_t src,uint32_t* hdrPtr,uint8_t* fmt);
bool waveHeader(uint8_t* st,void(*fatal)(uint32_t),WaveDecode decode){
 if(rd<int32_t>(st,0x86)!=2) fatal(0x5D);   // Fog #10265(0x5d) message then _exit(-1)
 uint32_t hp=rd<uint32_t>(st,0x70); uint8_t fmt[16];
 uint32_t d=decode(rd<uint32_t>(st,0x6C),&hp,fmt);
 if(d&&rd<uint16_t>(fmt,14)==0x10&&rd<uint32_t>(fmt,4)==0x5622){
  std::memcpy(st+0x74,&d,4); st[0x50]=(rd<uint16_t>(fmt,2)==2); std::memcpy(st+0x78,&hp,4); return true; }
 st[0x81]=1; return false;
}
// Sound slot table: 16 slots of 8 dwords at DAT_6fbc75c0; [0]=handle [1]=class [2]=record ptr
// retail slots hold 32-bit pointers (stride 0x20); a native pointer would be 8 bytes on wasm64
struct P32 { uint32_t v; operator uint8_t*() const { return (uint8_t*)(uintptr_t)v; } P32& operator=(const uint8_t* p) { v = (uint32_t)(uintptr_t)p; return *this; } };
struct SoundSlot { uint32_t handle,cls; P32 rec; uint32_t pad[5]; };
static_assert(sizeof(SoundSlot)==0x20&&offsetof(SoundSlot,rec)==8,"retail slot layout");
struct SoundSvc {
 virtual ~SoundSvc()=default;
 virtual uint32_t stopHandle(uint32_t h)=0;   // Ordinal_10032
 virtual void release(SoundSlot&)=0;          // Ordinal_10006
 virtual bool busy(SoundSlot&)=0;             // Ordinal_10044
 virtual bool stillPlaying(SoundSlot&)=0;     // Ordinal_10003/10044 variants
 virtual const uint8_t* soundRec(int32_t cls)=0; // DAT_6fbcc24c + cls*0x8e, 0x8e stride, null on range
 virtual void releaseSlot(SoundSlot& sl); // 5CE30 (FN seam overrides it with the hooked callee)
};
// 1.13c D2Client+0x5CE30 : release one slot
void slotRelease(SoundSvc& s,SoundSlot& sl){
 if(sl.handle){ if(sl.rec){ uint32_t v=s.stopHandle(sl.handle); std::memcpy((uint8_t*)((uint8_t*)sl.rec)+0x41,&v,4);} s.release(sl); }
 if(sl.rec){ uint32_t two=2,zero=0; std::memcpy(((uint8_t*)sl.rec)+0x30,&two,4); std::memcpy(((uint8_t*)sl.rec)+0x2C,&zero,4); sl.rec=nullptr; }
}
void SoundSvc::releaseSlot(SoundSlot& sl){ slotRelease(*this,sl); }
// 1.13c D2Client+0x5CE70 : find or evict a slot for a new sound (record `rec`, class `cls`)
SoundSlot* slotAcquire(SoundSvc& s,SoundSlot* t,const uint8_t* rec,uint32_t cls){
 for(int i=0;i<16;i++) if(t[i].rec==nullptr&&t[i].cls==cls&&!s.busy(t[i])) return &t[i];
 SoundSlot* best=nullptr;
 for(int i=0;i<16;i++){
  if(t[i].cls!=cls||!t[i].rec) continue;
  const uint8_t* a=t[i].rec;
  if(best){ const uint8_t* b=best->rec;
   bool take=false;
   if(a[0x3C]<b[0x3C]) take=true;
   else if(a[0x3C]==b[0x3C]){ float fa=rd<float>(a,0x18),fb=rd<float>(b,0x18);
    if(fb<fa) take=true; else if(fb==fa){ uint32_t ta=rd<uint32_t>(a,0x34),tb=rd<uint32_t>(b,0x34);
     if(ta<tb) take=true; else if(ta==tb&&a<b) take=true; } } // pointer tie-break kept as retail
   if(!take) continue; }
  best=&t[i];
 }
 if(!best) return nullptr;
 const uint8_t* b=best->rec; const uint8_t* n=rec;
 if(b[0x3C]<n[0x3C]||(b[0x3C]<=n[0x3C]&&b<n)){ slotRelease(s,*best); return best; }
 return nullptr;
}
// 1.13c D2Client+0x5CF90 : switch the active sound-bank id; closes the previous one when idle or unset
void switchBank(uint32_t& cur,uint32_t next,int32_t curState,bool curOpen,void(*close)()){
 uint32_t keep=cur;
 if(next!=cur){ keep=next; if((curState==2||next==0)&&curOpen) close(); } // every path assigns next
 cur=keep;
}
// 1.13c D2Client+0x5D080 : reap finished sound slots
void reapSlots(SoundSvc& s,SoundSlot* t){
 for(int i=0;i<16;i++){
  if(!t[i].rec) continue;
  const uint8_t* r=s.soundRec((int32_t)rd<uint32_t>(t[i].rec,4));
  if(r&&r[0x3E]) continue;
  if(!s.busy(t[i])){ if(s.stillPlaying(t[i])) s.releaseSlot(t[i]);
   if(t[i].rec){ uint32_t two=2,zero=0; std::memcpy(((uint8_t*)t[i].rec)+0x30,&two,4); std::memcpy(((uint8_t*)t[i].rec)+0x2C,&zero,4); t[i].rec=nullptr; } }
 }
}
// 1.13c D2Client+0x5C7F0 : per-frame hireling HUD. Blocked states clear the two flags and return.
struct HudSvc : GlobalSvc {
 virtual int32_t blockedState()=0;      // DAT_6fbcc414 in {2,3} or DAT_6fbaada4/DAC/viewMode==2
 virtual bool hudOn()=0;                // DAT_6fbc9854 && !DAT_6fbaae0c && viewed unit
 virtual int32_t& sweepDone()=0;        // DAT_6fbaae0c
 virtual bool portraits(int32_t* v)=0;  // FUN_5BA90 result
 virtual bool classPanel(int32_t kind,int32_t* v)=0; // 5B890 (1) / 5B760 (2) / 5B6C0 (3) / 5B930
 virtual int32_t classKind(int32_t i)=0; virtual int32_t classCount()=0; // table stride 0xE0, +0xE
 virtual bool popupPending()=0; virtual void popupClear()=0; // DAT_6fbcc29c / DAT_6fbaae10 / FUN_72790(0x24)
 virtual bool tooltipReady()=0; virtual void tooltip()=0;    // DAT_6fbcc298 && DAT_6fbcc294
};
void hireHud(HudSvc& h){
 int32_t v[3]={0,0x48,0xF};
 if(h.blockedState()){ clearFlags(h); return; }
 if(h.hudOn()){ bool done; panelSweep(h,0x8C,done);
  if(done){ if(h.openFlag()!=0||true){} h.sweepDone()=1; h.fire(nullptr,true); } }
 bool any=h.portraits(v);
 any|=h.classPanel(0,v); // 5B930
 for(int i=0;i<h.classCount();i++){ int32_t k=h.classKind(i); if(k>=1&&k<=3) any|=h.classPanel(k,v); }
 if(!any){ h.expansion(); if(h.panelCount()!=0x26) h.fatal(0x139B); int32_t was=h.openFlag(); h.openFlag()=0; if(was) h.fire(nullptr,false); }
 if(h.popupPending()) h.popupClear();
 if(h.tooltipReady()) h.tooltip();
}
}
// 1.13c D2Client+0x5B530 FN seam: name = 0xA5D90's result, draw = 0x5AC70 (buffer, a, b, 0).
typedef void (*d2c5b530_draw)(const uint16_t*, int32_t, int32_t, int32_t);
extern "C" void d2_client_5b530(int32_t have, int32_t a, int32_t b, const void* name, d2c5b530_draw draw) {
 struct S : d2client_batch_5b120::Services { const void* n; d2c5b530_draw d;
  void drawNameU(const void* u,int32_t x,int32_t y,int32_t c) override { d((const uint16_t*)u,x,y,c); }
  const void* playerName() override { return n; }
  const uint8_t* hireRec(int32_t) override{return 0;} uint32_t bitMask14() override{return 0;} const uint8_t* hireNode() override{return 0;}
  uint32_t loadSprite(const char*) override{return 0;} uint32_t loadSlotIcon(int32_t) override{return 0;} bool expansion() override{return 0;}
  uint32_t* allocCache(unsigned) override{return 0;} uint32_t colour(int32_t,int32_t,int32_t) override{return 0;}
  void fillRect(int32_t,int32_t,int32_t,int32_t,int32_t,int32_t) override{} const uint8_t* findPlayerRec(int32_t,int32_t) override{return 0;}
  uint32_t* iconSlots() override{return 0;} uint32_t* cache() override{return 0;} int32_t cacheCount() override{return 0;}
  uint8_t* colourCache() override{return 0;} int32_t viewMode() override{return 0;} void drawName(const char*,int32_t,int32_t) override{}
  void drawIcon(void*,int32_t,int32_t,int32_t,int32_t,int32_t) override{} [[noreturn]] void fatal(int32_t) override{for(;;);} } s;
 s.n = name; s.d = draw; d2client_batch_5b120::hireName(s, have != 0, a, b);
}
// 1.13c D2Client+0x5D080 FN seam: table = DAT 0x1175C0 (16 slots), recs = DAT 0x11C24C (ptr) / 0x11C250 (count).
struct d2c5d080_calls { bool (*busy)(uint32_t handle); bool (*playing)(uint32_t handle); void (*release)(); };
extern "C" void d2_client_5d080(void* table, const uint8_t* recs, int32_t count, const d2c5d080_calls& c) {
 struct S : d2client_batch_5b120::SoundSvc { const uint8_t* r; int32_t n; const d2c5d080_calls* c;
  uint32_t stopHandle(uint32_t) override{return 0;}
  void release(d2client_batch_5b120::SoundSlot&) override{}
  void releaseSlot(d2client_batch_5b120::SoundSlot&) override{ c->release(); }
  bool busy(d2client_batch_5b120::SoundSlot& s) override{ return c->busy(s.handle); }
  bool stillPlaying(d2client_batch_5b120::SoundSlot& s) override{ return c->playing(s.handle); }
  const uint8_t* soundRec(int32_t i) override{ return (r&&i>=0&&i<n)? r+i*0x8e : nullptr; } } s;
 s.r=recs; s.n=count; s.c=&c;
 d2client_batch_5b120::reapSlots(s,(d2client_batch_5b120::SoundSlot*)table);
}
