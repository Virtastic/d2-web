// src/core/d2win/d2win_16b90.cpp -- 1.13c D2Win.dll+0x16B90, add an item to a list control (UTF-16 text)
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile or, where Ghidra defines no function, the
// disassembly. Imported callees and D2Win state are seams on the Env struct, passed last, so the
// retail argument order is preserved. Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win/
// (control type numbering, list item layout, control chain fields).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win16B90Env {
    void (*lock)(uint32_t v);                                // D2Win+0x12FE0 (SetUnicodeTextFont)
    uint8_t* (*alloc)(uint32_t size, const char* file, int32_t line, int32_t n0);  // Fog #10042
    int32_t (*textWidth)(const uint16_t* text);              // D2Win+0x11FB0 (EAX)
    int32_t (*textHeight)(void);                             // D2Win+0x11E80, result unused
};

// 1.13c D2Win+0x16B90 (ordinal 10191; list in ECX, text in EDX; a1, a2 byte, a3, head flag,
// select flag on the stack). Control type must be 7 (else fatal 0xA1); text must be shorter than
// 0x100 units (else fatal 0xA6). The item is 0x224 bytes: text at +0, width (text width + 8) at
// +0x200, a1 at +0x204, zeros at +0x208..+0x214, a2 at byte +0x218, a3 at +0x21C, next at +0x220.
// The list's widest item (+0x60) grows to fit. head flag prepends, else the item is appended.
// The count (+0x50) goes up; select flag makes it current (+0x68) at index count-1 (+0x54).
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_16B90(uint8_t* list, const uint16_t* text, uint32_t a1, uint8_t a2,
                                                    uint32_t a3, int32_t head, int32_t select,
                                                    const D2Win16B90Env* env) {
    if (rd32(list, 0) != 7) fatal(0xA1);
    env->lock(rd32(list, 0x40));   // the list's font (eFont), decompile param_1[0x10]
    uint8_t* item = env->alloc(0x224, "..\\Source\\D2Win\\Src\\D2WinList.cpp", 0xA5, 0);
    if (u16len(text) >= 0x100) fatal(0xA6);
    u16cpy(reinterpret_cast<uint16_t*>(item), text);
    int32_t w = env->textWidth(text) + 8;
    (void)env->textHeight();
    if (w > (int32_t)rd32(list, 0x60)) wr32(list, 0x60, (uint32_t)w);
    wr32(item, 0x204, a1);
    wr32(item, 0x200, (uint32_t)w);
    wr32(item, 0x208, 0); wr32(item, 0x20C, 0); wr32(item, 0x210, 0); wr32(item, 0x214, 0);
    wr8(item, 0x218, a2);
    wr32(item, 0x21C, a3);
    if (head) {
        wr32(item, 0x220, rd32(list, 0x64));
        wr32(list, 0x64, slot(item));
    } else {
        uint32_t p = rd32(list, 0x64);
        if (!p) {
            wr32(list, 0x64, slot(item));
        } else {
            while (rd32(ptr(p), 0x220)) p = rd32(ptr(p), 0x220);
            wr32(ptr(p), 0x220, slot(item));
        }
        wr32(item, 0x220, 0);
    }
    wr32(list, 0x50, rd32(list, 0x50) + 1);
    env->lock(1);
    if (select) {
        wr32(list, 0x68, slot(item));
        wr32(list, 0x54, rd32(list, 0x50) - 1);
    }
}
