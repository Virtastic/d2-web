// 1.13c Storm+0x2D830: bignum long division (Knuth D), stdcall(q, r, n, d, ctx), ret 0x14.
// Vectors are {cap,size,data,grain,base}; the callee seams (every one a register-convention retail routine) are
// the env: Storm+0x36D0 trim, 0x35C0 touch, 0x3680 setlen, 0x3BF0 set-from-words, 0x2ADC0 bitlen, 0x2C210 shl,
// 0x2C0B0 shr, 0x2CD30 small divide, 0x2CBD0 small multiply, 0x2BA20 sub, 0x2AE30 compare. touch/setlen take
// retail's ECX: an index RELATIVE to the vector's base. The probe binds the env to the verified ports.
#include <cstdint>

struct D2830Vec { uint32_t cap, size; uint32_t* data; uint32_t grain, base; };
struct D2830Ctx { D2830Vec t[16]; uint32_t depth; };  // 16 temp vectors then the depth dword (retail +0x140)

struct D2830Env {
    void (*trim)(D2830Vec*);                                                  // 0x36D0
    void (*touch)(D2830Vec*, uint32_t rel);                                   // 0x35C0
    void (*setlen)(D2830Vec*, uint32_t rel);                                  // 0x3680
    void (*setWords)(D2830Vec* dst, const uint32_t* src, uint32_t count);     // 0x3BF0
    uint32_t (*bitlen)(const D2830Vec*);                                      // 0x2ADC0
    void (*shl)(D2830Vec* v, uint32_t shift, const D2830Vec* src);            // 0x2C210
    void (*shr)(D2830Vec* v, uint32_t shift, const D2830Vec* src);            // 0x2C0B0
    void (*divsmall)(D2830Vec* dst, uint32_t* rem, const D2830Vec* src, uint32_t divLo, uint32_t divHi);  // 0x2CD30
    void (*mulsmall)(D2830Vec* dst, const D2830Vec* src, uint32_t lo, uint32_t hi);                      // 0x2CBD0
    void (*sub)(D2830Vec* v, const D2830Vec* a, const D2830Vec* b);           // 0x2BA20
    int32_t (*cmp)(const D2830Vec*, const D2830Vec*);                         // 0x2AE30
};

static uint32_t& D2830At(D2830Vec* v, uint32_t rel) { return v->data[v->base + rel]; }

extern "C" void storm_sub_2d830(D2830Vec* q, D2830Vec* r, D2830Vec* n, D2830Vec* d, D2830Ctx* ctx, const D2830Env* e) {
    e->trim(n);
    e->trim(d);
    const uint32_t nl = n->size - n->base, dl = d->size - d->base;
    if (dl > nl) {  // n < d: r = n, q = 0
        if (r != n) e->setWords(r, n->data, n->size);
        r->grain = n->grain;
        r->base = n->base;
        e->setlen(q, 0);
        return;
    }
    if (dl < 2) {  // single-limb divisor
        uint32_t dv = d->base < d->size ? d->data[d->base] : 0;
        e->touch(r, 0);
        e->divsmall(q, &r->data[r->base], n, dv, 0);
        e->setlen(r, 1);
        return;
    }
    const uint32_t k = ctx->depth;
    D2830Vec* t = &ctx->t[k];
    D2830Vec* u = &ctx->t[k + 1];
    D2830Vec* p = &ctx->t[k + 2];
    ctx->depth = k + 3;
    const uint32_t shift = 0x1f - (e->bitlen(d) & 0x1f);
    e->shl(t, shift, n);
    e->shl(u, shift, d);
    e->touch(u, dl - 1);
    const uint32_t top = D2830At(u, dl - 1) + 1;
    uint32_t cnt = nl - dl + 1;
    e->setlen(q, cnt);
    while (cnt != 0) {
        --cnt;
        q->base = cnt;
        if (cnt) e->touch(q, 0xFFFFFFFFu);
        t->base = cnt;
        if (cnt) e->touch(t, 0xFFFFFFFFu);
        uint32_t digit;
        if (top == 0) {
            e->touch(t, dl);
            digit = D2830At(t, dl);
            e->touch(q, 0);
        } else {
            e->touch(t, dl);
            e->touch(t, dl - 1);
            const uint64_t v = ((uint64_t)D2830At(t, dl) << 32) + D2830At(t, dl - 1);  // __allmul(x,0,0,1) + low
            e->touch(q, 0);
            digit = (uint32_t)(v / top);  // __aulldiv(v, top): the low dword
        }
        D2830At(q, 0) = digit;
        e->touch(q, 0);
        if (D2830At(q, 0) != 0) {
            e->touch(q, 0);
            e->mulsmall(p, u, D2830At(q, 0), 0);
            e->sub(t, t, p);
        }
        for (;;) {
            e->touch(t, dl);
            if (D2830At(t, dl) == 0 && e->cmp(t, u) < 0) break;
            e->touch(q, 0);
            D2830At(q, 0)++;
            e->sub(t, t, u);
        }
    }
    e->shr(r, shift, t);
    e->trim(r);
    ctx->depth -= 3;
}
