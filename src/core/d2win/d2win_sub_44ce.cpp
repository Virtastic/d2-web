// src/core/d2win/d2win_sub_44ce.cpp -- D2Win VC7.1 CRT heap-lock release stub.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Win+0x044ce: tail-calls FUN_2396(4), the CRT __unlock(4) (leave heap lock 4).
// wasm build is single-threaded, so there is no lock to take; no-op.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_44ce(void) {}
