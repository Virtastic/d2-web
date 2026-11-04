// Prior art: none in D2MOO (SCMD/SNET internals); retail 1.13c cached decompile is authoritative.
// Record layout (SCMD arg): +4 next, +8 flags, +C id, +10 name[16], +20 nameLen, +24 value,
// +28 mask, +2C dest ptr(32-bit), +30 destSize, +34 callback, +38 handled, +3C value/str ptr(32-bit).
#include "storm_batch_b144a.h"
#include <cstring>
#include <strings.h>

namespace storm_b144a {
static const char* kScmd = "..\\3rdParty\\STORM\\SOURCE\\SCMD.CPP";
static const char* kSnet = "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP";
static uint32_t rd(const void* p) { uint32_t v; std::memcpy(&v,p,4); return v; }
static void wr(void* p,uint32_t v) { std::memcpy(p,&v,4); }
static uint8_t* at(Ctx& c,const uint8_t* rec,int o) { return c.st->resolve(rd(rec+o)); }

// 1.13c Storm+0x1a970
void trimBlock(Ctx& c,uint8_t* heap,uint8_t* blk,int32_t* off,int32_t* total) {
    int32_t o=*off; uint16_t size; std::memcpy(&size,blk,2);
    uint8_t* end=blk+size; uint32_t rem=(uint32_t)size-o;
    if(c.st->encode(end)==rd(heap+0x20)) { wr(heap+0x20,c.st->encode(blk+o)); return; }
    if(rem>0xf) {
      uint8_t* q=blk+o; uint16_t r=(uint16_t)rem; std::memcpy(q,&r,2);
      q[2]=0; q[3]=2;
      uint32_t b=(rem&0xffff)>>5; if(b>7) b=8;
      wr(q+4,rd(heap+0x38+b*4)); wr(heap+0x38+b*4,c.st->encode(q)); return;
    }
    end[3]&=0xef; *off+=rem; *total+=rem;
}
// 1.13c Storm+0x25290
bool loadFile(Ctx& c,const char* path,int32_t* a,uint32_t* b,void* cc,void* errCb) {
    std::FILE* f=std::fopen(path,"rb");
    if(!f) { if(errCb) c.reportError(errCb,c.lastError()); return false; }
    std::fseek(f,0,SEEK_END); long n=std::ftell(f); std::fseek(f,0,SEEK_SET);
    char* buf=static_cast<char*>(c.alloc((uint32_t)n+1,kScmd,0x153));
    size_t got=std::fread(buf,1,(size_t)n,f); (void)got; std::fclose(f);
    buf[n]=0; char* cur=buf;
    bool r=c.parse(c,&cur,a,b,cc,errCb);
    c.free(buf,kScmd,0x164); return r;
}
// 1.13c Storm+0x24c60
void setBool(uint8_t* rec,const char* str,int32_t* consumed) {
    bool on;
    if(*str=='-') { on=false; *consumed=1; }
    else if(*str=='+') { on=true; *consumed=1; }
    else on=(rec[8]&1)!=1;
    uint32_t v=rd(rec+0x3c)&~rd(rec+0x28); wr(rec+0x3c,v);
    if(on) wr(rec+0x3c,rd(rec+0x24)|v);
    // dest at +2C is a 32-bit retail address: caller's resolve is needed, see setBoolDest
}
// The CRT strtoul/strtol that retail calls (0x6851 / 0x683A, 32-bit longs): blanks, sign, base 0 prefixes
// (0x -> 16 when a hex digit follows, 0 -> 8, else 10), no digits -> 0 with endptr at the start, overflow
// saturates to ULONG_MAX or LONG_MAX / LONG_MIN. A host strtol is 64-bit on this build and would not match.
static uint32_t crtStrto32(const char* s, char** end, int base, bool sgn) {
    const char* p=s;
    while(*p==' '||(*p>=9&&*p<=13)) ++p;
    bool neg=false;
    if(*p=='-'||*p=='+') { neg=(*p=='-'); ++p; }
    auto isx=[](char ch){ return (ch>='0'&&ch<='9')||(ch>='a'&&ch<='f')||(ch>='A'&&ch<='F'); };
    if((base==0||base==16)&&p[0]=='0'&&(p[1]=='x'||p[1]=='X')&&isx(p[2])) { base=16; p+=2; }
    else if(base==0) base=(p[0]=='0')?8:10;
    uint64_t acc=0; bool any=false, ovf=false;
    for(;;) {
        int ch=(unsigned char)*p, d;
        if(ch>='0'&&ch<='9') d=ch-'0';
        else if(ch>='a'&&ch<='z') d=ch-'a'+10;
        else if(ch>='A'&&ch<='Z') d=ch-'A'+10;
        else break;
        if(d>=base) break;
        any=true;
        if(!ovf) { acc=acc*(uint64_t)base+(uint64_t)d; if(acc>0xFFFFFFFFull) ovf=true; }
        ++p;
    }
    if(!any) { if(end) *end=const_cast<char*>(s); return 0; }
    if(end) *end=const_cast<char*>(p);
    if(!sgn) { if(ovf) return 0xFFFFFFFFu; uint32_t v=(uint32_t)acc; return neg?0u-v:v; }
    if(neg) { if(ovf||acc>0x80000000ull) return 0x80000000u; return 0u-(uint32_t)acc; }
    if(ovf||acc>0x7FFFFFFFull) return 0x7FFFFFFFu;
    return (uint32_t)acc;
}
// 1.13c Storm+0x24e60
void setNumber(Ctx& c,uint8_t* rec,const char* str,int32_t* consumed) {
    char* e=nullptr;
    uint32_t v=crtStrto32(str,&e,0,(rec[8]&1)!=0);
    wr(rec+0x3c,v);
    *consumed=e?(int32_t)(e-str):(int32_t)std::strlen(str);
    if(uint8_t* d=at(c,rec,0x2c)) { uint32_t n=rd(rec+0x30); if(n>4) n=4; std::memcpy(d,rec+0x3c,n); }
}
// 1.13c Storm+0x25350
void setString(Ctx& c,uint8_t* rec,const char* str,int32_t* consumed) {
    *consumed=(int32_t)std::strlen(str);
    if(rd(rec+0x3c)) c.free(c.st->resolve(rd(rec+0x3c)),kScmd,0x8b);
    char* p=static_cast<char*>(c.alloc((uint32_t)std::strlen(str)+1,kScmd,0x8c));
    std::strcpy(p,str); wr(rec+0x3c,c.st->encode(p));
    if(uint8_t* d=at(c,rec,0x2c)) { uint32_t n=rd(rec+0x30); std::strncpy((char*)d,str,n); if(n) ((char*)d)[n-1]=0; }
}
// 1.13c Storm+0x253c0
bool applyArg(Ctx& c,uint8_t* rec,const char* str,int32_t* consumed) {
    *consumed=0; uint32_t mode=rd(rec+8)&0x30000;
    if(mode==0) {
      setBool(rec,str,consumed);
      // bool mirrors into the dest word (+2C) when present
      if(uint8_t* d=at(c,rec,0x2c)) {
        uint32_t w=rd(d)&~rd(rec+0x28); bool on=(rd(rec+0x3c)&rd(rec+0x24))!=0;
        wr(d,on?(w|rd(rec+0x24)):w);
      }
    } else if(mode==0x10000) setNumber(c,rec,str,consumed);
    else if(mode==0x20000) setString(c,rec,str,consumed);
    else return false;
    wr(rec+0x38,1);
    if(uint32_t cb=rd(rec+0x34)) {
      uint32_t a[7]={rd(rec+8),rd(rec+0xc),c.st->encode(rec+0x10),rd(rec+0x24),rd(rec+0x28),rd(rec+0x2c),rd(rec+0x3c)};
      // args order in retail: flags,id,name,value,mask,dest,current (local_1c..local_4 order is flags,id,name,limit,..)
      if(!c.callCb(cb,a,7)) return false;
    }
    for(int g=0;g<2;++g) {
      uint8_t* list=static_cast<uint8_t*>(c.st->commandLists[g==0?1:0]);
      for(uint32_t n=list?rd(list+8):0;(int32_t)n>0;) {
        uint8_t* p=c.st->resolve(n); n=rd(p+4);
        if(rd(p+0xc)!=rd(rec+0xc)) continue;
        uint32_t m=rd(p+8)&0x30000; if(m!=(rd(rec+8)&0x30000)||p==rec) continue;
        wr(p+0x38,1);
        if(m==0x20000) {
          if(rd(p+0x3c)) c.free(c.st->resolve(rd(p+0x3c)),kScmd,0x120);
          const char* s=reinterpret_cast<const char*>(c.st->resolve(rd(rec+0x3c)));
          char* q=static_cast<char*>(c.alloc((uint32_t)std::strlen(s)+1,kScmd,0x121));
          std::strcpy(q,s); wr(p+0x3c,c.st->encode(q));
        } else wr(p+0x3c,rd(rec+0x3c));
      }
    }
    return true;
}
// 1.13c Storm+0x24df0
uint8_t* findLongest(Ctx& c,uint8_t* head,int32_t minLen,const char* str) {
    int32_t bound=(int32_t)std::strlen(str), best=minLen-1; uint8_t* r=nullptr;
    for(uint8_t* p=head;p;) {
      int32_t len=(int32_t)rd(p+0x20);
      if(best<len && len<=bound) {
        int d=(rd(p+8)&0x100)?std::strncmp((char*)p+0x10,str,len):strncasecmp((char*)p+0x10,str,len);
        if(!d) { best=len; r=p; }
      }
      uint32_t n=rd(p+4); if((int32_t)n<=0) break; p=c.st->resolve(n);
    }
    return r;
}
// 1.13c Storm+0x25520: st[0]=current record addr, st[5]=min length
bool applyChain(Ctx& c,const char* str,int32_t* st,int32_t* maxConsumed) {
    *maxConsumed=0; uint32_t cur=(uint32_t)st[0]; st[0]=0;
    while(cur) {
      int32_t used; if(!applyArg(c,c.st->resolve(cur),str,&used)) return false;
      if(used>*maxConsumed) *maxConsumed=used;
      uint8_t* nx=findLongest(c,c.searchHead,st[5],str); cur=c.st->encode(nx);
    }
    return true;
}
// 1.13c Storm+0x255a0: st[0]=record addr, st[1..4]=name, st[5]=name length
bool parseOption(Ctx& c,const char* str,int32_t* st,void* errCb) {
    char local[256]; local[0]=0;
    while(*str) {
      size_t sl=std::strlen(str), ll=std::strlen(local);
      size_t k=ll?ll:1; uint8_t* rec=nullptr; int32_t len=-1;
      for(;;) {
        do { if(!k) goto fail; --k; } while(sl+k>0xff);
        std::strncpy(local+k,str,0x100-k);
        rec=findLongest(c,c.searchHead,0,local); if(rec) break;
      }
      len=(int32_t)rd(rec+0x20); local[len]=0;
      fail:
      if(!rec) { if(errCb) c.reportError(errCb,c.lastError()); return false; }
      const char* p=str+len;
      while(*p&&std::strchr(c.separators,*p)) ++p;
      st[0]=(int32_t)c.st->encode(rec); st[5]=len;
      std::strncpy((char*)(st+1),local,16);
      if(!*p && (rd(rec+8)&0x30000)) return true;
      int32_t used; if(!applyChain(c,p,st,&used)) return false;
      str=p+used;
    }
    return true;
}
// 1.13c Storm+0x119f0 (registers: EAX = out2, EBX = cb, EDI = buf; stack: p1, p2, p3; ret 0xC): query the provider callback (stdcall, 8 words) for an
// image: call 1 (p1, p2, 0, 0, 0, &B, &A, &C0) fills B, A and C0; out2 = {B, A}; the buffer is (B*A*C0 / 8) bytes, rounded toward zero (SMemAlloc
// SNET.CPP:0xB56); call 2 (p1, p2, &C1, buf, size, &B, &A, &C0) fills it. p3 != 0 loads the reply's words at C+8 as palette entry 0xFE
// (0x19520 (1, 0xFE, &C2, 0)). A failed call 2 frees the buffer (SNET.CPP:0xB62). Any failure returns 0; a null cb leaves out2 and *buf zero.
bool netQuery(Ctx& c,uint32_t cb,uint32_t p1,uint32_t p2,uint32_t p3,uint32_t* out2,void** buf) {
    *buf=nullptr; out2[0]=out2[1]=0;
    if(!cb) return false;
    uint32_t A=0,B=0,C[0x100]={};
    auto enc=[&](const void* p){ return c.st->encode(p); };
    const uint32_t a[8]={p1,p2,0,0,0,enc(&B),enc(&A),enc(&C[0])};
    if(!c.callCb(cb,a,8)) return false;
    out2[0]=B; out2[1]=A;
    int32_t prod=(int32_t)(B*A*C[0]);
    const uint32_t size=(uint32_t)((prod+((prod>>31)&7))>>3);
    *buf=c.alloc(size,kSnet,0xb56);
    const uint32_t b[8]={p1,p2,enc(&C[1]),enc(*buf),size,enc(&B),enc(&A),enc(&C[0])};
    if(c.callCb(cb,b,8)) {
      if(p3) c.setPalette(1,0xfe,&C[2],0);
      return true;
    }
    c.free(*buf,kSnet,0xb62); *buf=nullptr; return false;
}
// 1.13c Storm+0x11c20
void postEvent(Ctx& c,uint32_t a,uint32_t b,const void* data,uint32_t len) {
    uint8_t* n=static_cast<uint8_t*>(c.newEvent());
    wr(n+8,a); wr(n+12,b);
    if(data&&len) {
      void* d=c.alloc(len,kSnet,0x8f3); std::memcpy(d,data,len);
      wr(n+16,c.st->encode(d)); wr(n+20,len);
    }
}
// 1.13c Storm+0x1b3a0 (256 critical sections == State::locks, already constructed)
void initMemory(Ctx& c) {
    if(c.st->heapReady) return;
    c.regLoad("Internal","Debug Memory",0,&c.debugMemory);
    c.regLoad("Internal","Protect Memory",0,&c.protectMemory);
    c.st->heapReady=1;
}
}
