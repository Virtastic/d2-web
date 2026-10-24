// src/core/d2sound/d2sound_crt_n24.cpp -- D2Sound.dll's statically linked MSVC 2003 CRT: env strings,
// heap init/term, realloc, SEH filters, locale refcounts, MBCS code page setup. Win32/heap-manager calls
// are host seams passed as arguments; CRT globals are the structs below (retail addresses in comments).
#include <cstdint>
#include <cstring>

typedef uint32_t u32;

// ---- 0x223B -------------------------------------------------------------------------------------
struct EnvHost {
    const uint16_t* (*getEnvW)();                       // GetEnvironmentStringsW
    void (*freeEnvW)(const uint16_t*);
    uint32_t (*lastError)();
    const char* (*getEnvA)();                           // GetEnvironmentStringsA
    void (*freeEnvA)(const char*);
    void* (*malloc_)(size_t);
    void (*free_)(void*);
    int (*wcToMb)(const uint16_t*, int, char*, int);    // WideCharToMultiByte(0,0,..)
};
static int g_envMode;                                   // 0x6F9C62C8: 0 unknown, 1 wide, 2 ansi

// 1.13c D2sound+0x223B
void* d2s_crtGetEnvironmentStringsA(const EnvHost& h) {
    const uint16_t* w = nullptr;
    bool wide = false;
    if (g_envMode == 0) {
        w = h.getEnvW();
        if (w) { g_envMode = 1; wide = true; }
        else if (h.lastError() == 0x78) g_envMode = 2;
    }
    if (!wide && g_envMode != 1) {
        if (g_envMode != 2 && g_envMode != 0) return nullptr;
        const char* src = h.getEnvA();
        if (!src) return nullptr;
        const char* p = src;
        while (*p) p += strlen(p) + 1;
        size_t n = (size_t)(p + 1 - src);
        void* d = h.malloc_(n);
        if (d) memcpy(d, src, n);
        h.freeEnvA(src);
        return d;
    }
    if (!w && !(w = h.getEnvW())) return nullptr;
    const uint16_t* p = w;
    while (*p) { while (*p) ++p; ++p; }
    int cch = (int)(p - w) + 1;
    int size = h.wcToMb(w, cch, nullptr, 0);
    char* out = nullptr;
    if (size) {
        char* b = (char*)h.malloc_((size_t)size);
        if (b) {
            out = b;
            if (h.wcToMb(w, cch, b, size) == 0) { h.free_(b); out = nullptr; }
        }
    }
    h.freeEnvW(w);
    return out;
}

// ---- 0x2377 is ported elsewhere; heap_term 0x23C8 -----------------------------------------------
struct SbhRegion { u32 pad[3]; void* reserved; void* desc; u32 pad2; };   // 20 bytes; +0xC, +0x10
struct HeapHost {
    void* (*heapCreate)(u32 flags, size_t init, size_t max);
    void (*heapDestroy)(void*);
    bool (*heapFree)(void*, u32, void*);
    void (*virtualFree)(void*, size_t, u32);
    void* (*malloc_)(size_t);
    void (*free_)(void*);
    void* (*realloc_)(void*, size_t);                   // HeapReAlloc(heap,0,p,n)
    int (*callnewh)(size_t);
    void (*lock4)();
    void (*unlock4)();
    void* (*sbhFindBlock)(void*);
    int (*sbhResizeBlock)(void*, void*, size_t);
    void* (*sbhAllocBlock)(size_t);
    void (*sbhFreeBlock)(void*, void*);
    void* (*heapAlloc)(void*, u32, size_t);
};
static void* g_hHeap;                                   // 0x6F9C6860
static int g_heapKind;                                  // 0x6F9C6864 (3 = small-block heap)
static int g_sbhRegionCount;                            // 0x6F9C6618
static char* g_sbhRegions;                              // 0x6F9C661C
static u32 g_sbhThreshold;                              // 0x6F9C6620
static int g_newMode;                                   // 0x6F9C6338

// 1.13c D2sound+0x23C8
void d2s_heap_term(const HeapHost& h) {
    if (g_heapKind == 3) {
        char* p = g_sbhRegions;
        for (int i = 0; i < g_sbhRegionCount; ++i, p += 20) {
            void* res = *(void**)(p + 0xC);
            h.virtualFree(res, 0x100000, 0x4000);
            h.virtualFree(res, 0, 0x8000);
            h.heapFree(g_hHeap, 0, *(void**)(p + 0x10));
        }
        h.heapFree(g_hHeap, 0, g_sbhRegions);
    }
    h.heapDestroy(g_hHeap);
}

