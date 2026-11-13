// The one-line forwarders of the 0x3EEB0.. group: ECX = this, this+8 = the owned object, the thunk loads its vtable slot and calls it with
// ECX = the object. A vtable word is a module address (the probe's recorder holds it as 0xB2000000|rva, the engine as base+rva).
#pragma once
#include "cl_env.h"
inline uint32_t d2cVcall(const cl::Env& e, uint32_t self, uint32_t slot, uint32_t stackWord) {
    const uint32_t obj = e.R(self + 8), fp = e.R(e.R(obj) + slot);
    const uint32_t rva = (fp >> 24) == 0xB2 ? (fp & 0xFFFFFFu) : fp - 0x6FAB0000u;
    return e.H(rva, stackWord, 0, 0, 0, 0, 0, obj);
}
