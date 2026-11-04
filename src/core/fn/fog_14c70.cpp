// probe for Fog 0x14C70 (sub_14C70, thread stack/code byte dump), over the HookEnv recorder. stdcall(hThread): r[0] = hThread, blob lg = r[14].
// Port: src/core/fog/fog_batch_n04.cpp (fog_sub_14C70) through the g_fogN04 seams. The recorded hooks: the GetThreadContext import slot
// (0x250B4: hThread, &ctx; returns 1, the CONTEXT stays zero), the cdecl logger 0x18AF0 (log 0x268EC, text) and the cdecl dumper 0x18950
// (7 stack words: 0x27810, 0x268EC, address, length, 0, 1, 0).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "hookenv.h"
struct FogN04Seams {
    void (*validateCs)(void* cs); void (*enterCs)(void* cs); void (*leaveCs)(void* cs); void (*deleteCs)(void* cs);
    uint32_t (*nameOf)(uint32_t handle); void (*strCopy)(void* dst, uint32_t src, uint32_t max); void (*free_)(void* p);
    void (*sub5E550)(void); void (*sub64260)(void); void (*closeSocket)(uint32_t s);
    uint32_t (*getThreadContext)(void* hThread, uint32_t* ctx); void (*logPrint)(void* ctx, const char* text);
    void (*dumpBytes)(void* log, void* ctx, const void* p, uint32_t n, uint32_t a, uint32_t b); void (*fatal)(int32_t line);
    uint32_t (*tickCount)(void); void (*sleepMs)(uint32_t ms); uint32_t (*cpuidLeaf1)(uint32_t out[4]); void (*fclose_)(void* f);
};
extern FogN04Seams g_fogN04;
extern "C" void fog_sub_14C70(void* hThread);
namespace {
HookEnv* H;
uint32_t getCtx(void* h, uint32_t* ctx) { H->h(0x250B4, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)h, (uint32_t)(uintptr_t)ctx); return 1; }
void logPrint(void*, const char* text) {
    const uint32_t tag = text[0] == 'S' ? 0xB2027814u : 0xB2027800u;
    H->h(0x18AF0, 0, 0, 0, 0, 0, 0xB20268ECu, tag);
}
void dump(void*, void*, const void* p, uint32_t n, uint32_t a, uint32_t b) {
    const uint32_t args[7] = { 0xB2027810u, 0xB20268ECu, (uint32_t)(uintptr_t)p, n, a, b, 0 };
    H->h(0x18950, 0, 0, 0, 0, 0, args[0], args[1], args[2], args[3]);
    H->blk(0x18950, args, 7);
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_14c70(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[14], (const uint32_t*)(uintptr_t)r[16]);
    H = &e;
    const FogN04Seams saved = g_fogN04;
    g_fogN04.getThreadContext = getCtx; g_fogN04.logPrint = logPrint; g_fogN04.dumpBytes = dump;
    fog_sub_14C70((void*)(uintptr_t)r[0]);
    g_fogN04 = saved;
    r[4] = 0;
}
