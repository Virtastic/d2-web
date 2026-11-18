// Storm batch 41: registry-value readers over the shared reader 0x3BDD0 and the SEVT state wrappers.
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <initializer_list>
extern int Storm_Sub3BDD0(const uint32_t* key, const char* name, uint8_t flags, uint32_t* type, uint8_t* buf, uint32_t cap);
extern int Storm_Sub36120(uint32_t a, uint32_t b);
extern int Storm_Sub36680(uint32_t a, uint32_t b);
static uint32_t g_lastError41;
static bool bad(const uint32_t* k, const char* n) { return !k || !(char)*k || !n || !*n; }

// 1.13c Storm+0x34110
int Storm_Sub34110(uint32_t x) { for (uint32_t c : {0x534d5348u, 0x534d5349u, 0x534d534au, 0x534d5347u}) Storm_Sub36120(c, x); return 1; }
// 1.13c Storm+0x345D0
int Storm_Sub345D0(uint32_t x) { for (uint32_t c : {0x534d5348u, 0x534d5349u, 0x534d534au, 0x534d5347u}) Storm_Sub36680(c, x); return 1; }
// 1.13c Storm+0x3C370
int Storm_Sub3C370(const uint32_t* k, const char* n, uint8_t f, uint8_t* buf, uint32_t cap, uint32_t* /*unused*/) {
  if (bad(k, n)) { g_lastError41 = 0x57; return 0; }
  uint32_t t; return Storm_Sub3BDD0(k, n, f, &t, buf, cap);
}
// 1.13c Storm+0x3C2B0
int Storm_Sub3C2B0(const uint32_t* k, const char* n, uint8_t f, uint8_t* buf, uint32_t cap) {
  if (bad(k, n) || !buf || !cap) { g_lastError41 = 0x57; return 0; }
  uint32_t t;
  if (!Storm_Sub3BDD0(k, n, f, &t, buf, cap)) return 0;
  if (t == 1) buf[cap - 1] = 0;  // ponytail: retail clamps to min(cap-1, size-out); size-out unrecovered
  else if (t == 4) snprintf((char*)buf, cap, "%u", *(uint32_t*)buf);
  return 1;
}
// 1.13c Storm+0x3C1D0
int Storm_Sub3C1D0(const uint32_t* k, const char* n, uint8_t f, unsigned long* out) {
  if (bad(k, n) || !out) { g_lastError41 = 0x57; return 0; }
  uint32_t buf[64] = {0}, t;
  if (!Storm_Sub3BDD0(k, n, f, &t, (uint8_t*)buf, 0x100)) return 0;
  if (t == 1) *out = strtoul((char*)buf, nullptr, 0); else if (t == 4) *out = buf[0];
  return 1;
}
