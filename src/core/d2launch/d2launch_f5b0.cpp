// 1.13c D2Launch+0xF5B0  sub_F5B0: builds the character-create record. EAX = out buffer, CL = class, stack: name, word (ret 8).
// Clears 0x14F bytes at 0x25570, magic 0xAA55AA55 / 0x60 / size 0x14F, the name through 9790 (dst 0x25584, max 0x7FFFFFFF), flags 0x25594 =
// (word & 0xFFFF) | 1 (| 0x20 for class 5 or 6), class byte 0x25598, 0x10/0x1E/1 at 0x25599..B, _time32(0) at 0x2559C and 0x255A0; the 16 bytes
// at 0x24D9C to 0x255F8 and 0x25608..0x25617 set to -1. The out buffer: 9844(out, 0xD), the first 11 bytes of 0x24D9C at +2, class + 1,
// eleven 0xFF, 1, 9844(flags), 9844(0), FF FF FF 00. g is the data range from 0x25570, d the 16 bytes at 0x24D9C.
#include <cstdint>
#include "d2launch_env.h"
extern "C" void d2l_f5b0(const D2lSeams* s, uint8_t* g, const uint8_t* d, uint32_t eax, uint32_t cl, uint32_t name, uint32_t word) {
    auto W = [&](uint32_t rva) -> uint32_t& { return *(uint32_t*)(g + (rva - 0x25570u)); };
    auto B = [&](uint32_t rva) -> uint8_t& { return g[rva - 0x25570u]; };
    const uint8_t c = (uint8_t)cl;
    for (uint32_t k = 0; k < 0x14F; ++k) g[k] = 0;
    W(0x25570) = 0xAA55AA55u; W(0x25574) = 0x60;
    s->t9790(0x6FA65584u, name, 0x7FFFFFFFu);
    W(0x25594) = (word & 0xFFFFu) | 1u;
    B(0x25598) = c; B(0x2559B) = 1;
    const uint32_t t = s->t1830(0);
    W(0x2559C) = t; W(0x255A0) = t; W(0x25578) = 0x14F; B(0x25599) = 0x10; B(0x2559A) = 0x1E;
    if (c == 6 || c == 5) W(0x25594) |= 0x20;
    s->t9844(eax, 0xD);
    const uint32_t* dw = (const uint32_t*)d;
    W(0x255F8) = dw[0]; W(0x255FC) = dw[1]; W(0x25600) = dw[2]; W(0x25604) = dw[3];
    uint8_t* p = (uint8_t*)(uintptr_t)eax + 2;
    for (int k = 0; k < 11; ++k) *p++ = d[k];
    *p++ = (uint8_t)(c + 1);
    W(0x25608) = W(0x2560C) = W(0x25610) = W(0x25614) = 0xFFFFFFFFu;
    for (int k = 0; k < 11; ++k) *p++ = 0xFF;
    *p++ = 1;
    s->t9844((uint32_t)(uintptr_t)p, W(0x25594));
    p += 2; s->t9844((uint32_t)(uintptr_t)p, 0);
    p += 2; p[0] = 0xFF; p[1] = 0xFF; p[2] = 0xFF; p[3] = 0;
}
