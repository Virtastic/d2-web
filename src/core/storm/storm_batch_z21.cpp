#include <cstdint>
#include <cstring>
// Seams for retail globals / callees (MSVC CRT statics inside Storm.dll).
extern "C" {
extern double z21_dat_3b558, z21_dat_37b10, z21_dat_37b78, z21_dat_42070, z21_dat_42078, z21_dat_42090; // doubles at 0x3B558/0x37B10/0x37B78/0x42070/0x42078/0x42090 (guessed values: read at link)
extern uint32_t z21_cxx_filter_ptr;                       // Storm+0x468F8 (user unhandled-exception filter)
extern int (*z21_math_err_hook)(void*);                   // PTR at Storm+0x41F00
extern uint32_t z21_cookie;                               // Storm+0x406BC
extern int32_t z21_dat_42098;                             // Storm+0x42098
extern const int32_t z21_ld12_dbl[6], z21_ld12_flt[6];    // Storm+0x41ED0 / 0x41EE8
void z21_terminate();
int z21_validate_execute(uint32_t fn);                    // Storm+0x9DAE
int32_t* z21_errno();                                     // Storm+0xA062
int z21_sptype(uint32_t lo, uint32_t hi);                 // Storm+0x4076E
int z21_IsZeroMan(void*); void z21_CopyMan(void*, void*); int z21_RoundMan(void*, int); void z21_ShrMan(void*, int); // Storm+0x3EF59/EFE6/EFFF/EFCB (guessed order)
bool z21_handle_exc(uint32_t, double*, uint32_t); void z21_raise_exc(void*, uint32_t*, uint32_t, int, double*, double*);
void z21_ctrlfp(); int z21_31123(); void z21_315ef(int);  // Storm+0x413CB.. (guessed names)
}

// 1.13c Storm+0x40697: __frnd, round to integral (fistp-style; default FPU mode = nearest-even)
double z21_frnd(double x) { return __builtin_nearbyint(x); }

// 1.13c Storm+0x406A8: __fpclass
int z21_fpclass(double x) {
  uint64_t b; memcpy(&b, &x, 8);
  uint32_t lo = (uint32_t)b, hi = (uint32_t)(b >> 32); bool neg = (b >> 63) != 0;
  if (((b >> 48) & 0x7ff0) == 0x7ff0) {
    int t = z21_sptype(lo, hi);
    if (t == 1) return 0x200;
    if (t == 2) return 4;
    if (t == 3) return 2;
    return 1;
  }
  if ((b & 0x7ff0000000000000ull) == 0 && ((b & 0xfffff00000000ull) != 0 || lo != 0)) return (neg ? -0x70 : 0) + 0x80;
  if (x == z21_dat_37b10) return (neg ? -0x20 : 0) + 0x40;
  return (neg ? -0xf8 : 0) + 0x100;
}

// 1.13c Storm+0x3EC4C: __d_inttype (0 non-integer/special, 1 integer, 2 even-multiple)
int z21_d_inttype(double x) {
  if ((z21_fpclass(x) & 0x90) == 0 && z21_frnd(x) == x) {
    double d = x / z21_dat_3b558;
    return d == z21_frnd(d) ? 2 : 1;
  }
  return 0;
}

// 1.13c Storm+0x3ECBA: __powhlp (special-value pow; a = x, b = y, out -> *r; returns 1 on domain error)
uint32_t z21_powhlp(double a, double b, double* r) {
  uint64_t ab, bb; memcpy(&ab, &a, 8); memcpy(&bb, &b, 8);
  int32_t ahi = (int32_t)(ab >> 32), alo = (int32_t)ab, bhi = (int32_t)(bb >> 32), blo = (int32_t)bb;
  (void)blo;
  double x = a, res = z21_dat_42070; uint32_t rc = 0;
  if (x < z21_dat_37b10) x = -x;
  // retail tests the high dword of the *second* operand against +inf and the first against +-inf
  if ((uint32_t)bhi == 0x7ff00000u && blo == 0) {
    if (z21_dat_37b78 < x) { *r = res; return 0; }
    if (!(z21_dat_37b78 <= x)) { *r = 0.0; return 0; }
    *r = z21_dat_42078; return 1;
  }
  if (b == -__builtin_inf()) {
    if (z21_dat_37b78 < x) { *r = 0.0; return 0; }
    if (x < z21_dat_37b78) { *r = res; return 0; }
    *r = z21_dat_42078; return 1;
  }
  if (ahi != 0x7ff00000) {
    if (ahi != -0x100000 || alo != 0) return 0;
    int it = z21_d_inttype(b);
    if (b <= z21_dat_37b10) res = (z21_dat_37b10 <= b) ? 1.0 : (it == 1 ? z21_dat_42090 : 0.0);
    else if (it == 1) res = -z21_dat_42070;
    *r = res; return 0;
  }
  if (alo != 0) return 0;
  if (z21_dat_37b10 < b) { *r = res; return rc; }
  *r = (z21_dat_37b10 <= b) ? 1.0 : 0.0;
  return rc;
}

