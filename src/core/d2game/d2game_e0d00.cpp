// 1.13c D2Game+0xE0D00, from the disassembly (jump table at D2Game+0xE0F20, six cases on unit+0): EDI = EAX in, ESI = ECX in,
// EBX = EDX (unit), EBP = p1. Prologue: unit+0x18 = byte of ord10459(ord10826(esi)), unit+0x1C = p1[0xBC + 4 * that byte],
// unit+0xC8 loses bit 8. p1 == 0 is fatal 0x45D, unit == 0 fatal 0x45E, unit+0 > 5 fatal 0x4AB. Case tails: A = ord10692 + clear
// unit+0x2C + ord11003 (the six-argument release); B = ord10775(unit,1), ord10671(unit,5), ord11056(unit+0x2C,0x8000).
// Epilogue: ord10331(unit) nonzero calls ord10158(unit); ord10067(unit,1) is the last call and its EAX is returned.
#include "d2game_e0d00.h"
#include <cstring>
namespace {
uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
void wr(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
}
uint32_t d2game_e0d00_port(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t p1, uint32_t p2, const D2GameE0D00Env* e) {
    const uint32_t edi = eax, esi = ecx, ebx = edx, ebp = p1;
    auto C = [&](uint32_t rva, uint32_t a, uint32_t s, uint32_t d, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0,
                 uint32_t s4 = 0, uint32_t s5 = 0) { return e->call(rva, a, s, d, s0, s1, s2, s3, s4, s5); };
    if (!ebp) e->fatal(0x45d);
    if (!ebx) e->fatal(0x45e);
    const uint32_t h = C(0xa556, 0, 0, 0, esi);
    const uint8_t al = (uint8_t)C(0xa56e, 0, 0, 0, h);
    const uint32_t flags = rd(ebx, 0xc8);
    *(uint8_t*)(uintptr_t)(ebx + 0x18) = al;
    const uint32_t type = rd(ebx, 0);
    wr(ebx, 0x1c, rd(ebp, 0xbc + al * 4u));
    wr(ebx, 0xc8, flags & 0xfffffeffu);
    if (type > 5) e->fatal(0x4ab);
    auto release = [&] {
        C(0xa58c, 0, 0, 0, rd(ebp, 0x1c), rd(ebx, 0x2c));
        wr(ebx, 0x2c, 0);
        C(0xa66a, 0, 0, 0, rd(ebp, 0x1c), esi, p2, edi, ebx, 0);
    };
    auto finish = [&] {   // 0x6FD00E11
        C(0xa6ac, 0, 0, 0, ebx, 1);
        C(0xa6d6, 0, 0, 0, ebx, 5);
        C(0xa682, 0, 0, 0, rd(ebx, 0x2c), 0x8000);
    };
    auto four = [&](uint32_t rva) { C(rva, 0, 0, 0, ebx, esi, p2, edi); };
    switch (type) {
    case 0:
        release();
        C(0x37e20, 0, ebp, ebx, p2, edi);
        { const uint32_t v = rd(ebx, 0x10); if (v == 0x11 || v == 0) finish(); }
        break;
    case 1:
        release();
        C(0xafab0, ebx, 0, 0, ebp, p2, edi);
        if (C(0xdf9a0, ebx, 0, 0)) C(0xaf920, ebp, 0, 0);
        else C(0xe0860, ebx, 0, 0, ebp);
        if (C(0xa688, 0, 0, 0, ebx)) finish();
        break;
    case 2: {
        const uint32_t idx = C(0xa520, 0, 0, 0, rd(ebx, 4));
        four(0xa5f2);
        if (*(const uint8_t*)(uintptr_t)(rd(ebx, 0x10) + idx + 0x120)) four(0xa69a);
        C(0xa6fa, 0, 0, 0, ebx, esi);
        break;
    }
    case 3:
        release();
        break;
    case 4:
        if (rd(ebx, 0x10) == 3) {
            four(0xa5f2);
            four(0xa69a);
            C(0xa6fa, 0, 0, 0, ebx, esi);
            const uint32_t v = C(0xe0a0, 0, ebx, ebp);
            wr(rd(ebx, 0x14), 0x24, v);
        }
        break;
    case 5:
        four(0xa5f2);
        four(0xa69a);
        C(0xa6fa, 0, 0, 0, ebx, esi);
        break;
    }
    if (C(0xa478, 0, 0, 0, ebx)) C(0xa4de, 0, 0, 0, ebx);
    return C(0xa6e8, 0, 0, 0, ebx, 1);
}
