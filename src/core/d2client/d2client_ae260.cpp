// 1.13c D2Client+0xAE260 (ECX = 11-byte id block, no result): key = bytes 3..6 of the block, kind = byte 1; looks the key
// (masked 0x7F) up in row kind (module RVA 0x10A608 + kind * 0x200) through 0xA4E20 and hands a hit to 0x82FE0.
// 0xAE260 and 0xAE2B0 have the same body.
#include <emscripten.h>
#include <cstdint>
extern "C" void d2c_port_ae260(const uint8_t* blk, uint32_t (*f4e20)(uint32_t key7, uint32_t rowRva, uint32_t kind), void (*f2fe0)(uint32_t hit)) {
    uint32_t kind = blk[1];
    uint32_t key = (uint32_t)blk[3] | (uint32_t)blk[4] << 8 | (uint32_t)blk[5] << 16 | (uint32_t)blk[6] << 24;
    uint32_t hit = f4e20(key & 0x7F, 0x10A608u + kind * 0x200u, kind);
    if (hit) f2fe0(hit);
}
