// 1.13c D2Client+0xCACC4  (_ftol: x87 ST0 -> int64, truncate toward zero)
// Original does fistp (round-to-nearest) then corrects by the sign of (x - r).
// Unverified: exact x87 control-word dependence; result modelled as truncation.
#include <stdint.h>
int64_t d2client_ftol_cacc4(long double x) {
  int64_t r = (int64_t)(x < 0 ? x - 0.5L : x + 0.5L);  // nearest, as fistp
  if (r == 0) return 0;
  long double d = x - (long double)r;
  if (r < 0) return r + (d > 0);
  return r - (d < 0);
}
