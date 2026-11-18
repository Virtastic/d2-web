// src/core/d2client/d2client_batch_072e4.cpp -- 1.13c D2Client.dll alpha lane, Stage A tiny batch:
// 0x072E4 __initptd, 0x0749A, 0x074A6, 0x08005, 0x080BC, reimplemented from
// build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000.
#include <cstdint>
#include <emscripten.h>

// 0x47AB _unlock(idx): recovered in d2client_batch_3664.cpp.
extern "C" void d2_client_47ab(uint32_t idx);

// 1.13c D2Client+0x072e4 (19 B, caller 0x4475 __CRT_INIT): __initptd(ptd, locale). Seed a _ptiddata
// block: _initaddr at +0x54 = DAT_6fb8b7b8 (stored as its RVA 0xDB7B8, the port's convention for
// retail VAs) and _holdrand at +0x14 = 1. The locale argument is not touched.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_initptd(uint8_t* ptd, void* locale) {
    (void)locale;
    *(uint32_t*)(ptd + 0x54) = 0xDB7B8u;   // _initaddr, retail 0x6FB8B7B8
    *(uint32_t*)(ptd + 0x14) = 1u;         // _holdrand
}

// Test seam (FN probes): the _unlock callee, replaceable so a probe can record the lock number.
extern "C" void (*d2_client_unlock_seam)(uint32_t) = d2_client_47ab;

// 1.13c D2Client+0x0749a (9 B, caller 0x7368): release CRT lock 0xD.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_749a(void) {
    d2_client_unlock_seam(0xD);
}

// 1.13c D2Client+0x074a6 (9 B, caller 0x7368): release CRT lock 0xC.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_74a6(void) {
    d2_client_unlock_seam(0xC);
}

// 1.13c D2Client+0x08005 (9 B, caller 0x7E9D): release CRT lock 4.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_8005(void) {
    d2_client_47ab(4);
}

// 1.13c D2Client+0x080bc (9 B, caller 0x804F): release CRT lock 4.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_80bc(void) {
    d2_client_47ab(4);
}
