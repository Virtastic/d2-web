// src/core/d2win/d2win_9f30.cpp -- 1.13c D2Win.dll+0x09F30, account list (type 12): remove a name
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

struct D2Win9F30Env {
    // Fog #10043 free(p, file, line, 0).
    void (*free)(void* p, const char* file, int32_t line, int32_t n0);
};

static const char kAccountListCpp[] = "..\\Source\\D2Win\\Src\\D2WinAccountList.cpp";

// 1.13c D2Win+0x09F30 (ordinal 10137). Nodes are 0x38 bytes: name at +0, next at +0x34, chained
// from [0x10]; [0x11] is the selection. Returns 1 when a node named `name` was unlinked and freed.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_9F30(int32_t* ctl, const char* name, const D2Win9F30Env* env) {
    if (ctl[0] != 0xC) fatal(0x90);
    uint32_t prev = 0;
    for (uint32_t n = (uint32_t)ctl[0x10]; n; prev = n, n = rd32(ptr(n), 0x34)) {
        if (std::strcmp((const char*)ptr(n), name) != 0) continue;
        if ((uint32_t)ctl[0x11] == n) ctl[0x11] = 0;
        if (prev) wr32(ptr(prev), 0x34, rd32(ptr(n), 0x34));
        else ctl[0x10] = (int32_t)rd32(ptr(n), 0x34);
        env->free(ptr(n), kAccountListCpp, 0xA7, 0);
        return 1;
    }
    return 0;
}
