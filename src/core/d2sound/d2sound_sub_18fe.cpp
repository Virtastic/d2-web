// 1.13c D2sound+0x018FE -- MSVC CRT __getptd: per-thread _tiddata via TLS, allocated on first use.
// Win32/CRT calls are host seams; error state (GetLastError) is saved/restored around the body.
#include <cstdint>

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
