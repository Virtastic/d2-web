// src/core/d2win/d2win_batch_9740.cpp -- 1.13c D2Win.dll function transcribed from the cached Ghidra
// decompile (build-wasm/ghidra-d2win/decomp/0x9740.c). Alpha batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector.
//
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win and include/D2Unicode.h (Unicode is a 2-byte
// code unit; utf8ToUnicode fills a counted UCS-2 buffer). The 0x93C0 callee is the text-box appender
// already implemented in src/core/gfx/d2win_batch_93c0.cpp; its host seams are passed straight through.
#include <cstdint>
#include <unistd.h>
#include <emscripten.h>
#include "../lang/strtable.h"

struct D2WinBatch93C0Env;
extern "C" uint32_t d2win_sub_93C0(void* box, const uint16_t* text, int32_t param3,
                                   uint32_t param4, int32_t param5,
                                   const D2WinBatch93C0Env* env);

// Retail: Fog #10265(0x11e) -> #10024("") -> _exit(-1).
[[noreturn]] static void d2win_fatal(uint32_t) { _exit(-1); }

// 1.13c D2Win+0x9740 (125 bytes): zero 2048 UCS-2 units on the stack, convert the caller's UTF-8
// string into at most 0x400 of them (Unicode::utf8ToUnicode), then hand the control, the UCS-2 buffer
// and the third argument to D2Win+0x93C0 with two zero words. A null control is fatal 0x11E.
// __fastcall: ECX = control, EDX = UTF-8 string, stack = third argument. Returns void; the appender's
// result is discarded.
// 1.13c D2Win+0x9740
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_9740(int32_t* param_1, const char* param_2, int32_t param_3,
                                         const D2WinBatch93C0Env* env) {
    if (param_1 == nullptr) d2win_fatal(0x11E);

    uint16_t str[2048];
    for (uint32_t i = 0; i < 2048; ++i) str[i] = 0;

    const std::u16string u = d2::utf8ToU16(param_2 ? param_2 : "");
    int32_t n = (int32_t)u.size();
    if (n > 0x3FF) n = 0x3FF;
    for (int32_t i = 0; i < n; ++i) str[i] = (uint16_t)u[(size_t)i];
    str[n] = 0;

    d2win_sub_93C0(param_1, str, param_3, 0, 0, env);
}
