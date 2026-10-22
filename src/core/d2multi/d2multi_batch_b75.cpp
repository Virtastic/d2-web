// D2Multi CRT batch b75. Pointer-sized fields are void* (wasm64).
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#define W __attribute__((weak))

extern void** d2multi_DAT_6fa0b4ac W;
extern void** d2multi_DAT_6fa0b4b0 W;
extern void* d2multi_malloc(size_t) W;
extern void d2multi_lock(int) W;
extern void d2multi_unlock(int) W;
extern void d2multi_onexit_lk(void*) W;   // retail passes func in EDI

// 1.13c D2Multi+0x3357
EMSCRIPTEN_KEEPALIVE int d2multi___onexitinit() {
    d2multi_DAT_6fa0b4b0 = (void**)d2multi_malloc(0x80);
    if (!d2multi_DAT_6fa0b4b0) return 0x18;
    *d2multi_DAT_6fa0b4b0 = nullptr;
    d2multi_DAT_6fa0b4ac = d2multi_DAT_6fa0b4b0;
    return 0;
}

// 1.13c D2Multi+0x3AAC
EMSCRIPTEN_KEEPALIVE void d2multi_sub_3AAC() { d2multi_unlock(4); }

// 1.13c D2Multi+0x337F
EMSCRIPTEN_KEEPALIVE void* d2multi___onexit(void* func) {
    d2multi_lock(8);
    d2multi_onexit_lk(func);
    d2multi_unlock(8);
    return func;
}

// 1.13c D2Multi+0x33B7
EMSCRIPTEN_KEEPALIVE int d2multi__atexit(void* func) {
    return (d2multi___onexit(func) != nullptr) - 1;
}
