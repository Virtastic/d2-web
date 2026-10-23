// src/core/d2win/d2win_sub_1b3c.cpp -- D2Win.dll CRT _output (printf core), from the cached decompile.
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <emscripten.h>

// 1.13c D2Win+0x01B3C (_output): format `fmt` with the argument list into the string stream `dest`
// (its only caller, _sprintf 0x101F, passes a 0x7fffffff-room string stream), return the characters
// written (negative on a conversion error, e.g. %c wctomb failure). The state machine (flags -+ #0,
// width/precision incl. *, h/l/w/I32/I64, c C d i u o x X p s S Z n e E f g G) is the VC7.1 table; here
// it is the libc formatter, which matches for those conversions. Unlike retail, the NUL is written
// here (the caller writes it again). %Z (ANSI_STRING) is not modelled.
extern "C"
EMSCRIPTEN_KEEPALIVE int d2win_sub_1B3C(char* dest, const char* fmt, va_list ap) {
    return vsnprintf(dest, dest ? 0x7fffffff : 0, fmt, ap);
}
