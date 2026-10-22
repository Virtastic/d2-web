// src/core/d2cmp/d2cmp_batch_b51.cpp -- alpha-lane batch: D2CMP rows from the cached Ghidra decompiles.
// No spec, no vector. Globals/callees of the original image are passed in via env structs.
#include <cstdint>
#include <cstddef>
typedef void* HANDLE;
typedef void* LPVOID;

typedef void (*D2CmpFreeFn)(void*);

struct D2CmpEnvB51 {
    void (*f1CA0)();                 // 0x1CA0
    D2CmpFreeFn free_;               // 0x2EBE _free
    void (*freeLconvMon)(void*);     // 0x7BAF
    void (*freeLconvNum)(void*);     // 0x7B50
    void (*freeLcTime)(void*);       // 0x79C0
    void* (*HeapReAlloc)(HANDLE, uint32_t, void*, uint32_t); void* (*HeapAlloc)(HANDLE, uint32_t, uint32_t);
    void (*HeapFree)(HANDLE, uint32_t, void*); void* (*VirtualAlloc)(void*, uint32_t, uint32_t, uint32_t);
    HANDLE heap;                     // DAT_6ff14e80
    int32_t* regionCount;            // DAT_6ff14e68
    int32_t* regionCap;              // DAT_6ff14e78
    void** regionTable;              // DAT_6ff14e6c
    void* defTimeInfo;               // DAT_6fe3495c
    void* defMonInfo;                // DAT_6fe34960
    void* defNumInfo;                // DAT_6ff13c00
    void* defMonPtr;                 // DAT_6ff13bfc
    void* defCtype;                  // DAT_6ff13bf8
    const char** lconvDef;           // *PTR_PTR_6fe32a94 (array of 9 strings)
    const char** lconvC;             // PTR_DAT_6fe32a64.. (array of 9 strings)
};

// 1.13c D2CMP+0x16C0
void d2cmp_16c0(const D2CmpEnvB51* e, uint32_t* self) {
    e->f1CA0();
    uint32_t first = self[1];
    if (!first) return;
    uint32_t prev = self[2];
    uint32_t* slot = ((int32_t)prev < 0) ? (uint32_t*)(uintptr_t)~prev
                                         : (uint32_t*)((uintptr_t)(self + 1) + (prev - ((uint32_t*)(uintptr_t)first)[1]));
    *slot = first;
    ((uint32_t*)(uintptr_t)self[1])[1] = self[2];
    self[1] = 0;
    self[2] = 0;
}

// 1.13c D2CMP+0x5209  ___sbh_alloc_new_region
uint32_t* d2cmp_5209(const D2CmpEnvB51* e) {
    if (*e->regionCount == *e->regionCap) {
        void* p = e->HeapReAlloc(e->heap, 0, *e->regionTable, (*e->regionCap * 5 + 0x50) * 4);
        if (!p) return nullptr;
        *e->regionCap += 0x10;
        *e->regionTable = p;
    }
    uint32_t* r = (uint32_t*)((uintptr_t)*e->regionTable + (uintptr_t)(*e->regionCount) * 0x14);
    void* hdr = e->HeapAlloc(e->heap, 8, 0x41c4);
    r[4] = (uint32_t)(uintptr_t)hdr;
    if (hdr) {
        void* base = e->VirtualAlloc(nullptr, 0x100000, 0x2000, 4);
        r[3] = (uint32_t)(uintptr_t)base;
        if (base) {
            r[2] = 0xffffffff;
            r[0] = 0;
            r[1] = 0;
            (*e->regionCount)++;
            *(uint32_t*)hdr = 0xffffffff;
            return r;
        }
        e->HeapFree(e->heap, 0, (LPVOID)(uintptr_t)r[4]);
    }
    return nullptr;
}

