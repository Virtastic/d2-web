// probe for Fog 0x15C00 (imagehlp stack walk trigger): regs eax = thread handle (r[0]); blobs g r[14] (the five globals Fog+0x56E54.., then the
// registry flag NoStackDump at +20), lg r[16] (hook log).
// The port is fog_stack_walk_15C00 with a host env: the registry read goes through hook 0xCE66 (the flag written through stack word 3 as retail's
// out-parameter), every log line is a 0x190A0 record, and the kernel32 calls are the descriptor's pinned imports (not logged).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "hookenv.h"
struct FogStackGlobals { uint32_t imagehlp, thread, trigger, done, dup; };
struct FogStackEnv {
    uint32_t (*noStackDump)();
    void (*log)(const char* msg);
    uint32_t (*currentThread)();
    void (*dupHandle)(uint32_t thread, uint32_t* out);
    void (*setEvent)(uint32_t ev);
    int32_t (*getPriority)(uint32_t h);
    void (*setPriority)(uint32_t h, int32_t p);
    void (*wait)(uint32_t ev);
    void (*closeHandle)(uint32_t h);
};
void fog_stack_walk_15C00(uint32_t thread, FogStackGlobals* g, const FogStackEnv* e);
namespace {
HookEnv* H;
int32_t gPrio;
uint32_t rdFlag() {
    uint32_t v = 0;
    H->h(0xCE66, 0, 0, 0, 0, 0, 0xB2026FD0u, 0xB20278D0u, 0, (uint32_t)(uintptr_t)&v);   // the queued flag (hookw 0xce66 3 V)
    return v;
}
void lg(const char* m) {
    static const struct { const char* s; uint32_t rva; } t[] = {
        { "sDumpStack: Stack dump turned off in registry", 0x278A0 }, { "sDumpStack: imagehlp.dll not loaded", 0x2787C },
        { "No event to trigger stack walk thread", 0x27854 }, { "No stack walk thread", 0x2783C },
        { "", 0x268EC }, { "ImageHelp Stack Crawl:", 0x27824 } };
    for (auto& e : t) if (!std::strcmp(e.s, m)) { H->h(0x190A0, 0, 0, 0, 0, 0, 0xB20268ECu, 0xB2000000u | e.rva); return; }
}
uint32_t cur() { return 0x77; }
void dup(uint32_t, uint32_t*) {}
void ev(uint32_t) {}
int32_t gp(uint32_t) { return gPrio; }
void sp(uint32_t, int32_t) {}
void wt(uint32_t) {}
void cl(uint32_t) {}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_15c00(uint32_t* r) {
    FogStackGlobals* g = (FogStackGlobals*)(uintptr_t)r[14];
    const uint32_t flag = ((const uint32_t*)(uintptr_t)r[14])[5];
    uint32_t sc[] = { (4u << 24) | 0xCE66u, flag, 0, 0 };
    HookEnv e((uint32_t*)(uintptr_t)r[16], sc);
    H = &e;
    gPrio = 0;   // the descriptor's GetThreadPriority pin is invisible to the log
    const FogStackEnv env = { rdFlag, lg, cur, dup, ev, gp, sp, wt, cl };
    fog_stack_walk_15C00(r[0], g, &env);
    r[4] = 0;
}
