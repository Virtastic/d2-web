// D2Common 1.13c batch: __fptrap dispatch table fill.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x86FAC (size 56, caller 0x86FE4 __fpmath): stores six CRT float-helper addresses
// into the __fptrap slots at 6fde9a68..6fde9a7c. Slot order: __cfltcvt, LAB_6fdd7166, __fassign,
// __forcdecpt, LAB_6fdd71b1, __cfltcvt (the first and last slot hold the same helper). The targets
// are retail code addresses, which do not exist under wasm, so the caller supplies them in
// fn[0..5] (retail32 values) and tbl is the six-slot table.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_86fac(uint32_t tbl[6], const uint32_t fn[6]) {
 tbl[0] = fn[0];
 tbl[1] = fn[1];
 tbl[2] = fn[2];
 tbl[3] = fn[3];
 tbl[4] = fn[4];
 tbl[5] = fn[5];
}
}
