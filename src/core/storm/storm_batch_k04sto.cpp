// src/core/storm/storm_batch_k04sto.cpp -- twelve 1.13c Storm.dll leaves transcribed from the cached
// decompiles (build-wasm/ghidra-storm/decomp/0x<RVA>.c). Batch lane: implementation + marker only.
// Records keep retail's 32-bit field layout (uint32 at byte offsets); register args (in_EAX,
// unaff_ESI/EBX) are explicit parameters. Callees not yet mapped are seams in retail argument order.
// SEH frame bookkeeping (ExceptionList) has no wasm analogue and is omitted.
#include <cstdint>
#include <cstring>

extern "C" {
void     Storm_Sub1C30(void* rec);                                              // 0x1C30
void     Storm_Sub1C20(void* arr);                                              // 0x2C20
void     Storm_Sub2D80(void* rec);                                              // 0x2D80
void     Storm_Sub1EB0(uint32_t* a, uintptr_t count, const uint8_t* src);       // 0x1EB0
void     Storm_Sub1F30(uint32_t* a, uint32_t count, const uint32_t* src);       // 0x1F30
extern "C" void Storm_Sub1FC0(void) {}  // 1.13c Storm+0x1FC0: body not recovered yet (GUESS: no-op); the 3270 FN vector judges it
void*    Storm_Sub1C580(uint32_t size, const char* src, int32_t line, uint32_t f);   // 0x1C580 SMemAlloc
void*    Storm_Sub1C760(void* p, uint32_t size, const char* src, int32_t line, uint32_t f); // 0x1C760 SMemReAlloc
int32_t  Storm_Sub1B960(void* p, const char* src, int32_t line);                // 0x1B960 SMemFree
void     Storm_Sub605E(void* p, uint32_t sz, int32_t n, void (*dtor)(void*));   // 0x605E _eh_vector_destructor_iterator_
void     Storm_Sub3820(void* p);                                                // 0x3820
void     Storm_Sub4D60(void* p);                                                // 0x4D60
void* Storm_Sub3080(void* src, void* dst);                                      // 0x3080
}

namespace {
inline uint32_t* w(void* p, uint32_t off) { return (uint32_t*)((uint8_t*)p + off); }
const char kAurgn[] = "(AURGN)";   // retail s___AURGN___6fc41c3c
const char kI[] = "(I)";           // retail s__I_6fc41c80
const char kE[] = "(E)";           // retail s__E_6fc41c74
const char kEvt[] = "(AUEVENTREC)";// retail s___AUEVENTREC___6fc41ca8
}

extern "C" void Storm_Sub2C60(uint32_t cap, uint32_t* self);

// 1.13c Storm+0x26E0
extern "C" void* Storm_Sub26E0(uint32_t* self) {
    if (self[0] < self[1] + 1) {
        if (self[3] == 0) Storm_Sub1C20(self);
        // 0x2C60 takes the new capacity in EAX (the grown count) and the array in ESI
        Storm_Sub2C60(self[0], self);
    }
    Storm_Sub2D80((uint8_t*)(uintptr_t)self[2] + self[1] * 0x60);
    uint32_t i = self[1]++;
    return (uint8_t*)(uintptr_t)self[2] + i * 0x60;
}

// 1.13c Storm+0x2C60
extern "C" void Storm_Sub2C60(uint32_t cap, uint32_t* self) {
    uint8_t* old = (uint8_t*)(uintptr_t)self[2];
    if (cap < self[1]) {
        uint8_t* p = old + (size_t)cap * 0x60;
        for (uint32_t i = cap; i < self[1]; i++, p += 0x60) Storm_Sub1C30(p);
    }
    self[0] = cap;
    uint8_t* n = (uint8_t*)Storm_Sub1C760(old, cap * 0x60, kAurgn, -2, 0x10);
    self[2] = (uint32_t)(uintptr_t)n;
    if (!n) {
        n = (uint8_t*)Storm_Sub1C580(cap * 0x60, kAurgn, -2, 0);
        self[2] = (uint32_t)(uintptr_t)n;
        if (old) {
            uint32_t c = self[1] < cap ? self[1] : cap;
            uint8_t* p = old;
            for (uint32_t off = 0; c; c--, off += 0x60, p += 0x60) {
                Storm_Sub3080(p, n + off);
                Storm_Sub1C30(p);
            }
            Storm_Sub1B960(old, kAurgn, -2);
        }
    }
}

