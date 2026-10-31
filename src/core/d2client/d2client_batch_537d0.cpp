// Implementation batch (alpha lane): D2Client+0x537D0..0x54AC0. Decompile-derived; not yet vectored.
// Imported-DLL ordinals and not-yet-recovered D2Client helpers are Svc virtuals (named by ordinal/RVA).
// Retail 32-bit pointers never appear; globals are fields of G. 0x54E10 (3216 bytes) is not in this batch.
#include <cstdint>
#include <cstring>
#include <string>
namespace d2client_batch_537d0 {
// Palette-shift record: id at +0, two 0x800 banks (8 rows x 256) at +4 and +0x804, next at +0x1004 (retail).
struct Pal { int32_t id; uint8_t bank[0x1000]; Pal* next; };
struct Unit { int32_t type; int32_t id; };
struct G {
 Pal* head608{};                // DAT_6fbc9608
 uint8_t* tab7804{};              // DAT_6fbc7804: slot table, 8 bytes per monster id, Pal* stored at slot+4 (retail)
 uint8_t* remap2d8{};           // DAT_6fbcc2d8: 256-byte remap table
 int32_t c2d4{}, c380{};
 uint8_t c7808[30*256]{};       // DAT_6fbc7808
 int32_t t20c0{};               // DAT_6fba20c0 entry count of table 6fba17d0
 struct Ent { int32_t key, a, b, c; } t17d0[64]{};
 int32_t t2128{};               // DAT_6fba2128
 struct Row { int32_t id, f1, f2, f3; } t20d4[64]{};
};
struct Svc {
 virtual ~Svc()=default;
 virtual void release(int32_t line)=0;               // Ordinal_10043 (free)
 virtual void ord10614(Unit*)=0;                     // Ordinal_10614 (args unrecovered)
 virtual int32_t f558e0(Unit*)=0;                    // FUN_6fb558e0 (arg unrecovered; reads state)
 virtual const uint8_t* f1260()=0;                   // FUN_6fab1260 -> record (byte +0x11c checked via recType)
 virtual int32_t recType(const uint8_t*)=0;          // *(char*)(rec+0x11c)
 [[noreturn]] virtual void fatal(int32_t line)=0;    // Ordinal_10265/10024 + _exit(-1)
 virtual int32_t monPalType(int32_t monId)=0;        // sgptDataTables monstats(+0xa78,0x1a8)->+0x18 -> table(+0xa90,0x134)+0x11c; -1 if any bound/null check fails
 virtual const char* f1230(int32_t id)=0;               // FUN_6fab1230(id in EAX) -> record; code chars at +0x10,+0x11
 virtual int32_t f1690()=0;                          // FUN_6fab1690 (count)
 virtual bool openPalshift(const char* path)=0;      // FUN_6fabd6a0(1); path build argument passing unrecovered
 virtual Pal* allocPal()=0;                          // Ordinal_10042(0xf4,0): 0x1008 bytes zeroed
 virtual void readPal(Pal*)=0;                       // FUN_6fabd520 (register args unrecovered)
 virtual void closeFile()=0;                         // FUN_6fabd670
 virtual int32_t stat(int32_t key,int32_t retry)=0;  // Ordinal_10680 (args unrecovered; retry=1 on the second call)
 virtual int32_t stat10973(int32_t id)=0;            // Ordinal_10973(unit,id,0)
 virtual int32_t stat10744(void* unit,int32_t id)=0; // Ordinal_10744
 virtual int32_t ord10620(int32_t)=0;
 virtual std::u16string str(int32_t a=0,int32_t b=0,int32_t c=0)=0; // Ordinal_10003 (string-table fetch; args unrecovered)
 virtual std::u16string fmt(const std::u16string& f,int32_t a,int32_t b,int32_t c)=0; // Unicode::sprintf
};
static std::u16string dec(int32_t v){ std::string s=std::to_string(v); return std::u16string(s.begin(),s.end()); } // Ordinal_578 "%d" (DAT_6fb834c4 assumed) + toUnicode

// 1.13c D2Client+0x537D0
void sub_537D0(Svc& s,G& g){
 while(g.head608){ Pal* n=g.head608->next; s.release(0x84); g.head608=n; }
 if(g.tab7804){ s.release(0x8a); g.tab7804=nullptr; }
 if(g.remap2d8){ s.release(0x90); g.remap2d8=nullptr; }
}
// 1.13c D2Client+0x537A0 (callee of 53C30, already read): list lookup by id
extern "C" Pal* D2Client_537A0_find(Pal* head,int32_t id){ for(Pal* p=head;p;p=p->next) if(p->id==id) return p; return nullptr; }
static Pal* find(G& g,int32_t id){ return D2Client_537A0_find(g.head608,id); }
// Slot table access at DAT_6fbc7804 + 4 + id*8 (retail stores a 4-byte pointer there; host keeps a Pal* in the same slot).
static Pal* tabGet(const G& g,int32_t id){ Pal* p; std::memcpy(&p,g.tab7804+4+(size_t)id*8,sizeof p); return p; }
static void tabPut(G& g,int32_t id,Pal* p){ std::memcpy(g.tab7804+4+(size_t)id*8,&p,sizeof p); }

// 1.13c D2Client+0x539E0  (eax = rng state; seed lo at +0x20, hi at +0x24)
int32_t sub_539E0(Svc& s,uint32_t* st){
 if(!st) return 9;
 s.ord10614(nullptr);
 uint64_t v=(uint64_t)st[0]*0x6ac690c5u+st[1];
 st[0]=(uint32_t)v; st[1]=(uint32_t)(v>>32);
 return (int32_t)((uint32_t)v%0x1e)+9;
}

// 1.13c D2Client+0x53A30  (eax = unit)
uint8_t* sub_53A30(Svc& s,G& g,Unit* u){
 if(!u||u->type!=1) return nullptr;
 Pal* p=tabGet(g,u->id); if(!p) return nullptr;
 int32_t k=s.f558e0(u);
 bool alt=false;
 if(k==0||k==1){ if((uint32_t)u->id<0x16b||(uint32_t)u->id>0x16c) return nullptr; }
 else {
  if(!(k>=2&&k<=7)){
   if(g.c2d4){ int32_t j=k-8; if(j>=0&&j<0x1e) return g.c7808+j*0x100; s.fatal(0x160); }
  }
  if(g.c380){ const uint8_t* r=s.f1260(); if(r&&s.recType(r)==2) alt=true; }
 }
 return p->bank+(alt?0x800:0)+k*0x100;
}

// 1.13c D2Client+0x53B20  (eax = monster id, param_2 = Pal*)
// monStats/palTypes: retail record arrays (strides 0x1a8/0x134), counts from sgptDataTables +0xa80/+0xa98.
static void shift53B20(Pal* d,const uint8_t* remap){
 for(int i=0;i<0x800;i++) d->bank[0x800+i]=d->bank[i];
 uint8_t* dst=d->bank+0x800;
 for(int row=0;row<8;row++) for(int i=0;i<256;i++){
  uint8_t c=remap[d->bank[row*256+i]]; if(c) dst[row*256+i]=c;
 }
}
extern "C" void D2Client_53B20(int32_t monId,Pal* d,const uint8_t* remap,const uint8_t* monStats,int32_t monCount,const uint8_t* palTypes,int32_t palCount){
 if(monId<0||monId>=monCount||!monStats||!palTypes) return;
 int16_t t=(int16_t)(monStats[monId*0x1a8+0x18]|(monStats[monId*0x1a8+0x19]<<8));
 if(t<0||t>=palCount||palTypes[t*0x134+0x11c]!=2) return;
 shift53B20(d,remap);
}
void sub_53B20(Svc& s,G& g,int32_t monId,Pal* d){
 if(monId<0||s.monPalType(monId)!=2) return;
 shift53B20(d,g.remap2d8);
}

// 1.13c D2Client+0x53C30  (ecx = monster id)
void sub_53C30(Svc& s,G& g,int32_t id){
 const char* rec=s.f1230(id);
 if(!rec||find(g,id)) return;
 // "Data\Global\Monsters\" + code[0..1] (a ' ' ends the string) + "\Cof\palshift.dat"
 char path[0x100]; int n=0;
 for(const char* q="Data\\Global\\Monsters\\";*q;) path[n++]=*q++;
 for(int k=0;k<2;k++){ char c=rec[0x10+k]; if(c==' ') c=0; if(!c) break; path[n++]=c; }
 for(const char* q="\\Cof\\palshift.dat";*q;) path[n++]=*q++;
 path[n]=0;
 if(!s.openPalshift(path)) return;
 Pal* p=s.allocPal();
 s.readPal(p);
 s.closeFile();
 sub_53B20(s,g,id,p);
 p->id=id; p->next=g.head608; g.head608=p;
 for(int32_t i=0;i<s.f1690();i++) if(p->id==i) tabPut(g,i,p);
}

// 1.13c D2Client+0x54000  (esi = int32_t out[0x2a]; ebp = unit/context)
void sub_54000(Svc& s,G& g,int32_t* o){
 for(int i=0;i<0x2a;i++) o[i]=0;
 for(int i=0;i<g.t20c0;i++){
  switch(g.t17d0[i].key){
  case 0x11: o[7]=s.stat(0x11,0); break;
  case 0x12: o[6]=s.stat(0x12,0); break;
  case 0x15: o[0]=s.stat(0x15,0); if(!o[0]) o[0]=s.stat(0x15,1); break;
  case 0x16: o[1]=s.stat(0x16,0); if(!o[1]) o[1]=s.stat(0x16,1); break;
  case 0x30: o[0x18]=s.stat(0x30,0); break;
  case 0x31: o[0x19]=s.stat(0x31,0); break;
  case 0x32: o[0x12]=s.stat(0x32,0); break;
  case 0x33: o[0x13]=s.stat(0x33,0); break;
  case 0x34: o[0x24]=s.stat(0x34,0); break;
  case 0x35: o[0x25]=s.stat(0x35,0); break;
  case 0x36: o[0xc]=s.stat(0x36,0); break;
  case 0x37: o[0xd]=s.stat(0x37,0); break;
  case 0x39: o[0x1e]=s.stat(0x39,0); break;
  case 0x3a: o[0x1f]=s.stat(0x3a,0); break;
  case 0x3b: o[0x20]=s.stat(0x3b,0); o[0x21]=s.stat10973(0x146); break;
  }
 }
 if(o[0]>0&&o[1]>0) o[4]=1;
 if(o[6]>0&&o[7]>0) o[10]=1;
 if(o[0xc]>0&&o[0xd]>0) o[0x10]=1;
 if(o[0x12]>0&&o[0x13]>0) o[0x16]=1;
 if(o[0x18]>0&&o[0x19]>0) o[0x1c]=1;
 if(o[0x1e]>0&&o[0x1f]>0) o[0x22]=1;
 if(o[0x24]>0&&o[0x25]>0) o[0x28]=1;
}

// 1.13c D2Client+0x54280  (eax = int32_t* in[0x2a] (output of 54000), param_1 = key, param_2 = out string)
int32_t sub_54280(Svc& s,int32_t* in,int32_t key,std::u16string& out){
 std::u16string buf;
 auto pr=[&](int32_t a,int32_t b,int32_t c){ buf=s.fmt(s.str(a,b,c),a,b,c); };
 switch(key){
 case 0x11: return in[10];
 case 0x12: {
  auto a=s.str(), b=s.str(), c=s.str(); auto d=dec(in[6]);
  if(!in[10]) return 0;
  buf=c+d+a+b+s.str(); break; }
 case 0x15: case 0x16: case 0x17:
  if(!in[5]){
   if(!in[4]) return 0;
   if(in[1]<=in[0]){ in[5]=0; in[4]=0; return 0; }
   in[5]=1; pr(in[0],in[1],0);
  }
  break;
 case 0x18: return in[5]?1:in[4];
 case 0x30: if(!in[0x1c]) return 0; if(in[0x18]<in[0x19]) pr(in[0x18],in[0x19],0); else pr(0,0,0); break;
 case 0x31: return in[0x1c];
 case 0x32: if(!in[0x16]) return 0; if(in[0x12]<in[0x13]) pr(in[0x12],in[0x13],0); else pr(0,0,0); break;
 case 0x33: return in[0x16];
 case 0x34: if(!in[0x28]) return 0; if(in[0x25]<=in[0x24]) pr(0,0,0); else pr(in[0x24],in[0x25],0); break;
 case 0x35: return in[0x28];
 case 0x36: if(!in[0x10]) return 0; if(in[0xc]<in[0xd]) pr(in[0xc],in[0xd],0); else pr(0,0,0); break;
 case 0x37: return in[0x10];
 case 0x39: {
  if(!in[0x22]) return 0;
  if(in[0x21]<1) in[0x21]=1;
  int32_t v=in[0x20]/in[0x21]; in[0x20]=v;
  int32_t i7=v/0x19, i5=(in[0x1f]*v+0x80)>>8, i1=(in[0x1e]*v+0x80)>>8;
  in[0x1e]=i1; in[0x1f]=i5;
  if(i5<=i1) pr(i5,i7,0); else pr(i1,i5,i7);   // retail passes (i5,i7) on the <= branch
  break; }
 case 0x3a: case 0x3b: return in[0x22];
 default: return 0;
 }
 out+=buf; return 1;
}

// 1.13c D2Client+0x546E0  (eax = base value, param_1 = unit, param_2 = id, edi = skill record)
int32_t sub_546E0(Svc& s,const uint8_t* rec,int32_t base,void* unit,int32_t unitType,int32_t id,
                  const uint8_t* (*lookup)(int32_t idx,int32_t* ok),int32_t* okOut){
 // lookup(): sgptDataTables +0xbcc table (0x144 stride, count +0xbd4); ok=0 => retail returns 0.
 if(rec[0x54]>1&&rec[0x54]<6){
  uint16_t idx=(uint16_t)(rec[0x56]|(rec[0x57]<<8)); int32_t ok=1;
  const uint8_t* r2=lookup(idx,&ok); if(!ok||!r2){ if(okOut)*okOut=0; return 0; }
  base=(s.stat10973(idx)>>(r2[0x18]&0x1f))*base>>(rec[0x55]&0x1f);
 }
 int32_t v=base>>(rec[0x18]&0x1f);
 if(id==0x7a&&unit&&unitType==4&&s.stat10744(unit,0x39)) v+=0x32;
 return v;
}

// 1.13c D2Client+0x54780  (param_1 = out string)
void sub_54780(Svc& s,std::u16string& out){
 auto a=s.str(), b=s.str(), c=s.str(), d=s.str(); auto n=dec(0x32);
 out+=a+n+d+c+s.str()+b;
}

// 1.13c D2Client+0x54880  (param_1 = unit, ebp/unaff_retaddr context; param_3 = G::Row* start of table 6fba20d4)
void sub_54880(Svc& s,G& g,int32_t unitArg,std::u16string& outA,std::u16string& outB){
 s.str(); s.str(); s.str();
 int32_t want=s.ord10620(unitArg);
 for(int32_t i=0;i<g.t2128;i++){
  G::Row& r=g.t20d4[i];
  if(r.id!=want) continue;
  int32_t v=s.stat10973(0x47);
  if(r.id==8||r.id==9||r.id==6||r.id==7||r.id==10||r.id==0xb) v>>=8;
  std::u16string tail; if(v) tail=s.str();
  if(!v) continue;
  auto d=dec(v);
  if(r.f1!=1) s.fatal(0x141);
  outA = v<1 ? d : tail+d;           // retail: v<1 -> strcpy(d); else strcpy(prefix)+strcat(d)
  if(r.f2!=2) s.fatal(0x13d);
  outB = tail;                        // then four strcat calls via strcat_exref (operands unrecovered)
 }
}

// 1.13c D2Client+0x54AC0  (param_1 = unit, param_2 = stat arg, param_3 = skill idx, param_4 = expected, param_5 = skill rec, param_6 = out flag)
int32_t sub_54AC0(Svc& s,void* unit,int32_t statArg,int32_t idx,int32_t expect,const uint8_t* rec5,uint32_t* flag,
                  int32_t count,const uint8_t* (*row)(int32_t),
                  int32_t (*calc)(Svc&,void*,int32_t)){
 (void)statArg;
 if(*(const int16_t*)(rec5+0x3e)==0) return 0;
 for(int32_t i=count-1;i>=0;i--){
  const uint8_t* r=row(i); if(!r) return 0;
  if(*(const int16_t*)(rec5+0x3e)==*(const int16_t*)(r+0x3e)){
   *flag=(i==idx);
   s.stat(i,0);                       // Ordinal_10680(param_2,i,0)
   if(expect!=calc(s,unit,i)) return 0;
  }
 }
 return 1;
}
}