// ---- 0x248D --------------------------------------------------------------------------------------
struct XcptAct { u32 code; u32 sig; void* handler; };
struct Ptd { XcptAct* initaddr; void* initarg; uintptr_t pxcptacttab; };
static u32 g_xcptCount = 5, g_fpeFirst, g_fpeCount;    // 0x6F9C5214, 5208, 520C

// 1.13c D2sound+0x248D
int d2s_XcptFilter(Ptd* ptd, u32 num, void* exc, int (*unhandled)(void*)) {
    XcptAct* t = ptd->initaddr;
    XcptAct* e = t;
    while (e < t + g_xcptCount && e->code != num) ++e;
    if (e >= t + g_xcptCount || e->code != num) e = nullptr;
    if (!e || !e->handler) return unhandled(exc);
    uintptr_t hv = (uintptr_t)e->handler;
    if (hv == 5) { e->handler = nullptr; return 1; }
    if (hv != 1) {
        void* saved = ptd->initarg;
        ptd->initarg = exc;
        if (e->sig == 8) {
            for (u32 i = g_fpeFirst; i < g_fpeFirst + g_fpeCount; ++i) ptd->initaddr[i].handler = nullptr;
            uintptr_t old = ptd->pxcptacttab;
            switch (e->code) {
                case 0xc000008e: ptd->pxcptacttab = 0x83; break;
                case 0xc0000090: ptd->pxcptacttab = 0x81; break;
                case 0xc0000091: ptd->pxcptacttab = 0x84; break;
                case 0xc0000093: ptd->pxcptacttab = 0x85; break;
                case 0xc000008d: ptd->pxcptacttab = 0x82; break;
                case 0xc000008f: ptd->pxcptacttab = 0x86; break;
                case 0xc0000092: ptd->pxcptacttab = 0x8a; break;
            }
            ((void (*)(int, uintptr_t))hv)(8, ptd->pxcptacttab);
            ptd->pxcptacttab = old;
        } else {
            e->handler = nullptr;
            ((void (*)(int))hv)((int)e->sig);
        }
        ptd->initarg = saved;
    }
    return -1;
}

// ---- 0x2A0A realloc ------------------------------------------------------------------------------
// 1.13c D2sound+0x2A0A
void* d2s_realloc(const HeapHost& h, void* p, size_t n) {
    if (!p) return h.malloc_(n);
    if (!n) { h.free_(p); return nullptr; }
    if (g_heapKind == 3) {
        for (;;) {
            void* out = nullptr;
            if (n > 0xffffffe0u) { h.callnewh(n); return nullptr; }
            h.lock4();
            void* blk = h.sbhFindBlock(p);
            if (blk) {
                size_t old = *((u32*)p - 1) - 1;
                if (n <= g_sbhThreshold) {
                    if (h.sbhResizeBlock(blk, p, n) == 0) {
                        out = h.sbhAllocBlock(n);
                        if (out) {
                            memcpy(out, p, n <= old ? n : old);
                            h.sbhFreeBlock(h.sbhFindBlock(p), p);
                        }
                    } else out = p;
                }
                if (!out) {
                    n = ((n ? n : 1) + 0xf) & ~(size_t)0xf;
                    out = h.heapAlloc(g_hHeap, 0, n);
                    if (out) {
                        memcpy(out, p, n <= old ? n : old);
                        h.sbhFreeBlock(blk, p);
                    }
                }
            }
            h.unlock4();
            if (!blk) {
                n = ((n ? n : 1) + 0xf) & ~(size_t)0xf;
                out = h.realloc_(p, n);
            }
            if (out) return out;
            if (!g_newMode) return nullptr;
            if (!h.callnewh(n)) return nullptr;
        }
    }
    if (n > 0xffffffe0u) { h.callnewh(n); return nullptr; }
    for (;;) {
        void* r = h.realloc_(p, n ? n : 1);
        if (r) return r;
        if (!g_newMode || !h.callnewh(n)) return nullptr;
        if (n > 0xffffffe0u) { h.callnewh(n); return nullptr; }
    }
}

