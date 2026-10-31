// src/core/d2win/d2win_a690.cpp -- 1.13c D2Win.dll+0x0A690, "Insert Cinematics Disc" message box
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile or, where Ghidra defines no function, the
// disassembly. Imported callees and D2Win state are seams on the Env struct, passed last, so the
// retail argument order is preserved. Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win/
// (control type numbering and header layout only).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2WinA690Env {
    // D2Gfx #10048: the game window.
    uint32_t (*hwnd)(void);
    // user32 MessageBoxA.
    int32_t (*messageBox)(uint32_t hwnd, const char* text, const char* caption, uint32_t type);
};

// 1.13c D2Win+0x0A690 (ordinal 10113): MB_OKCANCEL | MB_ICONEXCLAMATION (0x31); 1 when IDOK.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_A690(const D2WinA690Env* env) {
    int32_t r = env->messageBox(env->hwnd(), "Insert Cinematics Disc", "Diablo II", 0x31);
    return r == 1;
}
