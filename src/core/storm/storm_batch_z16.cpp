#include <cstdint>
#include <cstring>
#include <cstddef>
// Seams: retail globals/callees (32-bit links as uint32_t).
extern "C" {
void z16_inconsistency(void);                       // Storm+0x76FA (_inconsistency)
uint32_t* z16_getptd_purecall(void);                // Storm+0x9DE3 (__getptd) -> &ptd->_purecall (unwind nesting counter)
uint32_t z16_nlg_notify1(uint32_t code);            // Storm+0xC031 (__NLG_Notify1) -> code ptr
void z16_call_member_fn1(uint32_t obj, uint32_t fn);// Storm+0x3C505
void z16_unwind_epilog(void);                       // Storm+0x3CDB8
uint32_t z16_find_handler(void* er, void* reg, void* ctx, void* dc, void* fi, uint8_t a, int b, void* c); // FindHandler
void z16_strtod_f(double* out, const uint8_t* s);   // Storm+0x3F1FE
void z16_strtold_i(int* out, const uint8_t* s);     // Storm+0x3F243
extern char z16_decimal_point;                      // DAT_6fc406b4
}

void Storm_Z16_CallSettingFrame(uint32_t, uint32_t, int);
// 1.13c Storm+0x3CBE2: x87 compare ST0 with rint(ST0); no observable effect.
void Storm_Z16_3CBE2(void) {}

// 1.13c Storm+0x3CC30: __aulldiv (unsigned 64/64).
uint64_t Storm_Z16_aulldiv(uint32_t lo, uint32_t hi, uint32_t dlo, uint32_t dhi) {
  return (((uint64_t)hi << 32) | lo) / (((uint64_t)dhi << 32) | dlo);
}

// 1.13c Storm+0x3CD05: ___FrameUnwindToState(frame, unused, funcInfo, target).
void Storm_Z16_FrameUnwindToState(uint8_t* frame, uint32_t, uint8_t* fi, int target) {
  int state = *(int*)(frame + 8);
  ++*z16_getptd_purecall();
  while (state != target) {
    if (state < 0 || *(int*)(fi + 4) <= state) z16_inconsistency();
    int32_t* ent = (int32_t*)(uintptr_t)(*(uint32_t*)(fi + 8) + state * 8);
    state = ent[0];
    if (ent[1] != 0) {
      *(int*)(frame + 8) = state;
      Storm_Z16_CallSettingFrame((uint32_t)ent[1], (uint32_t)(uintptr_t)frame, 0x103);
    }
  }
  z16_unwind_epilog();
  if (state != target) z16_inconsistency();
  *(int*)(frame + 8) = state;
}

// 1.13c Storm+0x3CDD3: ___DestructExceptionObject.
void Storm_Z16_DestructExceptionObject(uint8_t* e) {
  if (!e) return;
  uint32_t fn = *(uint32_t*)(uintptr_t)(*(uint32_t*)(e + 0x1c) + 4);
  if (fn) z16_call_member_fn1(*(uint32_t*)(e + 0x18), fn);
}

// 1.13c Storm+0x3CE18: AdjustPointer(obj=EAX, pmd=ECX).
void* Storm_Z16_AdjustPointer(uint8_t* obj, const int32_t* pmd) {
  uint8_t* r = obj + pmd[0];
  if (pmd[1] >= 0)
    r += *(int32_t*)(uintptr_t)(*(uint32_t*)(obj + pmd[1]) + pmd[2]) + pmd[1];
  return r;
}

