// D2Common 1.13c CRT data: decimal point byte and the extended-precision power-of-ten tables
// used by the string->_LDBL12 scaler (0x88745). The tables are generated here from arithmetic
// (12-byte entries, stride 12: u16 low word, u64 mantissa, u16 biased exponent); the consumer
// reads only entries 8..27 (offset 0x54 + 0x54*k + r*12 -> 10^(+/-r*8^k)), the rest is zero.
#include <cstdint>
#include <cstring>
extern "C" {
// 1.13c D2Common+0x99964 (VA 6FDE9964): C-locale decimal point.
char d2common_dat_6fde9964[1] = { '.' };
uint8_t d2common_dat_6fdef280[0x160]; // 1.13c D2Common+0x9F280 (VA 6FDEF280): positive powers of ten.
uint8_t d2common_dat_6fdef3e0[0x160]; // 1.13c D2Common+0x9F3E0 (VA 6FDEF3E0): negative powers of ten.
}
namespace {
struct Big { uint32_t w[80]; };
void mul10(Big& a) { uint64_t c = 0; for (auto& x : a.w) { c += (uint64_t)x * 10; x = (uint32_t)c; c >>= 32; } }
void shl(Big& a, int n) {
 for (int i = 79; i >= 0; i--) {
  const int s = i - n / 32; uint64_t v = s >= 0 ? (uint64_t)a.w[s] << (n % 32) : 0;
  if (s >= 1 && n % 32) v |= (uint64_t)a.w[s - 1] >> (32 - n % 32);
  a.w[i] = (uint32_t)v;
 }
}
int cmp(const Big& a, const Big& b) { for (int i = 79; i >= 0; i--) if (a.w[i] != b.w[i]) return a.w[i] < b.w[i] ? -1 : 1; return 0; }
void sub(Big& a, const Big& b) { int64_t c = 0; for (int i = 0; i < 80; i++) { c += (int64_t)a.w[i] - b.w[i]; a.w[i] = (uint32_t)c; c >>= 32; } }
int bl(const Big& a) { for (int i = 79; i >= 0; i--) if (a.w[i]) return i * 32 + 32 - __builtin_clz(a.w[i]); return 0; }
// round-to-nearest B-bit mantissa of num/den (normalised to [2^(B-1),2^B)); *sh = scale so value = q * 2^-sh.
unsigned __int128 mant(const Big& num, const Big& den, int B, int* sh) {
 int s = (B - 1) - (bl(num) - bl(den));
 for (;;) {
  Big n = num, d = den; if (s >= 0) shl(n, s); else shl(d, -s);
  unsigned __int128 q = 0;
  for (int i = B; i >= 0; i--) { Big t = d; shl(t, i); if (cmp(n, t) >= 0) { sub(n, t); q |= (unsigned __int128)1 << i; } }
  if (q >> B) { s--; continue; }
  if (!(q >> (B - 1))) { s++; continue; }
  shl(n, 1); if (cmp(n, d) >= 0) q++;
  *sh = s; return q;
 }
}
void entry(uint8_t* out, int e, bool neg) {
 Big one{}, p{}; one.w[0] = 1; p.w[0] = 1; for (int i = 0; i < e; i++) mul10(p);
 int sh, sh64; const auto& num = neg ? one : p; const auto& den = neg ? p : one;
 const unsigned __int128 q80 = mant(num, den, 80, &sh), q64 = mant(num, den, 64, &sh64);
 const uint16_t lo = (uint16_t)q80, ex = (uint16_t)(79 - sh + 16383); const uint64_t hi = (uint64_t)q64;
 memcpy(out, &lo, 2); memcpy(out + 2, &hi, 8); memcpy(out + 10, &ex, 2);
}
struct Init { Init() {
 for (int j = 8; j < 28; j++) {
  const int k = (j - 1) / 7 - 1, r = (j - 1) % 7 + 1; int e = r; for (int i = 0; i < k; i++) e *= 8;
  entry(d2common_dat_6fdef280 + j * 12, e, false); entry(d2common_dat_6fdef3e0 + j * 12, e, true);
 }
} } init_;
}
