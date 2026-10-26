// D2sound batch z24 -- statically linked MSVC CRT pieces. Retail 32-bit addresses are uint32_t and
// touched only through the host seam Z24Env.
#include <cstdint>

struct Z24Env {
    uint32_t (*rd)(uint32_t a);
    void     (*wr)(uint32_t a, uint32_t v);
    uint32_t (*heapAlloc)(uint32_t flags, uint32_t size);
    uint32_t (*heapReAlloc)(uint32_t flags, uint32_t p, uint32_t size);
    void     (*heapFree)(uint32_t flags, uint32_t p);
    uint32_t (*virtualAlloc)(uint32_t size, uint32_t type, uint32_t prot);
    uint32_t (*loadProc)(const char* mod, const char* name);   // LoadLibrary+GetProcAddress, 0 if absent
    uint32_t (*callProc)(uint32_t fn, const uint32_t* args, int n);
    void     (*doexit)(int a, int b, int c);                   // D2sound+0x10C4
    uint32_t (*controlfp)(uint32_t nw, uint32_t mask);         // D2sound+0xCC6E
    uint32_t (*addl)(uint32_t a, uint32_t b, uint32_t* out);   // ___addl D2sound+0xD1E5 (carry)
    void*    (*fcvtE)(void*, int, int, int);                   // 0xAEEB
    void*    (*fcvtF)(void*, char*, int);                      // 0xAFF7
    void*    (*fcvtG)(void*, char*, int, int);                 // 0xB05E
    uint32_t (*getModuleHandle)(const char*);
};

// 1.13c D2sound+0x2450 -- __chkstk: stack-probe helper, no portable body.
void z24_chkstk() {}

// 1.13c D2sound+0x458d -- ___crtMessageBoxA. Globals: 633c MessageBoxA, 6340 GetActiveWindow,
// 6344 GetLastActivePopup, 6348 GetProcessWindowStation, 634c GetUserObjectInformationA,
// 6004 platform id, 6010 OS major version.
int z24_crtMessageBoxA(const Z24Env* e, uint32_t text, uint32_t caption, uint32_t type) {
    uint32_t iVar2 = 0;
    if (!e->rd(0x6f9c633c)) {
        uint32_t mb = e->loadProc("user32.dll", "MessageBoxA");
        if (!mb) return 0;
        e->wr(0x6f9c633c, mb);
        e->wr(0x6f9c6340, e->loadProc("user32.dll", "GetActiveWindow"));
        e->wr(0x6f9c6344, e->loadProc("user32.dll", "GetLastActivePopup"));
        if (e->rd(0x6f9c6004) == 2) {
            uint32_t g = e->loadProc("user32.dll", "GetUserObjectInformationA");
            e->wr(0x6f9c634c, g);
            if (g) e->wr(0x6f9c6348, e->loadProc("user32.dll", "GetProcessWindowStation"));
        }
    }
    uint32_t ws = e->rd(0x6f9c6348), ok = 1;
    if (ws) {
        uint32_t h = e->callProc(ws, nullptr, 0), flags = 0;
        ok = 0;
        if (h) {
            uint32_t buf[3] = {0, 0, 0}, len = 0, a[5] = {h, 1, 0, 0xc, 0};
            (void)buf; (void)len; (void)a;
            // GetUserObjectInformationA(h,1,buf,0xc,&len): out buffer is host memory; seam only.
            ok = 0; (void)flags;
        }
    }
    if (ok) {
        uint32_t ga = e->rd(0x6f9c6340);
        if (ga && (iVar2 = e->callProc(ga, nullptr, 0)) != 0) {
            uint32_t pop = e->rd(0x6f9c6344);
            if (pop) { uint32_t a[1] = {iVar2}; iVar2 = e->callProc(pop, a, 1); }
        }
    } else {
        type |= (e->rd(0x6f9c6010) < 4) ? 0x40000u : 0x200000u;
    }
    uint32_t a[4] = {iVar2, text, caption, type};
    return (int)e->callProc(e->rd(0x6f9c633c), a, 4);
}

// 1.13c D2sound+0x38a7 -- ___sbh_alloc_new_region; entries 0x14 bytes; 0 on failure.
uint32_t z24_sbh_alloc_new_region(const Z24Env* e) {
    uint32_t cnt = e->rd(0x6f9c6618), cap = e->rd(0x6f9c6628), arr = e->rd(0x6f9c661c);
    if (cnt == cap) {
        uint32_t p = e->heapReAlloc(0, arr, (cap * 5 + 0x50) * 4);
        if (!p) return 0;
        e->wr(0x6f9c6628, cap + 0x10);
        e->wr(0x6f9c661c, arr = p);
    }
    uint32_t r = arr + cnt * 0x14;
    uint32_t hdr = e->heapAlloc(8, 0x41c4);
    e->wr(r + 16, hdr);
    if (hdr) {
        uint32_t base = e->virtualAlloc(0x100000, 0x2000, 4);
        e->wr(r + 12, base);
        if (base) {
            e->wr(r + 8, 0xffffffff); e->wr(r, 0); e->wr(r + 4, 0);
            e->wr(0x6f9c6618, cnt + 1);
            e->wr(hdr, 0xffffffff);
            return r;
        }
        e->heapFree(0, hdr);
    }
    return 0;
}

