// Prior art used: D2MOO Storm.h ordinal names/signatures; Storm.cpp is import stubs.
// StormLib SBaseCommon.cpp hash was cross-checked: its MPQ table is a different algorithm.
// Retail 1.13c cached bodies are authoritative. Pointer-bearing retail records remain bytes;
// resolve/encode translate their 32-bit addresses without truncating native wasm64 pointers.
#include "storm_batch_25220.h"
#include <cstring>

namespace storm25220 {
static uint32_t rd(const void* p) { uint32_t v; std::memcpy(&v,p,4); return v; }
static void wr(void* p,uint32_t v) { std::memcpy(p,&v,4); }
static void error(State& s,uint32_t e) { s.error=e; s.setLastError(e); }
static bool ready(State& s,const char* name) {
    if(s.heapReady) return true;
    error(s,0x8510007d); if(s.reportErrors) s.fatal(0x8510007d,name,-1,nullptr,1,1);
    return false;
}
// 1.13c Storm+0x3AFD0
uint32_t length(State& s,const char* text) {
    if(!text) { error(s,0x57); return 0; }
    uint32_t n=0; while(text[n]) ++n; return n;
}
// 1.13c Storm+0x3B080
uint32_t copy(State& s,char* dst,const char* src,uint32_t capacity) {
    if(!dst || !src) { error(s,0x57); return 0; }
    if(!capacity) return 0;
    uint32_t n=0; while(n+1<capacity && src[n]) { dst[n]=src[n]; ++n; }
    dst[n]=0; return n;
}
// 1.13c Storm+0x3AE00
uint32_t append(State& s,char* dst,const char* src,uint32_t capacity) {
    if(!dst || !src) { error(s,0x57); return 0; }
    if(!capacity) return 0;
    if(capacity!=0x7fffffff) dst[capacity-1]=0;
    uint32_t n=length(s,dst); return n+copy(s,dst+n,src,capacity-n);
}
// 1.13c Storm+0x22C50
void fill(void* dst,uint32_t count,uint8_t value) { std::memset(dst,value,count); }
// 1.13c Storm+0x3B230
int32_t compare(State& s,const char* a,const char* b,uint32_t count) {
    if(!a || !b) { error(s,0x57); return static_cast<int32_t>(s.encode(a)-s.encode(b)); }
    // Retail MSVCRT _strncmp returns exactly -1, 0 or 1.
    const int result=std::strncmp(a,b,count);
    return (result>0)-(result<0);
}
// 1.13c Storm+0x3AC10
uint32_t hash(State& s,const uint8_t* text,uint32_t flags,uint32_t seed) {
    if(!text) { error(s,0x57); return 0; }
    constexpr uint32_t table[]={0x486e26eeu,0xdcaa16b3u,0xe1918eefu,0x202dafdbu,
      0x341c7dc7u,0x1c365303u,0x40ef2d37u,0x65fd5e49u,0xd6057177u,0x904ece93u,
      0x1c38024fu,0x98fd323bu,0xe3061ae7u,0xa39b0fa1u,0x9797f25fu,0xe4444563u};
    if(!seed) seed=0x7fed7fed;
    uint32_t second=0xeeeeeeee;
    while(uint32_t c=*text++) {
      if(!(flags&1)) { if(c>='a' && c<='z') c-=32; if(c=='/') c='\\'; }
      seed=(seed+second)^(table[c>>4]-table[c&15]); second=second*33+c+3+seed;
    }
    return seed?seed:1;
}
// 1.13c Storm+0x250C0
uint32_t registerArgument(State& s,uint32_t flags,uint32_t id,const char* name,
                         uint32_t callback,uint32_t kind,uint32_t value,uint32_t limit,uint32_t extra) {
    if(!name) name="";
    uint32_t n=length(s,name), group=flags&0x3000000;
    if(n>15 || (kind && !callback) || (group==0x2000000 && s.requiredRegistered) ||
       (!group && !n) || (!(flags&0x30000) && callback && kind!=4)) {
      error(s,0x57); return 0;
    }
    auto node=s.allocateNode(s.commandLists[group?1:0],0,0);
    copy(s,reinterpret_cast<char*>(node+16),name,16);
    wr(node+12,id); wr(node+44,callback); wr(node+48,kind); wr(node+40,limit);
    wr(node+32,n); wr(node+8,flags); wr(node+36,value); wr(node+52,extra);
    wr(node+60,(!(flags&0x30000) && (flags&1))?value:0);
    if(group==0x1000000) s.requiredRegistered=1;
    return 1;
}
// 1.13c Storm+0x25220
uint32_t registerList(State& s,const uint32_t* entries,uint32_t count) {
    if(!entries) { error(s,0x57); return 0; }
    for(uint32_t i=0;i<count;++i,entries+=4)
      if(!registerArgument(s,entries[0],entries[1],reinterpret_cast<char*>(s.resolve(entries[2])),
                           0,0,1,0xffffffff,entries[3])) return 0;
    return 1;
}
// 1.13c Storm+0x1ADF0
uint32_t heapByCaller(State& s) {
    return ready(s,"SMemGetHeapByCaller()")?s.heapByCaller():0;
}
// 1.13c Storm+0x1AE50
uint32_t findNextHeap(State& s,uint32_t previous,uint32_t* next,uint32_t* info) {
    if(!ready(s,"SMemFindNextHeap()")) return 0;
    if(!next || !info || info[0]!=0x128) { error(s,0x57); return 0; }
    std::memset(info+1,0,0x124);
    uint32_t last=0;
    for(uint32_t bucket=previous?previous&255:0;bucket<256;++bucket) {
      std::lock_guard lock(s.locks[bucket]);
      for(uint32_t address=s.heaps[bucket];address;) {
        auto p=s.resolve(address);
        if(rd(p+16)) {
          uint32_t id=rd(p+4);
          if(last==previous && id!=previous) {
            *next=id; info[1]=id; info[0x43]=rd(p+0x5c); info[0x47]=0x7fffffff;
            copy(s,reinterpret_cast<char*>(info+2),reinterpret_cast<char*>(p+0x60),0x104);
            do {
              if(rd(p+4)==id) {
                info[0x45]+=rd(p+44)+rd(p+52); info[0x46]+=rd(p+48)+rd(p+52);
                info[0x48]+=rd(p+20); info[0x49]+=rd(p+24);
              }
              address=rd(p); if(address) p=s.resolve(address);
            } while(address);
            return 1;
          }
          last=id;
        }
        address=rd(p);
      }
    }
    *next=0; return 0;
}
// 1.13c Storm+0x1BAE0
uint32_t findNextBlock(State& s,uint32_t heap,uint8_t* previous,uint32_t* next,uint32_t* info) {
    if(!ready(s,"SMemFindNextBlock()")) return 0;
    if(!heap || !next || !info || info[0]!=0x1c) { error(s,0x57); return 0; }
    std::memset(info+1,0,24);
    std::lock_guard lock(s.locks[heap&255]);
    uint8_t* target=nullptr;
    if(previous) target=(previous[-5]&8)?s.resolve(rd(previous-12)):previous-8;
    uint32_t head=s.heaps[heap&255],tail=head;
    if(!head) { *next=0; return 0; }
    while(rd(s.resolve(tail))) tail=rd(s.resolve(tail));
    uint8_t* prior=nullptr;
    for(uint32_t node=tail;;) {
      auto p=s.resolve(node);
      if(rd(p+4)==heap) {
        for(uint32_t block=rd(p+28);block!=rd(p+32);) {
          auto b=s.resolve(block);
          if(prior==target) {
            auto user=(b[3]&4)?s.resolve(rd(b+8)):b+8;
            *next=s.encode(user); info[1]=*next; info[2]=~(b[3]>>1)&1;
            uint16_t magic=0; if(user) std::memcpy(&magic,user-2,2);
            info[3]=(magic==0x6f6d)?((uint8_t)~user[-5]>>1)&1:0;
            s.blockDetails(info+5,user); return 1;
          }
          prior=b; uint16_t size; std::memcpy(&size,b,2); block+=size;
        }
      }
      if(node==head) break;
      uint32_t before=head; while(rd(s.resolve(before))!=node) before=rd(s.resolve(before));
      node=before;
    }
    *next=0; return 0;
}
// 1.13c Storm+0x3B3A0
void tokenize(State& s,const char** input,char* out,uint32_t capacity,const char* delims,uint32_t* quoted) {
    if(!input || !*input || !delims || (!out && capacity)) { error(s,0x57); return; }
    bool quotes=std::strchr(delims,'"')!=nullptr, inQuote=false;
    uint32_t sawQuote=0,n=0; const char* p=*input;
    while(*p && std::strchr(delims,*p)) {
      if(quotes && *p=='"') { inQuote=true; sawQuote=1; ++p; break; }
      ++p;
    }
    while(*p) {
      char c=*p;
      if(quotes && c=='"') {
        if(n && !inQuote) break;
        inQuote=!inQuote; sawQuote=1; ++p; if(!inQuote) break;
      } else {
        if(!inQuote && std::strchr(delims,c)) { ++p; break; }
        if(n+1<capacity) out[n++]=c;
        ++p;
      }
    }
    if(n<capacity) out[n]=0;
    *input=p; if(quoted) *quoted=sawQuote;
}
} // namespace storm25220
