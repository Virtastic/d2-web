#include <cstdint>
#include <cstring>
#include <initializer_list>

// Host-CRT glue of D2Common, written against env seams (guest globals and Win32 calls are host hooks).
// Not wired into callers: these run only inside retail's DLL startup path.

// 1.13c D2Common+0x02C8F entry(hinst, reason, reserved): DllMain wrapper around CRT init and the game's own init.
struct D2CommonEntryEnv {
    int32_t gotInitFlag;                      // DAT 0x5E8 (nonzero once the CRT was started)
    int32_t (*preHook)(void*, int32_t, void*);  // optional DAT 0xF2848, null if unset
    int32_t (*crtInit)(void*, int32_t);       // __CRT_INIT@12
    int32_t (*dllMain)(void*, int32_t);       // sub_70F80
};
int32_t d2common_entry_2c8f(const D2CommonEntryEnv& e, void* h, int32_t reason, void* res)
{
    int32_t r = 1;
    if (reason == 0 && e.gotInitFlag == 0) return 0;
    if (reason == 1 || reason == 2) {
        if (e.preHook) r = e.preHook(h, reason, res);
        if (r == 0 || e.crtInit(h, reason) == 0) return 0;
    }
    r = e.dllMain(h, reason);
    if (reason == 1 && r == 0) e.crtInit(h, 0);
    if (reason == 0 || reason == 3) {
        if (e.crtInit(h, reason) == 0) r = 0;
        if (r != 0 && e.preHook) r = e.preHook(h, reason, res);
    }
    return r;
}

// 1.13c D2Common+0x0300A _atexit: 0 when __onexit accepted the function, else -1.
int32_t d2common_atexit_300a(uintptr_t (*onexit)(uintptr_t), uintptr_t fn) { return (onexit(fn) != 0) - 1; }

// 1.13c D2Common+0x0301C __RTC_Initialize: the RTC init-table is empty (begin == end), loop never runs.
void d2common_rtc_initialize_301c() {}

// 1.13c D2Common+0x03445 __mtterm: free the FLS/TLS slot if allocated, then delete the CRT locks.
struct D2CommonMtTermEnv { int32_t* flsIndex; void (*flsFree)(int32_t); void (*deleteLocks)(); };
void d2common_mtterm_3445(const D2CommonMtTermEnv& e)
{
    if (*e.flsIndex != -1) { e.flsFree(*e.flsIndex); *e.flsIndex = -1; }
    e.deleteLocks();
}

// 1.13c D2Common+0x03462 __initptd: per-thread data gets the init address and rand seed 1 (offsets 0x54, 0x14).
void d2common_initptd_3462(uint8_t* ptd, uint32_t initAddr)
{
    std::memcpy(ptd + 0x54, &initAddr, 4);
    const uint32_t one = 1;
    std::memcpy(ptd + 0x14, &one, 4);
}

// 1.13c D2Common+0x034E6 __freefls: release a thread's CRT block (32-bit layout; pointers passed as u32 via env).
struct D2CommonFreeFlsEnv {
    void (*free)(uint32_t);
    void (*lock)(int32_t);
    void (*unlockLocale)();     // 0x3618 (lock 13 release)
    void (*unlockLconv)();      // 0x3624 (lock 12 release)
    void (*freeTlocInfo)(uint32_t);
    uint32_t (*rd)(uint32_t);   // read u32 guest memory
    void (*wr)(uint32_t, uint32_t);
    uint32_t defaultPtd54;      // &DAT 0xE9720
    uint32_t defaultLconv;      // DAT 0xF14C8
    uint32_t defaultTloc1;      // PTR DAT 0xE91C4
    uint32_t defaultTloc2;      // &DAT 0xE9170
};
void d2common_freefls_34e6(const D2CommonFreeFlsEnv& e, uint32_t p)
{
    if (!p) return;
    for (uint32_t off : {0x24u, 0x2cu, 0x34u, 0x3cu, 0x44u, 0x48u}) {
        uint32_t q = e.rd(p + off);
        if (q) e.free(q);
    }
    if (e.rd(p + 0x54) != e.defaultPtd54) e.free(e.rd(p + 0x54));
    e.lock(0xd);
    uint32_t c = e.rd(p + 0x60);
    if (c) {
        e.wr(c, e.rd(c) - 1);
        if (e.rd(c) == 0 && c != e.defaultLconv) e.free(c);
    }
    e.unlockLocale();
    e.lock(0xc);
    uint32_t t = e.rd(p + 0x64);
    if (t) {
        e.wr(t, e.rd(t) - 1);
        for (uint32_t i : {0xbu, 0xdu, 0xcu, 0x10u}) {
            uint32_t r = e.rd(t + i * 4);
            if (r) e.wr(r, e.rd(r) - 1);
        }
        uint32_t m = e.rd(t + 0x13 * 4);
        e.wr(m + 0xb4, e.rd(m + 0xb4) - 1);
        if (t != e.defaultTloc1 && t != e.defaultTloc2 && e.rd(t) == 0) e.freeTlocInfo(t);
    }
    e.unlockLconv();
    e.free(p);
}