// 1.13c Storm+0x3D4A0: ___InternalCxxFrameHandler.
uint32_t Storm_Z16_InternalCxxFrameHandler(uint8_t* er, uint8_t* reg, void* ctx, uint8_t* dc, uint8_t* fi,
                                           int p6, void* p7, uint8_t p8) {
  if ((*(uint32_t*)fi & 0x1fffffff) != 0x19930520) z16_inconsistency();
  if ((er[4] & 0x66) == 0) {
    if (*(int*)(fi + 0xc) != 0) {  // nTryBlocks
      uint32_t ti = *(uint32_t*)(er + 0x1c);
      if (*(int*)er == -0x1f928c9d && *(uint32_t*)(er + 0x14) > 0x19930520 && *(uint32_t*)(uintptr_t)(ti + 8) != 0) {
        typedef uint32_t (*Fn)(void*, void*, void*, void*, void*, int, void*, uint8_t);
        return ((Fn)(uintptr_t) * (uint32_t*)(uintptr_t)(ti + 8))(er, reg, ctx, dc, fi, p6, p7, p8);
      }
      z16_find_handler(er, reg, ctx, dc, fi, p8, p6, p7);
    }
  } else if (*(int*)(fi + 4) != 0 && p6 == 0) {  // maxState
    Storm_Z16_FrameUnwindToState(reg, (uint32_t)(uintptr_t)dc, fi, -1);
  }
  return 1;
}

// 1.13c Storm+0x3D550: __CallSettingFrame@12 (NLG notify around the funclet call).
void Storm_Z16_CallSettingFrame(uint32_t funclet, uint32_t frame, int code) {
  (void)funclet; (void)frame;
  z16_nlg_notify1((uint32_t)code);  // retail calls the returned funclet with EBP=frame; not representable here
  if (code == 0x100) code = 2;
  z16_nlg_notify1((uint32_t)code);
}

// 1.13c Storm+0x3D63D: __fassign(flag, dst, str).
void Storm_Z16_fassign(int flag, char* dst, const char* str) {
  if (flag) { double d; z16_strtod_f(&d, (const uint8_t*)str); memcpy(dst, &d, 8); }
  else { int v; z16_strtold_i(&v, (const uint8_t*)str); memcpy(dst, &v, 4); }
}

// 1.13c Storm+0x3D67B: __shift(EAX=str, EDI=n): shift string (with NUL) by n bytes.
void Storm_Z16_shift(char* s, int n) { if (n) memmove(s + n, s, strlen(s) + 1); }

// 1.13c Storm+0x3D698: __cftoe2(EAX=strflt{sign,decpt,mantissa*}, EBX=buf, ndigits, caps, g_fmt).
void Storm_Z16_cftoe2(const int32_t* sf, char* buf, int ndigits, int caps, char shift) {
  if (shift) Storm_Z16_shift(buf, 1);
  if (sf[0] == 0x2d) *buf++ = '-';
  char* p = buf;
  if (ndigits > 0) { p = buf + 1; buf[0] = buf[1]; buf[1] = z16_decimal_point; }
  char* e = p + ndigits + (shift == 0);
  memcpy(e, "e+000", 6);
  if (caps) *e = 'E';
  if (*(const char*)(uintptr_t)(uint32_t)sf[3] != '0') {
    int x = sf[1] - 1;
    if (x < 0) { x = -x; e[1] = '-'; }
    if (x > 99) { e[2] += (char)(x / 100); x %= 100; }
    if (x > 9) { e[3] += (char)(x / 10); x %= 10; }
    e[4] += (char)x;
  }
}

// 1.13c Storm+0x3D7B6: __cftof2(EAX=strflt, buf, ndigits, g_fmt).
char* Storm_Z16_cftof2(const int32_t* sf, char* buf, size_t nd, char gfmt) {
  int dec = sf[1];
  if (gfmt && (uint32_t)(dec - 1) == nd) {
    buf[(sf[0] == 0x2d) + (uint32_t)(dec - 1)] = '0';
    buf[(sf[0] == 0x2d) + (uint32_t)(dec - 1) + 1] = 0;
  }
  char* p = buf;
  if (sf[0] == 0x2d) *p++ = '-';
  if (sf[1] < 1) { Storm_Z16_shift(p, 1); *p++ = '0'; } else p += sf[1];
  if ((int)nd > 0) {
    Storm_Z16_shift(p, 1);
    *p = z16_decimal_point;
    if (sf[1] < 0) {
      size_t n = (size_t)-sf[1];
      if (gfmt || (int)n <= (int)nd) nd = n;
      Storm_Z16_shift(p + 1, (int)nd);
      memset(p + 1, '0', nd);
    }
  }
  return buf;
}

// 1.13c Storm+0x3DB30: GUESSED (decompile unavailable, ghidra project locked): 25-byte stub; empty body.
void Storm_Z16_3DB30(void) {}
