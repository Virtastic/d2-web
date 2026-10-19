// Stage A, BNClient 1.13c cache and SHA-1. Prior art used: D2MOO BnClient.h
// (interface only; no cache implementation), bncsutil sha1.c (SHA-1 cross-check),
// Storm/MPQ hash/encryption algorithm. Logic and register arguments checked against
// the cached 1.13c decompiles and the retail x86 instructions. Host calls are seams;
// native pointers never occupy serialized cache fields. Not a parity verdict.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <algorithm>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

struct BnCache13190State {
    uint32_t active;                         // f634
    uint32_t* header;                        // f624, eleven retail dwords
    uint32_t* blocks;                        // f628, four dwords per entry
    uint32_t* hashes;                        // f630, four dwords per entry
    uint32_t* crypt;                         // f62c, 0x500 dwords
    uintptr_t file;                         // dce8, native host handle
    uint32_t time_tick, time_seconds;       // f900, f904
    uint64_t time_value;                    // f908/f90c
};
struct BnCacheSystemTime { uint16_t year,month,weekday,day,hour,minute,second,millisecond; };
static_assert(sizeof(BnCacheSystemTime)==16);
struct BnCache13190Env {
    void (*enter)();                         // recursive critical section f8b4
    void (*leave)();
    void* (*alloc)(uint32_t bytes, uint32_t line, uint32_t flags);
    void (*free)(void*, uint32_t line);
    uintptr_t (*open)(const char*, uint32_t access, uint32_t sharing,
                      uint32_t creation, uint32_t attributes);
    void (*close)(uintptr_t);
    uintptr_t (*module_handle)(const char*);
    uint32_t (*module_filename)(uintptr_t, char*, uint32_t);
    void (*remove)(const char*);
    uint32_t (*file_size)(uintptr_t);
    void (*seek)(uintptr_t, uint32_t offset);
    void (*read)(uintptr_t, void*, uint32_t count, uint32_t* actual);
    void (*write)(uintptr_t, const void*, uint32_t count, uint32_t* actual);
    uint32_t (*tick_count)();
    void (*system_time)(BnCacheSystemTime*);
    int32_t (*system_to_file_time)(const BnCacheSystemTime*, uint64_t*);
    void (*log)(const char*, const char* name, uint32_t code);
    void (*fatal_null_buffer)();             // Fog assert/error/exit(-1), must not return
};
namespace {
constexpr uintptr_t invalid_file = static_cast<uintptr_t>(-1);
uint32_t load32(const void* p) { uint32_t n; std::memcpy(&n,p,4); return n; }
void put32(void* p,uint32_t n) { std::memcpy(p,&n,4); }
void put64(void* p,uint64_t n) { put32(p,uint32_t(n)); put32(static_cast<uint8_t*>(p)+4,uint32_t(n>>32)); }
uint64_t load64(const void* p) { return uint64_t(load32(p))|(uint64_t(load32(static_cast<const uint8_t*>(p)+4))<<32); }
uint32_t crc_bytes(const uint8_t* p,uint32_t n) {
    uint32_t c=0xffffffffu;
    while(n--) { c ^= *p++; for(int j=0;j<8;++j) c=(c>>1)^(0xedb88320u&(0u-(c&1))); }
    return ~c;
}
void init_crypt(uint32_t* table) {
    uint32_t seed=0x100001;
    for(uint32_t i=0;i<256;++i) for(uint32_t j=0;j<5;++j) {
        seed=(seed*125+3)%0x2aaaab; uint32_t high=(seed&0xffff)<<16;
        seed=(seed*125+3)%0x2aaaab; table[i+j*256]=high|(seed&0xffff);
    }
}
// 1.13c BNClient+0x130B0
uint32_t hash_name(const BnCache13190State* s,const char* name,uint32_t type) {
    uint32_t a=0x7fed7fed,b=0xeeeeeeee;
    if(name) while(*name) {
        uint8_t c=static_cast<uint8_t>(*name++);
        if(c>='a'&&c<='z') c-=32;
        // Retail sign-extends the uppercase char before the table index/addition.
        int32_t ch=static_cast<int8_t>(c);
        a=(a+b)^s->crypt[int32_t(type*256)+ch]; b=b*33+uint32_t(ch)+a+3;
    }
    return a;
}
void decrypt(BnCache13190State* s,uint8_t* p,uint32_t key,uint32_t bytes) {
    uint32_t b=0xeeeeeeee;
    for(uint32_t n=bytes>>2;n;--n,p+=4) {
        b+=s->crypt[0x400+(key&255)]; uint32_t v=load32(p)^(b+key);
        put32(p,v); b=b*33+v+3; key=(key>>11)|((~key<<21)+0x11111111u);
    }
}
bool equal_ci(const char* a,const char* b) {
    for(;;++a,++b) { auto lower=[](uint8_t c){return c>='A'&&c<='Z'?uint8_t(c+32):c;};
        uint8_t x=lower(uint8_t(*a)),y=lower(uint8_t(*b)); if(x!=y)return false; if(!x)return true; }
}
}

