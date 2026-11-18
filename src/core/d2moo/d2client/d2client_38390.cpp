// 1.13c D2Client+0x38390: edit-box number in (thiscall, ECX = this, one stack word = the int, ret 4).
// sprintf("%d") into a 20-byte local; when the text is longer than the capacity this+0x34 (16 bits) the byte at index capacity-1 is
// cleared; 0xCEAC0 (win2Unicode: ECX = this+0x28 buffer, EDX = the local, stack word = capacity) widens it, 0xCEAB0 measures it; the
// length goes to this+0x36 and this+0x3A and is returned. The capture logs the local's first len+1 bytes (hookblk) after the widen call.
#include "cl_env.h"
extern "C" uint32_t d2c_38390(uint32_t self, uint32_t value, const cl::Env& e) {
    uint8_t txt[20] = {};
    uint32_t len = 0;
    {
        uint8_t rev[12]; uint32_t n = 0, mag = (int32_t)value < 0 ? 0u - value : value;
        do { rev[n++] = (uint8_t)('0' + mag % 10); mag /= 10; } while (mag);
        if ((int32_t)value < 0) txt[len++] = '-';
        while (n) txt[len++] = rev[--n];
    }
    const uint32_t cap = e.R(self + 0x34) & 0xffff, buf = e.R(self + 0x28), a = (uint32_t)(uintptr_t)txt;
    if (len > cap && cap) txt[cap - 1] = 0;   // cap 0 writes one byte below the local in retail (a saved register's top byte); the descriptor keeps cap >= 1
    e.H(0xceac0, cap, 0, 0, 0, 0, 0, buf, a);
    if (e.blk) {   // the capture logs exactly strlen+1 bytes of the local, zero-padded to the word: bytes past the NUL are not part of the record
        uint32_t k = 0; while (txt[k]) k++;
        uint8_t pad[20] = {}; for (uint32_t i = 0; i <= k; ++i) pad[i] = txt[i];
        e.blk(e.ctx, 0xceac0, (uint32_t)(uintptr_t)pad, (k + 1 + 3) / 4);
    }
    const uint32_t n = e.H(0xceab0, 0, 0, 0, 0, 0, 0, buf);
    e.W(self + 0x36, n);
    e.W(self + 0x3a, n);
    return n;
}
