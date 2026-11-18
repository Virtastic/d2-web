// 1.13c BNClient+0x0B0C0 (345 B): SRP client shared secret, retail register layout: EAX = pointer to the cell holding the server value v,
// stack (ret 0x10): P (+4 = g, +8 = N handles), A1 (cell: addend), C (cell: secret exponent), R (cell: result handle). Rule: v < N and v != 0, u = 0xA910(EDX = &v)
// non-zero, W = g^(*C) mod N, T = v < W ? v + N : v, T2 = T - W, T3 = 0xA180(*C, u), T3 += *A1, *R = T2^T3 mod N. Returns 1 (0 on a failed check).
// Every callee is an injected seam carrying its retail operands: Storm #601 cmp (0x871C), #624 new (0x8740), #612 set (0x872E), #628 powmod (0x86FE),
// add (0x873A), sub (0x8722), #606 del (0x870A), BNClient 0xA910 and 0xA180.
#include <cstdint>
struct BnB0c0Env {
    int32_t (*big_cmp)(uint32_t a, uint32_t b);                       // 0x871C
    void (*big_new)(uint32_t* slot);                                  // 0x8740
    void (*big_set)(uint32_t h, uint32_t v);                          // 0x872E
    void (*big_powmod)(uint32_t r, uint32_t b, uint32_t e, uint32_t n);  // 0x86FE
    void (*big_add)(uint32_t r, uint32_t a, uint32_t b);              // 0x873A
    void (*big_sub)(uint32_t r, uint32_t a, uint32_t b);              // 0x8722
    void (*big_del)(uint32_t h);                                      // 0x870A
    void (*srp_u)(const uint32_t* cell, uint32_t* out);               // 0xA910 (EDX = cell)
    void (*step_a180)(uint32_t p1, uint32_t p2, uint32_t p3);         // 0xA180
};
extern "C" uint32_t d2_bnclient_b0c0(const uint32_t* vc, const uint32_t* p, const uint32_t* a1, const uint32_t* c, const uint32_t* rc,
                                     const BnB0c0Env* e) {
    const uint32_t v = *vc, n = p[2];
    if (e->big_cmp(v, n) >= 0) return 0;
    uint32_t z;
    e->big_new(&z);
    e->big_set(z, 0);
    const int32_t t = e->big_cmp(v, z);
    e->big_del(z);
    if (t == 0) return 0;
    uint32_t u;
    e->srp_u(vc, &u);
    if (u == 0) return 0;
    uint32_t w;
    e->big_new(&w);
    e->big_set(w, 0);
    e->big_powmod(w, p[1], *c, n);
    uint32_t t2, t3;
    e->big_new(&t2);
    e->big_set(t2, 0);
    e->big_new(&t3);
    e->big_set(t3, 0);
    if (e->big_cmp(v, w) < 0) {
        e->big_add(t3, v, n);
        e->big_sub(t2, t3, w);
    } else {
        e->big_sub(t2, v, w);
    }
    e->step_a180(t3, *c, u);
    e->big_add(t3, t3, *a1);
    e->big_powmod(*rc, t2, t3, n);
    e->big_del(t3);
    e->big_del(t2);
    e->big_del(w);
    return 1;
}