// 1.13c BNClient+0x12EE0
// Append the cache filename after the final backslash; no backslash means
// retail appends to the complete module filename, rather than replacing it.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_12ee0(char* path,
    const BnCache13190Env* e) {
    e->module_filename(e->module_handle(nullptr),path,260);
    char* slash=std::strchr(path,'\\');
    if(slash) {
        for(char* next; (next=std::strchr(slash+1,'\\'))!=nullptr;) slash=next;
        *slash=0;
    }
    std::memcpy(path+std::strlen(path),"\\bncache.dat",13);
}
// 1.13c BNClient+0x13110
// Reuse the FILETIME only when BOTH seconds and the millisecond tick match.
// Keep the second tick read after conversion, including tick-wrap behaviour.
extern "C" EMSCRIPTEN_KEEPALIVE uint64_t d2_fn_bnclient_13110(BnCache13190State* s,
    const BnCache13190Env* e,uint32_t seconds) {
    if(seconds==s->time_seconds && e->tick_count()==s->time_tick) return s->time_value;
    BnCacheSystemTime time;
    e->system_time(&time);
    e->system_to_file_time(&time,&s->time_value);
    s->time_value+=uint64_t(seconds)*10000000;
    s->time_seconds=seconds;
    s->time_tick=e->tick_count();
    return s->time_value;
}

