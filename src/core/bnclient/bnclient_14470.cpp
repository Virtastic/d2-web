// 1.13c BNClient+0x14470: one SHA-1 compression. The retail routine is the fully unrolled
// FIPS 180-1 round function (ch 0x5A827999, parity 0x6ED9EBA1 and 0xCA62C1D6 rounds, majority 0x8F1BBCDC): the 80-word
// schedule is W[t] = rol1(W[t-3]^W[t-8]^W[t-14]^W[t-16]); the state is h[0..4] and the result is added back in place.
#include <cstdint>

namespace {
inline uint32_t rol(uint32_t x, unsigned n) { return (x << n) | (x >> (32 - n)); }
}  // namespace

// 1.13c BNClient+0x14470: h = five state words (stack word 0), p = the 64-byte block (EAX).
extern "C" void bn_sha1_block_14470(uint32_t* h, const uint8_t* p) {
    uint32_t w[80];
    for (int i = 0; i < 16; ++i)
        w[i] = (uint32_t)p[4 * i] << 24 | (uint32_t)p[4 * i + 1] << 16 | (uint32_t)p[4 * i + 2] << 8 | p[4 * i + 3];
    for (int i = 16; i < 80; ++i) w[i] = rol(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
    for (int i = 0; i < 80; ++i) {
        uint32_t f, k;
        if (i < 20)      { f = (b & c) | (~b & d);             k = 0x5A827999u; }
        else if (i < 40) { f = b ^ c ^ d;                      k = 0x6ED9EBA1u; }
        else if (i < 60) { f = (b & c) | (b & d) | (c & d);    k = 0x8F1BBCDCu; }
        else             { f = b ^ c ^ d;                      k = 0xCA62C1D6u; }
        uint32_t t = rol(a, 5) + f + e + k + w[i];
        e = d; d = c; c = rol(b, 30); b = a; a = t;
    }
    h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e;
}
