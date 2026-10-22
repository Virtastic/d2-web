// D2CMP sprite-cache entry builder; callees injected (allocator, fatal exit).
#include <cstdint>

// 1.13c D2CMP+0x0DDC0
bool d2cmp_ddc0(uint8_t* spr, uint8_t* flag, int p3, uint32_t p4,
                uint32_t** (*alloc)(int, int, int), void (*fill)(uint32_t, uint8_t*, uint8_t*, int),
                uint32_t* (*newNode)(), void (*d1cfc0)(uint8_t*, uint8_t*), int (*d310)(), void (*fatal)())
{
    if (!flag) return false;
    uint32_t** h = alloc(0xb8, 0, 0);
    if (!h) return false;
    uint32_t* e = *h;
    for (int i = 0; i < 0x2e; i++) e[i] = 0;
    auto s = [&](int o) { return *(uint32_t*)(spr + o); };
    e[0] = s(8); e[3] = s(0x18); e[6] = s(0x24); e[4] = s(0x1c); e[5] = s(0x20); e[7] = s(0x28);
    e[0x2d] = (uint32_t)(uintptr_t)h;
    if (!(*flag & 1)) { e[2] = s(0xc); e[1] = s(0x10); fill(p4, flag, spr, p3); }
    uint32_t* n = newNode();
    if (!n) fatal();
    ((uint32_t*)h)[2] = (uint32_t)(uintptr_t)n;
    n[9] = (uint32_t)(uintptr_t)h;
    if (*flag & 1) { *(uint8_t*)(e + 8) |= 1; return false; }
    if (e[10] == 0) d1cfc0(flag, spr);
    int r = d310();
    *(int*)(spr + 0x3c) = r;
    return r != 0;
}
