// 1.13c D2Client tiny wrappers.
// Each returns the callee call list / decision; the FN probes in src/core/fn replay it onto the capture's hook log.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x45BD0: EAX=arg; calls thunk 0xC1A8 (D2Common#10202) with (arg, 0x10, 0x999, &str@0xD608C).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_45bd0(uint32_t arg, uint32_t* w) { w[0] = arg; w[1] = 0x10; w[2] = 0x999; w[3] = 0xD608C; }

// 1.13c D2Client+0x48730: fastcall(ecx=0xC, edx=1, [0]) at 0xC2790, then tail-jump 0x48150 (no args).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_48730(uint32_t* ecx, uint32_t* edx, uint32_t* s0) { *ecx = 0xC; *edx = 1; *s0 = 0; }

// 1.13c D2Client+0x4F750: calls 0x5DE10 with ECX = 0xAA0, 0xAA4 ... while < 0xB3D; returns the count.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_4f750(uint32_t* ecx) {
    uint32_t n = 0; for (uint32_t i = 0xAA0; i < 0xB3D; i += 4) ecx[n++] = i; return n;
}

// 1.13c D2Client+0x78220: a=[0x11BBFC], b=[0x11BFB8]; ret = (a && callee(a,0x36)) ? 1 : b. *called says whether 0xC1FC ran.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_78220(uint32_t a, uint32_t b, uint32_t calleeRet, uint32_t* called) {
    *called = a != 0; return (a && calleeRet) ? 1 : b;
}

// 1.13c D2Client+0x8BB00: g=[0x11BC34]; if g==6 return 0; g=6; return 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_8bb00(uint32_t* g) { if (*g == 6) return 0; *g = 6; return 1; }