// 1.13c Storm+0x2D20
extern "C" void Storm_Sub2D20(uint32_t* p) {
    p[1] = 0; p[2] = 0; p[0] = 0;
    p[1] = (uint32_t)(uintptr_t)&p[1];
    p[2] = ~(uint32_t)(uintptr_t)&p[1];
    p[0] = 0x10;
    p[1] = (uint32_t)(uintptr_t)&p[1];
    p[2] = ~(uint32_t)(uintptr_t)&p[1];
}

extern "C" void* Storm_Sub30D0(uint8_t* d, uint8_t* s);
extern "C" void* Storm_Sub3190(uint32_t* p, uint8_t* src);
extern "C" void* Storm_Sub3200(uint32_t* p, uint8_t* src);
extern "C" void* Storm_Sub3270(uint32_t* p, uint8_t* src);

// 1.13c Storm+0x3080
extern "C" void* Storm_Sub3080(void* self, void* dst) {
    if (dst) Storm_Sub30D0((uint8_t*)dst, (uint8_t*)self);
    return self;
}

// 1.13c Storm+0x30D0
extern "C" void* Storm_Sub30D0(uint8_t* d, uint8_t* s) {
    *w(d, 4) = 0; *w(d, 8) = 0; *w(d, 0xc) = 0; *w(d, 0x10) = 0;
    Storm_Sub3190(w(d, 0x18), s);
    Storm_Sub3200(w(d, 0x28), s);
    Storm_Sub3270(w(d, 0x38), s);
    for (uint32_t o = 0x48; o <= 0x5c; o += 4) *w(d, o) = *w(s, o);
    return d;
}

// 1.13c Storm+0x3190
extern "C" void* Storm_Sub3190(uint32_t* p, uint8_t* src) {
    p[0] = 0; p[1] = 0; p[2] = 0;
    Storm_Sub1EB0(p, *w(src, 4), (const uint8_t*)(uintptr_t)*w(src, 8));
    p[3] = *w(src, 0xc);
    return p;
}

// 1.13c Storm+0x1F30: set a 0x10-byte-element (AURECTF) array {alloc,count,data} to `count` copies of src.
extern "C" void Storm_Sub1F30(uint32_t* a, uint32_t count, const uint32_t* src) {
    a[0] = count;
    if (a[2] || count) {
        a[2] = (uint32_t)(uintptr_t)Storm_Sub1C760((void*)(uintptr_t)a[2], count << 4, "AURECTF", -2, 0);
        if (count) memcpy((void*)(uintptr_t)a[2], src, count << 4);
    }
    a[1] = count;
}

// 1.13c Storm+0x3200
extern "C" void* Storm_Sub3200(uint32_t* p, uint8_t* src) {
    p[0] = 0; p[1] = 0; p[2] = 0;
    Storm_Sub1F30(p, *w(src, 4), (const uint32_t*)(uintptr_t)*w(src, 8));
    p[3] = *w(src, 0xc);
    return p;
}

// 1.13c Storm+0x3270
extern "C" void* Storm_Sub3270(uint32_t* p, uint8_t* src) {
    p[0] = 0; p[1] = 0; p[2] = 0;
    Storm_Sub1FC0();
    p[3] = *w(src, 0xc);
    return p;
}

// 1.13c Storm+0x3770
extern "C" void Storm_Sub3770(uint8_t* self) {
    if (*w(self, 8)) Storm_Sub1B960((void*)(uintptr_t)*w(self, 8), kI, -2);
}

// 1.13c Storm+0x3910
extern "C" void Storm_Sub3910(void* p) { Storm_Sub605E(p, 0x14, 0x10, Storm_Sub3820); }

// 1.13c Storm+0x3950
extern "C" void Storm_Sub3950(uint8_t* self) {
    if (*w(self, 8)) Storm_Sub1B960((void*)(uintptr_t)*w(self, 8), kE, -2);
}

// 1.13c Storm+0x4250
extern "C" uint32_t Storm_Sub4250(uint8_t* param, int32_t* esi) {
    int32_t* q = esi ? esi : (int32_t*)(param + 4);
    uint32_t v = (uint32_t)q[1];
    Storm_Sub4D60(esi);
    Storm_Sub1B960(esi, kEvt, -2);
    return (((int32_t)v < 1) - 1) & v;
}
