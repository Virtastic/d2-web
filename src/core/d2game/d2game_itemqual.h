// Shared by the item-quality roll family (the superior / magic / rare / normal quality setters, the two affix-set
// rollers and the two finishing dispatchers): the callee seams in retail argument order. P is the item (*ppItem),
// Q the caller's low seed (*pLowSeed), `drop` the item-drop record (quality at +0x30, prefix +0x68, suffix +0x74,
// the owner unit at +0 whose +4 is its id, else the id at +0x40). Imports are stdcall thunks; the internal helpers
// take some arguments in registers, named in the field comments. Retail is 32-bit, so the record's pointers are
// 32-bit words read as they lie.
#pragma once
#include <cstdint>
struct D2GItemQualEnv {
    void     (*e210)(void);                                   // internal, EDI = the caller's pP (not observable), no stack
    void     (*imp_a5b0)(uint32_t p, uint32_t q);
    uint32_t (*imp_a5ec)(uint32_t p);
    void     (*imp_a628)(uint32_t ecx, uint32_t edx);         // fastcall (ECX, EDX)
    void     (*ee90)(uint32_t p, void* drop);
    void     (*imp_b090)(uint32_t p, uint32_t state);
    uint32_t (*imp_afb2)(uint32_t ecx);                       // fastcall (ECX)
    uint16_t (*imp_b37e)(uint32_t p);                         // AX result
    uint32_t (*imp_b378)(uint32_t p);
    void     (*sub14f20)(void* drop);
    void     (*sub15240)(void* drop);
    void     (*sub140b0)(uint32_t p, void* drop);
    uint32_t (*sub148a0)(uint32_t h, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
    uint32_t (*sub144d0)(uint32_t h, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);
    void     (*imp_b3c6)(uint32_t p, uint32_t res, uint32_t z);
    void     (*imp_b3b4)(uint32_t p, uint32_t res, uint32_t z);
    void     (*imp_a65e)(uint32_t p, uint32_t a, uint32_t b);
    void     (*sub13de0)(uint32_t eaxP, void* drop);            // EAX = P, stack = drop
    uint32_t (*sub103c0)(const uint32_t* pP, void* drop);
    void     (*sub10600)(void* eaxDrop, uint32_t ecxP, uint32_t stackArg);   // EAX = drop, ECX = P
    void     (*sub10550)(uint32_t eaxP, void* ediDrop, uint32_t stackArg);   // EAX = P, EDI = drop
    void     (*sub10240)(uint32_t esiP, void* drop);            // ESI = P, stack = drop
    uint32_t (*imp_a430)(uint32_t p, uint32_t flag);
    void     (*imp_b1aa)(uint32_t p, uint32_t id);
    uint32_t (*imp_aa66)(uint32_t p);
    uint32_t (*sub132a0)(uint32_t eaxP, uint32_t ebx);            // EAX = P, EBX = 0/1, no stack
    void     (*subec90)(uint32_t ediP, uint32_t stackArg);        // EDI = P
    void     (*fatal)(uint32_t p);                                // the no-status-set assertion; does not return in retail
};
// The state preamble every quality setter shares: up to the state write at drop+0x30.
static inline void d2g_itemqual_pre(const D2GItemQualEnv* env, uint32_t P, uint32_t Q, void* drop, uint32_t state) {
    env->e210();
    env->imp_a5b0(P, Q);
    uint32_t x = env->imp_a5ec(P);
    env->imp_a628(x, Q);
    env->ee90(P, drop);
    env->imp_b090(P, state);
    *(uint32_t*)((uint8_t*)drop + 0x30) = state;
}
// The drop owner's id word: the owner record's +4 when there is one, else +0x40.
static inline uint32_t d2g_itemqual_owner(const void* drop) {
    const uint8_t* c = (const uint8_t*)drop;
    uint32_t own = *(const uint32_t*)c;
    return own ? *(const uint32_t*)(uintptr_t)(own + 4) : *(const uint32_t*)(c + 0x40);
}
