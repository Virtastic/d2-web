// src/core/fog/fog_batch_z17.cpp -- twelve 1.13c Fog.dll alpha-lane rows (MSVC CRT/EH runtime
// pieces linked into Fog) from build-wasm/ghidra-fog/decomp/0x<RVA>.c. Base 0x6FF50000.
// 32-bit links are uint32_t; retail globals and callees outside this batch are seams.
#include <cstdint>
#include <cstring>

namespace {
// ---- seams (retail callees / globals not in this batch) ----
void (*seam_CallMemberFunction1_z17)(uint32_t obj, uint32_t fn);          // Fog+0x3DEB
uint32_t (*seam_NLG_Notify1_z17)(uint32_t);                                // Fog+0x920D, returns code addr
void (*seam_inconsistency_z17)();                                          // Fog+0x79C3
void (*seam_FindHandler_z17)(void*, void*, void*, void*, void*, uint8_t, int, void*); // Fog+0x230FE
void (*seam_FrameUnwindToState_z17)(int frame, void* disp, int funcInfo, int state);  // Fog+0x22B67
int  (*seam_sptype_z17)(uint32_t lo, uint32_t hi);                         // Fog+0xAD0C
double (*seam_frnd_z17)(double);                                           // Fog+0xACD1
void (*seam_terminate_z17)();                                              // Fog+0x24792
int  (*seam_ValidateExecute_z17)(uint32_t fn);                             // Fog+0x798E
uint32_t g_unhandledFilter_z17;                                            // Fog+0x56EE0 (DAT_6ffa6ee0)
int  (*seam_callFilter_z17)(uint32_t fn, void* exceptionPointers);
void (*seam_FUN_51310_z17)();                                              // Fog+0x1310
void (*seam_FUN_51080_z17)(int*);                                          // Fog+0x1080
// retail double constants (guessed from use): 7F9050=0.0, 7F9040=1.0, 7EBC0=+inf, 7EBC8=1.0, 7EBE0=-0.0?, 7CA28=2.0
const double kZero = 0.0, kOne = 1.0, kInf = __builtin_huge_val(), kHalfStep = 2.0;
const double kNegZeroish = -0.0;
}

// 1.13c Fog+0x22C35  ___DestructExceptionObject
extern "C" void DestructExceptionObject_z17(uint32_t p) {
  if (!p) return;
  uint32_t info = *(uint32_t*)(uintptr_t)(p + 0x1c);
  uint32_t fn = *(uint32_t*)(uintptr_t)(info + 4);
  if (fn) seam_CallMemberFunction1_z17(*(uint32_t*)(uintptr_t)(p + 0x18), fn);
}

// 1.13c Fog+0x22C7A  AdjustPointer (EAX = object base, ECX = PMD*)
extern "C" uint32_t AdjustPointer_z17(uint32_t base, const int32_t* pmd) {
  uint32_t r = base + (uint32_t)pmd[0];
  if (pmd[1] >= 0) {
    uint32_t vbtab = *(uint32_t*)(uintptr_t)(base + (uint32_t)pmd[1]);
    r += *(uint32_t*)(uintptr_t)(vbtab + (uint32_t)pmd[2]) + (uint32_t)pmd[1];
  }
  return r;
}

// 1.13c Fog+0x23302  ___InternalCxxFrameHandler (param_1 = EHExceptionRecord*, param_5 = _s_FuncInfo*)
extern "C" uint32_t InternalCxxFrameHandler_z17(uint8_t* rec, void* reg, void* ctx, void* disp,
                                                int32_t* fi, int p6, void* p7, uint8_t p8) {
  if ((fi[0] & 0x1fffffff) != 0x19930520) seam_inconsistency_z17();
  if ((rec[4] & 0x66) == 0) {
    if (fi[3] != 0) {  // nTryBlocks
      uint32_t ex1c = *(uint32_t*)(rec + 0x1c);
      if (*(int32_t*)rec == -0x1f928c9d && *(uint32_t*)(rec + 0x14) > 0x19930520 &&
          *(uint32_t*)(uintptr_t)(ex1c + 8) != 0) {
        typedef uint32_t (*H)(void*, void*, void*, void*, void*, int, void*, uint8_t);
        return ((H)(uintptr_t) * (uint32_t*)(uintptr_t)(ex1c + 8))(rec, reg, ctx, disp, fi, p6, p7, p8);
      }
      seam_FindHandler_z17(rec, reg, ctx, disp, fi, p8, p6, p7);
    }
  } else if (fi[1] != 0 && p6 == 0) {  // maxState
    seam_FrameUnwindToState_z17((int)(intptr_t)reg, disp, (int)(intptr_t)fi, -1);
  }
  return 1;
}

