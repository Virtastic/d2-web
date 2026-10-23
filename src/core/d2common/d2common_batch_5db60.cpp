// D2Common 1.13c batch: DataTbls.cpp tail unload (flat form). Globals are retail 32-bit slots.
#include <cstdint>
#include <emscripten.h>

extern "C" {
#define W __attribute__((weak))
W void Ordinal_10046(const char* file, int32_t line, int32_t z);
W void Ordinal_10212(uint32_t p);
W char DAT_6fde9e20[4];
W char DAT_6fdf0a64[4], DAT_6fdf0a68[4], DAT_6fdf0a6c[4], DAT_6fdf0a70[4], DAT_6fdf0a74[4], DAT_6fdf0a78[4];
W char DAT_6fdf0a7c[4], DAT_6fdf0a80[4], DAT_6fdf0a84[4], DAT_6fdf0a88[4], DAT_6fdf0a8c[4];
W char DAT_6fdeff18[4], DAT_6fdeff1c[4], DAT_6fdeff20[4], DAT_6fdeff24[4], DAT_6fdeff28[4], DAT_6fdeff2c[4];

#define U(x) (*(uint32_t*)(x))
static const char kF[] = "..\\Source\\D2Common\\DATATBLS\\DataTbls.cpp";

// 1.13c D2Common+0x5DB60
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_5db60() {
  if (U(DAT_6fdf0a7c)) { Ordinal_10046(kF, 0x5b8, 0); U(DAT_6fdf0a7c) = 0; }
  if (U(DAT_6fdf0a84)) { Ordinal_10046(kF, 0x5be, 0); U(DAT_6fdf0a84) = 0; }
  U(DAT_6fdf0a80) = 0;
  if (U(DAT_6fdf0a8c)) { Ordinal_10046(kF, 0x5c5, 0); U(DAT_6fdf0a8c) = 0; }
  U(DAT_6fdf0a88) = 0;
  if (U(DAT_6fdeff18)) { Ordinal_10046(kF, 0x5cc, 0); U(DAT_6fdeff18) = 0; }
  U(DAT_6fdeff1c) = 0; U(DAT_6fdeff20) = 0;
  if (U(DAT_6fdeff24)) { Ordinal_10046(kF, 0x5d4, 0); U(DAT_6fdeff24) = 0; }
  U(DAT_6fdeff28) = 0; U(DAT_6fdeff2c) = 0;
  int32_t ln = U(DAT_6fde9e20) ? 0x8f6 : 0x8f8;
  if (U(DAT_6fdf0a70)) Ordinal_10046(kF, ln, 0);
  Ordinal_10212(U(DAT_6fdf0a74)); U(DAT_6fdf0a78) = 0;
  if (U(DAT_6fdf0a64)) Ordinal_10046(kF, ln, 0);
  Ordinal_10212(U(DAT_6fdf0a68)); U(DAT_6fdf0a6c) = 0;
}
}
