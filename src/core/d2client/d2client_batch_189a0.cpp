// Prior art: D2Client has no D2MOO implementation; community pointer tables provide
// names only. Bodies follow cached 1.13c decompiles, with disassembly resolving
// 190B0's room-array/count outputs. Stage A only: no retail-vector verdict.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// Retail memory contains 32-bit addresses, never native pointers. The adapter
// resolves them; call arguments containing host pointers retain uintptr_t width.
// call identifies an internal callee by RVA or an imported callee by ordinal.
// Its adapter supplies the named callee's retail register/stack convention.
struct D2C189a0Env {
    void* (*resolve)(uint32_t);
    uintptr_t (*call)(uint32_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);
    uint32_t* globalClient; // DAT_6fbcbbfc, retail address
    uint32_t* registry;     // DAT_6fbcb800, eight retail addresses
    uint32_t* listHead;     // DAT_6fbcc4d4
    uint32_t* unitBuckets;  // DAT_6fbba808, 128 retail addresses
    uint32_t* shutdownFlag; // DAT_6fbcc3b8
};
static uint32_t word(const void* p, uint32_t offset) {
    uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p)+offset, 4); return v;
}
static void clearWord(void* p, uint32_t offset) {
    const uint32_t zero=0; std::memcpy(static_cast<uint8_t*>(p)+offset,&zero,4);
}
static void* ptr(const D2C189a0Env* e, uint32_t address) { return address ? e->resolve(address) : nullptr; }
static uintptr_t call(const D2C189a0Env* e, uint32_t fn, uintptr_t a=0, uintptr_t b=0, uintptr_t c=0, uintptr_t d=0, uintptr_t f=0) { return e->call(fn,a,b,c,d,f); }
static uintptr_t host(const void* p) { return reinterpret_cast<uintptr_t>(p); }
static bool requireRoom(void* client,const D2C189a0Env* e,uint32_t text=0x946) {
    call(e,0x2A0B0,host(client));
    if (word(client,0x2c)) return true;
    auto message=call(e,10265,text); call(e,10024,0xD359F,message); call(e,0x3664,UINT32_MAX); return false;
}
static void unitList(void* window,const D2C189a0Env* e) {
    void* p=ptr(e,*e->listHead);
    while (p && (word(p,4)!=7 || word(p,0xc)!=word(window,0xc) || word(p,0x20)!=0)) p=ptr(e,word(p,0x30));
    if (!p) return;
    uint32_t id=word(p,8); if(id==UINT32_MAX) return;
    void* u=ptr(e,e->unitBuckets[id&127]);
    while(u && word(u,0xc)!=id) u=ptr(e,word(u,0xe4));
    if(!u) return;
    if(word(u,0)!=1) { auto message=call(e,10265,0x5a); call(e,10024,0xD359F,message); call(e,0x3664,UINT32_MAX); return; }
    // Retail uses the first entry with this id, even if a later entry has mode 2.
    for(p=ptr(e,*e->listHead);p;p=ptr(e,word(p,0x30))) if(word(p,8)==id) {
        if(word(p,0x2c)==2) call(e,0x511F0,host(window),0);
        return;
    }
}
// 1.13c D2Client+0x189a0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_189a0(void* client,uint32_t rgb,const D2C189a0Env* e) {
    if(client && word(client,0)<=1 && word(client,0x64)) call(e,0xA86C0,rgb?(rgb>>16)&255:255,rgb?(rgb>>8)&255:255,rgb?rgb&255:255);
}
// 1.13c D2Client+0x18A00
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_18a00(void* state,const D2C189a0Env* e) {
    // 185E0 leaves the context in ECX; adapter returns that host context.
    void* context=reinterpret_cast<void*>(call(e,0x185E0,host(state)));
    void* window=ptr(e,word(context,4)); if(window && *e->listHead) unitList(window,e); return 0;
}
// 1.13c D2Client+0x18AC0
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_18ac0(int32_t a,int32_t b,const D2C189a0Env* e) {
    void* client=ptr(e,*e->globalClient); if(!client || word(client,0)!=0) return 0;
    void* msg=ptr(e,word(client,0x14)); return msg && word(msg,0x154)==0x13 && word(msg,0x158)==uint32_t(a) && word(msg,0x15c)==uint32_t(b);
}
// 1.13c D2Client+0x18bc0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_18bc0(void* client,const D2C189a0Env* e) {
    if(!client || word(client,0)>1 || !word(client,0x64)) return;
    if(word(client,0)) { if(!call(e,0x1260)) return; call(e,11077); } call(e,0xA8B60);
}
// 1.13c D2Client+0x18E20
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_18e20(void* client,uint32_t p1,uint32_t p2,uint32_t p3,const D2C189a0Env* e) {
    int32_t mode=static_cast<int32_t>(word(client,0x10)); uint32_t old=word(client,0x2c); uintptr_t opened=0;
    uint32_t type=0,id=0; bool active=mode>1 && (mode<4 || mode==6);
    if(active) { if(!requireRoom(client,e)) return;
        opened=call(e,10392,host(ptr(e,word(client,0x2c))));
        if(!opened) call(e,0xA1340,host(&type),host(&id)); }
    call(e,10906,host(ptr(e,old)),host(client),p1,p2,p3);
    if(p3) { uint32_t tuple[4]={}; // decompile zeroes the locals before the call: A1340/opened outputs are discarded
        uint32_t next=opened?(mode==3?0x18:0):(mode==3?0x17:1); call(e,0x19830,next,host(client),host(tuple),0);
    }
}
// 1.13c D2Client+0x18F40
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_18f40(void* param1,int32_t index,const void* table,int32_t count,const D2C189a0Env* e) {
    if(index>=count) { auto message=call(e,10265,0x2cb); call(e,10024,0xD359F,message); call(e,0x3664,UINT32_MAX); return; }
    uint16_t value; std::memcpy(&value,static_cast<const uint8_t*>(table)+int64_t(index)*4,2);
    if(value) call(e,0x23200,host(param1),value);
}
// 1.13c D2Client+0x190B0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_190b0(const D2C189a0Env* e) {
    void* client=ptr(e,*e->globalClient); if(!client) return;
    uintptr_t act=call(e,10331,host(client)); if(!act) return;
    uint32_t rooms=0,count=0; call(e,10383,act,host(&rooms),host(&count));
    void* array=ptr(e,rooms);
    for(uint32_t i=0;i<count;++i) for(void* u=ptr(e,word(ptr(e,word(array,i*4)),0x74));u;u=ptr(e,word(u,0xe8)))
        if(word(u,0)==1 && call(e,10494,host(u),0x3f)) call(e,0x6A220,host(u),0);
}
// 1.13c D2Client+0x19130
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_19130(uint32_t address,const D2C189a0Env* e) {
    void* client=ptr(e,address);
    for(int i=0;i<8;++i) if(e->registry[i]==address) { e->registry[i]=0; break; }
    call(e,0x69700,host(client)); if(*e->shutdownFlag) { call(e,10505,host(client)); call(e,11016,host(client)); }
    // 10692's ECX output is forwarded by the adapter to A5C50.
    uintptr_t ecx=call(e,10692,0,host(ptr(e,word(client,0x2c)))); clearWord(client,0x2c); call(e,0xA5C50,ecx,0);
    call(e,10820,host(ptr(e,word(client,0x60)))); clearWord(client,0x60);
    if(auto aux=call(e,10327,host(client),0)) call(e,0xA8630,aux);
    if(word(client,100)) { call(e,0xA8630,host(ptr(e,word(client,100)))); clearWord(client,100); }
    if(address==*e->globalClient) { if(*e->globalClient) call(e,0xC0250); *e->globalClient=0; }
    call(e,11002,0,host(client)); d2_client_190b0(e);
}
// 1.13c D2Client+0x19D80
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_19d80(uint32_t address,const D2C189a0Env* e) {
    if(address!=*e->globalClient) return;
    void* client=ptr(e,address); if(call(e,10973,host(client),10,0)) return;
    uint32_t old=word(client,0x2c); if(!requireRoom(client,e)) return;
    auto opened=call(e,10392,host(ptr(e,word(client,0x2c)))); void* room=ptr(e,old);
    if(call(e,10392,host(room))) {
        uint32_t tuple[7]={opened?word(reinterpret_cast<void*>(opened),0):6,opened?word(reinterpret_cast<void*>(opened),0xc):UINT32_MAX};
        call(e,0x19830,0,host(client),host(tuple),0); return;
    }
    auto type=call(e,10983,host(room)); auto id=call(e,10764,host(room));
    if(auto aux=call(e,10327,host(client),0)) call(e,0xA8630,aux);
    call(e,0x197D0,host(client),address==*e->globalClient); call(e,11103,host(client),0);
    if(word(client,0x10)!=0x13) {
        call(e,10498,host(ptr(e,word(client,0x2c)))); auto act=call(e,10331,host(client)); auto special=call(e,10057,act);
        uint32_t first=special?5:1,second=special?6:2;
        if(call(e,0x2A040,host(client),type,id)) { call(e,0x2A6C0,host(client),second); return; }
        call(e,0x19710,host(client),first,1);
    }
}
// 1.13c D2Client+0x1AAA0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1aaa0(void* context,const D2C189a0Env* e) {
    void* client=ptr(e,word(context,4)); if(!client || word(client,0)!=0) return;
    if(!requireRoom(client,e) || call(e,10392,host(ptr(e,word(client,0x2c))))) return;
    uint32_t mode=word(client,0x10); if(mode==2 || mode==3 || mode==6) call(e,0x1A010,host(ptr(e,word(context,0xc))),mode==3?3:1,host(client));
}
// 1.13c D2Client+0x1B620
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1b620(void* state,const D2C189a0Env* e) {
    call(e,0x185E0,host(state)); void* window=ptr(e,word(state,4));
    if(window && *e->listHead) unitList(window,e);
    d2_client_1aaa0(state,e); return 0;
}
