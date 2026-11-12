// Link seams for the z14 number/strans batch (callees not yet ported). The numeric ones below are reached by the string-to-double probes.
#include <cstdint>
#include "storm_batch_34d60.h"
uint32_t z14_vtbl36848;
// Last-error global and the strchr/strrchr pair for the z14 string helpers (their only definition; the probes reset and read it).
uint32_t z14_lastErr;
void z14_SetLastError(uint32_t) {}
// Storm+0x3B280 / 0x3B2D0: the ported strrchr / strchr, their invalid-argument error landing in the same last-error global.
static Storm34d60Env& z14_strEnv() {
  static Storm34d60Env e{};
  e.error = [](uint32_t c) { z14_lastErr = c; };
  return e;
}
char* z14_Sub3B280(char* s, char c) { return Storm_Sub3B280(z14_strEnv(), s, c); }
char* z14_Sub3B2D0(char* s, char c) { return Storm_Sub3B2D0(z14_strEnv(), s, c); }
extern const uint32_t z14_tab35458[2] = {0, 0};
// The string-to-double helpers' constants: 0.0, 0.0 and 2^32 (the unsigned-to-double correction), and the fraction-digit table
// (21 places x 10 digits). Retail fills that table at start-up from an integer power whose negative exponents divide to 0, so every
// entry is 0.0.
extern const double z14_d37af8 = 0, z14_d37b10 = 0, z14_d37b08 = 4294967296.0;
extern const double z14_tab42918[210] = {};
// Storm+0x1000: integer power (ECX base, EDX exponent): 32-bit wrapping product; a negative exponent gives 1 / product by integer
// division, loaded with fild.
long double z14_Pow(int base, int e) {
  uint32_t n = e < 0 ? 0u - (uint32_t)e : (uint32_t)e, r = 1, b = (uint32_t)base;
  while (true) { if (n & 1) r *= b; n >>= 1; if (!n) break; b *= b; }
  if (e >= 0) return (long double)(int32_t)r;
  return (int32_t)r == 0 ? 0 : (long double)(1 / (int32_t)r);
}
int64_t z14_Sub2C908() { return 0; }
uint32_t Storm_Sub3AD50_z14(const char* s);
uint32_t z14_Atoi(char* s) { return Storm_Sub3AD50_z14(s); }  // Storm+0x3AD50
void z14_Strupr(char*) {}
void z14_Strlwr(char*) {}
void z14_Sub01120(uint32_t*) {}
void z14_Sub011A0(uint32_t*) {}
void z14_Sub0B960(void*, const char*, int) {}
uint8_t* z14_Sub0C580(uint32_t, const char*, int, int) { return nullptr; }
int z14_Sub23EA0(uint32_t*, uint32_t*, uint8_t*, uint32_t, uint32_t*, uint32_t*, uint32_t*) { return 0; }
void z14_Sub27C10(uint8_t*, uint32_t, uint32_t, int, uint32_t*, void*, uint32_t*) {}
int z14_Sub291E0(uint32_t*, const void*, uint32_t*) { return 0; }
void z14_Sub2B080(void*, const void*, uint32_t) {}
uint32_t z14_vsnprintf(char*, uint32_t, const char*, void*) { return 0; }
uint32_t z14_vsprintf(char*, const char*, void*) { return 0; }
