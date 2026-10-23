// src/core/d2win/d2win_sub_19315.cpp -- 1.13c D2Win.dll statically linked CRT string->double (atof core),
// from the cached Ghidra decompile (build-wasm/ghidra-d2win/decomp/0x19315.c).
#include <cstdlib>
#include <emscripten.h>

// 1.13c D2Win+0x19315
// Parses the string via the CRT 12-byte long-double scanner (sub_19872) then narrows to double (__ld12tod).
// ponytail: libc strtod stands in for the scanner; differs only on inf/nan/hex spellings the CRT rejects.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_19315(double* out, const unsigned char* str) {
    *out = strtod((const char*)str, nullptr);
}
