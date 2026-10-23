// Prior art used: D2MOO Storm export definitions (names/ordinals); StormLib compression
// pipeline as cross-check. Bodies follow cached 1.13c decompiles and x86 disassembly.
// Stage A: host seams carry real effects, native pointers never enter retail uint32 fields.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdarg>

struct Storm34d60Log {
    uint32_t id;
    Storm34d60Log* next;
    void* file;
    uint32_t used, pending, counter, field18;
    char* buffer; // native projection of the inline retail +0x1c buffer
};
struct Storm34d60Env {
    Storm34d60Log* logs[4];
    uint32_t debug, last_error;
    void (*error)(uint32_t);
    void (*enter)(uint32_t);
    void (*leave)(uint32_t);
    void (*prepare)(Storm34d60Log*, uint32_t);
    int32_t (*vsprintf_)(char*, const char*, va_list);
    int32_t (*vsnprintf_)(char*, uint32_t, const char*, va_list);
    void (*debug_string)(const char*);
    void (*write)(void*, const void*, uint32_t, uint32_t*);
    void (*flush)(void*);
    void* (*alloc)(uint32_t, const char*, int32_t, uint32_t);
    void (*free_)(void*, const char*, int32_t, uint32_t);
    // ECX=destination, EDX=&output size; source and length are stack arguments.
    void (*decompress[4])(void*, uint32_t*, const void*, uint32_t);
    uint32_t (*hash)(const void*, uint32_t, uint32_t); // ECX=data, stack=len, EAX=seed
    void (*destroy)(void*);
    void (*insert)(void*, const void*, uint32_t); // EAX=object
    void (*append)(void*, uint32_t); // EAX=object
    void (*splice)(void*, const void*, const void*); // EAX=object
    void (*combine)(void*, void*, void*, void*, void*);
};
namespace {
void invalid(Storm34d60Env& e) { e.last_error=0x57; e.error(0x57); }
// REP MOVSD then MOVSB is a forward copy, including overlapping buffers.
void forward_copy(uint8_t* d, const uint8_t* s, uint32_t n) {
    while(n>=4) { uint32_t v; std::memcpy(&v,s,4); std::memcpy(d,&v,4); s+=4; d+=4; n-=4; }
    while(n--) *d++=*s++;
}
bool overlaps(const void* a,const void* b,uint32_t n) {
    auto x=reinterpret_cast<uintptr_t>(a), y=reinterpret_cast<uintptr_t>(b);
    return x<y+n && y<x+n;
}
}
// 1.13c Storm+0x34D60
void Storm_Sub34D60(Storm34d60Env& e,uint32_t id,const char* fmt,...) {
    if(!id) return;
    uint32_t bucket=id&3; e.enter(bucket);
    auto p=e.logs[bucket];
    while(p && p->id!=id) p=p->next;
    if(p) {
        e.prepare(p,1);
        va_list ap; va_start(ap,fmt); e.vsprintf_(p->buffer+p->used,fmt,ap); va_end(ap);
        p->used+=static_cast<uint32_t>(std::strlen(p->buffer+p->used));
        p->buffer[p->used]='\r'; p->buffer[p->used+1]='\n'; p->buffer[p->used+2]=0;
        p->used+=2;
        if(e.debug) e.debug_string(p->buffer+p->pending);
        p->pending=p->used;
        if(p->used>0xbfff) {
            e.write(p->file,p->buffer,p->used,&id); e.flush(p->file);
            p->used=0; p->pending=0;
        }
    }
    e.leave(bucket);
}
// 1.13c Storm+0x34900
uint32_t Storm_Sub34900(Storm34d60Env& e,uint32_t id,int32_t delta) {
    if(!id) return 0;
    uint32_t bucket=id&3; e.enter(bucket);
    auto p=e.logs[bucket]; while(p && p->id!=id) p=p->next;
    uint32_t old=0;
    if(p) {old=p->counter; p->counter+=static_cast<uint32_t>(delta);}
    e.leave(bucket); return old;
}
// 1.13c Storm+0x22700
uint32_t Storm_Sub22700(Storm34d60Env& e,void* output,uint32_t* out_size,
                       const void* input,uint32_t length) {
    const uint32_t capacity=*out_size; // retail dereferences before argument validation
    if(!output || !input || !length || capacity<length) {invalid(e); return 0;}
    auto dst=static_cast<uint8_t*>(output);
    auto src=static_cast<const uint8_t*>(input);
    if(length==capacity) { if(dst!=src) forward_copy(dst,src,length); return 1; }
    uint32_t mask=*src++, n=length-1;
    if(mask&0x36) return 0;
    uint32_t remaining=((mask&8)!=0)+((mask&1)!=0)+((mask&128)!=0)+((mask&64)!=0);
    uint8_t* tmp=nullptr;
    if(remaining>1 || (remaining && overlaps(src,dst,n)))
        tmp=static_cast<uint8_t*>(e.alloc(capacity,"..\\3rdParty\\STORM\\SOURCE\\SCOMP.CPP",0x549,0));
    constexpr uint32_t bits[]={8,1,128,64};
    for(uint32_t i=0;i<4;++i) {
        if(!(mask&bits[i])) continue;
        --remaining;
        auto target=(remaining&1)?tmp:dst;
        if(overlaps(src,target,n)) {
            if(overlaps(target,tmp,n)) target=dst;
            else target=tmp;
        }
        uint32_t produced=capacity;
        e.decompress[i](target,&produced,src,n);
        src=target; n=produced;
    }
    if(src!=dst) forward_copy(dst,src,n);
    *out_size=n;
    if(tmp) e.free_(tmp,"..\\3rdParty\\STORM\\SOURCE\\SCOMP.CPP",0x568,0);
    return 1;
}
// 1.13c Storm+0x3B2D0
char* Storm_Sub3B2D0(Storm34d60Env& e,char* s,char value) {
    if(!s) {invalid(e);return nullptr;}
    for(;*s;++s) if(*s==value) return s;
    return nullptr;
}
// 1.13c Storm+0x3B280
char* Storm_Sub3B280(Storm34d60Env& e,char* s,char value) {
    if(!s) {invalid(e);return nullptr;}
    char* found=nullptr; for(;*s;++s) if(*s==value) found=s;
    return found;
}
// 1.13c Storm+0x3BA80
uint32_t Storm_Sub3BA80(Storm34d60Env& e,char* dst,uint32_t cap,const char* fmt,...) {
    if(!dst || !fmt) {invalid(e);return 0;}
    if(!cap) return 0;
    va_list ap;va_start(ap,fmt);
    uint32_t n=static_cast<uint32_t>(cap==0x7fffffff?e.vsprintf_(dst,fmt,ap):e.vsnprintf_(dst,cap,fmt,ap));
    va_end(ap);
    if(cap!=0x7fffffff && n>=cap) {dst[cap-1]=0; return cap-1;}
    return n;
}
// 1.13c Storm+0x3B330
uint32_t Storm_Sub3B330(Storm34d60Env& e,const uint8_t* s) {
    uint8_t normalized[1024];uint32_t n=0;
    for(;*s && n<1023;++s,++n) {
        auto c=*s; normalized[n]=(c>='a' && c<='z')?c-32:(c=='/'?'\\':c);
    }
    normalized[n]=0;return e.hash(normalized,n,0);
}
// 1.13c Storm+0x2D4C0
void Storm_Sub2D4C0(Storm34d60Env& e,void* object,const void* a,const void* b) {e.splice(object,a,b);}
// 1.13c Storm+0x2AF70
void Storm_Sub2AF70(Storm34d60Env& e,void* object) {
    if(object) {e.destroy(object);e.free_(object,"delete",-1,0);}
}
// 1.13c Storm+0x2DE00
void Storm_Sub2DE00(Storm34d60Env& e,uint32_t* object,void* a,void* b) {
    uint32_t depth=object[0x55];object[0x55]=depth+1;
    // x86 LEA multiplies in 32 bits before adding the native base.
    uint32_t offset=0x14+depth*20;
    e.combine(object,reinterpret_cast<uint8_t*>(object)+offset,a,b,object+5);
    --object[0x55];
}
// 1.13c Storm+0x2D460
void Storm_Sub2D460(Storm34d60Env& e,void* object,const void* data,uint32_t size) {e.insert(object,data,size);}
// 1.13c Storm+0x2D440
void Storm_Sub2D440(Storm34d60Env& e,void* object,uint32_t value) {e.append(object,value);}