// 1.13c D2Common+0x0362D __freeptd: free the calling thread's block (or the given one) and clear the slot.
struct D2CommonFreePtdEnv {
    int32_t flsIndex;
    uint32_t (*flsGet)(int32_t);
    void (*flsSet)(int32_t, uint32_t);
    void (*freeFls)(uint32_t);
};
void d2common_freeptd_362d(const D2CommonFreePtdEnv& e, uint32_t ptd)
{
    if (e.flsIndex == -1) return;
    if (!ptd) ptd = e.flsGet(e.flsIndex);
    e.freeFls(ptd);
    e.flsSet(e.flsIndex, 0);
}

// 1.13c D2Common+0x0365C __mtinit: set up locks, resolve Fls* (fall back to Tls*), allocate the main thread's block.
struct D2CommonMtInitEnv {
    int32_t (*initLocks)();
    void (*mtterm)();
    bool (*resolveFls)();        // GetModuleHandleA/GetProcAddress for FlsAlloc/FlsGetValue/FlsSetValue/FlsFree; false => caller installs Tls fallback
    void (*installTlsFallback)();
    int32_t (*flsAlloc)();       // with __freefls as destructor
    uint8_t* (*calloc1)(uint32_t, uint32_t);
    int32_t (*flsSet)(int32_t, uint8_t*);
    uint32_t (*threadId)();
    int32_t* flsIndex;
    uint32_t defaultPtd54;
};
int32_t d2common_mtinit_365c(const D2CommonMtInitEnv& e)
{
    if (!e.initLocks()) { e.mtterm(); return 0; }
    if (!e.resolveFls()) e.installTlsFallback();
    *e.flsIndex = e.flsAlloc();
    if (*e.flsIndex != -1) {
        uint8_t* b = e.calloc1(1, 0x8c);
        if (b && e.flsSet(*e.flsIndex, b)) {
            std::memcpy(b + 0x54, &e.defaultPtd54, 4);
            const uint32_t one = 1, neg = 0xffffffffu, tid = e.threadId();
            std::memcpy(b + 0x14, &one, 4);
            std::memcpy(b + 4, &neg, 4);
            std::memcpy(b, &tid, 4);
            return 1;
        }
    }
    e.mtterm();
    return 0;
}

// 1.13c D2Common+0x0559F __ioterm: for each of 32 io-block slots, delete the critical sections of live entries
// (36-byte entries, 32 per block, flag at +8, section at +0xC), then free the block and null the slot.
struct D2CommonIoTermEnv {
    uint32_t* slots;                       // 32 u32 block pointers
    uint32_t (*rd)(uint32_t);
    void (*deleteCs)(uint32_t);
    void (*free)(uint32_t);
};
void d2common_ioterm_559f(const D2CommonIoTermEnv& e)
{
    for (int i = 0; i < 32; i++) {
        uint32_t b = e.slots[i];
        if (!b) continue;
        for (uint32_t p = b; p < b + 0x480; p += 0x24)
            if (e.rd(p + 8)) e.deleteCs(p + 0xc);
        e.free(b);
        e.slots[i] = 0;
    }
}

// 1.13c D2Common+0x055EB __setenvp: split the NUL-separated environment block into a pointer array, skipping
// entries starting with '='. Returns 0 on success, -1 on failure; out receives the array (null-terminated).
int32_t d2common_setenvp_55eb(char* block, char**& out, char* (*alloc)(size_t), void (*release)(void*))
{
    if (!block) return -1;
    size_t n = 0;
    for (char* p = block; *p; p += std::strlen(p) + 1) if (*p != '=') n++;
    char** arr = reinterpret_cast<char**>(alloc((n + 1) * sizeof(char*)));
    out = arr;
    if (!arr) return -1;
    char** w = arr;
    for (char* p = block; *p;) {
        size_t l = std::strlen(p);
        if (*p != '=') {
            char* s = alloc(l + 1);
            *w = s;
            if (!s) { release(out); out = nullptr; return -1; }
            std::memcpy(s, p, l + 1);
            w++;
        }
        p += l + 1;
    }
    release(block);
    *w = nullptr;
    return 0;
}

// 1.13c D2Common+0x042A2 __stbuf: give stdout/stderr (only when a tty) a 4 KiB temp buffer, or the 2-byte
// in-FILE charbuf if allocation fails. FILE fields as in the 32-bit CRT layout via env accessors.
struct D2CommonStbufFile { uint8_t* ptr; int32_t cnt; uint8_t* base; int32_t flag; int32_t file; int32_t bufsiz; uint8_t* charbuf; };
int32_t d2common_stbuf_42a2(D2CommonStbufFile* f, bool isStdout, bool isStderr, bool (*isatty)(int32_t),
                            int32_t& tmpCount, uint8_t* tmpBuf[2], uint8_t* (*alloc)(size_t))
{
    if (!isatty(f->file)) return 0;
    int idx;
    if (isStdout) idx = 0; else if (isStderr) idx = 1; else return 0;
    tmpCount++;
    if (f->flag & 0x10c) return 0;
    if (!tmpBuf[idx]) {
        tmpBuf[idx] = alloc(0x1000);
        if (!tmpBuf[idx]) {
            f->base = f->ptr = f->charbuf;
            f->bufsiz = f->cnt = 2;
            f->flag |= 0x1102;
            return 1;
        }
    }
    f->base = f->ptr = tmpBuf[idx];
    f->bufsiz = f->cnt = 0x1000;
    f->flag |= 0x1102;
    return 1;
}
