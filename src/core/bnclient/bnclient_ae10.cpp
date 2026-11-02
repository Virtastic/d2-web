// 1.13c BNClient+0x0AE10 -- SRP client public value. ECX = state s (0x80 bytes), EDX = pointer to the group pointer {ready, g +4, N +8},
// EAX = account string, stack = password string (ret 4). s[0..0x20) := account, s[0x20..0x40) := password (Storm #501, max 0x7FFFFFFF);
// the hash stream (0xAAD0, ctx = EDX) fills a := s[0x40..0x60); A = g^a mod N (#624 new, #609 from binary, #612 zero, #628 powmod, #606 del);
// #638 writes A into s[0x60..0x80) with its length and a shorter result is zeroed to 0x20 (#494 / 0x806C).
#include <cstdint>
#include <cstring>
struct BnAe10Host {
    void (*stream_read)(const void* ctx, uint8_t* out, uint32_t n);          // 0xAAD0
    void (*big_new)(uint32_t* slot);                                          // 0x8740 #624
    void (*big_from_binary)(uint32_t h, const uint8_t* d, uint32_t n);        // 0x8728 #609
    void (*big_from_unsigned)(uint32_t h, uint32_t v);                        // 0x872E #612
    void (*big_powmod)(uint32_t r, uint32_t g, uint32_t a, uint32_t n);       // 0x86FE #628
    void (*big_del)(uint32_t h);                                              // 0x870A #606
    void (*big_to_binary)(uint32_t h, uint8_t* buf, uint32_t cap, uint32_t* len); // 0x8716 #638
};
extern "C" void bn_srp_pub_ae10(uint8_t* s, const void* ctx, const uint32_t* grp, const char* a, const char* b, const BnAe10Host* h) {
    std::strcpy((char*)s, a);
    std::strcpy((char*)s + 0x20, b);
    h->stream_read(ctx, s + 0x40, 0x20);
    uint32_t av = 0, r = 0;
    h->big_new(&av);
    h->big_from_binary(av, s + 0x40, 0x20);
    h->big_new(&r);
    h->big_from_unsigned(r, 0);
    h->big_powmod(r, grp[1], av, grp[2]);
    h->big_del(av);
    uint32_t len = 0;
    h->big_to_binary(r, s + 0x60, 0x20, &len);
    if (len < 0x20) std::memset(s + 0x60 + len, 0, 0x20 - len);
    h->big_del(r);
}
