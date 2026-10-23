// 1.13c D2CMP+0x19B20 -- MSVC CRT _atodbl: parse a decimal string into a double (out param).
// Retail: FUN_6fe2a153 (string -> 12-byte long double, end ptr discarded) then __ld12tod. Host strtod
// does the same string->double conversion.
#include <cstdlib>
void d2cmp_crt_19b20(double* out, const char* str) { *out = strtod(str, nullptr); }
