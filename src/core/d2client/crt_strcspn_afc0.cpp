// 1.13c D2Client+0xAFC0 -- CRT _strcspn: length of the leading run of s holding no byte of control.
// Same 256-bit membership set as the CRT: bit (c & 7) of byte (c >> 3).
#include <cstddef>
#include <cstdint>

size_t d2_client_strcspn_afc0(const char* s, const char* control) {
  uint8_t set[32] = {};
  for (const uint8_t* c = reinterpret_cast<const uint8_t*>(control); *c; ++c)
    set[*c >> 3] = static_cast<uint8_t>(set[*c >> 3] | (1u << (*c & 7)));
  size_t n = 0;
  for (const uint8_t* p = reinterpret_cast<const uint8_t*>(s); *p; ++p, ++n)
    if ((set[*p >> 3] >> (*p & 7)) & 1) return n;
  return n;
}
