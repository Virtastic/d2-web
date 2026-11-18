// src/core/d2win/d2win_14ec0.cpp -- 1.13c D2Win.dll+0x14EC0, key handler of the edit-box control (type 1)
// Decompile-derived from the retail bytes and the cached Ghidra decompile, resolved against the disassembly: the
// register inputs, the key byte passed by pointer to the filter callback, the two jump tables, the callbacks' calling
// shapes and which exits skip the post callback. Imported callees and D2Win state are seams on the Env struct, passed
// last, so the retail argument order is preserved.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win14EC0Env {
    const uint32_t* bitMasks;                               // 0x6F8FB17C -> gdwBitMasks
    const uint32_t* fontIndex;                              // 0x6F8FFD34
    const uint32_t* fontTable;                              // 0x6F901518, 5 dwords per entry
    uint32_t* nestLevel;                                    // 0x6F9014A8, bumped around the control's own callbacks
    int16_t (*getKeyState)(int32_t vk);                     // user32 GetKeyState
    int32_t (*selActive)(uint8_t* ctl);                     // D2Win+0x137B0 (control in EAX)
    void (*selDelete)(uint8_t* ctl);                        // D2Win+0x139A0 (control on the stack)
    void (*selClear)(uint8_t* ctl);                         // D2Win+0x13770 (control in ECX)
    void (*lineXY)(uint8_t* ctl, int32_t* x, int32_t* y);   // D2Win+0x14350
    void (*placeCaret)(uint32_t* ctlAndZero, int32_t x, int32_t y);   // D2Win+0x13DA0
    void (*refresh)(uint8_t* ctl);                          // D2Win+0x13AC0 (control in ESI)
    void (*focusTo)(uint8_t* ctl);                          // D2Win+0x17A90 (control on the stack)
    void (*focusFix)(uint8_t* ctl);                         // D2Win+0x13720 (control in ECX)
    // The control's stored callbacks (32-bit retail pointers), invoked by the seam.
    int32_t (*callFilter)(uint32_t cb, uint8_t* ctl, uint32_t cursor, uint8_t* key);   // +0x268: (ctl, cursor, &key)
    void (*callArg)(uint32_t cb, uint32_t arg);             // +0x264 / +0x26C: (arg)
    void (*memmove16)(void* d, const void* s, uint32_t n);  // D2Win+0x1230
};

static inline int32_t idxOf(const uint8_t* c, const uint16_t* p) { return (int32_t)(((const uint8_t*)p - c - 0x5c) >> 1); }
static inline uint16_t* textAt(uint8_t* c, uint32_t i) { return (uint16_t*)(c + 0x5c) + i; }

