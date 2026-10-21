// D2Common 1.13c batch: stat-list helper keyed by a 0x144-byte table row.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 32-bit retail pointer to the data tables (weak, provided by the link).
__attribute__((weak)) uint32_t sgptDataTables_39430;
__attribute__((weak)) int32_t FUN_6fd88a80(int32_t row);

// 1.13c D2Common+0x39430: total stat value for row p2 of the table at tables+0xbcc, 0 if unset.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_39430(const int32_t* p1, int32_t p2) {
 if (p1[0x5c / 4] == 0) return 0;
 const uint8_t* t = reinterpret_cast<const uint8_t*>(static_cast<intptr_t>(sgptDataTables_39430));
 if (p2 >= 0 && p2 < *reinterpret_cast<const int32_t*>(t + 0xbd4)) {
  intptr_t row = static_cast<intptr_t>(*reinterpret_cast<const uint32_t*>(t + 0xbcc)) + p2 * 0x144;
  if (row) return FUN_6fd88a80(static_cast<int32_t>(row));
 }
 return 0;
}
}
