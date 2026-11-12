// Shared helpers of the small D2Client message handlers and unit callbacks written from the 1.13c disassembly.
// Module globals are named by their absolute retail address (0x6FAB0000 + RVA); every callee and import goes through
// e.H with the registers and stack words the retail call site passes.
#pragma once
#include <cstdint>
#include "cl_env.h"
namespace sn18 {
constexpr uint32_t kBase = 0x6FAB0000u;
constexpr uint32_t kTick = 0xCEF5C;   // GetTickCount IAT slot
// the common message-handler tail: busy=1, scroll=0, then the Ordinal 511 thunk
inline uint32_t tail(const cl::Env& e, uint32_t msg) {
    e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0);
    return e.H(0xBE84, msg);
}
inline uint32_t R16(const cl::Env& e, uint32_t a) { return e.R(a) & 0xffffu; }
inline void W16(const cl::Env& e, uint32_t a, uint32_t v) { e.W(a, (e.R(a) & 0xffff0000u) | (v & 0xffffu)); }
inline void blk(const cl::Env& e, uint32_t rva, uint32_t p, uint32_t n) { if (e.blk) e.blk(e.ctx, rva, p, n); }
}  // namespace sn18
