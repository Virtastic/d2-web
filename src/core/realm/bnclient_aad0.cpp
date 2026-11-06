#include <cstddef>
#include <cstdint>
#include <cstring>

// 1.13c BNClient+0x0AAD0 (265 B, callers 0xACD0, 0xAE10, 0xAED0): stdcall (ctx, out, n), ret 0xC. ctx is a
// pointer to the state-block pointer (retail dword [ctx]). Drain `n` bytes from the hash-stream pool: S = *ctx,
// remaining count at S+0x48, the pool is the 20 bytes ending at S+0x48 (read from the tail). When the pool runs
// dry: copy what is left, then on a stack SHA-1 context (IV, zero counts) update with S+0xc, S+0x34, S+0x20 (20 B
// each, 0x9D00: EBX = context, stack data/len) and finalise (0x9DC0: EAX = context, EDI = S+0x34) to regenerate
// the pool in place, and reset the count to 20.
typedef void (*BnShaUpdate)(void* ctx, uint8_t* data, uint32_t len);   // 0x9D00
typedef void (*BnShaFinal)(void* ctx, uint8_t* digest);                // 0x9DC0
// Returns the remaining count at the last loop test (retail's stale EAX after a refill: [esp+0x78] reloaded at 0x6FF2AB97).
extern "C" uint32_t d2_bnclient_stream_read_n(const uint32_t* ctx, uint8_t* out, uint32_t n, BnShaUpdate update,
                                              BnShaFinal final_) {
    if (!n) return 0;
    uint8_t* s = (uint8_t*)(uintptr_t)*ctx;
    uint32_t left = *(uint32_t*)(s + 0x48);
    while (left < n) {
        memcpy(out, s + 0x48 - left, left);
        out += left;
        n -= left;
        uint32_t c[23] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0, 0, 0};   // SHA-1 context, 92 bytes
        update(c, s + 0xc, 0x14);
        update(c, s + 0x34, 0x14);
        update(c, s + 0x20, 0x14);
        final_(c, s + 0x34);
        *(uint32_t*)(s + 0x48) = 0x14;
        left = 0x14;
    }
    if (n) {
        memcpy(out, s + 0x48 - left, n);
        *(uint32_t*)(s + 0x48) -= n;
    }
    return n;
}
extern "C" void d2_bnclient_stream_read(const uint32_t* ctx, uint8_t* out, uint32_t n, BnShaUpdate update, BnShaFinal final_) {
    d2_bnclient_stream_read_n(ctx, out, n, update, final_);
}
