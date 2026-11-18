// src/core/d2win/d2win_be80.cpp -- 1.13c D2Win.dll+0x0BE80, set a control's guild emblem and its two colours
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

#include <cstdio>

struct D2WinBE80Env {
    // D2Win+0xA7A0: load a cell file by path (path in ECX, 0 in EDX in retail).
    uint32_t (*loadCell)(const char* path, uint32_t n0);
    // The colour byte tables, indexed 1..10: 0x6F9A8C8B[c1] and 0x6F9A9CA7[c2] (base = index 0).
    const uint8_t* colors1;
    const uint8_t* colors2;
};

// 1.13c D2Win+0x0BE80 (ordinal 10183). An emblem outside 1..0x20 changes nothing; otherwise
// +0xBC = the emblem cell "DATA\GLOBAL\ui\emblems\icon%02dc" (emblem - 1), and +0xC0 / +0xC1 the
// colours from the two tables (0 when the index is outside 1..10).
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_BE80(uint8_t* ctl, uint8_t emblem, uint8_t c1, uint8_t c2,
                                                   const D2WinBE80Env* env) {
    if (!ctl) fatal(0xDA5);
    if (emblem < 1 || emblem > 0x20) return;
    char path[0x200];
    std::snprintf(path, sizeof path, "%s\\ui\\emblems\\icon%02dc", "DATA\\GLOBAL", (int)emblem - 1);
    wr32(ctl, 0xBC, env->loadCell(path, 0));
    wr8(ctl, 0xC0, (c1 >= 1 && c1 <= 10) ? env->colors1[c1] : 0);
    wr8(ctl, 0xC1, (c2 >= 1 && c2 <= 10) ? env->colors2[c2] : 0);
}
