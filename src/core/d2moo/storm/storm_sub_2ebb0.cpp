// 1.13c Storm+0x2EBB0: probable-prime test on bignum n. Retail: EAX = ctx, stack = n (ret 4); returns 1 = probable prime.
// Vectors are {cap,size,data,grain,base}; the ctx is 16 temp vectors then the depth dword (+0x140). Four temps are taken at
// the current depth: t0 = n-1, t1 = odd part of n-1, t2 = work, t3 = witness. Steps: n must be odd; trial division by the
// 171 odd primes 3..1021 (retail table at Storm+0x46CD0, the prime 2 just before it at +0x46CCC), failing unless the quotient
// is 1; t0 = n-1, t1 = copy of t0 shifted right by its trailing-zero count s; Miller-Rabin with witnesses 3,5,7,11,13,17
// (stopping, still passing, at the first witness >= n); then the Fermat check 2^n mod n == 2.
// Callees are the env (register-convention retail routines): 0x36D0 trim, 0x2CD30 small divide, 0x2BBB0 subtract word,
// 0x1C760 SMemReAlloc, 0x2AD10 trailing zeros, 0x2C0B0 shr, 0x2B6A0 set word, 0x2AE30 compare, 0x2E930 modexp,
// 0x2C840 multiply, 0x2D830 divmod.
#include <cstdint>

struct D2EBB0Vec { uint32_t cap, size; uint32_t* data; uint32_t grain, base; };
struct D2EBB0Ctx { D2EBB0Vec t[16]; uint32_t depth; };
struct D2EBB0Env {
    void (*trim)(D2EBB0Vec*);                                                                  // 0x36D0
    void (*divsmall)(D2EBB0Vec* dst, uint32_t* rem, const D2EBB0Vec* src, uint32_t lo, uint32_t hi);  // 0x2CD30
    void (*subword)(D2EBB0Vec* v, uint32_t w, const D2EBB0Vec* src);                           // 0x2BBB0
    uint32_t* (*realloc)(uint32_t* p, uint32_t bytes);                                         // 0x1C760 (file, -2, 0)
    uint32_t (*lowbit)(const D2EBB0Vec*);                                                      // 0x2AD10
    void (*shr)(D2EBB0Vec* v, uint32_t shift, const D2EBB0Vec* src);                           // 0x2C0B0
    void (*setword)(D2EBB0Vec* v, uint32_t val);                                               // 0x2B6A0
    int32_t (*cmp)(const D2EBB0Vec*, const D2EBB0Vec*);                                        // 0x2AE30
    void (*modexp)(D2EBB0Vec* r, D2EBB0Vec* b, D2EBB0Vec* x, D2EBB0Vec* m, D2EBB0Ctx* c);      // 0x2E930
    void (*mul)(D2EBB0Vec* d, D2EBB0Vec* a, D2EBB0Vec* b, D2EBB0Ctx* c);                       // 0x2C840
    void (*divmod)(D2EBB0Vec* q, D2EBB0Vec* r, D2EBB0Vec* n, D2EBB0Vec* d, D2EBB0Ctx* c);      // 0x2D830
};

namespace {
struct PrimeTable { uint32_t p[172]; };   // p[0] = 2 (retail +0x46CCC), p[1..171] = 3..1021 (+0x46CD0)
constexpr PrimeTable makePrimes() {
    PrimeTable t{};
    uint32_t n = 0;
    for (uint32_t c = 2; n < 172; c++) {
        bool prime = true;
        for (uint32_t k = 0; k < n && t.p[k] * t.p[k] <= c; k++) if (c % t.p[k] == 0) { prime = false; break; }
        if (prime) t.p[n++] = c;
    }
    return t;
}
constexpr PrimeTable kPrimes = makePrimes();
static_assert(kPrimes.p[171] == 1021, "table ends at 1021");
bool isOne(const D2EBB0Vec* v) { return v->size - v->base == 1 && v->base < v->size && v->data[v->base] == 1; }
}  // namespace

extern "C" int32_t storm_sub_2ebb0(D2EBB0Vec* n, D2EBB0Ctx* ctx, const D2EBB0Env* e) {
    const uint32_t d = ctx->depth;
    D2EBB0Vec *t0 = &ctx->t[d], *t1 = &ctx->t[d + 1], *t2 = &ctx->t[d + 2], *t3 = &ctx->t[d + 3];
    ctx->depth = d + 4;
    int32_t ok = 1;
    e->trim(n);
    e->trim(n);
    if (n->size == n->base || !((n->base < n->size ? n->data[n->base] : 0) & 1)) ok = 0;
    for (uint32_t i = 1; ok && i < 172; i++) {
        uint32_t rem;
        e->divsmall(t2, &rem, n, kPrimes.p[i], 0);
        if (rem == 0) { e->trim(t2); if (!isOne(t2)) ok = 0; }
    }
    e->subword(t0, 1, n);
    if (t1 != t0) {
        const uint32_t cnt = t0->size; const uint32_t* src = t0->data;
        t1->cap = cnt;
        if (t1->data || cnt) t1->data = e->realloc(t1->data, cnt << 2);
        for (uint32_t k = 0; k < cnt; k++) if (t1->data + k) t1->data[k] = src[k];
        t1->size = cnt;
    }
    t1->grain = t0->grain;
    t1->base = t0->base;
    const uint32_t s = e->lowbit(t1);
    if (s) e->shr(t1, s, t1);
    for (uint32_t i = 1; ok && i <= 6; i++) {
        e->setword(t3, kPrimes.p[i]);
        if (e->cmp(t3, n) >= 0) break;
        e->modexp(t2, t3, t1, n, ctx);
        e->trim(t2);
        if (isOne(t2)) continue;
        uint32_t j = 0;
        while (e->cmp(t2, t0) != 0) {
            if (++j == s) { ok = 0; break; }
            D2EBB0Vec* sq = &ctx->t[ctx->depth++];
            e->mul(sq, t2, t2, ctx);
            e->divmod(sq, t2, sq, n, ctx);
            ctx->depth--;
            e->trim(t2);
            if (isOne(t2)) { ok = 0; break; }
        }
    }
    if (ok) {
        e->setword(t3, kPrimes.p[0]);
        e->modexp(t2, t3, n, n, ctx);
        if (e->cmp(t2, t3) != 0) ok = 0;
    }
    ctx->depth -= 4;
    return ok;
}
