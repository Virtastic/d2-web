#include <cstdint>

// 1.13c D2sound+0xA057
// MSVC CRT x87 helper: a 64-entry computed-jump table (selector = EAX & 0x3f)
// of register-permutation stubs, each tail-calling the CRT fp classifier
// sub_9F40 (D2sound+0x9F40) with a pair of x87 stack registers (STi, STj) as
// 80-bit operands; the result is one of the registers, or ST0/ST0 for the
// degenerate cases. The wasm build uses no x87 (fixed-width ints, no float),
// so there is no selector to dispatch: callers (sub_8830) compile to native ops.
int d2sound_a057_x87dispatch(uint32_t selector) { return (int)(selector & 0x3f); }