// 1.13c Storm+0x3EE1C: __CxxUnhandledExceptionFilter (EXCEPTION_POINTERS: [0]=record32; record: code@0, nparams@0x18, info@0x1C)
long z21_CxxUnhandledExceptionFilter(uint32_t* ep) {
  uint32_t* rec = (uint32_t*)(uintptr_t)ep[0];
  if (rec[0] == 0xe06d7363u && rec[6] == 3 && (rec[7] == 0x19930520u || rec[7] == 0x19930521u)) z21_terminate();
  if (z21_cxx_filter_ptr && z21_validate_execute(z21_cxx_filter_ptr) != 0)
    return ((long(*)(void*))(uintptr_t)z21_cxx_filter_ptr)(ep);
  return 0;
}

// 1.13c Storm+0x3F07A: __ld12cvt (guessed: helper arg order/ptr widths)
int z21_ld12cvt(uint16_t* p, uint32_t* out, const int32_t* f) {
  uint16_t se = p[5];
  uint32_t m[3]; int32_t save[3];
  m[0] = *(uint32_t*)(p + 3); m[1] = *(uint32_t*)(p + 1); int32_t top = (int32_t)((uint32_t)p[0] << 16);
  uint32_t e = se & 0x7fff; int32_t ex = (int32_t)e - 0x3fff; int rc;
  if (ex == -0x3fff) {
    ex = 0;
    if (z21_IsZeroMan(&m[0])) { rc = 0; goto done; }
    m[0] = m[1] = 0;
  } else {
    z21_CopyMan(save, &m[0]);
    if (z21_RoundMan(&m[0], f[2])) ex = (int32_t)e - 0x3ffe;
    if (ex < f[1] - f[2]) m[0] = m[1] = 0;
    else {
      if (f[1] < ex) {
        if (f[0] <= ex) { m[1] = 0; top = 0; m[0] = 0x80000000u; z21_ShrMan(&m[0], f[3]); ex = f[5] + f[0]; rc = 1; goto done; }
        m[0] &= 0x7fffffffu; ex = f[5] + ex; z21_ShrMan(&m[0], f[3]); rc = 0; goto done;
      }
      z21_CopyMan(&m[0], save); z21_ShrMan(&m[0], f[1] - ex); z21_RoundMan(&m[0], f[2]); z21_ShrMan(&m[0], f[3] + 1);
    }
  }
  ex = 0; rc = 2;
done:
  m[0] = ((uint32_t)ex << ((0x1f - f[3]) & 0x1f)) | ((se & 0x8000) ? 0x80000000u : 0) | m[0];
  (void)top;
  if (f[4] == 0x40) { out[1] = m[0]; out[0] = m[1]; } else if (f[4] == 0x20) out[0] = m[0];
  return rc;
}
// 1.13c Storm+0x3F1D2: __ld12tod
int z21_ld12tod_a(uint16_t* p, uint32_t* d) { return z21_ld12cvt(p, d, z21_ld12_dbl); }
// 1.13c Storm+0x3F1E8: __ld12tof
int z21_ld12tof_a(uint16_t* p, uint32_t* d) { return z21_ld12cvt(p, d, z21_ld12_flt); }

