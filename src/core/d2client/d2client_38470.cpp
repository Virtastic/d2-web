// 1.13c D2Client+0x38470: edit-box draw. fastcall(ECX = box), no stack args, plain ret. Written from the disassembly;
// every callee is a call-log record through cl::Env::H. Box fields (packed): +0x10 x, +0x14 y, +0x18 wrap width, +0x1c single-line flag,
// +0x28 text (wchar*), +0x2c scratch (wchar*), +0x34 u16 capacity, +0x36 cursor, +0x3a length, +0x46 blink counter, +0x4a blink flag, +0x4e font, +0x56 border.
#include "d2client_editbox.h"
namespace {
inline uint32_t w16(const cl::Env& e, uint32_t a) { return e.R(a) & 0xffff; }
inline void put16(const cl::Env& e, uint32_t a, uint32_t v) { e.W(a, (e.R(a) & 0xffff0000u) | (v & 0xffff)); }
inline uint32_t ptr(const void* p) { return (uint32_t)(uintptr_t)p; }
}
extern "C" void d2c_38470(uint32_t t, const cl::Env& e) {
    e.W(t + 0x46, e.R(t + 0x46) + 1);
    if (e.R(t + 0x56) != 0) {
        const uint32_t y = e.R(t + 0x14) - 0x10, x = e.R(t + 0x10) - 10;
        e.H(0xd0ba, x, y, x + e.R(t + 0x18), y + 0x12, 0);   // 6 stack words (x, y, right, bottom, 0, 2); the descriptor's X logs words 4 and 5
        const uint32_t w[2] = { 0, 2 };
        e.Xw(0xd0ba, w, 2);
    }
    e.H(0xd3a8, 0, 0, 0, 0, 0, 0, e.R(t + 0x4e));
    // locals: the wide cursor "_" (win2Unicode of the narrow "_") and the 200-char scratch line; the Unicode ctors are unhooked (they zero)
    uint16_t cur[2] = {}, line[200] = {};
    const uint8_t narrow[2] = { 0x5f, 0 };
    e.H(0xceac0, 2, 0, 0, 0, 0, 0, ptr(cur), ptr(narrow));
    const uint32_t flag = e.R(t + 0x4a), cnt = e.R(t + 0x46);
    if (flag == 0 ? (cnt % 10 == 0) : (cnt % 20 == 0)) e.W(t + 0x4a, flag == 0 ? 1u : 0u);
    const uint32_t x = e.R(t + 0x10), y = e.R(t + 0x14);
    if (e.R(t + 0x1c) == 1) {
        int32_t edi = 0;
        const int32_t w = (int32_t)e.H(0xd35a, 0, 0, 0, 0, 0, 0, e.R(t + 0x28));
        if (w > (int32_t)e.R(t + 0x18)) {
            const int32_t n = (int32_t)e.H(0xceab0, 0, 0, 0, 0, 0, 0, e.R(t + 0x28));
            if (n > 0) {
                do {
                    const int32_t w2 = (int32_t)e.H(0xd35a, 0, 0, 0, 0, 0, 0, e.R(t + 0x28) + (uint32_t)edi * 2);
                    if (w2 <= (int32_t)e.R(t + 0x18)) break;
                    ++edi;
                } while (edi < n);
            }
        }
        const uint32_t off = (uint32_t)edi * 2;
        e.H(0xd37e, y, 0, 0, 0, 0, 0, e.R(t + 0x28) + off, x);
        const int32_t cursor = (int32_t)e.R(t + 0x36);
        if ((int32_t)w16(e, t + 0x34) <= cursor) return;
        if (e.R(t + 0x4a) == 0) return;
        if (cursor > (int32_t)e.R(t + 0x3a)) return;
        if (e.H(0xceab0, 0, 0, 0, 0, 0, 0, e.R(t + 0x28) + off) != 0) {
            e.H(0xceab8, 0, 0, 0, 0, 0, 0, e.R(t + 0x2c), e.R(t + 0x28));
            put16(e, e.R(t + 0x2c) + e.R(t + 0x36) * 2, 0);
            const uint32_t cw = e.H(0xd35a, 0, 0, 0, 0, 0, 0, e.R(t + 0x2c));
            e.H(0xd37e, y, 0, 0, 0, 0, 0, ptr(cur), cw + e.R(t + 0x10));
            return;
        }
        e.H(0xd37e, y, 0, 0, 0, 0, 0, ptr(cur), e.R(t + 0x10));
        return;
    }
    // multi-line: 10059 breaks the text at the wrap width into a list of {char16* text; next}
    uint32_t wout = 0;
    uint32_t node = e.H(0xd378, e.R(t + 0x18), 0, 0, 0, 0, 0, e.R(t + 0x28), ptr(&wout));
    int32_t lines = 0, total = 0, done = 0;
    uint32_t ebp = 0, lastNext = 0;
    if (node != 0) {
        do {
            e.H(0xd37e, y + ebp, 0, 0, 0, 0, 0, e.R(node), x);
            const int32_t len = (int32_t)e.H(0xceab0, 0, 0, 0, 0, 0, 0, e.R(node));
            lastNext = e.R(node + 4);
            const int32_t cursor = (int32_t)e.R(t + 0x36);
            ++lines;
            ebp += 0x14;
            total += len;
            if (cursor < (int32_t)w16(e, t + 0x34) && e.R(t + 0x4a) != 0 && total >= cursor && done == 0) {
                int32_t ebx = cursor - total + len;
                done = 1;
                if (ebx < 0) ebx = 0;
                uint32_t dx;
                uint32_t cy;
                if (e.H(0xceab0, 0, 0, 0, 0, 0, 0, e.R(node)) != 0) {
                    if ((int32_t)e.H(0xceab0, 0, 0, 0, 0, 0, 0, e.R(node)) > 0xc7) return;
                    e.H(0xceab8, 0, 0, 0, 0, 0, 0, ptr(line), e.R(node));
                    cy = y + ebp - 0x14;
                    line[ebx] = 0;
                    dx = e.H(0xd35a, 0, 0, 0, 0, 0, 0, ptr(line)) + x;
                } else {
                    cy = y + ebp - 0x14;
                    dx = x;
                }
                e.H(0xd37e, cy, 0, 0, 0, 0, 0, ptr(cur), dx);
            }
            e.H(0xbf86, 0x103, 0, 0, 0, 0, 0, e.R(node), 0x6fb86918u);
            e.H(0xbf86, 0x104, 0, 0, 0, 0, 0, node, 0x6fb86918u);
            node = lastNext;
        } while (node != 0);
        (void)lastNext;
    }
    if (lines != 0) return;
    if (e.R(t + 0x4a) == 0) return;
    e.H(0xd37e, y, 0, 0, 0, 0, 0, ptr(cur), x);
}
