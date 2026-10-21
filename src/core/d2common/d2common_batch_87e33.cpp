// D2Common 1.13c batch: CRT string -> _LDBL12 parser (__strgtold12), state machine over *str.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" {
// Unwired callees (weak): _isdigit, __multtenpow12-style scaler +0x88745, digits->ld12 +0x87D4B (wired).
__attribute__((weak)) int d2common_crt_isdigit_90f0(int c);
__attribute__((weak)) char d2common_dat_6fde9964[1];
__attribute__((weak)) void D2Common_88745(uint32_t* ld12, int32_t exp10, int32_t p4);
void D2Common_87D4B(const char* digits, int32_t count, uint32_t* out);

// 1.13c D2Common+0x87E33: ld12 = 12-byte result {u16 @0, u32 @2, u32 @6, u16 @10 sign|exp}; *endp gets
// the end pointer (truncated to 32 bits as retail); returns status (0 ok, 1 underflow, 2 overflow, 4 none).
EMSCRIPTEN_KEEPALIVE intptr_t D2Common_87E33(uint16_t* ld12, int32_t* endp, const uint8_t* s, int32_t p4,
                                             int32_t p5, int32_t p6, int32_t p7) {
 const uint8_t dp = (uint8_t)d2common_dat_6fde9964[0];
 int32_t st = 0, sign = 1, exp = 0, drop = 0, status = 0;
 uint32_t cnt = 0, uVar5;
 uint16_t sgn = 0;
 bool b1 = false, b3 = false, b2 = false;
 char buf[25];
 char* pc = buf;
 const uint8_t* save = s;
 const uint8_t* q;
 uint8_t c;
 uint32_t t[3];
 uint32_t w2 = 0, w6 = 0;
 uint16_t w0 = 0, w10 = 0;
 while ((c = *s) == 0x20 || c == 9 || c == 10 || c == 13) ++s;
top:
 c = *s;
 q = s + 1;
 switch (st) {
 case 0:
  if (c > '0' && c < ':') goto L7eb3;
  if (c == dp) goto L7ec2;
  st = 2;
  if (c == '+') { sgn = 0; s = q; goto top; }
  if (c == '-') { sgn = 0x8000; s = q; goto top; }
  break;
 case 1:
  st = 1; b1 = true;
  if (c > '0' && c < ':') goto L7eb3;
  if (c == dp) goto L7f11;
  if (c == '+' || c == '-') goto L7f52;
  s = q;
  if (c != '0') goto L7f2b;
  goto top;
 case 2:
  if (c > '0' && c < ':') goto L7eb3;
  s = save;
  if (c == dp) goto L7ec2;
  break;
 case 3:
  while (d2common_crt_isdigit_90f0(c) != 0) {
   if (cnt < 0x19) { ++cnt; *pc++ = (char)(c - 0x30); } else ++drop;
   c = *q++;
  }
  if (c != dp) goto L8001;
  goto L7f11;
 case 4:
  b3 = true;
  if (cnt == 0) while (c == 0x30) { --drop; c = *q++; }
  while (d2common_crt_isdigit_90f0(c) != 0) {
   if (cnt < 0x19) { ++cnt; *pc++ = (char)(c - 0x30); --drop; }
   c = *q++;
  }
  goto L8001;
 case 5:
  b3 = true;
  s = save;
  if (d2common_crt_isdigit_90f0(c) != 0) { st = 4; goto L80d2; }
  goto L808f;
 case 6:
  save = s - 1;
  if ((char)c < '1' || '9' < (char)c) {
   if (c == '+') goto L8107;
   if (c == '-') goto L80fb;
   s = save;
   if (c == '0') goto L8069;
   goto L808f;
  }
  goto L80d0;
 case 7:
  if (c > '0' && c < ':') goto L80d0;
  s = save;
  if (c != '0') goto L808f;
  goto L8069;
 case 8:
  b2 = true;
  while (c == 0x30) c = *q++;
  if (c > '0' && c < ':') goto L80d0;
  goto L808b;
 case 9:
  b2 = true;
  exp = 0;
  goto L814e;
 default:
  goto Ldef;
 case 0xb:
  if (p7 != 0) {
   save = s;
   if (c == '+') goto L8107;
   if (c != '-') goto L808f;
   goto L80fb;
  }
  st = 10;
  q = s;
 Ldef:
  s = q;
  if (st != 10) goto top;
  goto L808f;
 }
 if (c != 0x30) goto L808f;
 st = 1; s = q;
 goto top;
L7eb3: st = 3; goto L80d2;
L7ec2: st = 5; s = q; goto top;
L7f11: b1 = true; st = 4; s = q; goto top;
L8001:
 if (c == '+' || c == '-') {
L7f52: b1 = true; st = 0xb; s = q - 1;
 } else {
L7f2b: b1 = true;
  if ((char)c < 'D' || ((char)c > 'E' && ((char)c < 'd' || (char)c > 'e'))) goto L808b;
  st = 6; s = q;
 }
 goto top;
L8107: st = 7; s = q; goto top;
L80fb: sign = -1; st = 7; s = q; goto top;
L8069: st = 8; s = q; goto top;
L80d0: st = 9;
L80d2: s = q - 1; goto top;
L814e:
 if (d2common_crt_isdigit_90f0(c) == 0) goto L8171;
 exp = (int8_t)c - 0x30 + exp * 10;
 if (0x1450 < exp) { exp = 0x1451; goto L8171; }
 c = *q++;
 goto L814e;
L8171:
 while (d2common_crt_isdigit_90f0(c) != 0) c = *q++;
 s = q - 1;
 goto L808f;
L808b: s = q - 1;
L808f:
 *endp = (int32_t)(intptr_t)s;
 if (b1) {
  if (cnt > 0x18) {
   if (buf[23] > 4) buf[23]++;
   --pc; ++drop; cnt = 0x18;
  }
  if (cnt != 0) {
   while (*--pc == 0) { --cnt; ++drop; }
   D2Common_87D4B(buf, (int32_t)cnt, t);
   if (sign < 0) exp = -exp;
   uVar5 = (uint32_t)(exp + drop);
   if (!b2) uVar5 += (uint32_t)p5;
   if (!b3) uVar5 -= (uint32_t)p6;
   if (0x1450 < (int32_t)uVar5) {
    w0 = 0; w2 = 0; w6 = 0x80000000; w10 = 0x7fff; status = 2;
    goto out;
   }
   if (-0x1451 < (int32_t)uVar5) {
    D2Common_88745(t, (int32_t)uVar5, p4);
    uint8_t* b = (uint8_t*)t;
    memcpy(&w0, b, 2); memcpy(&w2, b + 2, 4); memcpy(&w6, b + 6, 4); memcpy(&w10, b + 10, 2);
    goto out;
   }
   status = 1;
  }
 } else status = 4;
 w0 = 0; w10 = 0; w6 = 0; w2 = 0;
out:
 ld12[0] = w0;
 memcpy(ld12 + 1, &w2, 4);
 ld12[5] = (uint16_t)(w10 | sgn);
 memcpy(ld12 + 3, &w6, 4);
 return status;
}
}
