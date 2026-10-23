// 1.13c Storm+0x2EBB0: Miller-Rabin probable-prime test on bignum n.
// Bignum = uint32[5]: [0]=size, [1]=count, [2]=word ptr, [3],[4]=sign/offset. Scratch stack of 0x14-byte
// bignums at ctx, depth at ctx+0x140. Callees are seams. Returns 1 = probable prime, 0 = composite.
#include <cstdint>
typedef uint32_t u32;
struct StormPrimeEnv {
    void (*norm)(u32* bn);                                   // Storm+0x36D0 (register args)
    void (*mod_small)(u32* out, u32* n, u32 p, u32 z);       // +0x2CD30: *out = n mod p
    void (*sub1)(u32* d, u32* n);                            // +0x2BBB0: d = n-1 style prep
    u32 (*bitlen)(u32* bn);                                  // +0x2AD10
    void (*shr)(u32* d, u32* s);                             // +0x2C0B0: strip/halve
    void (*setsmall)(u32 v);                                 // +0x2B6A0
    int (*cmp)(u32* a, u32* b);                              // +0x2AE30
    void (*modexp)(u32* r, u32* base, u32* d);               // +0x2E930
    void (*mul)(u32* r, u32* a, u32* b, uint8_t* ctx);       // +0x2C840
    void (*mulmod)(u32* r, u32* a, u32* b, u32* n, uint8_t* ctx);  // +0x2D830
    void* (*realloc_fn)(void* p, u32 bytes, const char* f, int line, int z);  // +0x1C760
    const u32* primes;                                       // DAT 0x36CD0 table
};

extern "C" int storm_sub_2ebb0(uint8_t* ctx, u32* n, const StormPrimeEnv* e) {
    int& depth = *reinterpret_cast<int*>(ctx + 0x140);
    int d0 = depth;
    u32* t0 = reinterpret_cast<u32*>(ctx + d0 * 0x14);
    u32* t1 = reinterpret_cast<u32*>(ctx + (d0 * 5 + 5) * 4);
    u32* t2 = reinterpret_cast<u32*>(ctx + (d0 * 5 + 10) * 4);
    u32* t3 = reinterpret_cast<u32*>(ctx + (d0 * 5 + 15) * 4);
    depth = d0 + 4;
    auto isone = [](u32* b) { return b[1] - b[4] == 1 && b[4] < b[1] && reinterpret_cast<u32*>(b[2])[b[4]] == 1; };
    int ok = 1;
    e->norm(t0); e->norm(t1);
    {   // n must be odd
        u32 w = n[4];
        int odd = 0;
        if (n[1] != w) odd = ((w < n[1] ? reinterpret_cast<u32*>(n[2])[w] : 0) & 1) != 0;
        ok = odd;
    }
    for (u32 i = 0; ok && i <= 0x2ab; i += 4) {   // trial division by small primes
        u32 r;
        e->mod_small(&r, n, *reinterpret_cast<const u32*>(reinterpret_cast<const uint8_t*>(e->primes) + i), 0);
        if (r == 0) { e->norm(t2); if (!isone(t2)) ok = 0; }
    }
    e->sub1(t0, n);
    if (t1 != t0) {
        u32 cnt = t0[1]; u32* src = reinterpret_cast<u32*>(t0[2]);
        t1[0] = cnt;
        if (t1[2] || cnt) t1[2] = (u32)(uintptr_t)e->realloc_fn(reinterpret_cast<void*>(t1[2]), cnt << 2, "I", -2, 0);
        for (u32 k = 0; k < cnt; k++) { u32* p = reinterpret_cast<u32*>(t1[2]) + k; if (p) *p = src[k]; }
        t1[1] = cnt;
    }
    t1[3] = t0[3]; t1[4] = t0[4];
    u32 s = e->bitlen(t1);
    if (s) e->shr(t1, t1);
    for (u32 i = 0; ok && i <= 0x17; i += 4) {   // witness rounds
        e->setsmall(e->primes[i / 4]);
        if (e->cmp(t3, n) < 0) break;
        e->modexp(t2, t3, t1);
        e->norm(t2);
        if (!isone(t2)) {
            u32 j = 0;
            int c = e->cmp(t2, t0);
            while (c != 0) {
                if (++j == s) { ok = 0; break; }
                u32* sq = reinterpret_cast<u32*>(ctx + *reinterpret_cast<int*>(ctx + 0x140) * 0x14);
                ++*reinterpret_cast<int*>(ctx + 0x140);
                e->mul(sq, t2, t2, ctx);
                e->mulmod(sq, t2, sq, n, ctx);
                --*reinterpret_cast<int*>(ctx + 0x140);
                e->norm(t2);
                if (isone(t2)) { ok = 0; break; }
                c = e->cmp(t2, t0);
            }
        }
    }
    if (ok) {   // final: gcd-style checks with first 4 table entries
        for (u32 i = 0; ok && i <= 3; i += 4) {
            e->setsmall(e->primes[-1 + i / 4]);   // DAT 0x36CCC
            e->modexp(t2, t3, n);
            if (e->cmp(t2, t3) != 0) ok = 0;
        }
    }
    depth -= 4;
    return ok;
}
