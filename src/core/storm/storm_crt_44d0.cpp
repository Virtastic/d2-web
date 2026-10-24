// 1.13c Storm batch n01: small list/CRT-lock helpers. Win32/CRT services are host seams passed as arguments.
// Layouts: retail slots are 4 bytes; here one slot is one uintptr_t (wasm64 pointers), index preserved.
#include <cstdint>
#include <cstdlib>
#include <cstring>

extern "C" int32_t Storm_Sub1B960(void* p, const char*, int32_t);

// 1.13c Storm+0x44D0 -- empty circular list head: [1]=&[1], [0]=0, [2]=~&[1]
extern "C" void Storm_Sub44D0(uintptr_t* p) {
    p[1] = 0; p[2] = 0;
    p[1] = (uintptr_t)&p[1];
    p[0] = 0;
    p[2] = ~(uintptr_t)&p[1];
}

// 1.13c Storm+0x4B40 -- SEH-framed thunk to Storm+0x4910 (SEH has no wasm analogue; body passed in)
extern "C" void Storm_Sub4B40(intptr_t arg, void (*sub4910)(intptr_t)) { sub4910(arg); }

// 1.13c Storm+0x4E60 -- free the owned buffer at slot [2] (retail passes "\\AU_PLAYERNAME\\", line -2)
extern "C" void Storm_Sub4E60(uintptr_t* obj) {
    if (obj[1]) Storm_Sub1B960((void*)obj[1], "\\AU_PLAYERNAME\\", -2);
}

// 1.13c Storm+0x50C0 -- zero four slots
extern "C" void Storm_Sub50C0(uintptr_t* p) { memset(p, 0, 4 * sizeof *p); }

// CRT lock table seam: lock(n)/unlock(n) = __lock / FUN_6fbf902a.
struct StormCrtHost {
    void (*lock)(int32_t);
    void (*unlock)(int32_t);
    uint32_t (*tlsAlloc)();
    void (*sbhFree)(void*);          // _free
};

// 1.13c Storm+0x595D -- type_info::~type_info: lock(0xe), free name cache at +4, unlock (Storm+0x599A)
extern "C" void Storm_TypeInfoDtor(void** self, const StormCrtHost* h) {
    h->lock(0xe);
    if (self[1]) h->sbhFree(self[1]);
    h->unlock(0xe);
}

// 1.13c Storm+0x76C1
extern "C" uint32_t Storm_Sub76C1(const StormCrtHost* h) { return h->tlsAlloc(); }
// 1.13c Storm+0x789D
extern "C" void Storm_Sub789D(const StormCrtHost* h) { h->unlock(0xd); }
// 1.13c Storm+0x78A9
extern "C" void Storm_Sub78A9(const StormCrtHost* h) { h->unlock(0xc); }
// 1.13c Storm+0x8D25
extern "C" void Storm_Sub8D25(const StormCrtHost* h) { h->unlock(4); }
// 1.13c Storm+0x8D9F
extern "C" void Storm_Sub8D9F(const StormCrtHost* h) { h->lock(8); }
// 1.13c Storm+0x8DA8
extern "C" void Storm_Sub8DA8(const StormCrtHost* h) { h->unlock(8); }

// 1.13c Storm+0x8BBD -- CRT realloc. The small-block heap (__sbh) is a retail heap detail; the wasm heap
// is one flat allocator, so every path collapses to HeapReAlloc semantics: min size 1, 0xFFFFFFE0 limit,
// new-handler retry loop (newh returns nonzero to retry).
extern "C" void* Storm_CrtRealloc(void* p, uint32_t n, int (*newh)(uint32_t)) {
    if (!p) {
        for (;;) {
            if (n > 0xffffffe0u) { if (newh) newh(n); return nullptr; }
            void* r = malloc(n ? n : 1);
            if (r) return r;
            if (!newh || !newh(n)) return nullptr;
        }
    }
    if (n == 0) { free(p); return nullptr; }
    for (;;) {
        if (n > 0xffffffe0u) { if (newh) newh(n); return nullptr; }
        void* r = realloc(p, n);
        if (r) return r;
        if (!newh || !newh(n)) return nullptr;
    }
}
