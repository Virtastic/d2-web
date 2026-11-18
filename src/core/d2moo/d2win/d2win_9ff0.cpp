// src/core/d2win/d2win_9ff0.cpp -- 1.13c D2Win.dll+0x09FF0, account list (type 12): push a name
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

struct D2Win9FF0Env {
    // Fog #10042 alloc(size, file, line, 0).
    void* (*alloc)(uint32_t size, const char* file, int32_t line, int32_t n0);
};

static const char kAccountListCpp[] = "..\\Source\\D2Win\\Src\\D2WinAccountList.cpp";

// 1.13c D2Win+0x09FF0 (ordinal 10038): a 0x38-byte node, strncpy(node, s, 0x32), pushed at the
// head [0x10]. Bytes 0x32-0x33 are whatever the allocator left there, as in retail.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_9FF0(int32_t* ctl, const char* s, const D2Win9FF0Env* env) {
    if (ctl[0] != 0xC) fatal(0x72);
    uint8_t* node = (uint8_t*)env->alloc(0x38, kAccountListCpp, 0x77, 0);
    std::strncpy((char*)node, s, 0x32);
    wr32(node, 0x34, (uint32_t)ctl[0x10]);
    ctl[0x10] = (int32_t)slot(node);
}
