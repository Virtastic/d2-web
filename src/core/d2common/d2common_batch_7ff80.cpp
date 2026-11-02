// D2Common 1.13c batch. The table base is the loaded arena table's address, supplied by the caller.
#include <cstdint>
#include <emscripten.h>

extern "C" {
#define W __attribute__((weak))
W uint32_t Ordinal_10265(uint32_t);
W void Ordinal_10024(const void*, uint32_t);
W char DAT_6fdda728[1];

// 1.13c D2Common+0x7ff80 (D2InvRemoveItem): 0x1C-byte record address for index idx; idx > 0 is fatal.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_7ff80_batch(int32_t idx, uint32_t nTableBase) {
 if (0 < idx) {
  Ordinal_10024(DAT_6fdda728, Ordinal_10265(0x41));
  __builtin_trap(); // retail: _exit(-1)
 }
 return idx * 0x1c + (int32_t)nTableBase;
}
}
