// Prior art used: network row; bncsutil sha1.c for the
// standard SHA-1 round schedule. Retail 1.13c decompile and x86 call sites
// settle the ABI, input mutation, counters and padding; no prior-art code copied.
#include <cstddef>
#include <cstdint>
#include <cstring>

struct BnclientSha1Context {
    uint32_t state[5];
    uint32_t bitLow;
    uint32_t bitHigh;
    uint8_t block[64];
};
static_assert(offsetof(BnclientSha1Context, bitLow) == 0x14);
static_assert(offsetof(BnclientSha1Context, bitHigh) == 0x18);
static_assert(offsetof(BnclientSha1Context, block) == 0x1c);
static_assert(sizeof(BnclientSha1Context) == 0x5c);

namespace {
uint32_t rol(uint32_t x, unsigned n) { return (x << n) | (x >> (32 - n)); }
uint32_t bigWord(const uint8_t* p) {
    return uint32_t(p[0]) << 24 | uint32_t(p[1]) << 16 | uint32_t(p[2]) << 8 | p[3];
}
void littleWord(uint8_t* p, uint32_t v) {
    for (unsigned i = 0; i < 4; ++i) p[i] = uint8_t(v >> (8 * i));
}
}

// Hidden retail EAX is a writable 64-byte input block; ECX is the state.
// The final 16 expanded words are written back to the input, little-endian.
// 1.13c BNClient+0x08780
extern "C" void d2_bnclient_sha1_transform(uint32_t* state, uint8_t* input) {
    uint32_t words[80];
    for (unsigned i = 0; i < 16; ++i) words[i] = bigWord(input + 4 * i);
    for (unsigned i = 16; i < 80; ++i) {
        words[i] = rol(words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16], 1);
        littleWord(input + 4 * (i & 15), words[i]);
    }
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];
    for (unsigned i = 0; i < 80; ++i) {
        uint32_t f, k;
        if (i < 20) { f = (b & c) | (~b & d); k = 0x5a827999; }
        else if (i < 40) { f = b ^ c ^ d; k = 0x6ed9eba1; }
        else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8f1bbcdc; }
        else { f = b ^ c ^ d; k = 0xca62c1d6; }
        const uint32_t next = rol(a, 5) + f + e + k + words[i];
        e = d; d = c; c = rol(b, 30); b = a; a = next;
    }
    state[0] += a; state[1] += b; state[2] += c; state[3] += d; state[4] += e;
}

// EBX carries context in retail. Complete blocks after the first buffered block
// are transformed in the caller's storage, including schedule writes.
// 1.13c BNClient+0x09D00
extern "C" void d2_bnclient_sha1_update(BnclientSha1Context* ctx, uint8_t* input, uint32_t length) {
    uint32_t offset = (ctx->bitLow >> 3) & 63;
    ctx->bitLow += length << 3;
    if (ctx->bitLow < (length << 3)) ++ctx->bitHigh;
    ctx->bitHigh += length >> 29;
    uint32_t consumed = 0;
    if (offset + length >= 64) {
        consumed = 64 - offset;
        std::memcpy(ctx->block + offset, input, consumed);
        d2_bnclient_sha1_transform(ctx->state, ctx->block);
        while (length - consumed >= 64) {
            d2_bnclient_sha1_transform(ctx->state, input + consumed);
            consumed += 64;
        }
        offset = 0;
    }
    if (length != consumed) std::memcpy(ctx->block + offset, input + consumed, length - consumed);
}

// EAX carries context and EDI the output in retail. Finalization retains the
// padded bit count and mutated workspace rather than resetting the context.
// 1.13c BNClient+0x09DC0
extern "C" void d2_bnclient_sha1_final(BnclientSha1Context* ctx, uint8_t* digest) {
    uint8_t count[8];
    for (unsigned i = 0; i < 4; ++i) {
        count[i] = uint8_t(ctx->bitHigh >> (24 - 8 * i));
        count[i + 4] = uint8_t(ctx->bitLow >> (24 - 8 * i));
    }
    uint8_t padding = 0x80;
    d2_bnclient_sha1_update(ctx, &padding, 1);
    padding = 0;
    while ((ctx->bitLow & 0x1f8) != 0x1c0) d2_bnclient_sha1_update(ctx, &padding, 1);
    d2_bnclient_sha1_update(ctx, count, 8);
    for (unsigned i = 0; i < 20; ++i) digest[i] = uint8_t(ctx->state[i >> 2] >> (24 - 8 * (i & 3)));
}
