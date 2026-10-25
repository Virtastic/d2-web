// Prior art used: D2MOO Storm exports (declarations only), StormLib replaces MPQ I/O.
// Bodies follow cached 1.13c decompiles, checked against x86 register arguments.
// Native pointers replace live x86 pointers; counts and limb arithmetic remain uint32_t.
#include <cstdint>
#include <cstring>
#include <algorithm>

#include "storm_big_types.h"
struct Storm2fEvent { Storm2fEvent* next; Storm2fEvent* prev; uint32_t a,b; void* payload; };
// Caller-owned state and actual callee seams: no private duplicate retail globals.
struct StormBatch2f0e0Env {
    int32_t (*f_2ebb0)(Storm2fVector*, Storm2fScratch*); // EAX scratch
    void (*f_2d830)(Storm2fVector*,Storm2fVector*,Storm2fVector*,Storm2fVector*,Storm2fScratch*);
    void (*f_2c840)(Storm2fVector*,Storm2fVector*,Storm2fVector*,Storm2fScratch*);
    void* (*allocate)(uint32_t,const char*,uint32_t,uint32_t);
    Storm2fBig* (*construct)(void*);
    void (*f_2e930)(Storm2fVector*,Storm2fVector*,Storm2fVector*,Storm2fVector*,Storm2fScratch*); // EAX fourth, EBX scratch
    void (*f_2c210)(Storm2fVector*,uint32_t,uint32_t); // EAX third
    void (*f_2ba20)(Storm2fVector*,uint32_t,uint32_t); // EAX first
    void (*f_2b020)(Storm2fBytes*,Storm2fVector*);
    Storm2fEvent** event_head; // retail +0x45A20, signed sentinel preserved
    void (*free_memory)(void*,const char*,int32_t);
    void (*unlink_event)(Storm2fEvent*);
};
// 1.13c Storm+0x2F0E0
int32_t Storm_2F0E0(Storm2fBig* p, StormBatch2f0e0Env& e) {
    return e.f_2ebb0(&p->value,&p->scratch);
}
// 1.13c Storm+0x2DD70
void Storm_2DD70(Storm2fBig* p,Storm2fBig* a,Storm2fBig* b,StormBatch2f0e0Env& e) {
    uint32_t n=p->scratch.used++;
    e.f_2d830(&p->scratch.slots[n],&p->value,&a->value,&b->value,&p->scratch);
    --p->scratch.used;
}
// 1.13c Storm+0x2D400
void Storm_2D400(Storm2fBig* p,Storm2fBig* a,Storm2fBig* b,StormBatch2f0e0Env& e) {
    e.f_2c840(&p->value,&a->value,&b->value,&p->scratch);
}
// 1.13c Storm+0x2AFA0
void Storm_2AFA0(Storm2fBig** out,StormBatch2f0e0Env& e) {
    // Retail 0x168-byte object expands only for native pointers in its vectors.
    void* p=e.allocate(static_cast<uint32_t>(sizeof(Storm2fBig)),"..\\3rdParty\\STORM\\SOURCE\\SBig.CPP",0x521,0);
    *out=p?e.construct(p):nullptr;
}
// 1.13c Storm+0x2F0C0
void Storm_2F0C0(Storm2fBig* p,Storm2fBig* a,Storm2fBig* b,Storm2fBig* c,StormBatch2f0e0Env& e) {
    e.f_2e930(&p->value,&a->value,&b->value,&c->value,&p->scratch);
}
// 1.13c Storm+0x2D370
void Storm_2D370(Storm2fBig* p,uint32_t a,uint32_t b,StormBatch2f0e0Env& e) {
    e.f_2c210(&p->value,a,b);
}
// 1.13c Storm+0x2D310
void Storm_2D310(Storm2fBig* p,uint32_t a,uint32_t b,StormBatch2f0e0Env& e) {
    e.f_2ba20(&p->value,a,b);
}
// 1.13c Storm+0x2B240
void Storm_2B240(Storm2fBig* p,void* out,uint32_t capacity,uint32_t* copied,StormBatch2f0e0Env& e) {
    p->bytes.count=0;
    e.f_2b020(&p->bytes,&p->value);
    uint32_t n=std::min(capacity,p->bytes.count);
    // Retail copies forward in 32-bit chunks, then bytes (including overlap).
    auto* dst=static_cast<uint8_t*>(out);
    uint32_t offset=0;
    for(;offset<n-(n&3);offset+=4) {
        uint32_t word;
        std::memcpy(&word,p->bytes.data+offset,4);
        std::memcpy(dst+offset,&word,4);
    }
    for(;offset<n;++offset) dst[offset]=p->bytes.data[offset];
    if(copied) *copied=n;
}
// 1.13c Storm+0x357A0
int32_t Storm_357A0(uint8_t* out,int32_t capacity,const uint16_t* in,int32_t length,int32_t* written,int32_t* consumed) {
    uint32_t i=0,o=0; int32_t result;
    constexpr uint8_t prefix[]={0,0,0xc0,0xe0,0xf0,0xf8,0xfc};
    for(;;) {
        if(length!=0x7fffffff && i>=static_cast<uint32_t>(length)) {result=-1;break;}
        uint32_t u=in[i],step=1;
        if(u>0xd7ff && u<0xdc00) {
            if(length!=0x7fffffff && i+1>=static_cast<uint32_t>(length)) {result=-1;break;}
            uint32_t lo=in[i+1];
            // Retail's unusual 0xd7f7 bias is intentional, not Unicode normalization.
            if(lo>0xdbff && lo<0xe000) {u=(u-0xd7f7)*0x400+lo;step=2;}
        }
        if(u==0) {
            result=0;
            if(static_cast<int64_t>(o)<capacity) out[o++]=0; else result=1;
            break;
        }
        if(u>0x7fffffff) u=0xfffd; // retail replacement char (unreachable from UTF-16 input)
        uint32_t n=u<0x80?1:u<0x800?2:u<0x10000?3:u<0x200000?4:u<0x4000000?5:6;
        if(static_cast<int64_t>(o)+n>capacity) {result=static_cast<int32_t>(n);break;}
        for(uint32_t j=n-1;j>0;--j) {out[o+j]=static_cast<uint8_t>((u&63)|128);u>>=6;}
        out[o]=prefix[n]|static_cast<uint8_t>(u);
        o+=n;i+=step;
    }
    if(consumed)*consumed=static_cast<int32_t>(i);
    if(written)*written=static_cast<int32_t>(o);
    return result;
}
// 1.13c Storm+0x35510
int32_t Storm_35510(uint16_t* out,int32_t capacity,const uint8_t* in,int32_t length,int32_t* written,int32_t* consumed) {
    constexpr uint32_t bias[]={0,0x3080,0xe2080,0x3c82080,0xfa082080,0x82082080};
    uint32_t i=0,o=0;int32_t result;
    for(;;) {
        if(length!=0x7fffffff && i>=static_cast<uint32_t>(length)){result=-1;break;}
        uint8_t c=in[i];
        uint32_t extra=c<0xc0?0:c<0xe0?1:c<0xf0?2:c<0xf8?3:c<0xfc?4:5;
        if(length!=0x7fffffff && i+extra>=static_cast<uint32_t>(length)){result=-1-static_cast<int32_t>(extra);break;}
        uint32_t u=0;
        for(uint32_t j=0;j<=extra;++j)u=(u<<6)+in[i+j];
        u-=bias[extra];
        if(static_cast<int64_t>(o)>=capacity){result=1;break;}
        if(u<0x10000) {
            out[o++]=static_cast<uint16_t>(u);
            if(u==0){result=0;break;}
        } else if(u<0x110000) {
            if(static_cast<int64_t>(o)+1>=capacity){result=1;break;}
            out[o++]=static_cast<uint16_t>(((u-0x10000)>>10)+0xd800);
            out[o++]=static_cast<uint16_t>(((u-0x10000)&0x3ff)+0xdc00);
        } else out[o++]=0xfffd;
        i+=extra+1;
    }
    if(consumed)*consumed=static_cast<int32_t>(i);
    if(written)*written=static_cast<int32_t>(o);
    return result;
}
// 1.13c Storm+0x35B50
int32_t Storm_35B50(uint16_t* out,const uint8_t* in,int32_t capacity) {
    constexpr uint16_t high[]={0x20ac,0xffff,0x201a,0x192,0x201e,0x2026,0x2020,0x2021,
        0x2c6,0x2030,0x160,0x2039,0x152,0xffff,0x17d,0xffff,
        0xffff,0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,
        0x2dc,0x2122,0x161,0x203a,0x153,0xffff,0x17e,0x178};
    if(!capacity)return 0;
    uint32_t n=0;
    while(*in) {
        if(!capacity)return static_cast<int32_t>(n);
        uint8_t c=*in++;
        out[n++]=c>=0x80&&c<0xa0?high[c-0x80]:c;
        capacity=static_cast<int32_t>(static_cast<uint32_t>(capacity)-1);
    }
    if(capacity)out[n++]=0;
    return static_cast<int32_t>(n);
}
// 1.13c Storm+0x112E0
void Storm_112E0(StormBatch2f0e0Env& e) {
    // Retail positive signed-pointer test excludes null and tagged negative sentinels.
    while(reinterpret_cast<intptr_t>(*e.event_head)>0) {
        Storm2fEvent* p=*e.event_head;
        if(p->payload)e.free_memory(p->payload,"..\\3rdParty\\STORM\\SOURCE\\SNET.CPP",0x723);
        e.unlink_event(p);
        e.free_memory(p,".?AUUSEREVENT@@",-2);
    }
}
// 1.13c Storm+0x39A0 (SBig ctor; inlines 0x37C0: 16 zeroed vectors, used=0)
Storm2fBig* Storm_39A0(Storm2fBig* p) {
    *p = Storm2fBig{};
    return p;
}
