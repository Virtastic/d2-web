#include <cstdint>
// 1.13c D2Client+0x40450 : index (EDI) -> record in sgptDataTables (+0xB98 base, +0xBA0 count, 0x23C stride).
// Every valid path ends in Ordinal_10786 (register args unrecovered); the flagged branch's side calls
// (10871, 10680) only select between identical tail calls, so they are injected and evaluated for order.
struct Ca40450 { int32_t (*o10786)(); int32_t (*o10871)(); int32_t (*o10680)(int32_t,int32_t,int32_t); uint8_t bit8; };
extern "C" int32_t d2c_unit_anim_len_40450(const uint8_t* tbl, int32_t idx, const Ca40450& c) {
  int32_t cnt = *(const int32_t*)(tbl + 0xBA0);
  if (idx < 0 || cnt <= idx) return 0;
  const uint8_t* r = *(const uint8_t* const*)(tbl + 0xB98) + (uint32_t)idx * 0x23C;
  if (c.bit8 & r[4]) {
    int16_t a = *(const int16_t*)(r + 0x80), b = *(const int16_t*)(r + 0x54);
    if (a < 0 || *(const int32_t*)(tbl + 0xC4) <= a) return c.o10786();
    if (b >= 0 && b < *(const int32_t*)(tbl + 0xBD4)) {
      int32_t v = c.o10871();
      if (v) c.o10680(v, b, 0);
    }
  }
  return c.o10786();
}
