// src/core/d2cmp/d2cmp_batch_d250.cpp -- ten UNMAPPED 1.13c D2CMP.dll rows from the cached Ghidra
// decompiles: three sprite-cache routines (0xD250 0xD7F0 0xC030) and seven statically linked MSVC CRT
// helpers. Alpha batch lane: implementation + marker only, no spec, no vector.
// The cache routines take register-passed `this` (EAX/EBX/EDI/ESI); here they are explicit
// parameters, inferred from the decompile (no disassembly). Callees with no port yet are env seams.
// Not ported (retail-CRT internals that host libc replaces): 0x2749 ___crtExitProcess (tries
// mscoree CorExitProcess then ExitProcess; host process exit is not ours), 0x2E8D __lock and
// 0x2EBE _free (small-block-heap + CRT lock table, state we do not carry), 0x7806 __fptrap
// (__amsg_exit(2), a runtime-abort message). 0x3DD2 _malloc and 0x6120 _strlen map to libc.
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct D2CmpCacheEnv {
    void (*lock)(void* cs);                        // EnterCriticalSection
    void (*unlock)(void* cs);                      // LeaveCriticalSection
    uint32_t (*lookupString)(uint32_t id);         // Storm/Fog ordinal 10265
    void (*fatal)(const void* fmt, uint32_t what); // ordinal 10024 then _exit(-1); must not return
    uint32_t* (*evictOne)(uint32_t* cache);        // 0xCB60 (EAX=cache)
    uint32_t (*findFree)(uint32_t limit, uint32_t* cache); // 0xBF00
    void (*mark)(uint32_t arg);                    // 0xBFD0
    int (*alloc)(int, uint32_t, uint32_t, int, void** cb, int, const char* file, uint32_t line); // ordinal 10091
    const void* fatalFmt;                          // DAT_6fe2d30f
    void* cbSpriteFree;                            // LAB_6fe1d760
};

