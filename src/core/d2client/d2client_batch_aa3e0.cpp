// Implementation batch D2Client+0xAA3E0..0xAC530 (waygate assets, record/replay list, packet queue, bit-unpacked packets, save).
// Decompile-derived; imported DLL calls and process globals go through Env. Globals are named by RVA
// (ga(rva) = address of the global). Pointers are uintptr_t. Register-passed args the decompiler lost
// are named explicitly in each signature.
#include <cstdint>
#include <cstring>
#include <cstdio>
namespace d2client_batch_aa3e0 {
struct Env {
 virtual ~Env()=default;
 virtual uint8_t* ga(uint32_t rva)=0;                       // address of D2Client global at RVA
 virtual uintptr_t alloc(int32_t line,uint32_t size)=0;     // Ordinal_10042(line,0)
 virtual void freeP(uintptr_t p,int32_t line)=0;            // Ordinal_10043(line,0)
 virtual void fatal(int32_t msgId)=0;                       // Ordinal_10265 + Ordinal_10024 + _exit(-1)
 virtual uintptr_t loadCel(const char* path)=0;             // FUN_6fadb420
 virtual int32_t q(int32_t ordinal,int32_t a,int32_t b)=0;  // Ordinal_10227 / 10033
 virtual uintptr_t fopenR(const char* name)=0;              // CreateFileA read-only
 virtual void fread(uintptr_t h,void* dst,uint32_t n)=0;    // ReadFile
 virtual void fclose(uintptr_t h)=0;
 virtual void dispatch(uint32_t idx,uintptr_t unit)=0;      // call table handler at DAT_6fb8de68[idx*3]
 virtual void win2uni(uint8_t* dst,const char* src,int32_t n)=0; // Unicode::win2Unicode
 virtual void bitInit(const uint8_t* src,int32_t len)=0;    // Ordinal_10126
 virtual uint32_t bitRead(int32_t bits)=0;                  // Ordinal_10130
 virtual void savePath(char* out)=0;                        // Ordinal_10115
 virtual bool writeFile(const char* path,const void* d,uint32_t n)=0; // fopen "wb" + fwrite + fclose
};
static uint32_t G32(Env& e,uint32_t rva){ uint32_t v; std::memcpy(&v,e.ga(rva),4); return v; }
static void S32(Env& e,uint32_t rva,uint32_t v){ std::memcpy(e.ga(rva),&v,4); }
static uintptr_t GP(Env& e,uint32_t rva){ uintptr_t v; std::memcpy(&v,e.ga(rva),sizeof v); return v; }
static void SP(Env& e,uint32_t rva,uintptr_t v){ std::memcpy(e.ga(rva),&v,sizeof v); }
static uintptr_t rdp(uintptr_t a){ uintptr_t v; std::memcpy(&v,(const void*)a,sizeof v); return v; }
static void wrp(uintptr_t a,uintptr_t v){ std::memcpy((void*)a,&v,sizeof v); }
static void w32(uintptr_t a,uint32_t v){ std::memcpy((void*)a,&v,4); }
static uint32_t r32(uintptr_t a){ uint32_t v; std::memcpy(&v,(const void*)a,4); return v; }

enum : uint32_t { // global RVAs (ptr - 0x6fab0000)
 R_WAYBG=0xFCDC1, R_WAYTABS=0xFCDC5, R_WAYICON=0xFCDC9, R_EXP=0x119854,
 R_LISTHEAD=0xFCD70, R_CURNODE=0xDE694, R_REC=0xFCD78, R_RECBUF=0xFAE28, R_RECCNT=0xFCD7C, R_RECREM=0xFCD80, R_RECN=0xFCD68,
 R_RECSTATE0=0x11BBE4, R_RECSTATE1=0x11BBE8, R_RECSZ=0x11BBD8, R_TBL=0xDDE64, R_TBLEND=0xDE698,
 R_NAME=0x119928, R_SAVELEN=0x11BBD0, R_SAVEBUF=0x11BBCC, R_CHAR=0x11B980, R_UNIC=0x11FED8, R_FLAG=0x1177E4 };
static const uint32_t NODE_NEXT=0x1f48; // node link; 8000 / 0x1f44 are cleared fields

// 1.13c D2Client+0xAA3E0
void loadWaygateAssets(Env& e){
 char p[260];
 if(!GP(e,R_WAYBG)){ snprintf(p,sizeof p,"%s\\ui\\menu\\waygatebackground","DATA\\GLOBAL"); SP(e,R_WAYBG,e.loadCel(p)); }
 if(!GP(e,R_WAYTABS)){
  bool x=e.q(10227,0,0)!=0 && G32(e,R_EXP)!=0;
  snprintf(p,sizeof p,x?"%s\\ui\\menu\\expwaygatetabs":"%s\\ui\\menu\\waygatetabs","DATA\\GLOBAL");
  SP(e,R_WAYTABS,e.loadCel(p));
 }
 if(!GP(e,R_WAYICON)){ snprintf(p,sizeof p,"%s\\ui\\menu\\waygateicons","DATA\\GLOBAL"); SP(e,R_WAYICON,e.loadCel(p)); }
}

// 1.13c D2Client+0xAB660
void freeNodeList(Env& e){
 uintptr_t n=GP(e,R_LISTHEAD);
 while(n){ uintptr_t nx=rdp(n+NODE_NEXT); e.freeP(n,0x120); n=nx; }
}

static void appendNode(Env& e){
 uintptr_t p=e.alloc(0x35,NODE_NEXT+sizeof(uintptr_t));
 w32(p+8000,0); w32(p+0x1f44,0);
 wrp(GP(e,R_CURNODE)+NODE_NEXT,p);
 wrp(p+NODE_NEXT,0);
 SP(e,R_CURNODE,p);
}
// 1.13c D2Client+0xAB780
void appendNode780(Env& e){ appendNode(e); }

// 1.13c D2Client+0xABA10  (ordinals none; push a 16-byte record {DAT_6fbc97f8,a,b,c} onto the current node, spill to a new node at 500)
void pushRecord(Env& e,uint32_t a,uint32_t b,uint32_t c){
 uintptr_t n=GP(e,R_CURNODE);
 if((int32_t)r32(n+8000)>499){ appendNode780(e); n=GP(e,R_CURNODE); }
 int32_t i=(int32_t)r32(n+8000);
 w32(n+8000,(uint32_t)(i+1));
 uintptr_t r=n+(uintptr_t)i*0x10;
 w32(r,G32(e,0x1197F8)); w32(r+4,a); w32(r+8,b); w32(r+12,c);
}

// 1.13c D2Client+0xAB8D0
void loadRecord(Env& e){
 uintptr_t h=e.fopenR("Record.dr1"); uint8_t tmp[4];
 e.fread(h,tmp,4); e.fread(h,e.ga(R_REC),0x14);
 if(G32(e,R_REC)!=1){ S32(e,R_RECSTATE0,0); S32(e,R_RECSTATE1,0); e.fclose(h); return; }
 int32_t cnt=(int32_t)G32(e,R_RECCNT);
 for(int32_t i=0;i<cnt;i++){
  if(i!=0) appendNode(e);
  int32_t rem=(int32_t)G32(e,R_RECREM);
  if(rem<0x1f5){ S32(e,R_RECN,(uint32_t)rem); if(i+1!=cnt) e.fatal(0x100); }
  else S32(e,R_RECN,500);
  e.fread(h,(void*)GP(e,R_CURNODE),G32(e,R_RECN)<<4);
 }
 SP(e,R_CURNODE,(uintptr_t)e.ga(R_RECBUF));
 e.fclose(h);
}

// Unit packet queue header: {cap,count,?,buf} at unit+0xd8.
struct Queue { int32_t cap,count,x; uintptr_t buf; };
// 1.13c D2Client+0xABC20  (tag = low 16 bits of the caller's return address in the decompile; dispatch-table tag check)
void runQueue(Env& e,uintptr_t unit,uint16_t tag){
 if(!unit) e.fatal(0x8a5);
 Queue* q=(Queue*)rdp(unit+0xd8);
 if(!q) return;
 if(q->cap<q->count) e.fatal(0x1524);
 uint32_t rs=G32(e,R_RECSZ);
 for(int32_t i=0;i<q->count;i++){
  uint32_t off=rs*i; uint8_t op=*(uint8_t*)(q->buf+off);
  if(!e.q(10033,(int32_t)unit,0)) return;
  if(op>0xae) e.fatal(0x1530);
  uint32_t t=r32((uintptr_t)e.ga(R_TBL+op*12));
  if(t!=0xffffffffu && tag!=t) e.fatal(0x1533);
  if(!r32((uintptr_t)e.ga(R_TBL+op*12+4))) e.fatal(0x1535);
  e.dispatch(op,unit);
 }
 if(q->count) std::memset((void*)q->buf,0,(size_t)q->count*rs);
 q->count=0;
}

// 1.13c D2Client+0xABD40  (unit in EDI)
void freeQueue(Env& e,uintptr_t unit){
 if(!unit) e.fatal(0x8a5);
 if(rdp(unit+0xd8)){
  Queue* q=(Queue*)rdp(unit+0xd8);
  e.freeP(q->buf,0x1515); e.freeP((uintptr_t)q,0x1516);
  wrp(unit+0xd8,0);
 }
}

// 1.13c D2Client+0xABDC0  (unit in EAX)
Queue* growQueue(Env& e,uintptr_t unit){
 if(!G32(e,R_RECSZ)){
  int32_t m=0;
  for(uint32_t a=R_TBL;a<R_TBLEND;a+=12){
   uint32_t sz=r32((uintptr_t)e.ga(a)), h=r32((uintptr_t)e.ga(a+4));
   if(h && m<(int32_t)sz) m=(int32_t)sz;
  }
  S32(e,R_RECSZ,(uint32_t)m+5);
 }
 if(!unit) e.fatal(0x8a5);
 Queue* q=(Queue*)rdp(unit+0xd8);
 if(!q){ q=(Queue*)e.alloc(0x1266,sizeof(Queue)); q->cap=0; q->count=0; q->x=0; q->buf=0; wrp(unit+0xd8,(uintptr_t)q); }
 uint32_t rs=G32(e,R_RECSZ); int32_t old=q->cap;
 uintptr_t nb=e.alloc(0x126b,rs*(uint32_t)(old+5));
 std::memset((void*)nb,0,(size_t)rs*(old+5));
 if(q->buf){ std::memcpy((void*)nb,(void*)q->buf,(size_t)old*rs); e.freeP(q->buf,0x126f); }
 q->cap=old+5; q->buf=nb;
 return q;
}

// 1.13c D2Client+0xABFD0  (ecx = 21-byte struct; bytes 1..15 + NUL are converted, bytes 17..20 -> flag)
void unpackName(Env& e,const uint8_t* s){
 char b[16]; std::memcpy(b,s+1,15); b[15]=0;
 e.win2uni(e.ga(R_UNIC),b,0x10);
 uint32_t f; std::memcpy(&f,s+17,4); S32(e,R_FLAG,f);
}

// Bit-packed packet unpackers: {dst offset, byte width, bit count} per field.
struct F { uint8_t off,w,bits; };
static void unpack(Env& e,const uint8_t* src,int32_t len,uint8_t* d,const F* f,int n){
 e.bitInit(src,len);
 for(int i=0;i<n;i++){ uint32_t v=e.bitRead(f[i].bits); if(f[i].w==1) d[f[i].off]=(uint8_t)v; else { uint16_t w=(uint16_t)v; std::memcpy(d+f[i].off,&w,2); } }
}
// 1.13c D2Client+0xAC140  (dst in ESI; src buffer is a register arg)
void unpack9(Env& e,const uint8_t* src,uint8_t* d){
 static const F f[]={{0,1,8},{1,2,15},{3,2,16},{5,2,16},{7,1,8},{8,1,8}};
 unpack(e,src,9,d,f,6);
}
// 1.13c D2Client+0xAC1B0
void unpack13(Env& e,const uint8_t* src,uint8_t* d){
 static const F f[]={{0,1,8},{1,2,15},{3,2,15},{5,2,15},{7,2,16},{9,2,16},{0xb,1,8},{0xc,1,8}};
 unpack(e,src,13,d,f,8);
}
// 1.13c D2Client+0xAC240
void unpack15(Env& e,const uint8_t* src,uint8_t* d){
 static const F f[]={{0,1,8},{1,2,15},{3,2,15},{5,2,15},{7,1,7},{8,1,7},{9,2,16},{0xb,2,16},{0xd,1,8},{0xe,1,8}};
 unpack(e,src,15,d,f,10);
}

// 1.13c D2Client+0xAC530
void saveCharacter(Env& e){
 if(!*e.ga(R_NAME)) e.fatal(0x25);
 uint32_t len=G32(e,R_SAVELEN); uintptr_t buf=GP(e,R_SAVEBUF);
 if(len>7 && r32(buf)==0xAA55AA55u){
  uintptr_t ch=GP(e,R_CHAR); uint16_t v;
  if(r32(buf+4)<0x5c){ if(len<0x82) return; v=(uint16_t)r32(buf+24); }
  else { if(len<0x14f) return; v=(uint16_t)r32(buf+36); }
  std::memcpy((void*)(ch+0x1eb),&v,2);
  char dir[1024],path[500]; e.savePath(dir);
  snprintf(path,sizeof path,"%s%s.d2s",dir,(const char*)e.ga(R_NAME));
  e.writeFile(path,(const void*)buf,len);
 }
}
}
