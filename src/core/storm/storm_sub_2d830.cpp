// 1.13c Storm+0x2d830: bignum long division, quotient q and remainder r of n / d.
// Vectors are {cap,len,data,gran,off}; digit i = data[off+i], 0 past len. Helpers are seams (Storm+0x3xxx/0x2xxxx).
#include <cstdint>

struct StormBigVec { uint32_t cap, len; uint32_t* data; uint32_t gran, off; };  // 0x14 bytes in retail

struct StormBigDivEnv {
    void (*trim)(StormBigVec*);                                   // +0x36D0 drop high zero limbs
    void (*assign)(StormBigVec* dst, const StormBigVec* src);     // +0x3BF0 / +0x2C0B0 copy
    void (*ensure)(StormBigVec*, uint32_t i);                     // +0x35C0 zero-grow so digit i exists
    void (*setlen)(StormBigVec*, uint32_t n);                     // +0x3680 len = off+n
    uint32_t (*bitlen)(const StormBigVec*);                       // +0x2ADC0 highest set bit index
    void (*copy)(StormBigVec* dst, const StormBigVec* src);       // +0x2C210
    void (*divsmall)(StormBigVec* q, StormBigVec* r, const StormBigVec* n, uint32_t d, uint32_t dhi);  // +0x2CD30
    void (*mulsmall)(StormBigVec* out, const StormBigVec* a, uint32_t m, uint32_t mhi);               // +0x2CBD0
    void (*sub)(StormBigVec* a, const StormBigVec* b);            // +0x2BA20 a -= b
    int  (*cmp)(const StormBigVec* a, const StormBigVec* b);      // +0x2AE30
};

static uint32_t dg(const StormBigVec* v, uint32_t i) { i += v->off; return i < v->len ? v->data[i] : 0; }

extern "C" void storm_sub_2d830(StormBigVec* q, StormBigVec* r, StormBigVec* n, StormBigVec* d, StormBigVec* scratch, const StormBigDivEnv* e) {
    e->trim(q); e->trim(q);
    uint32_t nl = n->len - n->off, dl = d->len - d->off;
    if (nl < dl) {  // n < d: quotient 0, remainder n
        if (r != n) e->assign(r, n);
        r->gran = n->gran; r->off = n->off;
        e->setlen(q, 0);
        return;
    }
    if (dl < 2) {  // single-limb divisor
        e->ensure(q, 0);
        e->divsmall(r, n, nullptr, dg(d, 0), 0);  // ponytail: retail passes r's digit pointer in arg 1; seam owns it
        e->setlen(q, 1);
        return;
    }
    int k = *(int*)((uint8_t*)scratch + 0x140);
    StormBigVec* t = reinterpret_cast<StormBigVec*>((uint8_t*)scratch + k * 0x14);
    StormBigVec* u = t + 1;
    StormBigVec* p = t + 2;
    *(int*)((uint8_t*)scratch + 0x140) = k + 3;
    e->bitlen(d);
    e->copy(t, n);
    e->copy(u, d);
    e->ensure(u, dl - 1);
    uint32_t top = dg(u, dl - 1) + 1;
    uint32_t cnt = nl - dl + 1;
    e->setlen(q, cnt);
    while (cnt) {
        --cnt;
        q->off = cnt; if (cnt) e->ensure(q, (uint32_t)-1);
        t->off = cnt; if (cnt) e->ensure(t, (uint32_t)-1);
        uint32_t digit;
        if (top == 0) {
            digit = dg(t, dl);
        } else {
            uint64_t v = (uint64_t)dg(t, dl) << 32 | dg(t, dl - 1);
            digit = (uint32_t)(v / top);
        }
        e->ensure(q, 0);
        q->data[q->off] = digit;
        const StormBigVec* sub = u;
        if (digit) { e->ensure(q, 0); e->mulsmall(p, u, digit, 0); sub = p; e->sub(t, sub); }
        for (;;) {
            e->ensure(t, dl);
            if (dg(t, dl) == 0 && e->cmp(t, u) < 0) break;
            e->ensure(q, 0);
            q->data[q->off]++;
            e->sub(t, u);
        }
    }
    e->assign(r, t);
    e->trim(q);
    *(int*)((uint8_t*)scratch + 0x140) -= 3;
}
