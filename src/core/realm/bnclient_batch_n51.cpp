// BNClient batch n51: the SRP bignum glue (rows 0x0A4E0 0x0A680 0x0A910 0x0AE10 0x0AED0 0x0B0C0), from the cached
// decompile of retail 1.13c Bnclient.dll. Retail reaches Storm's SBig* imports; here each is a function pointer of
// BnN51Host (the port's seam): 494 SMemZero, 506 SStrLen, 601 SBigAdd, 603 SBigCompare, 606 SBigDel,
// 609 SBigFromBinary, 612 SBigFromUnsigned, 624 SBigNew, 628 SBigPowMod, 636 SBigSub, 638 SBigToBinaryBuffer.
// The decompiler drops the stack operands of several Storm calls and aliases three SHA1 contexts in 0xA680; where
// an operand role is a reconstruction (not read off the listing) the function says so. SRP state *ctx is
// {int ready; bn g (+4); bn N (+8)} in retail (32-bit); a plain struct here.
#include <cstdint>
#include <cstring>

typedef void* BnN51Big;

struct BnN51Host {
    BnN51Big (*big_new)();                                                    // #624
    void (*big_del)(BnN51Big b);                                              // #606
    void (*big_from_binary)(BnN51Big b, const void* p, uint32_t n);           // #609
    void (*big_from_unsigned)(BnN51Big b, uint32_t v);                        // #612
    void (*big_to_binary)(BnN51Big b, void* buf, uint32_t cap, uint32_t* n);  // #638
    void (*big_powmod)(BnN51Big r, BnN51Big base, BnN51Big exp, BnN51Big mod);// #628
    void (*big_add)(BnN51Big r, BnN51Big a, BnN51Big b);                      // #601
    void (*big_sub)(BnN51Big r, BnN51Big a, BnN51Big b);                      // #636
    int (*big_cmp)(BnN51Big a, BnN51Big b);                                   // #603
    void (*sha1)(const void* p, uint32_t n, uint8_t out20[20]);               // 0x9D00 + 0x9DC0 (BNCS variant)
    void (*stream_read)(void* ctx, void* out, uint32_t n);                    // 0xAAD0
    BnN51Big (*srp_x)(void* ctx);                                             // 0xA9D0 (register-passed operands)
    uint32_t (*step_a180)(BnN51Big out, BnN51Big a, BnN51Big b);              // 0xA180
};

struct BnN51Group { int32_t ready; BnN51Big g; BnN51Big n; };

static void bn_n51_bin32(const BnN51Host& h, BnN51Big b, uint8_t* out) {
    uint32_t len = 0;
    h.big_to_binary(b, out, 0x20, &len);
    if (len < 0x20) memset(out + len, 0, 0x20 - len);   // Ordinal_494 on the tail
}

// 1.13c BNClient+0x0A4E0
// Install the SRP group: free a previous pair, then build g (+4) and N (+8) from the caller's 32-byte value.
// Both Ordinal_609 calls show param_1 as their source; their lengths are not in the listing (0x20 as in 0xAE10).
extern "C" uint32_t bnclient_a4e0(const BnN51Host* h, BnN51Group* grp, const void* param_1) {
    if (grp->ready) { h->big_del(grp->n); h->big_del(grp->g); }
    BnN51Big a = h->big_new();
    h->big_from_binary(a, param_1, 0x20);
    grp->g = a;
    BnN51Big b = h->big_new();
    h->big_from_binary(b, param_1, 0x20);
    grp->n = b;
    grp->ready = 1;
    return 1;   // the trailing `if (!*ctx) { free; *ctx=0 }` is dead: *ctx was just set to 1
}

// 1.13c BNClient+0x0A680
// M1 prefix: H(bin32(N)) xor H(bin32(g)) (20 bytes), then H over the account name, and the working buffers wiped.
// Reconstruction: which of the three aliased SHA1 contexts holds which digest follows the SRP-3 shape of the loop.
extern "C" void bnclient_a680(const BnN51Host* h, const BnN51Group* grp, const char* name,
                              uint8_t xor20[20], uint8_t name_hash[20]) {
    uint8_t buf[0x20], hn[20], hg[20];
    bn_n51_bin32(*h, grp->n, buf); h->sha1(buf, 0x20, hn);
    bn_n51_bin32(*h, grp->g, buf); h->sha1(buf, 0x20, hg);
    for (int i = 0; i < 0x14; i++) xor20[i] = hn[i] ^ hg[i];
    h->sha1(name, (uint32_t)strlen(name), name_hash);   // Ordinal_506 length
    memset(buf, 0, sizeof buf); memset(hn, 0, sizeof hn); memset(hg, 0, sizeof hg);
}

