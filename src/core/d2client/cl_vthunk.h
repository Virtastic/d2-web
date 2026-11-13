// The twelve one-line forwarders of the panel-object family: ECX = this; ECX := this[2] (the owned child), EAX := child's vtable,
// then a tail jump to vtable slot `slot` (byte offset) with the child as ECX and the caller's stack words untouched. The slot holds
// a module address; its D2Client RVA is the low 24 bits.
#pragma once
#include "cl_env.h"
namespace clc {
inline uint32_t vforward(const cl::Env& e, uint32_t self, uint32_t slot) {
    const uint32_t child = e.R(self + 8);
    const uint32_t vt = e.R(child);
    return e.H(e.R(vt + slot) & 0xFFFFFFu, 0, 0, 0, 0, 0, 0, child);
}
}  // namespace clc
