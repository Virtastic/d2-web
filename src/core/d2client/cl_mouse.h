// Shared rules of the D2Client mouse / window message handlers (the handlers that take the input message pointer at
// [esp+4] and return with ret 4). The message x is the word at +0xC and y the word at +0xE; "handled" is +0x18 = 1 and
// +0x1C = 0. The value left in EAX is modelled, so each port returns it and the vector compares it.
#pragma once
#include "cl_cmd.h"
namespace clm {
inline uint32_t X(const cl::Env& e, uint32_t m) { return e.R(m + 0xC) & 0xffffu; }
inline uint32_t Y(const cl::Env& e, uint32_t m) { return e.R(m + 0xC) >> 16; }
inline void handled(const cl::Env& e, uint32_t m) { e.W(m + 0x18, 1); e.W(m + 0x1C, 0); }
// mark handled, then the handled-message routine
inline uint32_t done(const cl::Env& e, uint32_t m) { handled(e, m); return e.H(0xBE84, m); }
// the six selection words at 0xE0EA8.. back to -1
inline void sel(const cl::Env& e) { for (uint32_t a = 0xE0EA8; a <= 0xE0EBC; a += 4) clc::S(e, a, 0xFFFFFFFFu); }
// the four hover words at 0x11BC28 / 2C / 38 / 3C cleared
inline void hov(const cl::Env& e) { clc::S(e, 0x11BC28, 0); clc::S(e, 0x11BC2C, 0); clc::S(e, 0x11BC38, 0); clc::S(e, 0x11BC3C, 0); }
// the cursor-reset routine: EDX = -1, ECX = 0, stack words -1, 0, 0
inline uint32_t reset(const cl::Env& e) { return e.H(0xD384, 0xFFFFFFFFu, 0, 0, 0, 0, 0, 0, 0xFFFFFFFFu); }
// mouse position stores
inline void pos(const cl::Env& e, uint32_t x, uint32_t y) { clc::S(e, 0x10B2A4, x); clc::S(e, 0x10B430, y); }
// x in [lo, hi) and y in [lo, hi] as the 94xxx handlers test their rect words (signed compares)
inline bool in(const cl::Env& e, uint32_t rx, uint32_t x, uint32_t y) {
    return (int32_t)x >= (int32_t)clc::G(e, rx) && (int32_t)x < (int32_t)clc::G(e, rx + 4) &&
           (int32_t)y >= (int32_t)clc::G(e, rx + 8) && (int32_t)y <= (int32_t)clc::G(e, rx + 12);
}
}  // namespace clm
