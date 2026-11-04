// Link seams for the z14 number/strans batch (callees not yet ported); never reached by the probes.
#include <cstdint>
uint32_t z14_vtbl36848;
extern const uint32_t z14_tab35458[2] = {0, 0};
extern const double z14_d37af8 = 0, z14_d37b10 = 0, z14_d37b08 = 0;
extern const double z14_tab42918[2] = {0, 0};
long double z14_Pow(int base, int e) {
  long double r = 1, b = (long double)base;
  bool neg = e < 0;
  unsigned n = neg ? (unsigned)(-(long long)e) : (unsigned)e;
  while (n) { if (n & 1) r *= b; b *= b; n >>= 1; }
  return neg ? 1 / r : r;
}
int64_t z14_Sub2C908() { return 0; }
uint32_t z14_Atoi(char*) { return 0; }
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
