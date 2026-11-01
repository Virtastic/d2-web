// Callee seam for the pair helpers: the first takes eax, ecx and eight stack words, the second five stack words.
#pragma once
#include <cstdint>
namespace d2client_33240 {
// A stack word that holds the address of one of the caller's own locals is passed as kLocal (the address itself is not a value
// anyone can compare); the callee's writes through it come back by the out parameters of c30E30loc.
constexpr uint32_t kLocal = 0xB3000000u;
struct Ops {
    // First helper: ecx = unit, eax = e, stack = s[0..7] (callee pops 32 bytes).
    virtual void c31220(uint32_t ecx, uint32_t eax, const uint32_t s[8]) = 0;
    // Second helper: stack = (unit, pMin, pMax, pType, x) (callee pops 20 bytes); eax is its result.
    virtual uint32_t c30E30(uint32_t unit, uint32_t pMin, uint32_t pMax, uint32_t pType, uint32_t x) = 0;
    // the same call with pMin/pMax the caller's locals: the callee adds into *mn and *mx
    virtual uint32_t c30E30loc(uint32_t unit, int32_t* mn, int32_t* mx, uint32_t pType, uint32_t x) = 0;
};
// fastcall(ecx = unit, edx = e1) + seven stack words, callee pops 28 bytes
uint32_t sub_33240(Ops&, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6);
// register-arg helper: eax = pType pointer, ecx = a first-helper word, esi = scale, edi = x; stack unit, pOutX, pOutY (callee pops 12 bytes)
uint32_t sub_34D70(Ops&, uint32_t eax, uint32_t ecx, int32_t esi, uint32_t edi, uint32_t unit, int32_t* pX, int32_t* pY);
}
