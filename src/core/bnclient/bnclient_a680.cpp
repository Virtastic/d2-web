// 1.13c BNClient+0x0A680 -- SRP M1 digest (stdcall, ret 0x1c): grp (arg0, {ready, g +4, n +8} 32-bit handles), name (arg1),
// a2, a3, a4 (0x20 bytes each), a5 (0x28 bytes), out (arg6, 20 bytes).
// A = SHA1(first len bytes of SBigToBinaryBuffer(n)), B = the same for g (each result zero-filled to 0x20 but only `len` hashed),
// x = A ^ B (20 bytes); C = SHA1(name[0..strlen]); out = SHA1(x || C || a2 || a3 || a4 || a5). Standard initial state each time.
#include <cstdint>
#include <cstring>

struct BnA680Host {
    void (*big_to_binary)(uint32_t h, void* buf, uint32_t cap, uint32_t* len);   // 0x8716 #638
};
extern "C" void d2_bnclient_sha1_update(void* ctx, uint8_t* data, uint32_t length);
extern "C" void d2_bnclient_sha1_final(void* ctx, uint8_t* digest);

namespace {
struct Ctx { uint32_t st[5], lo, hi; uint8_t blk[64]; };
void init(Ctx& c) { c = Ctx{ { 0x67452301u, 0xefcdab89u, 0x98badcfeu, 0x10325476u, 0xc3d2e1f0u }, 0, 0, {} }; }
void big_digest(const BnA680Host* h, uint32_t handle, uint8_t* d) {
    uint8_t buf[0x20]; uint32_t len = 0;
    h->big_to_binary(handle, buf, 0x20, &len);
    if (len < 0x20) std::memset(buf + len, 0, 0x20 - len);
    Ctx c; init(c);
    d2_bnclient_sha1_update(&c, buf, len); d2_bnclient_sha1_final(&c, d);
}
}

extern "C" void bn_srp_m1_a680(const uint32_t* grp, const char* name, uint8_t* a2, uint8_t* a3, uint8_t* a4, uint8_t* a5,
                               uint8_t* out, const BnA680Host* h) {
    uint8_t a[20], b[20], x[20], cdig[20];
    big_digest(h, grp[2], a);
    big_digest(h, grp[1], b);
    for (int i = 0; i < 20; ++i) x[i] = a[i] ^ b[i];
    Ctx c; init(c);
    d2_bnclient_sha1_update(&c, (uint8_t*)name, (uint32_t)std::strlen(name)); d2_bnclient_sha1_final(&c, cdig);
    init(c);
    d2_bnclient_sha1_update(&c, x, 0x14);
    d2_bnclient_sha1_update(&c, cdig, 0x14);
    d2_bnclient_sha1_update(&c, a2, 0x20);
    d2_bnclient_sha1_update(&c, a3, 0x20);
    d2_bnclient_sha1_update(&c, a4, 0x20);
    d2_bnclient_sha1_update(&c, a5, 0x28);
    d2_bnclient_sha1_final(&c, out);
}