// 1.13c D2sound+0x11a9 -- __cexit
void z24_cexit(const Z24Env* e) { e->doexit(0, 0, 1); }

// 1.13c D2sound+0xb0fd -- __cfltcvt; selector 'e'/'E' (0x65/0x45), 'f' (0x66), else g-style.
void* z24_cfltcvt(const Z24Env* e, void* arg, char* buf, int fmtCh, int format, int precision) {
    if (fmtCh == 0x65 || fmtCh == 0x45) return e->fcvtE(arg, (int)(intptr_t)buf, format, precision);
    if (fmtCh == 0x66) return e->fcvtF(arg, buf, format);
    return e->fcvtG(arg, buf, format, precision);
}

// 1.13c D2sound+0xb14e -- __setdefaultprecision
void z24_setdefaultprecision(const Z24Env* e) { e->controlfp(0x10000, 0x30000); }

// 1.13c D2sound+0xb160 -- __ms_p5_test_fdiv. The three doubles at 0990/09c8/09d0 are host data;
// the x87 FDIV-bug probe is evaluated in double on the host.
uint32_t z24_ms_p5_test_fdiv(double a990, double a9c8, double a9d0) {
    double q = a9c8 / a9d0;
    return a990 < a9c8 - q * a9d0 ? 1 : 0;
}

// 1.13c D2sound+0xb1a0 -- __ms_p5_mp_test_fdiv. Result of IsProcessorFeaturePresent(0) is dropped
// by retail's decompile; with no export it falls through to the fdiv probe.
void z24_ms_p5_mp_test_fdiv(const Z24Env* e, double a, double b, double c) {
    uint32_t h = e->getModuleHandle("KERNEL32");
    if (h) {
        uint32_t fn = e->loadProc("KERNEL32", "IsProcessorFeaturePresent");
        if (fn) { uint32_t a0[1] = {0}; e->callProc(fn, a0, 1); return; }
    }
    z24_ms_p5_test_fdiv(a, b, c);
}

// Mantissa helpers: 3-dword big-endian-word mantissa m[0..2].
// 1.13c D2sound+0xc5bc -- __ZeroTail: 1 if all bits below bit `n` are zero.
uint32_t z24_ZeroTail(const uint32_t* m, int n) {
    int i = n / 0x20;
    uint32_t v = m[i] & ~(0xffffffffu << ((0x1f - (n % 0x20)) & 0x1f));
    while (v == 0) {
        if (++i > 2) return 1;
        v = m[i];
    }
    return 0;
}

// 1.13c D2sound+0xc5ee -- __IncMan: add 1 at bit n, propagate carry upward.
void z24_IncMan(const Z24Env* e, uint32_t* m, int n) {
    uint32_t c = e->addl(m[n / 0x20], 1u << ((0x1f - (n % 0x20)) & 0x1f), &m[n / 0x20]);
    for (int i = n / 0x20 - 1; i >= 0 && c; i--) c = e->addl(m[i], 1, &m[i]);
}

// 1.13c D2sound+0xc63b -- __RoundMan: round-half-up-ish to n bits; returns carry-out of IncMan.
uint32_t z24_RoundMan(const Z24Env* e, uint32_t* m, int n) {
    uint32_t ret = 0;
    int i = n / 0x20;
    uint8_t sh = 0x1f - (n % 0x20);
    if ((m[i] & (1u << (sh & 0x1f))) && !z24_ZeroTail(m, n)) {
        z24_IncMan(e, m, n - 1);   // retail returns IncMan's (void) result: eax is stale; 0 here
    }
    m[i] &= 0xffffffffu << (sh & 0x1f);
    for (i++; i < 3; i++) m[i] = 0;
    return ret;
}

// 1.13c D2sound+0xc6e1 -- __ShrMan: shift 3-dword mantissa right by n bits.
void z24_ShrMan(uint32_t* m, int n) {
    uint8_t b = n % 0x20;
    uint32_t carry = 0;
    for (int i = 0; i < 3; i++) {
        uint32_t v = m[i];
        m[i] = (v >> (b & 0x1f)) | carry;
        carry = (v & ~(0xffffffffu << (b & 0x1f))) << ((0x20 - b) & 0x1f);
    }
    int w = n / 0x20;
    for (int i = 2; i >= 0; i--) m[i] = i < w ? 0 : m[i - w];
}
