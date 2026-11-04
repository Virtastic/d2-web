// Callee seam for the D2Multi glue ports (src/core/d2multi/d2multi_<rva>.cpp): every retail callee (an import thunk, a
// D2Win/Fog/Storm ordinal, a sibling D2Multi function) is one call(): the callee's RVA, the five registers it reads
// (eax ecx edx ebx esi) and up to four stack words, returning its EAX. A pointer to a D2Multi global is passed as
// D2M_MOD(rva) (the capture records it the same way); a local buffer is passed as its address.
#pragma once
#include <cstdint>

struct D2mEnv {
    void* ctx;
    uint32_t (*call)(void* ctx, uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t s,
                     uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
    void (*edi)(void* ctx, uint32_t rva, uint32_t v);   // a callee that takes EDI as an argument (descriptor REGS `J`): called right after call(), logs EDI
};

#define D2M_MOD(rva) (0xB2000000u | (uint32_t)(rva))
static inline uint32_t d2m_call(const D2mEnv& e, uint32_t rva, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0,
                                uint32_t s = 0, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0) {
    return e.call(e.ctx, rva, a, c, d, b, s, s0, s1, s2, s3);
}
static inline uint32_t d2m_ptr(const void* p) { return (uint32_t)(uintptr_t)p; }
// EDI handed to the callee the rule just called (REGS `J`): a local buffer logs as a stack pointer.
static inline void d2m_edi(const D2mEnv& e, uint32_t rva, uint32_t v) { e.edi(e.ctx, rva, v); }
