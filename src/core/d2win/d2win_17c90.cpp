// src/core/d2win/d2win_17c90.cpp -- 1.13c D2Win.dll+0x17C90, module initialiser (ordinal 10052)
// Decompile-derived from the retail bytes and the cached Ghidra decompile, resolved against the disassembly.
// Imported/ordinal callees and the D2Win globals are seams on the Env struct, passed last.
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win17C90Env {
    uint32_t* initFlag;                                  // 0x6F9014C0
    int32_t (*ord10072)(uint32_t p1, uint32_t p2);       // D2Win+0x77DA
    void (*ord10068)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);   // D2Win+0x7732
    uint32_t (*ord10048)();                              // D2Win+0x7816
    uint32_t* g1490;                                     // 0x6F901490
    uint32_t* zeroed[12];                                // 6F901494, 9014A0, 9014A4, 9014A8, 9014B0, 9014B4, 9014AC, 9014BC, 9014B8, 6F9A9CEC, 9014E8, 6F9A9CF0
    uint32_t* g149c;                                     // 0x6F90149C
    uint32_t* g1498;                                     // 0x6F901498
};

// Entry (stdcall p1, p2; ret 8). Returns 0 when the first ordinal fails, else 1.
// 1.13c D2Win+0x17C90
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_17C90(uint32_t p1, uint32_t p2, const D2Win17C90Env* e) {
    *e->initFlag = 1;
    if (e->ord10072(p1, p2) == 0) return 0;
    e->ord10068(0, 0x800000, 0x7d000, 0);
    *e->g1490 = e->ord10048();
    for (int i = 0; i < 12; i++) *e->zeroed[i] = 0;
    *e->g149c = p2;
    *e->g1498 = p1;
    return 1;
}
