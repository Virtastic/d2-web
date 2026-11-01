// 1.13c D2sound+0x018FE -- MSVC CRT __getptd: per-thread _tiddata via TLS, allocated on first use.
// Win32/CRT calls are host seams; error state (GetLastError) is saved/restored around the body.
#include <cstdint>
#include <cstdlib>

struct D2sndPtd { uint32_t tid, thandle, terrno, tdoserrno, fpds, holdrand; uint32_t pad[11]; const void* initaddr; uint32_t rest[19]; };

struct D2sndGetPtdHost {
    uint32_t (*getLastError)();
    void (*setLastError)(uint32_t);
    void* (*tlsGet)();               // (*DAT_6f9c61b4)(tlsIndex)
    int (*tlsSet)(void*);            // (*DAT_6f9c61b8)(tlsIndex, p)
    void* (*calloc)(uint32_t, uint32_t); // 0x1C45
    uint32_t (*getCurrentThreadId)();
    void (*amsgExit)(int);           // 0x143F
    const void* initAddr;            // &DAT_6f9c5190
};

D2sndPtd* D2sound_18FE(const D2sndGetPtdHost& h) {
    uint32_t err = h.getLastError();
    D2sndPtd* p = (D2sndPtd*)h.tlsGet();
    if (!p) {
        p = (D2sndPtd*)h.calloc(1, 0x8c);
        if (p && h.tlsSet(p)) {
            p->initaddr = h.initAddr;
            p->holdrand = 1;
            p->thandle = 0xffffffff;
            p->tid = h.getCurrentThreadId();
        } else
            h.amsgExit(0x10);
    }
    h.setLastError(err);
    return p;
}

// One thread in the wasm build: the TLS slot holds the single _tiddata that __getptd creates on first use.
static void* g_tlsSlot;
static uint32_t hostGetLastError() { return 0; }
static void hostSetLastError(uint32_t) {}
static void* hostTlsGet() { return g_tlsSlot; }
static int hostTlsSet(void* p) { g_tlsSlot = p; return 1; }
static void* hostCalloc(uint32_t n, uint32_t s) { return calloc(n, s); }
static uint32_t hostThreadId() { return 1; }
static void hostAmsgExit(int) { abort(); }
static const uint32_t g_initAddrStub = 0;   // stands in for &DAT_6f9c5190 (the CRT init address)

// 1.13c D2sound+0x11B8 (_rand) reads and advances the holdrand word (_tiddata +0x14) of the block __getptd returns.
uint32_t* d2s_holdrand_slot(void) {
    static const D2sndGetPtdHost host = { hostGetLastError, hostSetLastError, hostTlsGet, hostTlsSet,
                                          hostCalloc, hostThreadId, hostAmsgExit, &g_initAddrStub };
    return &D2sound_18FE(host)->holdrand;
}