// 1.13c Fog+0x233B0  __CallSettingFrame@12
extern "C" void CallSettingFrame_z17(uint32_t, uint32_t, int p3) {
  uint32_t fn = seam_NLG_Notify1_z17((uint32_t)p3);
  typedef void (*F)();
  ((F)(uintptr_t)fn)();
  if (p3 == 0x100) p3 = 2;
  seam_NLG_Notify1_z17((uint32_t)p3);
}

// 1.13c Fog+0x243D5  __fload_withFB (ECX unused, EDX/param_2 = double*)
extern "C" uint32_t fload_withFB_z17(uint32_t, const uint8_t* d) {
  uint32_t e = *(const uint32_t*)(d + 4) & 0x7ff00000;
  return e != 0x7ff00000 ? e : *(const uint32_t*)(d + 4);
}

// 1.13c Fog+0x248A6  __fpclass
extern "C" int fpclass_z17(double x) {
  uint64_t b; memcpy(&b, &x, 8);
  uint32_t lo = (uint32_t)b, hi = (uint32_t)(b >> 32);
  if ((hi & 0x7ff00000) == 0x7ff00000) {
    int t = seam_sptype_z17(lo, hi);
    if (t == 1) return 0x200;
    if (t == 2) return 4;
    if (t == 3) return 2;
    return 1;
  }
  uint32_t neg = (hi >> 31) ? 0xffffffffu : 0;
  if ((b & 0x7ff0000000000000ull) == 0 && ((b & 0xfffff00000000ull) != 0 || lo != 0))
    return (int)((neg & 0xffffff90u) + 0x80);
  if (x == kZero) return (int)((neg & 0xffffffe0u) + 0x40);
  return (int)((neg & 0xffffff08u) + 0x100);
}

// 1.13c Fog+0x2451C  __d_inttype
extern "C" int d_inttype_z17(double x) {
  if ((fpclass_z17(x) & 0x90) == 0 && seam_frnd_z17(x) == x) {
    double h = x / kHalfStep;
    return h == seam_frnd_z17(h) ? 2 : 1;
  }
  return 0;
}

// 1.13c Fog+0x2458A  __powhlp (x = lo/hi dwords, y = param_3)
extern "C" uint32_t powhlp_z17(int lo, int hi, double y, double* out) {
  uint64_t xb = ((uint64_t)(uint32_t)hi << 32) | (uint32_t)lo; double x; memcpy(&x, &xb, 8);
  uint32_t ylo, yhi; uint64_t yb; memcpy(&yb, &y, 8); ylo = (uint32_t)yb; yhi = (uint32_t)(yb >> 32);
  uint32_t ret = 0; double r = kInf;
  if (x < kZero) x = -x;
  if (yhi == 0x7ff00000 && ylo == 0) {  // y == +inf
    if (kOne < x) goto done;
    if (kOne <= x) { ret = 1; r = kOne; goto done; }
    r = 0.0; goto done;
  }
  if (y == -__builtin_huge_val()) {
    if (kOne < x) { r = 0.0; goto done; }
    if (x < kOne) goto done;
    ret = 1; r = kOne; goto done;
  }
  if (hi != 0x7ff00000) {
    if (hi != -0x100000 || lo != 0) return 0;
    int t = d_inttype_z17(y);
    if (y <= kZero) r = (kZero <= y) ? 1.0 : (t != 1 ? 0.0 : kNegZeroish);
    else r = (t == 1) ? -kInf : kInf;
    goto done;
  }
  if (lo != 0) return 0;
  if (kZero < y) goto done;
  r = (kZero <= y) ? 1.0 : 0.0;
done:
  *out = r;
  return ret;
}

// 1.13c Fog+0x246EC  __CxxUnhandledExceptionFilter
extern "C" int CxxUnhandledExceptionFilter_z17(uint32_t* const* ep) {
  const uint32_t* rec = ep[0];  // ExceptionCode@0, NumberParameters@0x18, ExceptionInformation@0x1c
  if (rec[0] == 0xe06d7363 && rec[6] == 3 && (rec[7] == 0x19930520 || rec[7] == 0x19930521))
    seam_terminate_z17();
  if (g_unhandledFilter_z17 && seam_ValidateExecute_z17(g_unhandledFilter_z17) != 0)
    return seam_callFilter_z17(g_unhandledFilter_z17, (void*)ep);
  return 0;
}

// 1.13c Fog+0x24970  Unwind_6ff74970
extern "C" void Unwind_74970_z17() { seam_FUN_51310_z17(); }

// 1.13c Fog+0x249B0  Unwind_6ff749b0 (frame EBP passed in)
extern "C" void Unwind_749B0_z17(uint32_t ebp) { seam_FUN_51080_z17((int*)(uintptr_t)*(uint32_t*)(uintptr_t)(ebp + 4)); }

// 1.13c Fog+0x249DD  Unwind_6ff749dd
extern "C" void Unwind_749DD_z17(uint32_t ebp) { seam_FUN_51080_z17((int*)(uintptr_t)*(uint32_t*)(uintptr_t)(ebp + 0xc)); }
