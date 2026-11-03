// src/core/d2win/d2win_12f60.cpp -- 1.13c D2Win.dll+0x12F60, draw text, measuring it first when centred
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile or, where Ghidra defines no function, the
// disassembly. Imported callees and D2Win state are seams on the Env struct, passed last, so the
// retail argument order is preserved. Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win/
// (control type numbering and header layout only; D2WinFont.cpp for the L"c" colour code).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win12F60Env {
    int32_t (*textWidth)(const uint16_t* text);   // D2Win+0x11FB0 (EAX)
    // D2Win+0x12760 (x in EAX; text, y, width, colour, centre, mode on the stack; ret 0x18). Its EAX is the result of 12F60.
    int32_t (*drawText)(int32_t x, const uint16_t* text, int32_t y, int32_t width, int32_t color,
                        int32_t center, int32_t mode);
};

// 1.13c D2Win+0x12F60 (ordinal 10096; text in ECX, x in EDX; y, colour, centre, mode on the stack; ret 0x10).
// Returns the eax left by the 0x12760 call (retail's last instruction before ret).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_12F60(const uint16_t* text, int32_t x, int32_t y, int32_t color,
                                                       int32_t center, int32_t mode, const D2Win12F60Env* env) {
    int32_t w = center ? env->textWidth(text) + 8 : 0;
    return env->drawText(x, text, y, w, color, center, mode);
}
