// 1.13c BNClient+0x0AED0 (137 B): client public value, retail register layout: EAX = 32-byte secret buffer, ECX = pointer to the
// state-block pointer (S = *ctx: S+4 = g, S+8 = N, both big-number handles), EBX = 32-byte destination, stack a / c (strings),
// ret 8. Every callee is an injected seam carrying its retail registers: 0xAAD0, 0xA9D0 (EAX = a, ESI = c), Storm #624 / #612 /
// #628 / #638 / #494 / #606 through the thunks 0x8740, 0x872E, 0x86FE, 0x8716, 0x806C, 0x870A.
#include <cstdint>
struct BnAed0Env {
    void (*stream_read)(const uint32_t* ctx, uint8_t* out, uint32_t n);                 // 0xAAD0
    void (*srp_x)(const char* a, const char* c, uint8_t* salt, uint32_t* out);          // 0xA9D0
    void (*big_new)(uint32_t* slot);                                                    // 0x8740
    void (*big_set)(uint32_t h, uint32_t v);                                            // 0x872E
    void (*big_powmod)(uint32_t r, uint32_t g, uint32_t x, uint32_t n);                 // 0x86FE
    void (*big_bin)(uint32_t h, uint8_t* buf, uint32_t n, uint32_t* len);               // 0x8716
    void (*memzero)(uint8_t* p, uint32_t n);                                            // 0x806C
    void (*big_del)(uint32_t h);                                                        // 0x870A
};
extern "C" void d2_bnclient_aed0(uint8_t* secret, const uint32_t* ctx, uint8_t* dest, const char* a, const char* c,
                                 const BnAed0Env* e) {
    e->stream_read(ctx, secret, 0x20);
    uint32_t x;
    e->srp_x(a, c, secret, &x);
    uint32_t r;
    e->big_new(&r);
    e->big_set(r, 0);
    const uint32_t* s = (const uint32_t*)(uintptr_t)*ctx;
    e->big_powmod(r, s[1], x, s[2]);
    uint32_t len;
    e->big_bin(r, dest, 0x20, &len);
    if (len < 0x20) e->memzero(dest + len, 0x20 - len);
    e->big_del(x);
    e->big_del(r);
}
