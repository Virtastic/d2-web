// Prior art used: ; no D2Client module in D2MOO. Imported-DLL calls go through Hooks.
#include <cstdint>
#include <cstring>
namespace d2client_3e7f0 {
struct Hooks {
 virtual ~Hooks()=default;
 virtual void* first(int32_t list)=0;            // Ordinal_10947
 virtual void* next(void* it)=0;                 // Ordinal_10273
 virtual int16_t* data(void* it)=0;              // Ordinal_10858 (ECX=item)
 virtual uint32_t flags(void* it)=0;             // Ordinal_10397
 virtual int32_t q10537(void* it,uint32_t* out10c,uint32_t* out114)=0;
 virtual int32_t count(void* it)=0;              // Ordinal_10353
 virtual int32_t q10787(int32_t list,void* it)=0;
 virtual int32_t q10306(int32_t list,void* it)=0;// called with 3rd arg 1
 virtual void sub_3E490(int16_t* d)=0;           // 0x3E490
 virtual int32_t sub_3CFA0(int32_t y)=0;         // 0x3CFA0
 virtual void sub_3E770(int16_t* d,int32_t a,int32_t b,uint32_t c)=0;
 virtual void sub_3D8A0(int16_t* d,int32_t a,int32_t cnt,int32_t x,int32_t y)=0;
};
// D2Client globals the body reads (pinned by the caller)
struct G {
 int32_t bc4c{}, listCbbfc{}, c97f8{}, c985c{};
 uint32_t bit10{}, bit18{};      // gdwBitMasks[0x10], [0x18] low bytes
 int32_t dtBase{};               // not dereferenced here: tables via rows()
 int32_t d9d74{}, d9d78{}, d9d7c{}, d9d80{};  // written outputs
};
struct Rows { // sgptDataTables lookups; null when index out of range
 virtual const uint8_t* row(int32_t idx)=0;      // +0xB98 stride 0x23C, count at +0xBA0
 virtual const uint8_t* sub(int32_t idx)=0;      // +0xB8C stride 0x120, count at +0xB94
};
// 1.13c D2Client+0x3E7F0
void sub_3E7F0(G& g,Hooks& h,Rows& r,int32_t p1,int32_t p2,int32_t p3,int32_t p4,int32_t p5){
 int32_t y=g.bc4c-0x56, step=(p3?0x60:0)-0x30, last=0; uint8_t seen[256]; std::memset(seen,0,sizeof seen);
 for(int cls=0;cls<=4;cls++){
  for(void* it=h.first(p1); it; it=h.next(it)){ // ponytail: next() read from Ordinal_10273 return; decompile aliases it with local_108
   int16_t* d=h.data(it); if(!d) continue;
   int32_t ix=d[0]; if(ix<0) continue;
   const uint8_t* t=r.row(ix); if(!t) continue;
   int32_t sx=(int16_t)(t[0x194]|t[0x195]<<8); if(sx<0) continue;
   const uint8_t* s=r.sub(sx); if(!s) continue;
   if(p3 && !(g.bit18 & ((const uint8_t*)d)[7])) continue;
   if(g.bit10 & ((const uint8_t*)d)[4]) continue;
   if(h.flags(it)&8) continue;
   uint32_t o10c=0xffffffff,o114=0; int32_t k=h.q10537(it,&o10c,&o114); int32_t cnt=0;
   if(!p3 && !k){ cnt=h.count(it); if(cnt>0 && (g.bit10 & ((const uint8_t*)d)[8])) k=1; }
   if(cls<4){ if(k||cls!=(int8_t)s[5]) continue; } else if(!k) continue;
   int32_t q=h.q10787(p1,it), mode;
   if(!q) mode=(p1==g.listCbbfc && g.c97f8<g.c985c)?1:0; else mode=(q!=6)?1:4;
   if(last<cls){ y-=0x30; last=cls; }
   uint8_t u=s[6]; if(u){ if(seen[u]) continue; seen[u]=1; }
   if(!p4){ if(!p5) h.sub_3E490(d); p2=h.sub_3CFA0(y); }  // 3CFA0 result lands in the p2 slot (iVar9)
   else { if(!cnt) h.sub_3E770(d,mode,h.q10306(p1,it),o114); else h.sub_3D8A0(d,mode,cnt,p2,y); p2+=step; }
   if(cls==1)g.d9d74=y; else if(cls==2)g.d9d78=y; else if(cls==3)g.d9d7c=y; else if(cls==4)g.d9d80=y;
  }
 }
}
}
