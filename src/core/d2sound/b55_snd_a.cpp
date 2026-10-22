// D2sound CRT/helper batch b55-snd-a. All external state goes through b55snda_Env seams.
#include <cstdint>
#include <cstddef>

struct b55snda_Env {
    void (*fn_1510)(int);                         // D2sound+0x1510
    int  heapMode;                                // DAT_6f9c6864
    void* heap;                                   // DAT_6f9c6860
    void (*lock)(int);                            // __lock
    void* (*sbhFindBlock)(void*);                 // ___sbh_find_block
    void (*sbhFreeBlock)(void*, void*);           // ___sbh_free_block
    void (*unlock4)();                            // D2sound+0x1C27
    void (*heapFree)(void*, uint32_t, void*);     // HeapFree
    void (*(*initCs)) (void*, uint32_t);          // DAT_6f9c62d0 (cached fn slot)
    int  osPlatform;                              // DAT_6f9c6004
    void* (*getModuleHandle)(const char*);
    void* (*getProcAddress)(void*, const char*);
    void (*initCsNoSpin)(void*, uint32_t);        // D2sound+0x27BD
    int* (*getPtdErrnoAddr)();                    // &__getptd()->_terrno
    void* (*nhMalloc)(size_t, int);               // D2sound+0x28DC
    int  newMode;                                 // DAT_6f9c6338
    uint32_t cookie;                              // DAT_6f9c56c0
    void (*reportFailure)();                      // D2sound+0x47B4
    int  (*sptype)(uint32_t lo, uint32_t hi);     // D2sound+0xD0CE
};
static b55snda_Env* g_b55snda;
void b55snda_SetEnv(b55snda_Env* e) { g_b55snda = e; }

// 1.13c D2sound+0x15BC
void b55snda_sub_15BC() { g_b55snda->fn_1510(10); }

// 1.13c D2sound+0x1BD4
void b55snda_free(void* mem) {
    b55snda_Env* e = g_b55snda;
    if (!mem) return;
    if (e->heapMode == 3) {
        e->lock(4);
        void* blk = e->sbhFindBlock(mem);
        if (blk) e->sbhFreeBlock(blk, mem);
        e->unlock4();
        if (blk) return;
    }
    e->heapFree(e->heap, 0, mem);
}

// 1.13c D2sound+0x27CD
void b55snda_crtInitCritSecAndSpinCount(void* cs, uint32_t spin) {
    b55snda_Env* e = g_b55snda;
    if (!*e->initCs) {
        if (e->osPlatform != 1) {
            void* h = e->getModuleHandle("kernel32.dll");
            if (h) {
                *e->initCs = (void (*)(void*, uint32_t))e->getProcAddress(h, "InitializeCriticalSectionAndSpinCount");
                if (*e->initCs) goto call;
            }
        }
        *e->initCs = e->initCsNoSpin;
    }
call:
    (*e->initCs)(cs, spin);
}

// 1.13c D2sound+0x2858
int* b55snda_sub_2858() { return g_b55snda->getPtdErrnoAddr(); }

// 1.13c D2sound+0x2908
void* b55snda_malloc(size_t size) { return g_b55snda->nhMalloc(size, g_b55snda->newMode); }

// 1.13c D2sound+0x47E5
void b55snda_security_check_cookie(uint32_t c) {
    if (c == g_b55snda->cookie) return;
    g_b55snda->reportFailure();
}

// 1.13c D2sound+0xD008
int b55snda_fpclass(double x) {
    uint64_t b = *(uint64_t*)&x;
    uint32_t lo = (uint32_t)b, hi = (uint32_t)(b >> 32);
    uint32_t neg = (b >> 63) ? 0xFFFFFFFFu : 0;
    if (((b >> 48) & 0x7ff0) == 0x7ff0) {
        int t = g_b55snda->sptype(lo, hi);
        if (t == 1) return 0x200;
        if (t == 2) return 4;
        if (t != 3) return 1;
        return 2;
    }
    if ((b & 0x7ff0000000000000ull) == 0 && ((b & 0xfffff00000000ull) != 0 || lo != 0))
        return (int)((neg & 0xffffff90u) + 0x80);
    if ((b & 0x7fffffffffffffffull) == 0) // _X == 0.0 (DAT_6f9c0988)
        return (int)((neg & 0xffffffe0u) + 0x40);
    return (int)((neg & 0xffffff08u) + 0x100);
}
