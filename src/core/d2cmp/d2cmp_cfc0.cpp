// D2CMP SpriteCache: bind per-frame 0x2c-byte records to a DC6-style block (EAX = sprite in retail).
#include <cstdint>
#include <cstddef>

// 1.13c D2CMP+0x0CFC0
// fatal(id): retail Ordinal_10265(id) + Ordinal_10024 + _exit(-1); never returns.
void d2cmp_cfc0(uint8_t* flags, int arg, uint8_t* spr, uint8_t* (*alloc)(uint32_t), uint8_t* (*dataBase)(),
                void (*fatal)(uint32_t))
{
    if (!arg) fatal(0x519);
    if (!spr) fatal(0x51a);
    if (*flags & 1) fatal(0x51b);
    if (*(uint32_t*)(spr + 0x28)) fatal(0x51e);
    uint32_t n = (uint32_t)spr[0x24] * spr[0x21];
    uint32_t* rec = (uint32_t*)alloc(n * 0x2c);  // SpriteCache.cpp:0x523, zeroed
    for (uint32_t i = 0; i < n * 11; i++) rec[i] = 0;
    uint8_t* base = dataBase();                   // Ordinal_10094
    if (!base) fatal(0x530);
    uint8_t* p = (uint8_t*)(uintptr_t)*(uint32_t*)(spr + 0x2c + spr[0x22] * 4);
    for (uint32_t i = 0; i < n; i++) {
        uint8_t* f = p + (uintptr_t)base;
        if (*(uint32_t*)f != 0 || *(uint32_t*)(f + 4) > 0x100 || *(uint32_t*)(f + 8) > 0x100) fatal(0x5ae);
        *(uint32_t**)(f + 0x18) = rec;
        p += 0x23 + *(uint32_t*)(f + 0x1c) + *(uint32_t*)(f + 0x14);
        rec += 11;
    }
    *(uint32_t*)(spr + 0x28) = 1;
}
