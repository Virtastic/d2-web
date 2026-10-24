// D2DDraw statically linked MSVC 2003 CRT helpers (batch n07). Win32/CRT services are host seams in Env.
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct d2ddraw_crt_n07_Env {
    uint32_t flsIndex;                              // DAT_6f8ce158 (-1 = none)
    void* (*flsGetValue)(uint32_t);                 // DAT_6f8cf094
    int (*flsSetValue)(uint32_t, void*);            // DAT_6f8cf098
    uint32_t (*flsAlloc)(void (*)(void*));          // DAT_6f8cf090
    void (*freeFls)(void*);                         // __freefls@4
    int (*mtinitlocks)();                           // 0x1B8E
    void (*mtterm)();                               // 0x1979
    uint32_t (*getCurrentThreadId)();
    void* defaultLocale;                            // &DAT_6f8ce190
    uint32_t heapMode;                              // DAT_6f8d04e0 (3 = small block heap)
    uint32_t sbhThreshold;                          // DAT_6f8d04cc
    uint32_t sbhFlag;                               // DAT_6f8cf1b4 (new handler installed)
    void (*lock)(int);                              // 0x2B0B
    void (*unlockHeap)();                           // 0x1EE7 / 0x1FCB
    uint32_t* (*sbhFindBlock)(void*);
    void (*sbhFreeBlock)(uint32_t*, void*);
    void* (*sbhAllocBlock)(uint32_t);
    void* (*heapAllocZero)(uint32_t);               // HeapAlloc(HEAP_ZERO_MEMORY)
    void (*heapFree)(void*);
    int (*callnewh)(uint32_t);
    void* (*malloc_)(uint32_t);
    void (*free_)(void*);
    uint32_t cookie;                                // DAT_6f8ce6c0
    void (*fatalExit)(int);                         // 0x560C + ExitProcess(3)
};

// 1.13c D2DDraw+0x1D76
void d2ddraw_freeptd(d2ddraw_crt_n07_Env* e, void* ptd)
{
    if (e->flsIndex == 0xffffffffu) return;
    if (!ptd) ptd = e->flsGetValue(e->flsIndex);
    e->freeFls(ptd);
    e->flsSetValue(e->flsIndex, nullptr);
}

// 1.13c D2DDraw+0x1F05  (calloc core; sizes in 32-bit units as retail)
void* d2ddraw_calloc(d2ddraw_crt_n07_Env* e, uint32_t n, uint32_t sz)
{
    if (n != 0 && sz > 0xffffffe0u / n) return nullptr;
    uint32_t size = n * sz, bytes = size ? size : 1;
    for (;;) {
        void* p = nullptr;
        if (bytes < 0xffffffe1u) {
            if (e->heapMode == 3) {
                bytes = (bytes + 0xf) & 0xfffffff0u;
                if (size <= e->sbhThreshold) {
                    e->lock(4);
                    p = e->sbhAllocBlock(size);
                    e->unlockHeap();
                    if (p) { memset(p, 0, size); return p; }
                }
            }
            p = e->heapAllocZero(bytes);
        }
        if (p) return p;
        if (e->sbhFlag == 0) return nullptr;
        if (e->callnewh(bytes) == 0) return nullptr;
    }
}

// 1.13c D2DDraw+0x1DA5
int d2ddraw_mtinit(d2ddraw_crt_n07_Env* e, uint32_t* ptdOut)
{
    if (!e->mtinitlocks()) { e->mtterm(); return 0; }
    e->flsIndex = e->flsAlloc(e->freeFls ? (void (*)(void*))e->freeFls : nullptr);
    if (e->flsIndex != 0xffffffffu) {
        uint32_t* p = (uint32_t*)d2ddraw_calloc(e, 1, 0x8c);
        if (p && e->flsSetValue(e->flsIndex, p)) {
            p[0x15] = (uint32_t)(uintptr_t)e->defaultLocale;
            p[5] = 1;
            p[1] = 0xffffffffu;
            p[0] = e->getCurrentThreadId();
            if (ptdOut) *ptdOut = (uint32_t)(uintptr_t)p;
            return 1;
        }
    }
    e->mtterm();
    return 0;
}

// 1.13c D2DDraw+0x1E94
void d2ddraw_free(d2ddraw_crt_n07_Env* e, void* mem)
{
    if (!mem) return;
    if (e->heapMode == 3) {
        e->lock(4);
        uint32_t* hdr = e->sbhFindBlock(mem);
        if (hdr) e->sbhFreeBlock(hdr, mem);
        e->unlockHeap();
        if (hdr) return;
    }
    e->heapFree(mem);
}

// ioinfo slot: 0x24 bytes = handle, flags(byte)+pad, lockInit, CRITICAL_SECTION(24)
struct d2ddraw_ioinfo { int32_t osfhnd; uint8_t osfile; uint8_t pipech; uint8_t pad[2]; int32_t lockinitflag; uint8_t cs[24]; };
static_assert(sizeof(d2ddraw_ioinfo) == 0x24, "ioinfo");

struct d2ddraw_io_Env {
    void* (*malloc_)(uint32_t);
    void (*free_)(void*);
    d2ddraw_ioinfo* pioinfo[0x40];                  // DAT_6f8d0500
    int32_t nhandle;                                // DAT_6f8d04e4
    const uint8_t* lpReserved2; uint16_t cbReserved2; // GetStartupInfoA
    uint32_t (*getFileType)(int32_t h);
    int32_t (*getStdHandle)(uint32_t id);
    int (*initCritSec)(void* cs, uint32_t spin);    // ___crtInitCritSecAndSpinCount
    void (*deleteCritSec)(void* cs);
    void (*setHandleCount)(int32_t);
};

