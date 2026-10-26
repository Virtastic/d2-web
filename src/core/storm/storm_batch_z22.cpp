#include <cstdint>
#include <cstring>
// Seams: retail globals/callees (MSVCRT 2003 fp-exception helpers linked into Storm).
extern "C" {
double z22_set_exp(double v, int e);            // Storm __set_exp (callee of 0x407C9)
uint32_t z22_statfp(void);                      // Storm __statfp
void z22_clrfp(void);                           // Storm __clrfp
void z22_RaiseException(uint32_t code, uint32_t flags, uint32_t n, uint32_t* args); // kernel32
int32_t* z22_errno_ptr(void);                   // Storm+0xA062
void z22_fp_nop(void);                          // Storm+0x41652 (called by __handle_exc)
extern const double z22_dat_37b10;              // 0.0 constant at Storm+0x37B10
extern const double z22_dat_42070;              // Storm+0x42070 (max-finite / inf pair)
extern const double z22_dat_42080;              // Storm+0x42080
void z22_unlink_4b80(void* e);                  // Storm+0x4B80
void z22_dtor_4c20(void);                       // Storm+0x4C20
}
static uint64_t z22_bits(double d) { uint64_t u; memcpy(&u, &d, 8); return u; }
static double z22_dbl(uint64_t u) { double d; memcpy(&d, &u, 8); return d; }

// 1.13c Storm+0x4076E: __sptype, classify a double (lo, hi words): 0 normal, 1 +inf, 2 -inf, 3 QNaN, 4 SNaN.
int z22_sptype(int lo, uint32_t hi) {
  if (hi == 0x7ff00000) { if (lo == 0) return 1; }
  else if (hi == 0xfff00000 && lo == 0) return 2;
  if (((hi >> 16) & 0x7ff8) == 0x7ff8) return 3;
  if (((hi >> 16) & 0x7ff8) != 0x7ff0 || ((hi & 0x7ffff) == 0 && lo == 0)) return 0;
  return 4;
}

// 1.13c Storm+0x407C9: __decomp, split a double into mantissa (via __set_exp) and exponent in *e.
double z22_decomp(uint32_t lo, uint32_t hi, int* e) {
  uint64_t v = ((uint64_t)hi << 32) | lo;
  double r; int ex;
  if (z22_dbl(v) == z22_dat_37b10) { r = 0; ex = 0; }
  else if ((hi & 0x7ff00000) == 0 && ((hi & 0xfffff) != 0 || lo != 0)) {  // denormal: normalise
    bool neg = z22_dbl(v) < z22_dat_37b10;
    while (!((hi >> 16) & 0x10)) { hi = (hi << 1) | ((lo & 0x80000000) ? 1 : 0); lo <<= 1; }
    if (neg) hi |= 0x80000000;
    v = (((uint64_t)hi << 32) | lo) & 0xffefffffffffffffULL;
    r = z22_set_exp(z22_dbl(v), 0);
    ex = 0; // retail takes extraout_EDX from __set_exp; seam does not expose it (guessed)
  } else {
    r = z22_set_exp(z22_dbl(v), 0);
    ex = (int)(((hi >> 16) & 0x7ff0) >> 4) - 0x3fe;
  }
  *e = ex;
  return r;
}

// 1.13c Storm+0x41126: __raise_exc, build exception record from FP status/control, raise, write control word back.
void z22_raise_exc(uint32_t* p, uint32_t* cw, uint32_t code, int prec, uint64_t* a, uint64_t* res) {
  uint32_t flags = code;
  p[1] = p[2] = p[3] = 0;
  if (flags & 0x10) { p[1] |= 1;    code = 0xc000008f; }
  if (flags & 2)    { p[1] |= 2;    code = 0xc0000093; }
  if (flags & 1)    { p[1] |= 4;    code = 0xc0000091; }
  if (flags & 4)    { p[1] |= 8;    code = 0xc000008e; }
  if (flags & 8)    { p[1] |= 0x10; code = 0xc0000090; }
  p[2] ^= (~(*cw << 4) ^ p[2]) & 0x10;
  p[2] ^= (~(*cw << 1) ^ p[2]) & 8;
  p[2] ^= (~(*cw >> 1) ^ p[2]) & 4;
  p[2] ^= (~(*cw >> 3) ^ p[2]) & 2;
  p[2] ^= (~(*cw >> 5) ^ p[2]) & 1;
  uint32_t st = z22_statfp();
  if (st & 1)    p[3] |= 0x10;
  if (st & 4)    p[3] |= 8;
  if (st & 8)    p[3] |= 4;
  if (st & 0x10) p[3] |= 2;
  if (st & 0x20) p[3] |= 1;
  uint32_t rc = *cw & 0xc00;
  if (rc == 0) p[0] &= ~3u;
  else if (rc == 0x400) p[0] = (p[0] & ~2u) | 1;
  else if (rc == 0x800) p[0] = (p[0] & ~1u) | 2;
  else if (rc == 0xc00) p[0] |= 3;
  uint32_t pc = *cw & 0x300;
  if (pc == 0) p[0] = (p[0] & 0xffffffebu) | 8;
  else if (pc == 0x200) p[0] = (p[0] & 0xffffffe7u) | 4;
  else if (pc == 0x300) p[0] &= 0xffffffe3u;
  p[0] ^= (((uint32_t)prec << 5) ^ p[0]) & 0x1ffe0;
  p[8] |= 1; p[8] = (p[8] & 0xffffffe3u) | 2;
  memcpy(p + 4, a, 8);
  p[0x18] |= 1; p[0x18] = (p[0x18] & 0xffffffe3u) | 2;
  memcpy(p + 0x14, res, 8);
  z22_clrfp();
  z22_RaiseException(code, 0, 1, (uint32_t*)&p);  // retail passes &param_1 (32-bit pointer to p)
  if (p[2] & 0x10) *cw &= ~1u;
  if (p[2] & 8)    *cw &= ~4u;
  if (p[2] & 4)    *cw &= ~8u;
  if (p[2] & 2)    *cw &= ~0x10u;
  if (p[2] & 1)    *cw &= ~0x20u;
  uint32_t r = p[0] & 3;
  if (r == 0) *cw &= 0xfffff3ffu;
  else if (r == 1) *cw = (*cw & 0xfffff7ffu) | 0x400;
  else if (r == 2) *cw = (*cw & 0xfffffbffu) | 0x800;
  else *cw |= 0xc00;
  uint32_t q = (p[0] >> 2) & 7;
  if (q == 0) *cw = (*cw & 0xfffff3ffu) | 0x300;
  else if (q == 1) *cw = (*cw & 0xfffff3ffu) | 0x200;
  else if (q == 2) *cw &= 0xfffff3ffu;
  memcpy(res, p + 0x14, 8);
}

