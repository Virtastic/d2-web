// src/core/d2win/d2win_a500.cpp -- 1.13c D2Win.dll+0x0A500, close the eight archive slots, then D2Win+0xA340
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

struct D2WinA500Env {
    // The eight slot globals in retail's order: 0x6F9A9CCC, CD4, CB8, CBC, CD0, CE4, CB4, CE0.
    // Each holds a pointer to a block whose first dword is a Storm archive handle.
    uint32_t* slots[8];
    // Storm #252 (SFileCloseArchive).
    void (*storm252)(uint32_t handle);
    // Fog #10043 free(p, file, line, 0).
    void (*free)(void* p, const char* file, int32_t line, int32_t n0);
    // D2Win+0xA340, tail-called.
    void (*sub_A340)(void);
};

// Retail EDX 0x6F8FD150 (rva 0x1D150).
static const char kArchiveCpp[] = "..\\Source\\D2Hell\\SRC\\Archive.cpp";

// 1.13c D2Win+0x0A500 (ordinal 10158)
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_A500(const D2WinA500Env* env) {
    for (int i = 0; i < 8; ++i) {
        uint32_t g = *env->slots[i];
        if (!g) continue;
        uint32_t h = rd32(ptr(g), 0);
        if (h) env->storm252(h);
        wr32(ptr(g), 0, 0);
        env->free(ptr(g), kArchiveCpp, 0x26E, 0);
        *env->slots[i] = 0;
    }
    env->sub_A340();
}