// 1.13c BNClient+0x0A910
// SRP u: SHA1 of the bignum serialised to 0x20 bytes (tail zero-padded); the first digest word is returned
// big-endian-assembled (CONCAT31 chain) through the out pointer.
extern "C" void bnclient_a910(const BnN51Host* h, uint32_t* out, BnN51Big* bn) {
    uint8_t buf[0x20], d[20];
    bn_n51_bin32(*h, *bn, buf);
    h->sha1(buf, 0x20, d);
    *out = (uint32_t)d[0] << 24 | (uint32_t)d[1] << 16 | (uint32_t)d[2] << 8 | d[3];
}

// 1.13c BNClient+0x0AE10
// Client public value: a = 32 random bytes from the hash stream into s+0x40; A = g^a mod N written to s+0x60
// (0x20 bytes, zero-padded). The two leading Ordinal_501 calls have no visible operands and are not modelled.
extern "C" void bnclient_ae10(const BnN51Host* h, uint8_t* s, const BnN51Group* grp, void* stream) {
    h->stream_read(stream, s + 0x40, 0x20);
    BnN51Big a = h->big_new();
    h->big_from_binary(a, s + 0x40, 0x20);
    BnN51Big r = h->big_new();
    h->big_from_unsigned(r, 0);
    h->big_powmod(r, grp->g, a, grp->n);
    h->big_del(a);
    bn_n51_bin32(*h, r, s + 0x60);
    h->big_del(r);
}

// 1.13c BNClient+0x0AED0
// Same shape as 0xAE10 with register operands: EAX = 32-byte secret buffer (stream_read target), EBX = 32-byte
// destination; the secret bignum is the 0xA9D0 result (EDI), the result bignum is created into ESI.
extern "C" void bnclient_aed0(const BnN51Host* h, const BnN51Group* grp, void* stream, uint8_t* secret32,
                              uint8_t* dest32) {
    h->stream_read(stream, secret32, 0x20);
    BnN51Big a = h->srp_x(stream);
    BnN51Big r = h->big_new();
    h->big_from_unsigned(r, 0);
    h->big_powmod(r, grp->g, a, grp->n);
    bn_n51_bin32(*h, r, dest32);
    h->big_del(a);
    h->big_del(r);
}

// 1.13c BNClient+0x0B0C0
// Validate a server value v (*pv): 0 < v < N, derive u = A910(v) (must be non-zero), then the SRP combine step:
// then PowMod, a conditional Add of N, a Sub, the 0xA180 step, an Add and a final PowMod. Operand roles after
// the u check are reconstructed from the listing's call order, not from per-operand evidence.
extern "C" uint32_t bnclient_b0c0(const BnN51Host* h, const BnN51Group* grp, BnN51Big* pv) {
    if (h->big_cmp(*pv, grp->n) >= 0) return 0;          // v must be < N
    BnN51Big z = h->big_new();
    h->big_from_unsigned(z, 0);
    int nz = h->big_cmp(*pv, z);
    h->big_del(z);
    if (nz == 0) return 0;                                // v != 0
    uint32_t u = 0;
    bnclient_a910(h, &u, pv);
    if (u == 0) return 0;
    BnN51Big ub = h->big_new();
    h->big_from_unsigned(ub, u);
    BnN51Big r = h->big_new();
    h->big_from_unsigned(r, 0);
    BnN51Big e = h->big_new();
    h->big_powmod(e, *pv, ub, grp->n);
    BnN51Big w = *pv;
    BnN51Big tmp = h->big_new();
    if (h->big_cmp(w, e) < 0) { h->big_add(tmp, w, grp->n); w = tmp; }
    h->big_sub(r, w, e);
    h->step_a180(r, r, ub);
    h->big_add(r, r, ub);
    h->big_powmod(r, r, e, grp->n);
    h->big_del(tmp); h->big_del(e); h->big_del(ub); h->big_del(r);
    return 1;
}
