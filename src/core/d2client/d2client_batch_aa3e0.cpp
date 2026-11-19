// D2Client+0xABDC0 growQueue, the one body left from the AA3E0..AC530 batch (the other functions live in their
// own d2client_<rva>.cpp files). Decompile-derived; imported DLL calls and process globals go through Env.
#include <cstdint>
#include <cstring>
namespace d2client_batch_aa3e0 {
struct Env {
 virtual ~Env()=default;
 virtual uint8_t* ga(uint32_t rva)=0;                       // address of D2Client global at RVA
 virtual uintptr_t alloc(int32_t line,uint32_t size)=0;     // Ordinal_10042(line,0)
 virtual void freeP(uintptr_t p,int32_t line)=0;            // Ordinal_10043(line,0)
 virtual void fatal(int32_t msgId)=0;                       // Ordinal_10265 + Ordinal_10024 + _exit(-1)
};
static uint32_t G32(Env& e,uint32_t rva){ uint32_t v; std::memcpy(&v,e.ga(rva),4); return v; }
static void S32(Env& e,uint32_t rva,uint32_t v){ std::memcpy(e.ga(rva),&v,4); }
static uintptr_t rdp(uintptr_t a){ uintptr_t v; std::memcpy(&v,(const void*)a,sizeof v); return v; }
static void wrp(uintptr_t a,uintptr_t v){ std::memcpy((void*)a,&v,sizeof v); }
static uint32_t r32(uintptr_t a){ uint32_t v; std::memcpy(&v,(const void*)a,4); return v; }

enum : uint32_t { R_RECSZ=0x11BBD8, R_TBL=0xDDE64, R_TBLEND=0xDE698 };

// Unit packet queue header: {cap,count,?,buf} at unit+0xd8.
struct Queue { int32_t cap,count,x; uintptr_t buf; };

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
}
