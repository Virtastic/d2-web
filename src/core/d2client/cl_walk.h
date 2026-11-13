// The listener walk of the D2Client panel-stack message handlers: the panel block (module pointer at 0x11BC00) holds a singly
// linked list at +0xB08 (node: object at +0, next at +4); each object's method table is called through a slot with the message
// (ECX = the object). A missing panel block is the assertion path (never reached in a running client).
#pragma once
#include "cl_pa4.h"
namespace clw {
inline uint32_t panel(const cl::Env& e, uint32_t line) {
    const uint32_t p = clc::G(e, 0x11bc00);
    if (!p) pa4::assertAt(e, line);
    return p;
}
// calls slot (byte offset) of each listener until one answers `stop` (nonzero when anyNz); returns true when one did, `ans` = its answer
inline bool walk(const cl::Env& e, uint32_t p, uint32_t slot, uint32_t msg, uint32_t& ans, bool anyNz = false) {
    for (uint32_t n = e.R(p + 0xb08); n; n = e.R(n + 4)) {
        const uint32_t o = e.R(n);
        if (!o) continue;
        ans = e.H(e.R(e.R(o) + slot) & 0xFFFFFFu, msg, 0, 0, 0, 0, 0, o);
        if (anyNz ? ans != 0 : ans == 1) return true;
    }
    return false;
}
// the shared fallthrough: the page-step routine (ECX = +-1, EDI = the panel block)
inline uint32_t step(const cl::Env& e, uint32_t p, uint32_t d) {
    e.Ed(p);
    const uint32_t r = e.H(0xa4560, 0, 0, 0, 0, 0, 0, d);
    e.Edj(0xa4560, p);   // the routine's EDI argument is part of the call record
    return r;
}
}  // namespace clw
