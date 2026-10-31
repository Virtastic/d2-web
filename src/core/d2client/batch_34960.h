// src/core/d2client/batch_34960.h -- the callee seam of the skill-description line builders in batch_34960.cpp.
#pragma once
#include <cstdint>
namespace d2client_batch_34960 {
// The skill-description line builders below take their callees through Ops: each method is one callee with the registers and
// stack words the 1.13c code hands it (out-parameters as pointers to the caller's locals). `srcFile` is the source-file string
// the line lookups pass, `pairCb` the pair callback 0x33240 that 0x30B40 receives.
struct Ops {
    uint32_t srcFile = 0, pairCb = 0;
    virtual uint32_t c2C230() = 0;
    virtual void c2C190() = 0;
    virtual void c32F30(uint32_t ecx, int32_t* la, int32_t* lb, int32_t* a, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6) = 0;
    virtual void c33240(uint32_t ecx, int32_t* la, int32_t* lb, int32_t* a, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6) = 0;
    virtual uint32_t c2F0A0(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3) = 0;
    virtual void c2E190(const uint8_t* eax, uint32_t ebx, uint32_t s0, int32_t* la, int32_t* lb, int32_t* a) = 0;
    virtual const uint8_t* c17F0(int32_t id) = 0;
    virtual uint32_t c10747(uint32_t unit) = 0;
    virtual uint32_t c10601(uint32_t unit, uint32_t file, uint32_t line) = 0;
    virtual uint32_t c10435(uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3) = 0;
    virtual int32_t c10567(uint32_t unit, int32_t skill, uint32_t s2, uint32_t s3) = 0;
    virtual int32_t c10297(uint32_t unit, int32_t skill, uint32_t s2, uint32_t s3) = 0;
    virtual int32_t c10121(uint32_t unit, int32_t skill, uint32_t s2, uint32_t s3) = 0;
    virtual int32_t c11091(uint32_t unit, int32_t skill, uint32_t s2, uint32_t s3) = 0;
    virtual uint32_t c30B40(const uint8_t* eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3,
                            uint32_t s4, uint32_t s5, uint32_t s6, uint32_t s7, uint32_t s8) = 0;
};
uint32_t sub_34960(Ops&, uint32_t eax0, uint32_t p1, uint32_t p2, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
uint32_t sub_349F0(Ops&, uint32_t eax0, uint32_t p1, uint32_t p2, const uint8_t* unit, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
uint32_t sub_34AB0(Ops&, uint32_t eax0, uint32_t p1, uint32_t p2, const uint8_t* a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
uint32_t sub_34C00(Ops&, uint32_t eax0, uint32_t p1, uint32_t p2, const uint8_t* a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
} // namespace d2client_batch_34960
