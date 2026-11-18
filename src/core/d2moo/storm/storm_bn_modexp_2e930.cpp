// 1.13c Storm+0x2E930
// Prior art used: cached decompile build-wasm/ghidra-storm/decomp/0x2E930.c; vector/ctx layout as in storm_bnvec_batch.cpp.
#include <cstdint>

struct StormBnVec { uint32_t cap, size; uint32_t* data; uint32_t grain, base; };
struct StormBnCtx { StormBnVec t[16]; uint32_t depth; };
void storm_bnvec_trim(StormBnVec*);
void storm_bnvec_set_one(StormBnVec*);
void storm_bnvec_copy(StormBnVec*, const uint32_t*, uint32_t);
void storm_bnvec_mul(StormBnVec*, StormBnVec*, StormBnVec*, StormBnCtx*);
void storm_bnvec_sqr(StormBnVec*, StormBnVec*, StormBnCtx*);
// retail sub_2D830 (UNMAPPED): quotient q, remainder r = num divmod den.
__attribute__((weak)) void storm_bnvec_divmod(StormBnVec* q, StormBnVec* r, StormBnVec* num, StormBnVec* den, StormBnCtx* ctx);

// result = base ^ exp mod m, 2-bit fixed window (powers 1..3 precomputed). Retail: ESI=result, EAX=m, EBX=ctx.
void storm_bn_modexp(StormBnVec* result, StormBnVec* base, StormBnVec* exp, StormBnVec* m, StormBnCtx* c) {
    storm_bnvec_trim(exp);
    if (exp->size == exp->base) { storm_bnvec_set_one(result); return; }
    uint32_t d = c->depth;
    StormBnVec* q = &c->t[d];
    StormBnVec* w[4] = {nullptr, base, &c->t[d + 1], &c->t[d + 2]};
    c->depth = d + 3;
    StormBnVec* r = result;
    if (result == base || result == exp || result == m) { r = &c->t[d + 3]; c->depth = d + 4; }
    StormBnVec* tmp = &c->t[c->depth++];
    storm_bnvec_mul(tmp, base, base, c);
    storm_bnvec_divmod(tmp, w[2], tmp, m, c);
    storm_bnvec_mul(tmp, w[2], base, c);
    storm_bnvec_divmod(tmp, w[3], tmp, m, c);
    c->depth--;
    storm_bnvec_set_one(r);
    uint32_t n = exp->size - exp->base;
    for (uint32_t i = n; i != 0;) {
        i--;
        uint32_t idx = i + exp->base;
        uint32_t e = idx < exp->size ? exp->data[idx] : 0;
        int bits = 32;
        if (i + 1 == n && (e & 0xc0000000u) == 0) {
            do { e <<= 2; bits -= 2; } while ((e & 0xc0000000u) == 0);  // retail loops forever on a zero top word
        }
        if (bits == 0) continue;
        for (int k = (bits - 1) / 2 + 1; k != 0; k--) {
            storm_bnvec_sqr(r, r, c);
            storm_bnvec_divmod(q, r, r, m, c);
            storm_bnvec_sqr(r, r, c);
            storm_bnvec_divmod(q, r, r, m, c);
            if (e >> 30) {
                StormBnVec* t = &c->t[c->depth++];
                storm_bnvec_mul(t, r, w[e >> 30], c);
                storm_bnvec_divmod(t, r, t, m, c);
                c->depth--;
            }
            e <<= 2;
        }
    }
    if (result != r) {
        storm_bnvec_copy(result, r->data, r->size);
        result->grain = r->grain;
        result->base = r->base;
        c->depth--;
    }
    c->depth -= 3;
}
