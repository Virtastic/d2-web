#include <cstdint>

// 1.13c BNClient+0xb3a0 (394 B, callers 0x11F80, 0x12000; stdcall-ish, param_1 = ctx (EBP), param_2 = BIGNUM* in):
// SRP step: t = bn(in); BN op612(t, in); if op603(...) != 1 -> free temps, return 0.
// Else x = srp_x(ctx.salt, &out) [FUN_A9D0], derive 0x20-byte key k = op609(x) with sub_B0C0(*x) as selector;
// if selector null -> return 0; else op638 serialises into a 0x20 buffer, zero-pads the tail from len (op494),
// copies the 0x20 bytes to ctx+0x80 via FUN_9E90 (SHA1 update), frees, FUN_A680(*in) and returns 1.
// Bignum ordinals (624 new, 609 mod/exp, 612 op, 603 cmp, 606 free, 638 to-bytes, 494 memset) are injected.
struct d2_b3a0_ops {
    void* (*bn_new)();
    int (*step)(void* t, void* in);       // 612 + 609 + 603 chain; returns 1 on success
    void (*free_)(void*);                 // 606
    void* (*derive)(void* in, uint8_t* out32, uint32_t* len); // A9D0 + B0C0 + 638; null on failure
    void (*absorb)(void* ctx, const uint8_t* k32);            // 9E90 at ctx+0x80
    void (*finish)(void* in);             // A680
};
int d2_bnclient_b3a0(void* ctx, void* in, const d2_b3a0_ops& o) {
    void* t = o.bn_new();
    int ok = o.step(t, in);
    o.free_(t);
    if (ok != 1) return 0;
    uint8_t k[0x20];
    uint32_t len = 0;
    void* r = o.derive(in, k, &len);
    if (!r) { o.free_(ctx); return 0; }
    for (uint32_t i = len; i < 0x20; ++i) k[i] = 0;
    o.absorb(ctx, k);
    o.free_(ctx);
    o.finish(in);
    return 1;
}
