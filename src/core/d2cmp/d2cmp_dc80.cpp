// D2CMP sprite-cache entry loader; callees injected (gate flag, allocator, locate, release, node alloc, fatal).
#include <cstdint>

// 1.13c D2CMP+0x0DC80
uint32_t d2cmp_dc80(const uint8_t* spr, uint8_t* flag, int p3, bool enabled,
                    uint32_t** (*alloc)(int, int, int), int (*d7f0)(uint8_t*, int), void (*d250)(int),
                    void (*cb60)(uint32_t**), uint32_t* (*newNode)(), void (*fatal)())
{
    if (!enabled) return 0;
    uint32_t** h = alloc(0xb8, 0, 1);
    if (!h) return 0;
    uint32_t* e = *h;
    for (int i = 0; i < 0x2e; i++) e[i] = 0;
    auto s = [&](int o) { return *(const uint32_t*)(spr + o); };
    e[3] = s(0x18); e[6] = s(0x24); e[4] = s(0x1c); e[5] = s(0x20); e[7] = s(0x28);
    e[0x2d] = (uint32_t)(uintptr_t)h;
    if (!(*flag & 1)) {
        e[2] = s(0xc); e[1] = s(0x10);
        int r = d7f0(flag, p3);
        if (r == -1) { cb60(h); return 0; }
        d250(r);
    }
    uint32_t* n = newNode();
    if (!n) fatal();
    ((uint32_t*)h)[2] = (uint32_t)(uintptr_t)n;
    n[9] = (uint32_t)(uintptr_t)h;
    if (*flag & 1) { *(uint8_t*)(e + 8) |= 1; return 0; }
    *(uint8_t*)(e + 8) |= 2;
    return 1;
}
