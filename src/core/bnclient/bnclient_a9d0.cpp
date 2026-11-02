// 1.13c BNClient+0x0A9D0 -- SRP x. EAX = a (string), ESI = c (string), stack: salt (32 bytes), out (handle*), ret 8; G = the string at *(DLL+0x3E300).
// h1 = SHA1(a || G || c) (each piece with its Storm #506 length, SHA-1 update 0x9D00, final 0x9DC0); h2 = SHA1(salt[0x20] || h1[0x14]);
// Storm #624 creates a big number through a pointer (0x8740), #609 loads it from h2 (0x8728: handle, data, 0x14); *out := the handle.
#include <cstdint>
#include <cstring>
struct BnclientSha1Context { uint32_t state[5]; uint32_t bitLow, bitHigh; uint8_t block[64]; };
extern "C" void d2_bnclient_sha1_update(BnclientSha1Context*, uint8_t*, uint32_t);
extern "C" void d2_bnclient_sha1_final(BnclientSha1Context*, uint8_t*);
struct BnA9d0Host {
    void (*big_new)(uint32_t* slot);                                   // 0x8740 #624
    void (*big_from_binary)(uint32_t h, const uint8_t* data, uint32_t n); // 0x8728 #609
};
static void bn_sha1_init(BnclientSha1Context* c) {
    c->state[0] = 0x67452301; c->state[1] = 0xefcdab89; c->state[2] = 0x98badcfe; c->state[3] = 0x10325476; c->state[4] = 0xc3d2e1f0;
    c->bitLow = 0; c->bitHigh = 0;
}
extern "C" void bnclient_a9d0(char* a, char* c, char* g, uint8_t* salt, uint32_t* out, const BnA9d0Host* h) {
    BnclientSha1Context ctx; std::memset(ctx.block, 0, sizeof ctx.block);
    uint8_t h1[0x14];
    bn_sha1_init(&ctx);
    d2_bnclient_sha1_update(&ctx, (uint8_t*)a, (uint32_t)std::strlen(a));
    d2_bnclient_sha1_update(&ctx, (uint8_t*)g, (uint32_t)std::strlen(g));
    d2_bnclient_sha1_update(&ctx, (uint8_t*)c, (uint32_t)std::strlen(c));
    d2_bnclient_sha1_final(&ctx, h1);
    bn_sha1_init(&ctx);
    d2_bnclient_sha1_update(&ctx, salt, 0x20);
    d2_bnclient_sha1_update(&ctx, h1, 0x14);
    d2_bnclient_sha1_final(&ctx, h1);
    uint32_t hd = 0;
    h->big_new(&hd);
    h->big_from_binary(hd, h1, 0x14);
    *out = hd;
}