// 1.13c BNClient+0x13190 -- ECX=key, EDX=byte count, EAX=buffer.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_13190(BnCache13190State* s,
    uint32_t key,uint32_t bytes,uint8_t* buffer) {
    uint32_t b=0xeeeeeeee;
    for(uint32_t n=bytes>>2;n;--n,buffer+=4) {
        b+=s->crypt[0x400+(key&255)]; uint32_t plain=load32(buffer);
        put32(buffer,plain^(b+key)); b=b*33+plain+3;
        key=(key>>11)|((~key<<21)+0x11111111u);
    }
}
// 1.13c BNClient+0x13250 -- ECX=buffer, EAX=byte count. Empty CRC is zero.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_bnclient_13250(const uint8_t* p,
    uint32_t n,const BnCache13190Env* e) {
    if(!p&&n) { e->fatal_null_buffer(); __builtin_trap(); }
    return crc_bytes(p,n);
}
// 1.13c BNClient+0x132D0 -- reset state and free in retail order, inside recursive lock.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_132d0(BnCache13190State* s,
    const BnCache13190Env* e) {
    e->enter(); s->active=0;
    if(s->blocks){e->free(s->blocks,0x140);s->blocks=nullptr;}
    if(s->hashes){e->free(s->hashes,0x144);s->hashes=nullptr;}
    if(s->header){e->free(s->header,0x148);s->header=nullptr;}
    if(s->file!=invalid_file){e->close(s->file);s->file=invalid_file;}
    if(s->crypt){e->free(s->crypt,0x150);s->crypt=nullptr;}
    e->leave();
}
// 1.13c BNClient+0x133D0 -- EAX=name (missing in decompile), stack=locale.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_bnclient_133d0(BnCache13190State* s,
    const char* name,uint32_t locale) {
    uint32_t first=hash_name(s,name,0),a=hash_name(s,name,1),b=hash_name(s,name,2);
    uint32_t mask=s->header[6]-1,slot=first&mask,fallback=0xffffffffu; first=slot;
    do { uint32_t* p=s->hashes+slot*4;
        if(p[3]==0xffffffffu)return fallback;
        if(p[0]==a&&p[1]==b&&p[3]!=0xfffffffeu) {
            if(p[2]==locale)return slot;
            if(!p[2])fallback=slot;
        }
        slot=(slot+1)&mask;
    }while(slot!=first);
    return fallback;
}
// 1.13c BNClient+0x13490 -- load/validate BN3 header and encrypted hash/block tables;
// rebuild old BN2 or invalid archives. Allocator flags 8 zero-initialize in retail.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_bnclient_13490(BnCache13190State* s,
    const BnCache13190Env* e) {
    e->enter(); if(s->active)d2_fn_bnclient_132d0(s,e);
    char path[260]={};d2_fn_bnclient_12ee0(path,e);
    s->file=e->open(path,0xc0000000,0,4,0x80);
    if(s->file==invalid_file){d2_fn_bnclient_132d0(s,e);e->leave();return 0;}
    s->crypt=static_cast<uint32_t*>(e->alloc(0x1400,0x17e,0));init_crypt(s->crypt);
    s->header=static_cast<uint32_t*>(e->alloc(0x2c,0x182,8));
    uint32_t size=e->file_size(s->file),actual=0; bool valid=false,warn=false;
    if(size&&size<=0xa00000) {
        if(size<0x2c)warn=true;
        else {e->read(s->file,s->header,0x2c,&actual);
            if(actual<0x2c)warn=true;
            else if(s->header[0]!=0x1a324e42) {
                if(s->header[0]!=0x1a334e42)warn=true;
                else {
                    auto* h=s->header;uint32_t crc=h[8];h[8]=0;
                    if(crc_bytes(reinterpret_cast<uint8_t*>(h),0x2c)!=crc)warn=true;
                    else {h[8]=crc;
                        if(!h[4]||!h[6]||!h[5]||!h[7])warn=true;
                        else {
                            uint32_t bytes=std::max(h[6],0x400u)*16;
                            if(size<uint32_t(h[4]+bytes))warn=true;
                            else {
                                s->hashes=static_cast<uint32_t*>(e->alloc(bytes,0x1b7,8));
                                e->seek(s->file,h[4]);e->read(s->file,s->hashes,bytes,&actual);
                                if(actual!=bytes||crc_bytes(reinterpret_cast<uint8_t*>(s->hashes),bytes)!=h[9])warn=true;
                                else {
                                    decrypt(s,reinterpret_cast<uint8_t*>(s->hashes),hash_name(s,"(hash table)",3),bytes);
                                    bytes=std::max(h[7],0x400u)*16;
                                    if(size<uint32_t(h[5]+bytes))warn=true;
                                    else {
                                        s->blocks=static_cast<uint32_t*>(e->alloc(bytes,0x1cd,8));
                                        e->seek(s->file,h[5]);e->read(s->file,s->blocks,bytes,&actual);
                                        if(actual!=bytes||crc_bytes(reinterpret_cast<uint8_t*>(s->blocks),bytes)!=h[10])warn=true;
                                        else {decrypt(s,reinterpret_cast<uint8_t*>(s->blocks),hash_name(s,"(block table)",3),bytes);valid=true;}
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if(!valid) {
        if(warn)e->log("WARNING- cache file is invalid. Replacing with new cache file",nullptr,0);
        if(s->active){e->leave();return 1;}
        if(s->hashes)e->free(s->hashes,0x1e7);
        if(s->blocks)e->free(s->blocks,0x1e9);
        // 13390 closes, resolves the path again, then deletes the old file.
        if(s->file!=invalid_file){e->close(s->file);s->file=invalid_file;}
        char remove_path[260]={};d2_fn_bnclient_12ee0(remove_path,e);e->remove(remove_path);
        s->file=e->open(path,0xc0000000,0,4,0x80);
        if(s->file==invalid_file){
            // Retail retains freed table aliases on the failed-reopen path,
            // so the cleanup callback sees those same values again.
            d2_fn_bnclient_132d0(s,e);e->leave();return 0;
        }
        auto* h=s->header;std::memset(h,0,0x2c);h[0]=0x1a334e42;h[1]=0x2c;
        s->hashes=static_cast<uint32_t*>(e->alloc(0x4000,0x207,8));
        h[4]=0x2c;h[6]=0x400;h[2]=std::max(h[2],0x402cu);
        for(uint32_t i=0;i<0x400;++i)s->hashes[4*i+3]=0xffffffffu;
        s->blocks=static_cast<uint32_t*>(e->alloc(0x4000,0x214,8));
        h[5]=0x402c;h[2]=std::max(h[2],0x802cu);
    }
    s->active=1;e->leave();return 1;
}
// 1.13c BNClient+0x13920 -- discard and reopen cache (two deletes when active).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_13920(BnCache13190State* s,
    const BnCache13190Env* e) {
    char path[260]={};
    if(s->active) {d2_fn_bnclient_132d0(s,e);
        if(s->file!=invalid_file){e->close(s->file);s->file=invalid_file;}
        d2_fn_bnclient_12ee0(path,e);e->remove(path);
    }
    path[0]=0;d2_fn_bnclient_12ee0(path,e);e->remove(path);d2_fn_bnclient_13490(s,e);
}
// 1.13c BNClient+0x139A0 -- ESI=name, passed as EAX on each 133d0 lookup.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_139a0(BnCache13190State* s,const char* name) {
    for(uint32_t i=d2_fn_bnclient_133d0(s,name,0);i!=0xffffffffu;i=d2_fn_bnclient_133d0(s,name,0)) {
        uint32_t* p=s->hashes+4*i+3;uint32_t block=*p;
        if(block<s->header[7]){s->blocks[4*block+2]=0;s->blocks[4*block+3]=0;}
        *p=0xfffffffeu;
    }
}
// 1.13c BNClient+0x13A00 -- ECX=name, EDX=data; stack=size, FILETIME*,
// first expiry seconds, second expiry seconds. All file offsets remain retail u32.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_bnclient_13a00(BnCache13190State* s,
    const BnCache13190Env* e,const char* name,const uint8_t* data,uint32_t length,
    const uint32_t* stamp,uint32_t expire1,uint32_t expire2) {
    if(!name||!*name||!data||!length||!stamp||!expire2||!s->active||length>0x1000000)return 0;
    e->enter();d2_fn_bnclient_139a0(s,name);
    auto perform=[&]() -> uint32_t {
        uint32_t block=0,count=s->header[7],need=length+0x144;
        for(;block<count;++block) {
            auto* b=s->blocks+block*4;
            if(!b[3]&&need<=b[1])break;
        }
        if(block>=count) {
            block=0;
            for(;block<s->header[7];++block) {
                auto* b=s->blocks+block*4;
                if(need<=b[1]) {
                    uint8_t h[0x144]={};uint32_t actual=0;
                    e->seek(s->file,b[0]);e->read(s->file,h,4,&actual);
                    if(actual!=4||load32(h)!=0x144)break;
                    e->read(s->file,h+4,0x140,&actual);
                    if(actual!=0x140)break;
                    if(d2_fn_bnclient_13110(s,e,0)>load64(h+0x30))break;
                }
            }
        }
        if(block<s->header[7]) {
            for(uint32_t i=0;i<s->header[6];++i)
                if(s->hashes[4*i+3]==block)s->hashes[4*i+3]=0xfffffffeu;
        } else {
            if(block>=0x400)return 0xb;
            auto* b=s->blocks+block*4;b[0]=s->header[2];b[1]=need;
            s->header[2]+=need;s->header[7]=block+1;
        }
        uint32_t first=hash_name(s,name,0),a=hash_name(s,name,1),b=hash_name(s,name,2);
        uint32_t mask=s->header[6]-1,slot=first&mask;first=slot;
        while(s->hashes[4*slot+3]!=0xffffffffu&&s->hashes[4*slot+3]!=0xfffffffeu) {
            slot=(slot+1)&mask;if(slot==first)break;
        }
        auto* hash=s->hashes+slot*4;
        if(hash[3]!=0xffffffffu&&hash[3]!=0xfffffffeu)return 0xc;
        hash[0]=a;hash[1]=b;hash[2]=0;hash[3]=block;
        auto* entry=s->blocks+block*4;entry[2]=length;entry[3]=0xc0000000;
        uint8_t h[0x144]={};put32(h,0x144);
        put64(h+0x28,d2_fn_bnclient_13110(s,e,0));put64(h+0x20,load64(stamp));
        put64(h+0x30,d2_fn_bnclient_13110(s,e,expire1));put64(h+0x38,d2_fn_bnclient_13110(s,e,expire2));
        put64(h+0x18,d2_fn_bnclient_13110(s,e,0));std::strcpy(reinterpret_cast<char*>(h+0x40),name);
        put32(h+0x10,crc_bytes(data,length));put32(h+0xc,crc_bytes(h,0x144));
        uint32_t actual=0;e->seek(s->file,entry[0]);e->write(s->file,h,0x144,&actual);
        if(actual!=0x144)return 0xd;
        e->write(s->file,data,length,&actual);if(actual!=length)return 0xe;
        uint32_t hb=std::max(s->header[6],0x400u)*16,bb=std::max(s->header[7],0x400u)*16;
        auto* tmp=static_cast<uint8_t*>(e->alloc(std::max(hb,bb),0x37c,0));
        e->seek(s->file,s->header[4]);std::memcpy(tmp,s->hashes,hb);
        d2_fn_bnclient_13190(s,hash_name(s,"(hash table)",3),hb,tmp);
        s->header[9]=crc_bytes(tmp,hb);actual=0;e->write(s->file,tmp,hb,&actual);
        if(actual!=hb){e->free(tmp,0x388);return 0xf;}
        e->seek(s->file,s->header[5]);std::memcpy(tmp,s->blocks,bb);
        d2_fn_bnclient_13190(s,hash_name(s,"(block table)",3),bb,tmp);
        s->header[10]=crc_bytes(tmp,bb);e->write(s->file,tmp,bb,&actual);e->free(tmp,0x396);
        if(actual!=bb)return 0x10;
        s->header[8]=0;s->header[8]=crc_bytes(reinterpret_cast<uint8_t*>(s->header),0x2c);
        e->seek(s->file,0);e->write(s->file,s->header,0x2c,&actual);
        return actual==0x2c?0:0x11;
    };
    uint32_t error=perform();
    if(error){e->log("bncache.dat is corrupt (%d) - rebuilding archive",nullptr,error);d2_fn_bnclient_13920(s,e);}
    e->leave();return error==0;
}
// 1.13c BNClient+0x14050 -- ECX=name, EDX=optional FILETIME output;
// stack=allocated native byte pointer output, u32 length output. Preserves retail's
// two error classes: rebuild archive (1..7), or delete filename (1..4).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_bnclient_14050(BnCache13190State* s,
    const BnCache13190Env* e,const char* name,uint32_t* stamp,uint8_t** data,uint32_t* length) {
    if(stamp){stamp[0]=0;stamp[1]=0;}if(data)*data=nullptr;if(length)*length=0;
    if(!name||!*name||!data||!length||!s->active)return 0;
    e->enter();uint32_t slot=d2_fn_bnclient_133d0(s,name,0);
    if(slot==0xffffffffu){e->leave();return 0;}
    uint32_t archive_error=0,file_error=0;
    auto perform=[&]() {
        uint32_t block=s->hashes[4*slot+3];
        if(block>=s->header[7]){archive_error=1;return;}
        auto* b=s->blocks+4*block;uint8_t h[0x144]={};uint32_t actual=0;
        if(b[3]&0xc0000000u) {
            e->seek(s->file,b[0]);e->read(s->file,h,4,&actual);
            if(actual!=4){archive_error=2;return;}
            if(load32(h)!=0x144){archive_error=3;return;}
            e->read(s->file,h+4,0x140,&actual);
            if(actual!=0x140){archive_error=4;return;}
            uint32_t saved=load32(h+0xc);put32(h+0xc,0);
            if(saved!=crc_bytes(h,0x144)){file_error=1;return;}
            put32(h+0xc,saved);
        }
        if(!equal_ci(name,reinterpret_cast<char*>(h+0x40))){file_error=2;return;}
        if(d2_fn_bnclient_13110(s,e,0)>load64(h+0x38)){file_error=3;return;}
        if(b[2]>0x1000000){archive_error=5;return;}
        if(stamp) {stamp[0]=load32(h+0x20);stamp[1]=load32(h+0x24);}
        put64(h+0x18,d2_fn_bnclient_13110(s,e,0));e->seek(s->file,b[0]);put32(h+0xc,0);
        put32(h+0xc,crc_bytes(h,0x144));uint32_t bytes=std::min(load32(h),0x144u);
        e->write(s->file,h,bytes,&actual);if(actual!=bytes){archive_error=6;return;}
        *data=static_cast<uint8_t*>(e->alloc(b[2],0x2a0,0));
        e->seek(s->file,uint32_t(b[0]+load32(h)));e->read(s->file,*data,b[2],length);
        if(*length!=b[2]){archive_error=7;return;}
        if(load32(h+0x10)!=crc_bytes(*data,*length))file_error=4;
    };
    perform();
    if(archive_error){e->log("bncache.dat is corrupt (%d) - rebuilding archive",nullptr,archive_error);d2_fn_bnclient_13920(s,e);}
    else if(file_error){e->log("File: '%s' in bncache.dat is corrupt (%d) - deleting from cache",name,file_error);d2_fn_bnclient_139a0(s,name);}
    if(archive_error||file_error){if(*data){e->free(*data,0x2c9);*data=nullptr;}*length=0;}
    e->leave();return !archive_error&&!file_error;
}

// 1.13c BNClient+0x14470 -- EAX=64 input bytes, stack=five native u32 hash words.
// Standard SHA-1 compression, including 32-bit modular additions and big endian loads.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_14470(uint32_t* state,const uint8_t* block) {
    auto rol=[](uint32_t v,uint32_t n){return (v<<n)|(v>>(32-n));};
    uint32_t w[80];
    for(uint32_t i=0;i<16;++i)w[i]=(uint32_t(block[4*i])<<24)|(uint32_t(block[4*i+1])<<16)|(uint32_t(block[4*i+2])<<8)|block[4*i+3];
    for(uint32_t i=16;i<80;++i)w[i]=rol(w[i-3]^w[i-8]^w[i-14]^w[i-16],1);
    uint32_t a=state[0],b=state[1],c=state[2],d=state[3],e=state[4];
    for(uint32_t i=0;i<80;++i) {
        uint32_t f,k;
        if(i<20){f=(b&c)|(~b&d);k=0x5a827999;}
        else if(i<40){f=b^c^d;k=0x6ed9eba1;}
        else if(i<60){f=((b|d)&c)|(b&d);k=0x8f1bbcdc;}
        else {f=b^c^d;k=0xca62c1d6;}
        uint32_t t=rol(a,5)+f+e+k+w[i];w[i]=0;e=d;d=c;c=rol(b,30);b=a;a=t;
    }
    state[0]+=a;state[1]+=b;state[2]+=c;state[3]+=d;state[4]+=e;
}
struct BnSha14930State { uint32_t bits_low,bits_high;uint32_t hash[5];uint8_t buffer[64]; };
static_assert(sizeof(BnSha14930State)==0x5c);
static_assert(offsetof(BnSha14930State,hash)==8);
static_assert(offsetof(BnSha14930State,buffer)==0x1c);
// 1.13c BNClient+0x14930 -- EDI=context, EAX=input, stack=u32 length.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_14930(BnSha14930State* s,
    const uint8_t* input,uint32_t bytes) {
    uint32_t used=(s->bits_low>>3)&63,old=s->bits_low;
    s->bits_low+=bytes<<3;s->bits_high+=(bytes>>29)+(s->bits_low<old);
    if(used) {
        // The retail sum is u32 even when the byte count wraps.
        uint32_t total=bytes+used;
        if(total<64) {
            for(uint32_t i=used;i<total;++i)s->buffer[i]=input[i-used];
            return;
        }
        for(uint32_t i=used;i<64;++i)s->buffer[i]=input[i-used];
        d2_fn_bnclient_14470(s->hash,s->buffer);
        input+=64-used;bytes=total-64;used=0;
    }
    while(bytes>=64){d2_fn_bnclient_14470(s->hash,input);input+=64;bytes-=64;}
    if(bytes)std::memcpy(s->buffer+used,input,bytes);
}
// 1.13c BNClient+0x149E0 -- EAX=context, stack=20-byte digest destination.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_149e0(BnSha14930State* s,uint8_t* digest) {
    uint64_t bits=(uint64_t(s->bits_high)<<32)|s->bits_low;uint8_t length[8];
    for(uint32_t i=0;i<8;++i)length[7-i]=uint8_t(bits>>(i*8));
    uint8_t pad[64]={0x80};uint32_t n=((0u-(s->bits_low>>3)-9)&63)+1;
    d2_fn_bnclient_14930(s,pad,n);d2_fn_bnclient_14930(s,length,8);
    for(uint32_t i=0;i<5;++i)for(uint32_t j=0;j<4;++j)digest[4*i+j]=uint8_t(s->hash[i]>>(24-j*8));
}
