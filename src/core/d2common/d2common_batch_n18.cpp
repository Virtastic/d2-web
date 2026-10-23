// src/core/d2common/d2common_batch_n18.cpp -- 1.13c D2Common.dll rows from the cached decompiles (alpha lane).
// Base 0x6FD50000. Retail globals and callees are host seam arguments (the d2client_batch_1019h convention).
#include <cstdint>

// 1.13c D2Common+0x3790: CRT ___ascii_stricmp. Compares case-folded (A-Z -> a-z) bytes; returns -1, 0 or 1.
extern "C" int D2Common_3790_ascii_stricmp(const uint8_t* a, const uint8_t* b) {
  for (;;) {
    uint8_t cb = *b++, ca = *a++;
    if (ca != cb) {
      uint8_t fb = (uint8_t)(cb - 'A') < 26 ? cb + 32 : cb;
      uint8_t fa = (uint8_t)(ca - 'A') < 26 ? ca + 32 : ca;
      if (fa != fb) return fa < fb ? -1 : 1;
    }
    if (cb == 0) return 0;
  }
}

// 1.13c D2Common+0x1A080: runs three init hooks when their done-flags (+0x28 bits 0x1000000, 0x2000000,
// 0x100000) are clear; the second only when +0x48 == 2. Returns +0x30. Hooks are seams (0x68400, 0xBF70, 0x7C0E0).
extern "C" uint32_t D2Common_1A080(uint8_t* u, void (*h68400)(), void (*hBF70)(uint8_t*), void (*h7C0E0)()) {
  uint32_t f = *(uint32_t*)(u + 0x28);
  if (!(f & 0x1000000)) h68400();
  if (!(*(uint32_t*)(u + 0x28) & 0x2000000) && *(int32_t*)(u + 0x48) == 2) hBF70(u);
  if (!(*(uint32_t*)(u + 0x28) & 0x100000)) h7C0E0();
  return *(uint32_t*)(u + 0x30);
}

// 1.13c D2Common+0x19B60: weighted random pick among the 12-byte entries at this+0x2C (count at +0x1d8).
// tbl = retail's 8-byte table at 0x6FDE4F48 (word0 flag, word1 group). An entry matches key if equal to it or
// both share a nonzero-flag group. LCG state at +0x1c4/+0x1c8: s = lo*0x6ac690c5 + hi (64-bit stored back).
// Outputs entry's two words; then seam 0x2E5C0(out1,out2,this[0x1b4],this).
extern "C" void D2Common_19B60(uint8_t* t, int32_t key, const int32_t* tbl, int32_t* o1, int32_t* o2,
                               void (*h2E5C0)(int32_t, int32_t, int32_t, uint8_t*)) {
  int32_t n = *(int32_t*)(t + 0x1d8);
  auto match = [&](int32_t e) { return key == e || (tbl[key * 2] != 0 && tbl[key * 2 + 1] == tbl[e * 2 + 1]); };
  uint32_t cnt = 0;
  for (int32_t i = 0; i < n; i++) cnt += match(*(int32_t*)(t + 0x34 + i * 12));
  int32_t pick = 0;
  if (cnt) {
    uint64_t s = (uint64_t)*(uint32_t*)(t + 0x1c4) * 0x6ac690c5ull + *(uint32_t*)(t + 0x1c8);
    *(uint64_t*)(t + 0x1c4) = s;
    pick = (cnt & (cnt - 1)) == 0 ? (int32_t)((cnt - 1) & (uint32_t)s) + 1 : (int32_t)((uint32_t)s % cnt) + 1;
  }
  int32_t i = 0;
  for (; i < n; i++) {
    if (match(*(int32_t*)(t + 0x34 + i * 12))) pick--;
    if (pick < 1) break;
  }
  *o1 = *(int32_t*)(t + 0x2c + i * 12);
  *o2 = *(int32_t*)(t + 0x30 + i * 12);
  h2E5C0(*o1, *o2, *(int32_t*)(t + 0x1b4), t);
}
