// n47 batch: D2sound CRT helpers. Host seams are passed as arguments.
#include <cstdint>
#include <cstddef>

static int d2s_addl(uint32_t a, uint32_t b, uint32_t* out) {
    uint32_t s = a + b;
    *out = s;
    return (s < a || s < b) ? 1 : 0;
}

// 1.13c D2sound+0x0B14E
void d2s_setdefaultprecision(void (*controlfp)(uint32_t, uint32_t)) { controlfp(0x10000, 0x30000); }

// 1.13c D2sound+0x0B160
int d2s_ms_p5_test_fdiv(double a, double b, double c) {
    // a=_DAT_9c0990 divisor-check constants (4195835/3145727 family)
    return (a < b - (b / c) * c) ? 1 : 0;
}

// 1.13c D2sound+0x0B1A0
void d2s_ms_p5_mp_test_fdiv(int (*isProcessorFeaturePresent)(int), int (*p5test)(void)) {
    if (isProcessorFeaturePresent) { isProcessorFeaturePresent(0); return; }
    p5test();
}

// 1.13c D2sound+0x0C5BC
int d2s_ZeroTail(const uint32_t* man, int bits) {
    int i = bits / 32;
    uint32_t v = man[i] & ~(0xFFFFFFFFu << ((0x1f - (bits % 32)) & 0x1f));
    for (;;) {
        if (v != 0) return 0;
        if (++i > 2) return 1;
        v = man[i];
    }
}

// 1.13c D2sound+0x0C5EE
void d2s_IncMan(uint32_t* man, int bits) {
    uint32_t* p = man + bits / 32;
    int carry = d2s_addl(*p, 1u << ((0x1f - (bits % 32)) & 0x1f), p);
    for (int i = bits / 32 - 1; i >= 0; --i) {
        if (!carry) return;
        carry = d2s_addl(man[i], 1, &man[i]);
    }
}

// 1.13c D2sound+0x0C63B
int d2s_RoundMan(uint32_t* man, int bits) {
    int ret = 0, w = bits / 32;
    unsigned sh = (0x1f - (bits % 32)) & 0x1f;
    if ((man[w] & (1u << sh)) && !d2s_ZeroTail(man, bits)) { d2s_IncMan(man, bits - 1); ret = 0; }
    man[w] &= 0xFFFFFFFFu << sh;
    for (int i = w + 1; i < 3; ++i) man[i] = 0;
    return ret;
}

// 1.13c D2sound+0x0C6E1
void d2s_ShrMan(uint32_t* man, int bits) {
    unsigned b = bits % 32;
    uint32_t carry = 0;
    for (int i = 0; i < 3; ++i) {
        uint32_t v = man[i];
        man[i] = (b ? v >> b : v) | carry;
        carry = b ? (v & ~(0xFFFFFFFFu << b)) << (32 - b) : 0;
    }
    int w = bits / 32;
    const uint32_t* src = man + (2 - w);
    for (int i = 2; i >= 0; --i, --src) man[i] = (i < w) ? 0 : *src;
}

// 1.13c D2sound+0x055BD
void d2s_security_init_cookie(uint32_t* cookie, uint32_t entropy) {
    if (*cookie == 0 || *cookie == 0xbb40e64eu) {
        *cookie = entropy ? entropy : 0xbb40e64eu;
    }
}

// 1.13c D2sound+0x034FE
int d2s_initmbctable(int* inited, void (*setmbcp)(int)) {
    if (*inited == 0) { setmbcp(-3); *inited = 1; }
    return 0;
}

// 1.13c D2sound+0x0291C
void d2s_global_unwind2(void* frame, void (*rtlUnwind)(void*, void*, void*, void*), void* target) {
    rtlUnwind(frame, target, nullptr, nullptr);
}

// 1.13c D2sound+0x02BBC
size_t d2s_msize(void* mem, int heapMode, unsigned (*sbhFindBlock)(void*), size_t (*heapSize)(void*)) {
    if (heapMode == 3) {
        unsigned hit = sbhFindBlock(mem);
        if (hit) return (size_t)(*(int*)((char*)mem - 4) - 9);
    }
    return heapSize(mem);
}

// 1.13c D2sound+0x0135B
int d2s_dllmain_entry(void* h, int reason, void* rsv, int attached, int (*userEntry)(void*, int, void*),
                      int (*crtInit)(void*, int), int (*attachWork)(void)) {
    int r = 1;
    if (reason == 0 && attached == 0) return 0;
    if (reason == 1 || reason == 2) {
        if (userEntry) r = userEntry(h, reason, rsv);
        if (r == 0 || crtInit(h, reason) == 0) return 0;
    }
    r = attachWork();
    if (reason == 1 && r == 0) crtInit(h, 0);
    if (reason == 0 || reason == 3) {
        if (crtInit(h, reason) == 0) r = 0;
        if (r != 0 && userEntry) r = userEntry(h, reason, rsv);
    }
    return r;
}
