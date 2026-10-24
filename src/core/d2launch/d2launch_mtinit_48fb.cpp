// D2Launch CRT __mtinit. Win32 callees are explicit host seams (style: d2launch_crt_l03.cpp).
#include <cstdint>

struct MtInitEnv {
    int32_t (*mtinitlocks)();                  // 0x33DB
    void (*mtterm)();                          // 0x46E4
    uint32_t (*flsAlloc)(void (*dtor)(void*)); // FlsAlloc, or the TlsAlloc shim at 0x46DB when FlsGetValue is absent
    int32_t (*flsSet)(uint32_t, void*);        // FlsSetValue / TlsSetValue
    void* (*calloc)(uint32_t n, uint32_t sz);  // 0x49EA
    uint32_t (*threadId)();                    // GetCurrentThreadId
    void (*freefls)(void*);                    // 0x4785
    const void* defaultLocInfo;                // DAT_6fa60590
};
static uint32_t g_flsIndex;                    // DAT_6fa60560

// 1.13c D2Launch+0x048FB
extern "C" int32_t d2launch_mtinit(const MtInitEnv* e) {
    if (e->mtinitlocks() == 0) { e->mtterm(); return 0; }
    g_flsIndex = e->flsAlloc(e->freefls);
    if (g_flsIndex != 0xFFFFFFFFu) {
        uint32_t* t = (uint32_t*)e->calloc(1, 0x8c);
        if (t && e->flsSet(g_flsIndex, t)) {
            t[0x15] = (uint32_t)(uintptr_t)e->defaultLocInfo;
            t[5] = 1; t[1] = 0xFFFFFFFFu; t[0] = e->threadId();
            return 1;
        }
    }
    e->mtterm();
    return 0;
}
