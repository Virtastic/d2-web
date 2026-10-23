// Fog event-slot pool acquire (1D870) and hacklist unlist (DE00); retail structs 32-bit, guest addresses index flat `mem`.
#include <cstdint>

struct FogB14Env {
    uint8_t* mem;
    uint32_t (*waitZero)(uint32_t h);        // WaitForSingleObject(h,0)
    uint32_t (*createEvent)(void);           // CreateEventA(0,1,0,0)
    void (*f1D740)(void);                    // Fog+0x1D740 (event slot reset)
    void (*lock_note)(uint32_t cs);          // Fog+0x11690
    void (*enter)(uint32_t cs);
    void (*leave)(uint32_t cs);
    void (*free_ptr)(uint32_t* pp);          // Fog+0x1B0C0
    uint32_t (*tick)(void);                  // GetTickCount
    void (*log)(const char* fmt, uint32_t v);// Fog+0x190A0
};
static inline uint32_t& W(FogB14Env* e, uint32_t a) { return *reinterpret_cast<uint32_t*>(e->mem + a); }

// 1.13c Fog+0x1D870 -- slot = {HANDLE, int busy, int}; table 0x6FF9AF70, count 0x6FF9AF6C, enabled 0x6FF9AF68.
extern "C" uint32_t fog_sub_1D870(FogB14Env* e) {  // 1.13c Fog+0x1D870
    if (!W(e, 0x6FF9AF68)) return 0;
    uint32_t tbl = W(e, 0x6FF9AF70), n = W(e, 0x6FF9AF6C);
    for (uint32_t i = 0, p = tbl; i < n; i++, p += 12)
        if (W(e, p + 4)) { W(e, p + 4) = 0; return p; }
    for (uint32_t i = 0, p = tbl; i < n; i++, p += 12)
        if (e->waitZero(W(e, p)) == 0) { e->f1D740(); W(e, p + 4) = 0; return p; }
    if (n < 0x400) {
        uint32_t p = tbl + n * 12;
        W(e, 0x6FF9AF6C) = n + 1;
        W(e, p) = e->createEvent();
        W(e, p + 4) = 0;
        return p;
    }
    return 0;
}

// 1.13c Fog+0xDE00 -- remove every node with key==k from bucket k%W(+0x8b0); node {key,.,.,next@+0xC}.
extern "C" void fog_sub_DE00(FogB14Env* e, uint32_t self, uint32_t k) {  // 1.13c Fog+0xDE00
    if (!W(e, self + 0xBD0)) return;
    uint32_t cs = self + 0x894;
    e->lock_note(cs); e->enter(cs);
    uint32_t slot = W(e, self + 0x8AC) + (k % W(e, self + 0x8B0)) * 4;
    uint32_t prev = 0, cur = W(e, slot);
    while (cur) {
        uint32_t next = W(e, cur + 0xC);
        if (W(e, cur) == k) {
            if (prev) W(e, prev + 0xC) = next; else W(e, slot) = next;
            e->free_ptr(&cur);
            W(e, self + 0x890)--;
        } else prev = cur;
        cur = next;
    }
    e->leave(cs);
    static uint32_t last;  // DAT_6ffa6ed4
    uint32_t t = e->tick();
    if (t - last > 2000) { e->log("[HACKLIST]  Hacklist size is %d", W(e, self + 0x890)); last = t; }
}