extern "C" {

// 1.13c D2CMP+0x0d250: resize node (EDI) of cache (EAX) to node.size = param_1 + node.size, evict
// until the cache total fits its budget; the node is pinned (flag 2) while evicting.
void d2cmp_d250(uint32_t* cache, uint32_t* node, uint32_t param_1, const D2CmpCacheEnv* e) {
    void* cs = (void*)(uintptr_t)cache[0x30 / 4];
    if (!cs) return;
    e->lock(cs);
    uint32_t sz = node[1];
    cache[2] -= sz;
    sz += param_1;
    node[1] = sz;
    node[3] |= 2;
    uint32_t total = cache[2] + sz;
    cache[2] = total;
    e->unlock(cs);
    while (cache[1] < total) {
        uint32_t* v = e->evictOne(cache);
        if (!v) break;
        if (v == node) e->fatal(e->fatalFmt, e->lookupString(0xa89));
        e->lock(cs);
        total = cache[2];
        e->unlock(cs);
    }
    e->lock(cs);
    node[3] &= ~2u;
    e->unlock(cs);
}

// 1.13c D2CMP+0x0c030 (0x0C030): (EDI=cache, EBX=bytes) account a new entry if under the entry cap (0x1400)
// and the byte budget; returns the slot found, 0 if none/not taken.
int d2cmp_c030(uint32_t* cache, uint32_t bytes, uint32_t param_1, const D2CmpCacheEnv* e) {
    int r = 0;
    void* cs = (void*)(uintptr_t)cache[0x30 / 4];
    if (!cs) return 0;
    e->lock(cs);
    if (cache[3] != 0x1400 && cache[2] + bytes <= cache[1]) {
        r = (int)e->findFree(cache[1], cache);
        e->mark(param_1);
        if (!r) e->fatal(e->fatalFmt, e->lookupString(0x9c8));
        cache[3]++;
        cache[2] += bytes;
    }
    e->unlock(cs);
    return r;
}

// 1.13c D2CMP+0x0d7f0: (EAX=dcc, ESI=sprite, EBX=dir count) fill a sprite's per-frame offsets from
// the directory table and allocate its pixel block; 0 ok, -1 on mismatch/alloc failure.
int d2cmp_d7f0(uint8_t* hdr, int dcc, uint8_t* spr, int dirs, int frame, const D2CmpCacheEnv* e) {
    if (!hdr) e->fatal(e->fatalFmt, e->lookupString(0x398));
    if (!spr) e->fatal(e->fatalFmt, e->lookupString(0x399));
    if (!dcc) e->fatal(e->fatalFmt, e->lookupString(0x39a));
    uint32_t lo = 0, hi = 0;
    int mode = *(int*)(uintptr_t)(dcc + 8);
    if (mode == 1) {
        if (dirs == 1 && *(int*)(hdr + 0x14) != 1) return -1;
    } else if (mode != 0) goto have;
    lo = (uint32_t)(frame * dirs);
    hi = lo + dirs;
have:
    {
        uint8_t* p = hdr + lo * 4 + 0x1c;
        int end = *(int*)(hdr + hi * 4 + 0x1c), beg = *(int*)p;
        if (end - beg == 0) e->fatal(e->fatalFmt, e->lookupString(0x3b2));
        if (*(int*)(spr + 0xb0)) e->fatal(e->fatalFmt, e->lookupString(0x3b3));
        spr[0x22] = (uint8_t)(frame * dirs);
        spr[0x21] = (uint8_t)dirs;
        spr[0x24] = hdr[0x18];
        spr[0x23] = (uint8_t)(hdr[0x14] + 1);
        uint8_t* q = p;
        for (uint32_t i = lo; i <= hi; i++, q += 4) {
            if (i > 0x20) e->fatal(e->fatalFmt, e->lookupString(0x3ca));
            int32_t d = *(int*)q - beg;
            ((int32_t*)(spr + 0x2c))[i] = d;
            if (d < 0) e->fatal(e->fatalFmt, e->lookupString(0x3cc));
        }
        void* cb[2] = {e->cbSpriteFree, hdr};
        bool cbOn = (hdr[0] & 2) != 0;
        if (cbOn) spr[0x20] |= 4;
        int a = e->alloc(1, (uint32_t)beg, (uint32_t)(end - beg), 0, cbOn ? cb : nullptr, -2,
                         "..\\Source\\D2CMP\\SRC\\SpriteCache.cpp", cbOn ? 0x3e3 : 0x3ef);
        *(int*)(spr + 0xb0) = a;
        return a ? 0 : -1;
    }
}

// 1.13c D2CMP+0x03DD2 _malloc (__nh_malloc with the new-handler mode; host libc here)
void* d2cmp_malloc(size_t n) { return malloc(n); }

// 1.13c D2CMP+0x03B03 _realloc: NULL -> malloc; size 0 -> free, NULL; else resize (host libc replaces the sbh/HeapReAlloc paths)
void* d2cmp_realloc(void* p, size_t n) {
    if (!p) return malloc(n);
    if (!n) { free(p); return nullptr; }
    if (n > 0xFFFFFFE0u) return nullptr;  // retail: __callnewh, returns NULL
    return realloc(p, n);
}

// 1.13c D2CMP+0x06120 _strlen
size_t d2cmp_strlen(const char* s) { return strlen(s); }

// 1.13c D2CMP+0x06263 __security_check_cookie: mismatch -> report_failure (abort)
void d2cmp_security_check_cookie(uintptr_t v, uintptr_t cookie) { if (v != cookie) abort(); }

// 1.13c D2CMP+0x18D38 _cftog: %g of *dbl into buf with `digits` significant digits; exponent < -4 or >= digits -> %e form (caps = upper E), else %f form; host snprintf replaces __fptostr/__cftoe2/__cftof2
void d2cmp_cftog(const double* dbl, char* buf, size_t digits, int caps) {
    char f[8]; snprintf(f, sizeof f, caps ? "%%.*G" : "%%.*g");
    snprintf(buf, digits + 16, f, (int)(digits ? digits - 1 : 0) + 1, *dbl);
}

}
