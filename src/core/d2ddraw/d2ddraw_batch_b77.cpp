// src/core/d2ddraw/d2ddraw_batch_b77.cpp -- UNMAPPED 1.13c D2DDraw.dll rows from the decompile
// build-wasm/ghidra-d2ddraw/decomp/0x82F0.c and 0x9644.c (alpha lane, no spec/vector yet).
// Skipped from the batch: 51D0/560C (direct Win32), 6E20/85A0 (register-passed), 6FF0 (Bink +
// critical section), 9844 (x87 float CRT), ___tolower_mt, __shift, __cftoe2, __cftof2.
#include <cstdint>
#include <initializer_list>
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#define W __attribute__((weak))

typedef void* (*vfn)(...);
// COM object: first member is the vtable; slot at retail byte offset `off` is vt[off/4].
static inline vfn slot(void* o, int off) { return (*(vfn**)o)[off / 4]; }

extern "C" {
W void* DAT_6f8d0218;  // primary surface
W void* DAT_6f8d021c;  // backbuffer
W void* DAT_6f8d0220;  // render surface
W void* DAT_6f8d0230;  // palette
W int32_t DAT_6f8d0234;  // buffer mode
W int32_t DAT_6f8d0240;  // palette dirty
W int32_t DAT_6f8d01c8, DAT_6f8d01d8;  // width, height
W uint8_t DAT_6f8cfdcc[1];  // palette entries
W void q_d2ddraw_batch_b77_FUN_6f8c7690(void* hr, const char* msg);

// 1.13c D2DDraw+0x082F0: blit render surface to the screen (BltFast), then flush a dirty palette.
EMSCRIPTEN_KEEPALIVE int32_t b77_FUN_6f8c82f0(void) {
  void* hr;
  int32_t r[4];
  if (DAT_6f8d0234 == 0) {
    if (DAT_6f8d0220) {
      r[0] = 0; r[1] = 0; r[2] = DAT_6f8d01c8; r[3] = DAT_6f8d01d8;
      if ((hr = slot(DAT_6f8d0218, 0x1c)(DAT_6f8d0218, 0, 0, DAT_6f8d0220, r, 0x10))) goto fail;
    }
  } else {
    if (DAT_6f8d0220 && DAT_6f8d021c && DAT_6f8d0220 != DAT_6f8d021c) {
      r[0] = 0; r[1] = 0; r[2] = DAT_6f8d01c8; r[3] = DAT_6f8d01d8;
      if ((hr = slot(DAT_6f8d021c, 0x1c)(DAT_6f8d021c, 0, 0, DAT_6f8d0220, r, 0x10))) goto fail;
    }
    slot(DAT_6f8d0218, 0x2c)(DAT_6f8d0218, 0, 1);  // Flip
  }
  if (DAT_6f8d0240 && DAT_6f8d0230) {
    DAT_6f8d0240 = 0;
    if ((hr = slot(DAT_6f8d0230, 0x18)(DAT_6f8d0230, 0, 1, 0xff, DAT_6f8cfdcc))) {
      q_d2ddraw_batch_b77_FUN_6f8c7690(hr, "SetEntries failed!");
      return 0;
    }
  }
  return 1;
fail:
  q_d2ddraw_batch_b77_FUN_6f8c7690(hr, "BltFast failed!");
  return 0;
}

W void *PTR_LAB_6f8cebcc, *PTR_LAB_6f8cebd0, *PTR_LAB_6f8cebd4, *PTR_LAB_6f8cebd8,
    *PTR_LAB_6f8cebdc, *PTR_LAB_6f8cebe0;
W void __cfltcvt(); W void __fassign(); W void FUN_6f8c969a();
W void LAB_6f8c96d6(); W void LAB_6f8c9721();

// 1.13c D2DDraw+0x09644: install the CRT float-conversion dispatch table.
EMSCRIPTEN_KEEPALIVE void b77_FUN_6f8c9644(void) {
  PTR_LAB_6f8cebcc = (void*)__cfltcvt;
  PTR_LAB_6f8cebd0 = (void*)LAB_6f8c96d6;
  PTR_LAB_6f8cebd4 = (void*)__fassign;
  PTR_LAB_6f8cebd8 = (void*)FUN_6f8c969a;
  PTR_LAB_6f8cebdc = (void*)LAB_6f8c9721;
  PTR_LAB_6f8cebe0 = (void*)__cfltcvt;
}
}