// ---- 0x2C32 __ValidateEH3RN ------------------------------------------------------------------------
struct EH3Host {
    u32 (*rd32)(uint64_t); uint16_t (*rd16)(uint64_t); uint8_t (*rd8)(uint64_t);
    // VirtualQuery(addr): 0 on failure
    int (*virtualQuery)(uint64_t addr, uint64_t* allocBase, u32* type, u32* protect);
    int (*xchg)(volatile int*, int);                    // InterlockedExchange
    uint64_t self;                                      // module's __ValidateEH3RN descriptor (+4 hi, +8 lo)
};
static int g_eh3Count;                                  // 0x6F9C62D8
static u32 g_eh3Cache[16];                              // 0x6F9C62E0
static volatile int g_eh3Lock;                          // 0x6F9C6320

static void eh3_shift(int idx, u32 page) {
    for (int i = 0; i <= idx; ++i) { u32 t = g_eh3Cache[i]; g_eh3Cache[i] = page; page = t; }
}

// 1.13c D2sound+0x2C32
int d2s_ValidateEH3RN(const EH3Host& h, uint64_t rn) {
    u32 ht = h.rd32(rn + 8);
    if (ht & 3) return 0;
    u32 lo = h.rd32(h.self + 8), hi = h.rd32(h.self + 4);
    if (lo <= ht && ht < hi) return 0;
    u32 cur = h.rd32(rn + 12);
    if (cur == 0xffffffffu) return 1;
    u32 i = 0, cnt = 0;
    uint64_t p = ht;
    do {
        u32 v = h.rd32(p);
        if (v != 0xffffffffu && i <= v) return 0;
        if (h.rd32(p + 4)) ++cnt;
        ++i; p += 12;
    } while (i <= cur);
    if (cnt) { u32 u = h.rd32(rn - 8); if (u < lo || (u32)rn <= u) return 0; }
    u32 page = ht & 0xfffff000u;
    for (int idx = 0; idx < g_eh3Count; ++idx) {
        if (g_eh3Cache[idx] != page) continue;
        if (idx < 1) return 1;
        if (h.xchg(&g_eh3Lock, 1) != 0) return 1;
        if (g_eh3Cache[idx] == page) { eh3_shift(idx, page); g_eh3Lock = 0; return 1; }
        int j = g_eh3Count - 1;
        while (j >= 0 && g_eh3Cache[j] != page) --j;
        if (j < 0) { if (g_eh3Count < 16) ++g_eh3Count; j = g_eh3Count - 1; }
        else if (j == 0) { g_eh3Lock = 0; return 1; }
        eh3_shift(j, page);
        g_eh3Lock = 0;
        return 1;
    }
    uint64_t base; u32 type, prot;
    if (!h.virtualQuery(ht, &base, &type, &prot)) return -1;
    if (type != 0x1000000) return -1;
    if (prot & 0xcc) {
        if (h.rd16(base) != 0x5a4d) return -1;
        uint64_t nt = base + h.rd32(base + 0x3c);
        if (h.rd32(nt) != 0x4550 || h.rd16(nt + 0x18) != 0x10b) return -1;
        u32 opt = h.rd16(nt + 0x14);
        if (h.rd16(nt + 6) == 0) return -1;
        u32 va = h.rd32(nt + opt + 0x24), rel = (u32)(ht - base);
        if (va <= rel && rel < h.rd32(nt + opt + 0x20) + va && (h.rd8(nt + opt + 0x3f) & 0x80)) return 0;
    }
    if (h.xchg(&g_eh3Lock, 1) != 0) return 1;
    int n = g_eh3Count, k = n;
    while (k > 0 && g_eh3Cache[k - 1] != page) --k;
    if (k == 0) {
        eh3_shift(n < 16 ? n : 15, page);
        if (n < 16) g_eh3Count = n + 1;
    }
    g_eh3Lock = 0;
    return 1;
}

