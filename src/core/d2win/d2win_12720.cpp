// src/core/d2win/d2win_12720.cpp -- 1.13c D2Win.dll+0x12720 (export 10051), the register shim in front of 0x11960
// Decompile-derived from the retail bytes: the body is `mov eax,edx ; jmp 0x6F8F1960`, a jump into D2Win's own
// glyph-by-glyph text routine (not an import thunk), so the export takes (text in ECX, x in EDX, y on the stack)
// and runs 0x11960 with x moved to EAX. The Env seam is 0x11960's.
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win11960Env;
extern "C" void d2win_sub_11960(int32_t x, const uint16_t* text, int32_t y, const D2Win11960Env* env);

// 1.13c D2Win+0x12720 (ordinal 10051; text in ECX, x in EDX, y on the stack).
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_12720(const uint16_t* text, int32_t x, int32_t y, const D2Win11960Env* env) {
    d2win_sub_11960(x, text, y, env);
}
