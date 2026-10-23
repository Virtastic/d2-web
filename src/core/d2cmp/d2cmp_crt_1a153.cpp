// 1.13c D2CMP+0x1A153 -- CRT __strgtold12: decimal string -> 12-byte x87 extended value.
// Scanner is a line-for-line port of the retail state machine. ponytail: value is formed by
// host strtold then repacked to x87 layout (retail multiplies by 10^n tables, may differ in
// the last guard bits); upgrade by porting 0x1A9D6/0x1AC0C + the power tables.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>

static inline bool dig(int c) { return c >= '0' && c <= '9'; }

// out: 12 bytes = {u16 guard, u64 mantissa(explicit int bit), u16 sign|exp}.
extern "C" int d2cmp_crt_1a153(uint16_t* out, const uint8_t** endp, const uint8_t* s, int p4, int p5, int p6, int p7) {
  (void)p4;
  int st = 0, sign = 1, ret = 0;
  uint16_t sb = 0;
  uint32_t n = 0;
  int dexp = 0, eexp = 0;
  bool seen = false, fracDig = false, expDig = false;
  uint8_t d[32] = {0};
  uint32_t pi = 0;
  const uint8_t* save = s;
  const uint8_t* q;
  uint8_t c;
  while ((c = *s) == ' ' || c == 9 || c == 10 || c == 13) s++;
  for (;;) {
    c = *s; q = s + 1;
    switch (st) {
    case 0:
      if (c > '0' && c <= '9') { st = 3; continue; }
      if (c == '.') { st = 5; s = q; continue; }
      st = 2;
      if (c == '+') { sb = 0; s = q; continue; }
      if (c == '-') { sb = 0x8000; s = q; continue; }
      goto c0;
    case 1:
      st = 1; seen = true;
      if (c > '0' && c <= '9') { st = 3; continue; }
      if (c == '.') goto dot;
      if (c == '+' || c == '-') goto sgn;
      s = q;
      if (c != '0') goto l24b;
      continue;
    case 2:
      if (c > '0' && c <= '9') { st = 3; continue; }
      s = save;
      if (c == '.') { st = 5; s = q; continue; }
      goto c0;
    case 3:
      while (dig(c)) { if (n < 0x19) { n++; d[pi++] = c - '0'; } else dexp++; c = *q++; }
      if (c != '.') goto l321;
    dot:
      seen = true; st = 4; s = q; continue;
    case 4:
      fracDig = true;
      if (n == 0) while (c == '0') { dexp--; c = *q++; }
      while (dig(c)) { if (n < 0x19) { n++; d[pi++] = c - '0'; dexp--; } c = *q++; }
    l321:
      if (c == '+' || c == '-') {
      sgn:
        seen = true; st = 0xb; s = q - 1;
      } else {
      l24b:
        seen = true;
        if (c < 'D' || (c > 'E' && (c < 'd' || c > 'e'))) { s = q - 1; goto fin; }
        st = 6; s = q;
      }
      continue;
    case 5:
      fracDig = true;
      st = 4;
      if (dig(c)) { s = q - 1; continue; }
      s = save; goto fin;
    case 6:
      save = s - 1;
      if (c < '1' || c > '9') {
        if (c == '+') goto plus;
        if (c == '-') goto minus;
        s = save;
        if (c == '0') goto z8;
        goto fin;
      }
    n9: st = 9; s = q - 1; goto e9;
    case 7:
      if (c > '0' && c <= '9') goto n9;
      s = save;
      if (c != '0') goto fin;
    z8: st = 8; s = q; continue;
    case 8:
      expDig = true;
      while (c == '0') c = *q++;
      if (c > '0' && c <= '9') goto n9;
      s = q - 1; goto fin;
    case 9:
      expDig = true; eexp = 0;
    e9:
      while (dig(c)) {
        eexp = (char)c - 0x30 + eexp * 10;
        if (eexp > 0x1450) { eexp = 0x1451; break; }
        c = *q++;
      }
      while (dig(c)) c = *q++;
      s = q - 1; goto fin;
    case 0xb:
      if (p7 != 0) {
        save = s;
        if (c == '+') { plus: st = 7; s = q; }
        else { if (c != '-') goto fin; minus: sign = -1; st = 7; s = q; }
        continue;
      }
      goto fin;
    default: goto fin;
    }
  c0:
    if (c != '0') goto fin;
    st = 1; s = q;
  }
fin:
  *endp = s;
  if (!seen) { ret = 4; goto zero; }
  if (n > 0x18) {
    if (d[23] > 4) d[23]++;
    pi--; dexp++; n = 0x18;
  }
  if (n == 0) goto zero;
  while (d[--pi] == 0) { n--; dexp++; }
  {
    if (sign < 0) eexp = -eexp;
    uint32_t e = (uint32_t)(eexp + dexp);
    if (!expDig) e += p5;
    if (!fracDig) e -= p6;
    if ((int)e > 0x1450) { memset(out, 0, 12); out[5] = 0x7fff | sb; *(uint32_t*)(out + 3) = 0x80000000u; return 2; }
    if ((int)e > -0x1451) {
      char buf[64]; int k = 0;
      for (uint32_t i = 0; i < n; i++) buf[k++] = '0' + d[i];
      snprintf(buf + k, 20, "e%d", (int)e);
      long double v = strtold(buf, nullptr);
      uint64_t w[2]; memcpy(w, &v, 16);  // wasm64: IEEE quad
      uint16_t ex = (w[1] >> 48) & 0x7fff;
      uint64_t m = (1ull << 63) | ((w[1] & 0xffffffffffffull) << 15) | (w[0] >> 49);
      memset(out, 0, 12); memcpy(out + 1, &m, 8); out[5] = ex | sb;
      return 0;
    }
    ret = 1;
  }
zero:
  memset(out, 0, 12);
  out[5] = sb;
  return ret;
}