// ---- 0x2E5B / 0x2F2B locale info ------------------------------------------------------------------
struct LocInfo {                                        // only the refcount slots the CRT touches
    int refcount;               // +0x00
    int pad[10];
    int* ref2C;                 // +0x2C
    int* ref30;                 // +0x30 (numeric)
    int* ref34;                 // +0x34 (monetary)
    int* ref38;
    struct LConv* lconv;        // +0x3C
    int* ref40;                 // +0x40 (ctype)
    char* pctype;               // +0x44
    int pad2[2];
    int* timeinfo;              // +0x50
};
// the original indexes these as dwords: [0xb]=2C [0xc]=30 [0xd]=34 [0x10]=40 [0x13]=4C -> codepage block +0xB4
struct LocHost {
    void (*free_)(void*);
    void (*free_lconv_mon)(void*); void (*free_lconv_num)(void*); void (*free_lc_time)(void*);
    LocInfo* c_locinfo;         // PTR_DAT_6f9c5484 (current global)
    void* c_lconv;              // 0x6F9C6358
    int* c_monref;              // 0x6F9C660C
    int* c_numref;              // 0x6F9C6610
    int* c_ctyperef;            // 0x6F9C6608
    void* c_time;               // 0x6F9C6354
};

// 1.13c D2sound+0x2E5B
void d2s_freetlocinfo(const LocHost& h, LocInfo* l) {
    if ((void*)l->lconv != h.c_lconv && l->lconv && *l->ref2C == 0) {
        int* m = l->ref34;
        if (m && *m == 0 && m != h.c_monref) { h.free_(m); h.free_lconv_mon(l->lconv); }
        int* n = l->ref30;
        if (n && *n == 0 && n != h.c_numref) { h.free_(n); h.free_lconv_num(l->lconv); }
        h.free_(l->ref2C);
        h.free_(l->lconv);
    }
    int* c = l->ref40;
    if (c != h.c_ctyperef && c && *c == 0) { h.free_(c); h.free_(l->pctype - 0xfe); }
    int* t = l->timeinfo;
    if ((void*)t != h.c_time && t && t[0x2d] == 0) { h.free_lc_time(t); h.free_(l->timeinfo); }
    h.free_(l);
}

// 1.13c D2sound+0x2F2B   ptd->_tfpecode is the thread's locinfo pointer (passed by address)
LocInfo* d2s_updatetlocinfo_lk(const LocHost& h, LocInfo** tlocinfo) {
    LocInfo* old = *tlocinfo;
    LocInfo* cur = h.c_locinfo;
    if (old != cur) {
        auto dec = [](int* r) { if (r) --*r; };
        auto cp = [](LocInfo* l) -> int* { return (int*)((char*)*(void**)((char*)l + 0x4c) + 0xb4); };
        if (old) { --old->refcount; dec(old->ref2C); dec(old->ref34); dec(old->ref30); dec(old->ref40); --*cp(old); }
        *tlocinfo = cur;
        ++cur->refcount;
        if (cur->ref2C) ++*cur->ref2C;
        if (cur->ref34) ++*cur->ref34;
        if (cur->ref30) ++*cur->ref30;
        if (cur->ref40) ++*cur->ref40;
        ++*cp(cur);
        if (old && old->refcount == 0 && old != cur) d2s_freetlocinfo(h, old);
    }
    return *tlocinfo;
}

// ---- 0x3027 / 0x3056 / 0x307F / 0x321A MBCS ---------------------------------------------------------
struct MbState {
    uint8_t ctype[257];         // 0x6F9C6640 (index +1)
    u32 codepage;               // 0x6F9C6744
    u32 isMb;                   // 0x6F9C6638
    u32 lcid;                   // 0x6F9C6630
    u32 info[3];                // 0x6F9C6750..58
    uint8_t casemap[256];       // 0x6F9C6760
    int unsetOk;                // 0x6F9C6330
};
static MbState g_mb;
struct CpEntry { u32 cp; u32 v[3]; uint8_t ranges[4][8]; };   // 0x30 bytes at 0x6F9C55C8
struct CpHost {
    // GetCPInfo: returns false on failure
    bool (*getCPInfo)(u32 cp, u32* maxCharSize, uint8_t leadByte[12]);
    void (*getStringTypeA)(u32 type, const char* src, int cch, uint16_t* out, u32 cp, u32 lcid);
    void (*lcMapA)(u32 lcid, u32 flags, const char* src, int cch, char* dst, int dcch, u32 cp);
    const uint8_t* masks;       // 0x6F9C55C0 [4]
    const CpEntry* table;       // 5 entries
};

// 1.13c D2sound+0x3027
u32 d2s_cp_to_lcid(int cp) {
    if (cp == 0x3a4) return 0x411;
    if (cp == 0x3a8) return 0x804;
    if (cp == 0x3b5) return 0x412;
    if (cp == 0x3b6) return 0x404;
    return 0;
}

