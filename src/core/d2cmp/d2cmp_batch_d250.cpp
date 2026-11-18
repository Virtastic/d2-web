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
        return a ? end - beg : -1;   // retail returns the frame span's byte length (the stack slot the decompiler read as 0)
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

// _fltout2's STRFLT: sign ('-' or ' '), decpt (value = 0.mantissa * 10^decpt), mantissa digits
struct D2CmpStrFlt { char sign; int32_t decpt; char man[24]; };

// 1.13c D2CMP+0x19CDB _fltout2: the sign, decimal point and 17 significant digits of x. Retail converts through __dtold and
// $I10_OUTPUT (80-bit scaling by its powers-of-ten table, trailing zeros dropped); the host's exact %.16e conversion supplies the
// same 17 rounded digits here. Zero is mantissa "0" with decpt 0; infinities and NaNs are $I10_OUTPUT's "1#INF" / "1#IND" /
// "1#QNAN" / "1#SNAN" with decpt 1.
static void d2cmp_fltout2(double x, D2CmpStrFlt* f) {
    uint64_t b; memcpy(&b, &x, 8);
    f->sign = (b >> 63) ? '-' : ' ';
    const uint64_t e = (b >> 52) & 0x7ff, m = b & 0xfffffffffffffull;
    if (e == 0x7ff) {
        f->decpt = 1;
        strcpy(f->man, !m ? "1#INF" : ((b >> 63) && m == 0x8000000000000ull) ? "1#IND" : (m >> 51) ? "1#QNAN" : "1#SNAN");
        return;
    }
    if (!e && !m) { f->decpt = 0; strcpy(f->man, "0"); return; }
    char t[40]; snprintf(t, sizeof t, "%.16e", (b >> 63) ? -x : x);   // d.dddddddddddddddde[+-]xx
    int32_t n = 0;
    for (const char* q = t; *q && *q != 'e'; ++q) if (*q != '.') f->man[n++] = *q;
    while (n > 1 && f->man[n - 1] == '0') --n;
    f->man[n] = 0;
    f->decpt = (int32_t)strtol(strchr(t, 'e') + 1, nullptr, 10) + 1;
}

// 1.13c D2CMP+0x18AFA _shift: move the string at s right by dist (with its terminator)
static void d2cmp_shift(char* s, int32_t dist) { if (dist) memmove(s + dist, s, strlen(s) + 1); }

// 1.13c D2CMP+0x19BAA _fptostr: '0' then `digits` mantissa digits (padded with '0'), rounded up on the next digit >= '5' with
// the carry through nines; a carry into the leading '0' raises decpt and keeps it, otherwise the leading '0' is dropped
static void d2cmp_fptostr(char* buf, int32_t digits, D2CmpStrFlt* f) {
    char* p = buf; const char* m = f->man;
    *p++ = '0';
    while (digits > 0) { *p++ = *m ? *m++ : '0'; --digits; }
    *p = 0;
    if (digits >= 0 && *m >= '5') {
        --p;
        while (*p == '9') *p-- = '0';
        *p += 1;
    }
    if (*buf == '1') f->decpt++;
    else memmove(buf, buf + 1, strlen(buf + 1) + 1);
}

// 1.13c D2CMP+0x18B17 _cftoe2: d.ddd form with a three-digit exponent ("e+000", 'E' when caps); g_fmt keeps ndec digits in all
static void d2cmp_cftoe2(char* buf, int32_t ndec, int32_t caps, const D2CmpStrFlt* f, int32_t g) {
    if (g) d2cmp_shift(buf + (f->sign == '-'), ndec > 0);
    char* p = buf;
    if (f->sign == '-') *p++ = '-';
    if (ndec > 0) { *p = p[1]; *++p = '.'; }
    p = strcpy(p + ndec + (g == 0), "e+000");
    if (caps) *p = 'E';
    ++p;
    if (f->man[0] != '0') {
        int32_t x = f->decpt - 1;
        if (x < 0) { x = -x; *p = '-'; }
        ++p;
        if (x >= 100) { *p += (char)(x / 100); x %= 100; }
        ++p;
        if (x >= 10) { *p += (char)(x / 10); x %= 10; }
        ++p;
        *p += (char)x;
    }
}

// 1.13c D2CMP+0x18C35 _cftof2: ddd.ddd form from the _fptostr digits
static void d2cmp_cftof2(char* buf, int32_t ndec, const D2CmpStrFlt* f, int32_t g) {
    if (g && f->decpt - 1 == ndec) {
        char* q = buf + (f->sign == '-') + f->decpt - 1;
        q[0] = '0'; q[1] = 0;
    }
    char* p = buf;
    if (f->sign == '-') *p++ = '-';
    if (f->decpt <= 0) { d2cmp_shift(p, 1); *p++ = '0'; }
    else p += f->decpt;
    if (ndec > 0) {
        d2cmp_shift(p, 1); *p++ = '.';
        if (f->decpt < 0) {
            int32_t n = -f->decpt;
            if (!g && ndec < n) n = ndec;
            d2cmp_shift(p, n);
            memset(p, '0', (size_t)n);
        }
    }
}

// 1.13c D2CMP+0x18D38 _cftog: %g of *dbl into buf with `digits` significant digits; exponent < -4 or >= digits -> %e form
// (caps = upper E), else %f form, a rounding carry dropping the extra digit
void d2cmp_cftog(const double* dbl, char* buf, size_t digits, int caps) {
    const int32_t ndec = (int32_t)digits;
    D2CmpStrFlt f;
    d2cmp_fltout2(*dbl, &f);
    const int32_t before = f.decpt - 1;
    char* pb = buf + (f.sign == '-');
    d2cmp_fptostr(pb, ndec, &f);
    const int32_t mag = f.decpt - 1;
    if (mag < -4 || mag >= ndec) { d2cmp_cftoe2(buf, ndec, caps, &f, 1); return; }
    if (before < mag) { while (*pb++) {} pb[-2] = 0; }
    d2cmp_cftof2(buf, ndec, &f, 1);
}

}