// 1.13c Storm+0x3A870  Jenkins lookup2 hash; ECX=key, stack=len, EAX=seed (golden ratio a=b).
uint32_t Storm_Sub3A870(const uint8_t* k, uint32_t len, uint32_t seed) {
    uint32_t a = 0x9e3779b9, b = a, c = seed, l = len;
    auto mix = [&]() {
        a = (a - b - c) ^ (c >> 13); b = (b - c - a) ^ (a << 8);  c = (c - a - b) ^ (b >> 13);
        a = (a - b - c) ^ (c >> 12); b = (b - c - a) ^ (a << 16); c = (c - a - b) ^ (b >> 5);
        a = (a - b - c) ^ (c >> 3);  b = (b - c - a) ^ (a << 10); c = (c - a - b) ^ (b >> 15);
    };
    auto w = [&](int o) { return (uint32_t)k[o] | k[o+1] << 8 | k[o+2] << 16 | (uint32_t)k[o+3] << 24; };
    for (; l >= 12; l -= 12, k += 12) { a += w(0); b += w(4); c += w(8); mix(); }
    c += len;
    switch (l) {
    case 11: c += (uint32_t)k[10] << 24; [[fallthrough]];
    case 10: c += (uint32_t)k[9] << 16;  [[fallthrough]];
    case 9:  c += (uint32_t)k[8] << 8;   [[fallthrough]];
    case 8:  b += (uint32_t)k[7] << 24;  [[fallthrough]];
    case 7:  b += (uint32_t)k[6] << 16;  [[fallthrough]];
    case 6:  b += (uint32_t)k[5] << 8;   [[fallthrough]];
    case 5:  b += k[4];                  [[fallthrough]];
    case 4:  a += (uint32_t)k[3] << 24;  [[fallthrough]];
    case 3:  a += (uint32_t)k[2] << 16;  [[fallthrough]];
    case 2:  a += (uint32_t)k[1] << 8;   [[fallthrough]];
    case 1:  a += k[0];
    }
    mix();
    return c;
}
