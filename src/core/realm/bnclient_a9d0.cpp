#include <cstddef>
#include <cstdint>

// 1.13c BNClient+0x0A9D0 (250 B, callers 0xAED0, 0xB3A0; EAX=a, ESI=c, stack: salt32*, BIGNUM** out, callee-cleaned ret 8):
// h1 = SHA1(a || G || c) with G the global string at DLL+0x3e300, each piece fed with its Ordinal_506 (strlen) length;
// h2 = SHA1(salt[0x20] || h1[0x14]); *out = bignum_from_bytes(new bignum, h2, 0x14) (Ordinals 624, 609).
// This is the SRP "x" derivation. Hash and bignum primitives are injected; the SHA1 here is BNCS's broken variant.
struct d2_sha1ctx;
void d2_bnclient_srp_x(const char* a, const char* g, const char* c, const uint8_t* salt, void** out,
                       void (*init)(d2_sha1ctx*), void (*update)(d2_sha1ctx*, const void*, uint32_t),
                       void (*final_)(d2_sha1ctx*, uint8_t*), void* (*bn_new)(),
                       void (*bn_from_bytes)(void*, const uint8_t*, uint32_t), d2_sha1ctx* ctx) {
    auto len = [](const char* s) { uint32_t n = 0; while (s[n]) ++n; return n; };
    uint8_t h[20];
    init(ctx);
    update(ctx, a, len(a));
    update(ctx, g, len(g));
    update(ctx, c, len(c));
    final_(ctx, h);
    init(ctx);
    update(ctx, salt, 0x20);
    update(ctx, h, 0x14);
    final_(ctx, h);
    void* bn = bn_new();
    bn_from_bytes(bn, h, 0x14);
    *out = bn;
}
