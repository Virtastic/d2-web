// D2Common 1.13c batch. Globals: DAT_6fdf0b94 = table base (0x1c0-byte records), DAT_6fdf0b98 = count.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x3E980: record pointer for index; out of range -> error report then _exit(-1).
EMSCRIPTEN_KEEPALIVE uintptr_t d2_fn_d2common_3e980(int32_t idx, uintptr_t base, int32_t count) {
 if (idx < count) {
  if (idx > -1) return base + static_cast<uintptr_t>(static_cast<int64_t>(idx) * 0x1c0);
 }
 __builtin_trap(); // retail: Ordinal_10265(0xdd/0xde), Ordinal_10024(msg), _exit(-1)
}
}