// Entry (ESI/EAX = the control, key code on the stack; ret 4). Returns 1 when the key was consumed.
// Keys: 8 backspace, 9 tab (focus next/previous, shift reverses), 13 enter, 27 escape, 0x23..0x28 End/Home/Left/Up/
// Right/Down (shift extends the selection), 0x2E delete, 0x70 (returns 0); anything else only runs the filter tail.
// 1.13c D2Win+0x14EC0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_14EC0(uint8_t* c, int8_t keyIn, const D2Win14EC0Env* e) {
    if (!c || rd32(c, 0) != 1) fatal(c ? 0x302 : 0x300);
    const uint32_t flags = rd32(c, 8);
    if ((e->bitMasks[2] & flags) == 0 || (e->bitMasks[0] & flags) == 0) return 0;
    int8_t key = keyIn;
    bool deleted = false;      // 0x10: a selection was deleted / text changed (post filter runs)
    bool selGone = false;      // ebx: the selection delete already consumed the key
    uint16_t* di = textAt(c, rd32(c, 0x25c));
    const bool shift = ((uint16_t)e->getKeyState(0x10) >> 15) & 1;
    const uint8_t k = (uint8_t)key;
    if (k == 8 || k == 0x2e) {
        if (e->selActive(c) != 0) {
            e->selDelete(c);
            selGone = true; deleted = true;
            di = textAt(c, rd32(c, 0x25c));
        }
    } else if (k >= 0x23 && k <= 0x28) {
        if (shift) { if (rd32(c, 0x54) == 0xffffffffu) { wr32(c, 0x54, rd32(c, 0x25c)); wr32(c, 0x58, rd32(c, 0x25c)); } }
        else e->selClear(c);
    }
    switch (k) {
    case 8:
        if (di > textAt(c, 0) && !selGone) {
            uint16_t* dst = di - 1;
            uint32_t n = ((uint32_t)u16len(di) + 1) * 2;
            e->memmove16(dst, di, n);
            if (dst > textAt(c, 0) && dst[-1] == 0xd) { dst -= 1; e->memmove16(dst, dst + 1, n); }
            deleted = true; di = dst;
        }
        break;
    case 9: {
        uint32_t off = e->getKeyState(0x10) < 0 ? 0x27c : 0x278;
        uint8_t* p = ptr(rd32(c, off));
        if (!p) return 1;
        while (p != c && !((e->bitMasks[2] & rd32(p, 8)) != 0 && (e->bitMasks[0] & rd32(p, 8)) != 0)) {
            p = ptr(rd32(p, off));
            if (!p) break;
        }
        e->focusTo(p);
        e->focusFix(p);
        return 1;
    }
    case 13: {
        const uint32_t cb = rd32(c, 0x264);
        if (cb != 0) {
            const uint32_t keep = rd32(c, 0x260) & 2;
            ++*e->nestLevel;
            e->callArg(cb, slot(c + 0x5c));
            --*e->nestLevel;
            if (keep == 0) return 1;
            di = textAt(c, rd32(c, 0x25c));
        }
        break;
    }
    case 0x1b: {
        const uint32_t cb = rd32(c, 0x264);
        if (cb == 0) return 1;
        ++*e->nestLevel;
        e->callArg(cb, 0);
        --*e->nestLevel;
        return 1;
    }
    case 0x23:
        while (*di != 0) ++di;
        if (shift) wr32(c, 0x58, (uint32_t)idxOf(c, di));
        break;
    case 0x24:
        di = textAt(c, 0);
        if (shift) wr32(c, 0x58, 0);
        break;
    case 0x25:
        di -= 1;
        if (*di == 0xa) di -= 1;
        if (di < textAt(c, 0)) di = textAt(c, 0);
        else if (shift && rd32(c, 0x54) != 0xffffffffu) wr32(c, 0x58, (uint32_t)idxOf(c, di));
        break;
    case 0x26:
        if ((rd8(c, 0x260) & 8) == 0) {
            di = textAt(c, 0);
            if (shift) wr32(c, 0x58, 0);
        } else {
            int32_t x = 0, y = 0;
            e->lineXY(c, &x, &y);
            const int32_t h = (int32_t)font_height(e->fontTable, *e->fontIndex);
            uint32_t s[2] = { slot(c), 0 };
            e->placeCaret(s, x, y - h);
            di = textAt(c, rd32(c, 0x25c));
        }
        break;
    case 0x27:
        if (*di != 0) {
            const uint16_t nx = di[1];
            di += 1;
            if (nx != 0 && nx == 0xa) di += 1;
            if (shift && rd32(c, 0x54) != 0xffffffffu) wr32(c, 0x58, (uint32_t)idxOf(c, di));
        }
        break;
    case 0x28:
        if ((rd8(c, 0x260) & 8) != 0) {
            int32_t x = 0, y = 0;
            e->lineXY(c, &x, &y);
            const int32_t h = (int32_t)font_height(e->fontTable, *e->fontIndex);
            uint32_t s[2] = { slot(c), 0 };
            e->placeCaret(s, x, h + y);
            di = textAt(c, rd32(c, 0x25c));
        } else {
            while (*di != 0) ++di;
            if (shift) wr32(c, 0x58, (uint32_t)idxOf(c, di));
        }
        break;
    case 0x2e:
        if (*di != 0 && !selGone) {
            uint16_t* nx = di + 1;
            uint32_t n = ((uint32_t)u16len(nx) + 1);
            e->memmove16(di, nx, n * 2);
            if (*di != 0 && *di == 0xa) e->memmove16(di, nx, n * 2 - 2);
            deleted = true;
        }
        break;
    case 0x70:
        return 0;
    default:
        break;
    }
    wr32(c, 0x25c, (uint32_t)idxOf(c, di));
    e->refresh(c);
    if (deleted && rd32(c, 0x268) != 0) {
        if (e->callFilter(rd32(c, 0x268), c, rd32(c, 0x25c), (uint8_t*)&key) == 0) {
            if ((uint8_t)key != 8 && (uint8_t)key != 0x2e) return 0;
        }
    }
    if (rd32(c, 0x26c) != 0) e->callArg(rd32(c, 0x26c), 0);
    return 1;
}
