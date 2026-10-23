// src/core/d2win/d2win_sub_2a5a.cpp -- CRT thunk used by __mtinit as the TlsAlloc fallback for FlsAlloc.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Win+0x2A5A: stub body is `jmp [TlsAlloc]`; returns the new TLS index (eax).
// Host seam: wasm has no Win32 TLS, so hand out indices from a counter (TlsAlloc's first index is 0).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_2a5a(void) { static uint32_t next = 0; return next++; }