// 1.13c Storm+0x3F288: __fptostr (st: +4 decpt, +0xC digits ptr32)
void z21_fptostr(char* buf, int n, uint32_t* st) {
  const char* s = (const char*)(uintptr_t)st[3]; char* p = buf + 1; *buf = '0';
  for (int i = 0; i < n; i++) { char c = *s; if (c) s++; else c = '0'; *p++ = c; }
  *p = 0;
  if (n >= 0 && *s > '4') { while (*--p == '9') *p = '0'; ++*p; }
  if (*buf == '1') st[1]++; else memmove(buf, buf + 1, strlen(buf + 1) + 1);
}

// 1.13c Storm+0x3F599: _fload-style math error dispatcher (struct: type@0 name@4 arg1@8 arg2@16 ret@24); guessed table from decompile
void z21_matherr_dispatch(uint64_t* p1, uint64_t* p2, uint64_t* p3, int code) {
  struct { int32_t type; uint32_t name; uint64_t a, b, r; } e;
  int path; // 0=A(err 0x22) 1=B(type1, err 0x21) 2=C(type4, no err)
  e.type = 0; e.name = 0;
  switch (code) {
    case 0xa2: e.type = 4; path = 0; break;
    case 0xa1: e.type = 3; path = 0; break;
    case 0xa6: e.type = 3; path = 0; break;
    case 0xaa: e.type = 2; path = 0; break;
    case 0x18: e.type = 3; path = 0; break;
    case 2: e.type = 2; path = 0; break;
    case 8: e.type = 2; path = 0; break;
    case 0xe: e.type = 3; path = 0; break;
    case 0x1b: e.type = 2; path = 0; break;
    case 0xf: case 0x19: path = 2; break;
    case 0x1a: *p3 = 0x3ff0000000000000ull; return;
    case 3: case 9: case 0xab: case 0x1c: case 0x3ee: path = 1; break;
    case 0x3e8: case 0x3e9: case 0x3ea: case 0x3eb: case 0x3ec: case 0x3ed: case 0x3ef: case 0x1d: *p3 = *p1; path = 1; break;
    default: return;
  }
  e.a = *p1; e.b = *p2; e.r = *p3;
  if (path == 1) e.type = 1; else if (path == 2) e.type = 4;
  int ok = z21_math_err_hook(&e); // name string seam omitted: retail stores a C string ptr in e.name
  if (path == 0 && !ok) *z21_errno() = 0x22;
  if (path == 1 && !ok) *z21_errno() = 0x21;
  *p3 = e.r;
}

// 1.13c Storm+0x4059B: _ctrlfp/raise tail of the math-error path (guessed field layout: p2[0]=type, [2..3]=arg1, [4..5]=arg2, [6..7]=ret)
void z21_matherr_raise(int fn, int32_t* p2, uint16_t* p3) {
  uint32_t sw = *p3, ex; uint32_t buf[12];
  switch (p2[0]) {
    case 1: case 5: ex = 8; break;
    case 2: ex = 4; break;
    case 3: ex = 0x11; break;
    case 4: ex = 0x12; break;
    case 7: p2[0] = 1; goto tail;
    case 8: ex = 0x10; break;
    default: goto tail;
  }
  if (!z21_handle_exc(ex, (double*)(p2 + 6), sw)) {
    if (fn == 0x10 || fn == 0x16 || fn == 0x1d) { /* retail also seeds buf from p2[4..5] and sets control bits */ }
    z21_raise_exc(buf, &sw, ex, fn, (double*)(p2 + 2), (double*)(p2 + 6));
  }
tail:
  z21_ctrlfp();
  if (p2[0] != 8 && z21_dat_42098 == 0 && z21_31123() != 0) return;
  z21_315ef(p2[0]);
}

// 1.13c Storm+0x40744: __set_exp (replace exponent field of x with e+0x3fe, i.e. 2^(e-1) scale in [0.5,1))
double z21_set_exp(double x, int16_t e) {
  uint64_t b; memcpy(&b, &x, 8);
  uint16_t top = (uint16_t)(((e + 0x3fe) * 0x10) | ((b >> 48) & 0x800f));
  b = (b & 0x0000ffffffffffffull) | ((uint64_t)top << 48);
  memcpy(&x, &b, 8); return x;
}