// 1.13c D2sound+0x3056
void d2s_setSBCS() {
    memset(g_mb.ctype, 0, sizeof g_mb.ctype);
    g_mb.codepage = 0; g_mb.isMb = 0; g_mb.lcid = 0;
    g_mb.info[0] = g_mb.info[1] = g_mb.info[2] = 0;
}

// 1.13c D2sound+0x307F
void d2s_mbcs_tables(const CpHost& h) {
    u32 maxc; uint8_t lead[12] = {0};
    if (h.getCPInfo(g_mb.codepage, &maxc, lead)) {
        char s[256];
        for (int i = 0; i < 256; ++i) s[i] = (char)i;
        s[0] = ' ';
        for (int i = 0; lead[i] != 0 && i < 12; i += 2)
            for (u32 c = lead[i]; c <= lead[i + 1]; ++c) s[c] = ' ';
        uint16_t type[256]; char up[256], lo[256];
        h.getStringTypeA(1, s, 256, type, g_mb.codepage, g_mb.lcid);
        h.lcMapA(g_mb.lcid, 0x100, s, 256, up, 256, g_mb.codepage);
        h.lcMapA(g_mb.lcid, 0x200, s, 256, lo, 256, g_mb.codepage);
        for (int i = 0; i < 256; ++i) {
            if (type[i] & 1) { g_mb.ctype[i + 1] |= 0x10; g_mb.casemap[i] = (uint8_t)up[i]; }
            else if (type[i] & 2) { g_mb.ctype[i + 1] |= 0x20; g_mb.casemap[i] = (uint8_t)lo[i]; }
            else g_mb.casemap[i] = 0;
        }
    } else {
        for (u32 i = 0; i < 256; ++i) {
            if (i >= 0x41 && i <= 0x5a) { g_mb.ctype[i + 1] |= 0x10; g_mb.casemap[i] = (uint8_t)(i + 0x20); }
            else if (i > 0x60 && i < 0x7b) { g_mb.ctype[i + 1] |= 0x20; g_mb.casemap[i] = (uint8_t)(i - 0x20); }
            else g_mb.casemap[i] = 0;
        }
    }
}

// 1.13c D2sound+0x321A  _setmbcp(codepage)
int d2s_setmbcp(const CpHost& h, u32 cp) {
    if (cp != 0) {
        for (int e = 0; e < 5; ++e) {
            const CpEntry& en = h.table[e];
            if (en.cp != cp) continue;
            memset(g_mb.ctype, 0, sizeof g_mb.ctype);
            for (int k = 0; k < 4; ++k)
                for (const uint8_t* r = en.ranges[k]; r[0] && r[1]; r += 2)
                    for (u32 c = r[0]; c <= r[1]; ++c) g_mb.ctype[c + 1] |= h.masks[k];
            g_mb.codepage = cp; g_mb.isMb = 1; g_mb.lcid = d2s_cp_to_lcid((int)cp);
            g_mb.info[0] = en.v[0]; g_mb.info[1] = en.v[1]; g_mb.info[2] = en.v[2];
            d2s_mbcs_tables(h);
            return 0;
        }
        u32 maxc; uint8_t lead[12] = {0};
        if (h.getCPInfo(cp, &maxc, lead)) {
            memset(g_mb.ctype, 0, sizeof g_mb.ctype);
            g_mb.codepage = cp; g_mb.lcid = 0;
            if (maxc < 2) g_mb.isMb = 0;
            else {
                for (int i = 0; lead[i] != 0 && lead[i + 1] != 0 && i < 12; i += 2)
                    for (u32 c = lead[i]; c <= lead[i + 1]; ++c) g_mb.ctype[c + 1] |= 4;
                for (u32 c = 1; c < 0xff; ++c) g_mb.ctype[c + 1] |= 8;
                g_mb.lcid = d2s_cp_to_lcid((int)cp);
                g_mb.isMb = 1;
            }
            g_mb.info[0] = g_mb.info[1] = g_mb.info[2] = 0;
            d2s_mbcs_tables(h);
            return 0;
        }
        if (g_mb.unsetOk == 0) return -1;
    }
    d2s_setSBCS();
    d2s_mbcs_tables(h);
    return 0;
}
