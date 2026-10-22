#include <cstddef>
#include <cstdint>
#include <cstring>

// 1.13c BNClient+0x0AAD0 (265 B, callers 0xACD0, 0xAE10, 0xAED0): drain `n` bytes from the hash-stream
// pool. State block S = *ctx; remaining count at S+0x48, the pool is the 20 bytes ending at S+0x48 (read from
// the tail). When the pool runs dry: copy what is left, feed S+0xc, S+0x34, S+0x20 (20 B each) to the SHA-1
// update (0x9D00), finalise (0x9DC0) to regenerate the pool, and reset the count to 20.
void d2_bnclient_stream_read(uint8_t** ctx, uint8_t* out, uint32_t n, void (*update)(uint8_t*, uint32_t),
                             void (*final_)(uint8_t*)) {
    if (!n) return;
    uint8_t* s = *ctx;
    uint32_t left = *(uint32_t*)(s + 0x48);
    while (left < n) {
        memcpy(out, s + 0x48 - left, left);
        out += left;
        n -= left;
        update(s + 0xc, 0x14);
        update(s + 0x34, 0x14);
        update(s + 0x20, 0x14);
        final_(s);
        *(uint32_t*)(s + 0x48) = 0x14;
        left = 0x14;
    }
    if (n) {
        memcpy(out, s + 0x48 - left, n);
        *(uint32_t*)(s + 0x48) = left - n;
    }
}
