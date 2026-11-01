// Callee seam: each method is one callee with the registers and stack words the caller hands it.
#pragma once
#include <cstdint>
namespace d2client_34e00 {
struct Ops {
    virtual const uint8_t* c17F0(int32_t id) = 0;   // eax = id (sign-extended word)
    virtual uint32_t c34C00(uint32_t ecx, uint32_t edx, const uint8_t* a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) = 0; // callee pops 20 bytes
};
uint32_t sub_34E00(Ops&, uint32_t eax0, uint32_t ecx, uint32_t edx, const uint8_t* a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
}
