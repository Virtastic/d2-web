// 1.13c BNClient+0x149E0: SHA-1 final on the 0x5C context. EAX = context, stack word 0 = 20-byte digest, big-endian.
// The pad is ((-(count>>3) - 9) & 0x3f) + 1 bytes (0x80 then zeros) through the update step 0x14930, then the 8-byte
// big-endian bit count taken before padding. The decompile omits the EAX loads of the pad and length sources; this
// port uses the standard SHA-1 sources, and the vector decides.
#include <cstdint>

extern "C" void bnclient_14930(uint32_t* ctx, const uint8_t* data, uint32_t length);

extern "C" void bnclient_149e0(uint32_t* ctx, uint8_t* digest) {
    const uint64_t bits = uint64_t(ctx[0]) | uint64_t(ctx[1]) << 32;
    uint8_t pad[64] = {0x80};
    const uint32_t n = ((0u - (ctx[0] >> 3) - 9u) & 0x3f) + 1;
    bnclient_14930(ctx, pad, n);
    uint8_t len[8];
    for (int i = 0; i < 8; ++i) len[i] = uint8_t(bits >> (56 - 8 * i));
    bnclient_14930(ctx, len, 8);
    for (int i = 0; i < 5; ++i) {
        const uint32_t w = ctx[2 + i];
        digest[4 * i] = uint8_t(w >> 24);
        digest[4 * i + 1] = uint8_t(w >> 16);
        digest[4 * i + 2] = uint8_t(w >> 8);
        digest[4 * i + 3] = uint8_t(w);
    }
}
