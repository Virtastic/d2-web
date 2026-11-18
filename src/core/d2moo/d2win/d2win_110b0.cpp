// src/core/d2win/d2win_110b0.cpp -- 1.13c D2Win.dll+0x110B0, create the popup menu (type 11) at the mouse
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

struct D2Win110B0Env {
    uint32_t* popup;                       // 0x6F9A9E24
    uint32_t* head;                        // 0x6F9014A0, control list (next at +0x3C)
    const int32_t* mouseX;                 // 0x6F901488
    const int32_t* mouseY;                 // 0x6F90148C
    int32_t (*destroy)(uint8_t* ctl);      // D2Win+0x17FC0
    void* (*alloc)(uint32_t size, const char* file, int32_t line, int32_t n0);   // Fog #10042
    uint32_t drawFn;                       // 0x6F8F0EF0 -> +0x1C
    uint32_t msgFn;                        // 0x6F8F0D80 -> +0x20
};

static const char kPopupCpp[] = "..\\Source\\D2Win\\Src\\D2WinPopup.cpp";

// 1.13c D2Win+0x110B0 (ordinal 10029). Any old popup is destroyed first; the new 0x468-byte
// control is zeroed, flagged bits 0, 2, 3, placed at the mouse, appended to the control list.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_110B0(const D2Win110B0Env* env) {
    if (*env->popup) {
        env->destroy(ptr(*env->popup));
        *env->popup = 0;
    }
    uint8_t* p = (uint8_t*)env->alloc(0x468, kPopupCpp, 0x29, 0);
    std::memset(p, 0, 0x468);
    wr32(p, 0, 0xB);
    wr32(p, 8, rd32(p, 8) | kBm0 | kBm2 | kBm3);
    wr32(p, 0xC, (uint32_t)*env->mouseX);
    wr32(p, 0x10, (uint32_t)*env->mouseY);
    wr32(p, 0x1C, env->drawFn);
    wr32(p, 0x20, env->msgFn);
    wr32(p, 0x3C, 0);
    for (uint32_t o = 0x24; o <= 0x34; o += 4) wr32(p, o, 0);
    wr32(p, 0x464, 0xFFFFFFFFu);
    if (!*env->head) {
        *env->head = slot(p);
    } else {
        uint8_t* c = ptr(*env->head);
        while (rd32(c, 0x3C)) c = ptr(rd32(c, 0x3C));
        wr32(c, 0x3C, slot(p));
    }
    *env->popup = slot(p);
}
