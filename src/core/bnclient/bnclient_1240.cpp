// 1.13c BNClient+0x01240 (90 B, caller 0x103B0): SHA-1 of a buffer in one call, stdcall(digest, data, length).
// Retail builds a local context with the standard initial state, runs the update step 0x14930 (EAX = data,
// EDI = context, length on the stack) and the final step 0x149E0, and the 20-byte digest is written to the
// first argument. The data is only read.
#include <cstdint>
#include <cstring>

namespace {
uint32_t rol1240(uint32_t x, unsigned n) { return (x << n) | (x >> (32 - n)); }

void block1240(uint32_t* h, const uint8_t* p) {
    uint32_t w[80];
    for (unsigned i = 0; i < 16; ++i)
        w[i] = uint32_t(p[4 * i]) << 24 | uint32_t(p[4 * i + 1]) << 16 | uint32_t(p[4 * i + 2]) << 8 | p[4 * i + 3];
    for (unsigned i = 16; i < 80; ++i) w[i] = rol1240(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
    for (unsigned i = 0; i < 80; ++i) {
        uint32_t f, k;
        if (i < 20) { f = (b & c) | (~b & d); k = 0x5a827999; }
        else if (i < 40) { f = b ^ c ^ d; k = 0x6ed9eba1; }
        else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8f1bbcdc; }
        else { f = b ^ c ^ d; k = 0xca62c1d6; }
        const uint32_t t = rol1240(a, 5) + f + e + k + w[i];
        e = d; d = c; c = rol1240(b, 30); b = a; a = t;
    }
    h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e;
}
}  // namespace

extern "C" void bn_sha1_message_1240(uint8_t* digest, const uint8_t* data, uint32_t length) {
    uint32_t h[5] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};
    uint32_t done = 0;
    for (; length - done >= 64; done += 64) block1240(h, data + done);
    uint8_t tail[128] = {};
    const uint32_t rest = length - done;
    if (rest) std::memcpy(tail, data + done, rest);
    tail[rest] = 0x80;
    const uint32_t total = rest < 56 ? 64 : 128;
    const uint64_t bits = uint64_t(length) * 8;
    for (unsigned i = 0; i < 8; ++i) tail[total - 1 - i] = uint8_t(bits >> (8 * i));
    for (uint32_t o = 0; o < total; o += 64) block1240(h, tail + o);
    for (unsigned i = 0; i < 20; ++i) digest[i] = uint8_t(h[i >> 2] >> (24 - 8 * (i & 3)));
}