static void io_initpage(d2ddraw_ioinfo* p)
{
    for (int i = 0; i < 0x20; i++) { p[i].osfhnd = -1; p[i].lockinitflag = 0; p[i].osfile = 0; p[i].pipech = 10; }
}

// 1.13c D2DDraw+0x1FDC
int d2ddraw_ioinit(d2ddraw_io_Env* e)
{
    d2ddraw_ioinfo* pg = (d2ddraw_ioinfo*)e->malloc_(0x480);
    if (!pg) return -1;
    e->nhandle = 0x20; e->pioinfo[0] = pg; io_initpage(pg);
    if (e->cbReserved2 != 0 && e->lpReserved2) {
        int32_t n; memcpy(&n, e->lpReserved2, 4);
        const uint8_t* flags = e->lpReserved2 + 4;
        const uint8_t* hnd = flags + n;
        if (n > 0x7ff) n = 0x800;
        int32_t cnt = n;
        for (int pi = 1; e->nhandle < n; pi++) {
            d2ddraw_ioinfo* q = (d2ddraw_ioinfo*)e->malloc_(0x480);
            if (!q) { cnt = e->nhandle; break; }
            e->nhandle += 0x20; e->pioinfo[pi] = q; io_initpage(q);
        }
        for (int32_t i = 0; i < cnt; i++, hnd += 4, flags++) {
            int32_t h; memcpy(&h, hnd, 4);
            if (h != -1 && (*flags & 1) && ((*flags & 8) || e->getFileType(h) != 0)) {
                d2ddraw_ioinfo* s = &e->pioinfo[i >> 5][i & 0x1f];
                s->osfhnd = h; s->osfile = *flags;
                if (!e->initCritSec(s->cs, 4000)) return -1;
                s->lockinitflag++;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        d2ddraw_ioinfo* s = &e->pioinfo[0][i];
        if (s->osfhnd == -1) {
            s->osfile = 0x81;
            uint32_t id = i == 0 ? 0xfffffff6u : 0xfffffff5u - (i != 1);
            int32_t h = e->getStdHandle(id);
            uint32_t t;
            if (h == -1 || (t = e->getFileType(h)) == 0) s->osfile |= 0x40;
            else {
                s->osfhnd = h;
                if ((t & 0xff) == 2) s->osfile |= 0x40; else if ((t & 0xff) == 3) s->osfile |= 8;
                if (!e->initCritSec(s->cs, 4000)) return -1;
                s->lockinitflag++;
            }
        } else s->osfile |= 0x80;
    }
    e->setHandleCount(e->nhandle);
    return 0;
}

// 1.13c D2DDraw+0x21DA
void d2ddraw_ioterm(d2ddraw_io_Env* e)
{
    for (int i = 0; i < 0x40; i++) {
        d2ddraw_ioinfo* pg = e->pioinfo[i];
        if (!pg) continue;
        for (int j = 0; j < 0x20; j++) if (pg[j].lockinitflag) e->deleteCritSec(pg[j].cs);
        e->free_(pg);
        e->pioinfo[i] = nullptr;
    }
}

// 1.13c D2DDraw+0x2226
// env block: double-NUL list. Returns 0 ok, -1 failure; envp is NULL-terminated array.
int d2ddraw_setenvp(const d2ddraw_crt_n07_Env* e, char*& block, char**& envp, int& envInitialized)
{
    if (!block) return -1;
    int n = 0;
    for (char* p = block; *p; p += strlen(p) + 1) if (*p != '=') n++;
    char** out = envp = (char**)e->malloc_((n + 1) * 4);
    if (!out) return -1;
    for (char* p = block; *p; p += strlen(p) + 1) {
        if (*p == '=') continue;
        char* s = (char*)e->malloc_((uint32_t)strlen(p) + 1);
        if (!s) { e->free_(envp); envp = nullptr; return -1; }
        strcpy(s, p);
        *out++ = s;
    }
    e->free_(block); block = nullptr;
    *out = nullptr;
    envInitialized = 1;
    return 0;
}

// 1.13c D2DDraw+0x4794
[[noreturn]] void d2ddraw_report_failure(const d2ddraw_crt_n07_Env* e)
{
    e->fatalExit(1);   // FUN_6f8c560c(1) then ExitProcess(3)
    for (;;) {}
}

// 1.13c D2DDraw+0x47C5
void d2ddraw_security_check_cookie(const d2ddraw_crt_n07_Env* e, uint32_t c)
{
    if (c == e->cookie) return;
    d2ddraw_report_failure(e);
}

// (dup of D2Launch crt n21 binding) D2DDraw+0x47E0
void* d2ddraw_memmove(void* dst, const void* src, uint32_t n)
{
    return memmove(dst, src, n);   // retail's copy order only matters for overlap, which memmove honours
}

// (dup of D2Launch crt n21 binding) D2DDraw+0x4B1D
void d2ddraw_free_lc_time(const d2ddraw_crt_n07_Env* e, void** p)
{
    if (!p) return;
    static const uint8_t order[] = {1,2,3,4,5,6,0,8,9,10,11,12,13,7,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42};
    for (uint8_t i : order) e->free_(p[i]);
}

// (dup of D2Launch crt n21 binding) D2DDraw+0x4CAD
// c_locale = default lconv strings (PTR_PTR_6f8ce7cc), emptyStr[3] = PTR_DAT_6f8ce79c/a0/a4
void d2ddraw_free_lconv_num(const d2ddraw_crt_n07_Env* e, void** p, void* const* c_locale, void* const* emptyStr)
{
    if (!p) return;
    for (int i = 0; i < 3; i++) if (p[i] != c_locale[i] && p[i] != emptyStr[i]) e->free_(p[i]);
}