// 1.13c Storm+0x413CB: __handle_exc, default FP exception results; true if no unmasked flag remains.
bool z22_handle_exc(uint32_t sw, double* v, uint32_t cw) {
  uint32_t u = sw & 0x1f;
  bool all = true;
  if ((sw & 8) && (cw & 1)) { z22_fp_nop(); u = sw & 0x17; }
  else if ((sw & 4) && (cw & 4)) { z22_fp_nop(); u = sw & 0x1b; }
  else if ((sw & 1) && (cw & 8)) {
    z22_fp_nop();
    uint32_t rc = cw & 0xc00; double d = z22_dat_42070; bool set = true;
    if (rc == 0) { if (*v <= z22_dat_37b10) d = -z22_dat_42070; }
    else if (rc == 0x400) { d = z22_dat_42080; if (*v <= z22_dat_37b10) d = -z22_dat_42070; }
    else if (rc == 0x800) { if (*v <= z22_dat_37b10) d = -z22_dat_42080; }
    else if (rc == 0xc00) { d = z22_dat_42080; if (*v <= z22_dat_37b10) d = -z22_dat_42080; }
    else set = false;
    if (set) *v = d;
    u = sw & 0x1e;
  } else if ((sw & 2) && (cw & 0x10)) {
    bool sticky = (sw & 0x10) != 0;
    if (*v != z22_dat_37b10) {
      int e; double m = z22_decomp((uint32_t)z22_bits(*v), (uint32_t)(z22_bits(*v) >> 32), &e);
      e -= 0x600;
      double out;
      if (e < -0x432) { out = m * z22_dat_37b10; sticky = all; }
      else {
        uint64_t b = (z22_bits(m) & 0xfffffffffffffULL) | 0x10000000000000ULL;
        if (e < -0x3fd) {
          for (int n = -0x3fd - e; n; --n) { if ((b & 1) && !sticky) sticky = all; b >>= 1; }
        }
        out = z22_dbl(b);
        if (m < z22_dat_37b10) out = -out;
      }
      *v = out;
      all = sticky;
    }
    if (all) z22_fp_nop();
    u &= ~2u;
  }
  if ((sw & 0x10) && (cw & 0x20)) { z22_fp_nop(); u &= ~0x10u; }
  return u == 0;
}

// 1.13c Storm+0x415EF: set errno from signal-ish code (1 -> 0x21 EDOM, 2..3 -> 0x22 ERANGE).
void z22_set_errno_415ef(int c) {
  if (c == 1) *z22_errno_ptr() = 0x21;
  else if (c > 1 && c < 4) *z22_errno_ptr() = 0x22;
}

// 1.13c Storm+0x4162E: __ctrlfp, returns the x87 control word (seam: no x87 in wasm).
extern "C" int z22_fpu_cw(void);
int z22_ctrlfp(void) { return (int16_t)z22_fpu_cw(); }

// 1.13c Storm+0x41652: empty body in decompile (x87 inline-asm stub, guessed empty).
void z22_41652(void) {}

// SEH unwind funclets: frame pointer slots passed explicitly (retail reads EBP-0x10 / EBP+4).
// 1.13c Storm+0x41990
void z22_unwind_41990(uint32_t* ebp) { z22_unlink_4b80((void*)(uintptr_t)ebp[-4]); }
// 1.13c Storm+0x419B8
void z22_unwind_419b8(uint32_t* ebp) { z22_unlink_4b80((void*)(uintptr_t)ebp[-4]); }
// 1.13c Storm+0x419D0
void z22_unwind_419d0() { z22_dtor_4c20(); }
// 1.13c Storm+0x419DB
void z22_unwind_419db() { z22_dtor_4c20(); }
// 1.13c Storm+0x419F0
void z22_unwind_419f0(uint32_t* ebp) { z22_unlink_4b80((void*)(uintptr_t)ebp[1]); }