// 1.13c D2CMP+0x52C0  ___sbh_alloc_new_group  (32-bit-layout words addressed through uintptr_t)
static inline uint32_t& W(uintptr_t a) { return *(uint32_t*)a; }
int32_t d2cmp_52c0(const D2CmpEnvB51* e, uint32_t* region) {
    uintptr_t hdr = region[4];
    int32_t idx = 0;
    for (int32_t m = (int32_t)region[2]; m > -1; m <<= 1) idx++;
    uintptr_t g = (uintptr_t)idx * 0x204 + 0x144 + hdr;
    uintptr_t q = g;
    for (int n = 0x3f; n; n--, q += 8) { W(q + 8) = (uint32_t)q; W(q + 4) = (uint32_t)q; }
    uintptr_t addr = (uintptr_t)idx * 0x8000 + region[3];
    if (!e->VirtualAlloc((LPVOID)addr, 0x8000, 0x1000, 4)) return -1;
    uintptr_t p = addr + 0x10;
    for (int n = 8; n; n--, p += 0x1000) {
        W(p - 8) = 0xffffffff;
        W(p + 0xfec) = 0xffffffff;
        W(p) = (uint32_t)(p + 0xffc);
        W(p - 4) = 0xff0;
        W(p + 4) = (uint32_t)(p - 0x1004);
        W(p + 0xfe8) = 0xff0;
    }
    W(g + 0x1fc) = (uint32_t)(addr + 0xc);
    W(addr + 0x14) = (uint32_t)(g + 0x1f8);
    W(g + 0x200) = (uint32_t)(addr + 0x700c);
    W(addr + 0x7010) = (uint32_t)(g + 0x1f8);
    W(hdr + 0x44 + idx * 4) = 0;
    W(hdr + 0xc4 + idx * 4) = 1;
    char c = *(char*)(hdr + 0x43);
    *(char*)(hdr + 0x43) = c + 1;
    if (c == 0) region[1] |= 1;
    region[2] &= ~(0x80000000u >> (idx & 0x1f));
    return idx;
}

// 1.13c D2CMP+0x666D  ___freetlocinfo (struct words are 32-bit-layout offsets, pointers widened)
void d2cmp_666d(const D2CmpEnvB51* e, uint32_t* li) {
    // li is indexed in 4-byte words of the original layout
    uint32_t* w = li;
    auto P = [&](int off) { return (uintptr_t)w[off / 4]; };
    if (P(0x3c) != (uintptr_t)e->defMonInfo && P(0x3c) != 0 && *(int*)P(0x2c) == 0) {
        uintptr_t p = P(0x34);
        if (p && *(int*)p == 0 && p != (uintptr_t)e->defMonPtr) {
            e->free_((void*)p);
            e->freeLconvMon((void*)P(0x3c));
        }
        p = P(0x30);
        if (p && *(int*)p == 0 && p != (uintptr_t)e->defNumInfo) {
            e->free_((void*)p);
            e->freeLconvNum((void*)P(0x3c));
        }
        e->free_((void*)P(0x2c));
        e->free_((void*)P(0x3c));
    }
    uintptr_t p = P(0x40);
    if (p != (uintptr_t)e->defCtype && p != 0 && *(int*)p == 0) {
        e->free_((void*)p);
        e->free_((void*)(P(0x44) - 0xfe));
    }
    uintptr_t t = P(0x50);
    if (t != (uintptr_t)e->defTimeInfo && t != 0 && ((uint32_t*)t)[0x2d] == 0) {
        e->freeLcTime((void*)t);
        e->free_((void*)P(0x50));
    }
    e->free_(li);
}

// 1.13c D2CMP+0x6D30  _memset
void* d2cmp_6d30(void* dst, int val, size_t size) {
    uint8_t* d = (uint8_t*)dst;
    while (size--) *d++ = (uint8_t)val;
    return dst;
}

// 1.13c D2CMP+0x79C0  ___free_lc_time
void d2cmp_79c0(const D2CmpEnvB51* e, uint32_t* t) {
    static const uint8_t order[43] = {1,2,3,4,5,6,0,8,9,10,11,12,13,7,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42};
    if (!t) return;
    for (int i = 0; i < 43; i++) e->free_((void*)(uintptr_t)t[order[i]]);
}

// 1.13c D2CMP+0x7B50  ___free_lconv_num
void d2cmp_7b50(const D2CmpEnvB51* e, uint32_t* c) {
    if (!c) return;
    for (int i = 0; i < 3; i++) {
        uintptr_t p = c[i];
        if (p != (uintptr_t)e->lconvDef[i] && p != (uintptr_t)e->lconvC[i]) e->free_((void*)p);
    }
}

// 1.13c D2CMP+0x7BAF  ___free_lconv_mon
void d2cmp_7baf(const D2CmpEnvB51* e, uint32_t* c) {
    if (!c) return;
    for (int i = 3; i < 10; i++) {
        uintptr_t p = c[i];
        if (p != (uintptr_t)e->lconvDef[i] && p != (uintptr_t)e->lconvC[i]) e->free_((void*)p);
    }
}
