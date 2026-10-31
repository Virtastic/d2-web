// D2Client implementation batch B: six functions from the cached 1.13c decompile that had no port in src/.
// Every import (ordinal veneer) and every D2Client global the function reads is a parameter, so the
// decompile's register and stack inputs are visible in the signature. Layouts are the 1.13c 32-bit
// offsets from the decompile (words are uint32_t, pointers held as 32-bit handles).
// Status: syntax-checked only. No FUNCTION capture yet; the vectors are listed in the private spec store.
#include <cstdint>
#include <cstring>

namespace d2client_implb {

// Word view of a 0x5C-byte request record (the decompile's local_5c / auStack_60 block).
struct Rec { uint32_t w[0x17]; };
static_assert(sizeof(Rec) == 0x5C, "request record is 0x5C bytes (0x17 dwords)");

static inline uint16_t get16(const void* p, uint32_t byteOff) {
  uint16_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + byteOff, 2); return v;
}

// ---------------------------------------------------------------------------------------------
// 1.13c D2Client+0x0DA70 mem_equal (fastcall: a = ECX, b = EDX, n = stack). Returns 1 when equal.
// Leading n&3 bytes first (one byte per step), then the n>>2 dwords in order.
// ---------------------------------------------------------------------------------------------
// 1.13c D2Client+0x0DA70
int32_t mem_equal_DA70(const uint8_t* a, const uint8_t* b, uint32_t n) {
  const uint32_t r = n & 3u;
  for (uint32_t i = 0; i < r; ++i) {
    if (a[i] != b[i]) return 0;
  }
  a += r;
  b += r;
  for (uint32_t c = n >> 2; c != 0; --c, a += 4, b += 4) {
    if (std::memcmp(a, b, 4) != 0) return 0;
  }
  return 1;
}

// ---------------------------------------------------------------------------------------------
// 1.13c D2Client+0x664A0 frame box around a unit. unit = EDI (0 is a retail trap), x0 = EBX, y0 = ESI.
// Globals: DAT_6FBC9960 (RVA 0x119960), DAT_6FBC995C (0x11995C), DAT_6FBCC418 (0x11C418).
// ---------------------------------------------------------------------------------------------
struct FrameBoxImports {
  virtual void ord10010(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t c0, uint32_t c1) = 0;  // edges
  virtual void ord10014(int32_t x, int32_t y, int32_t x1, int32_t y1, uint32_t c0, uint32_t c1) = 0;   // fill
  virtual int32_t ord10651() = 0;
  virtual int32_t ord11142() = 0;
  virtual void ord11087(int32_t* x, int32_t* y) = 0;  // outputs are not read back by the decompile
  virtual void fatal(uint32_t msg) = 0;              // ord 10265(msg) -> ord 10024 -> _exit(-1)
};
struct FrameBoxGlobals {
  int32_t g9960;  // DAT_6FBC9960
  int32_t g995c;  // DAT_6FBC995C
  int32_t g418;   // DAT_6FBCC418
};

// 1.13c D2Client+0x664A0
void frame_box_664A0(FrameBoxImports& I, const FrameBoxGlobals& g, const uint8_t* unit, int32_t x0, int32_t y0) {
  if (unit == nullptr) { I.fatal(0x61); return; }
  int32_t lx = get16(unit, 0x8C);
  int32_t ly = get16(unit, 0x8E);
  I.ord11087(&lx, &ly);

  const int32_t d = g.g418 - g.g9960;   // (DAT_6FBCC418 - DAT_6FBC9960)
  const int32_t e = g.g995c;            // DAT_6FBC995C
  int32_t ebx = d + x0;
  int32_t iv2 = (y0 - e) + 0x10;
  int32_t iv4 = ebx + 0x10;
  I.ord10010(ebx, (y0 - e) + 8, iv4, iv2, 0xff, 0xff);

  iv2 = d + iv2;
  int32_t iv3 = (iv4 - e) + 0x10;
  int32_t iv5 = iv2 - 0x10;
  I.ord10010(iv2, (iv4 - e) + 8, iv5, iv3, 0xff, 0xff);

  iv3 = d + iv3;
  iv2 = (iv5 - e) + 0x18;
  I.ord10010(iv3 - 0x10, (iv5 - e) + 0x10, iv3, iv2, 0xff, 0xff);

  iv2 = d + iv2;
  I.ord10010(iv2 + 0x10, (iv3 - e) + 0x10, iv2, (iv3 - e) + 0x18, 0xff, 0xff);

  iv4 = g.g9960 - g.g418;               // DAT_6FBC9960 - DAT_6FBCC418
  iv2 = I.ord10651() - iv4;
  iv3 = e - 8;
  iv4 = I.ord11142() - iv3;
  I.ord10014(iv2, iv4, iv2 + 4, iv4 + 4, 0xff, 5);
}

// ---------------------------------------------------------------------------------------------
// 1.13c D2Client+0x9CD07 target pick. unit = EDI (int words: [0] first choice, [1] and [2] candidate
// bases, [4] result, [5] stamp). stackFlag = the stack word the decompile tests against 0.
// The seed is the 64-bit LCG at seed+0x20 (low dword) / seed+0x24 (high dword), multiplier 0x6AC690C5.
// Globals: DAT_6FBCBBFC (RVA 0x11BBFC, seed block pointer), DAT_6FBCC3B8 (0x11C3B8), DAT_6FBC97F8 (0x1197F8).
// ---------------------------------------------------------------------------------------------
struct TargetPickImports {
  virtual int32_t ord10396(int32_t v) = 0;        // bucket of DAT_6FBCC3B8
  virtual int32_t choose_5EE70(int32_t idx) = 0;  // FUN_6fb0ee70, RVA 0x5EE70 (not expanded here)
};
struct TargetPickGlobals {
  uint8_t* seed;   // *(DAT_6FBCBBFC): the seed block holding the LCG words at +0x20/+0x24
  int32_t g3b8;    // DAT_6FBCC3B8
  int32_t g97f8;   // DAT_6FBC97F8
};

// 1.13c D2Client+0x9CD07
void target_pick_9CD07(TargetPickImports& I, const TargetPickGlobals& g, int32_t* unit, int32_t stackFlag) {
  auto advance = [&g]() -> uint64_t {
    uint32_t lo, hi;
    std::memcpy(&lo, g.seed + 0x20, 4);
    std::memcpy(&hi, g.seed + 0x24, 4);
    const uint64_t v = uint64_t(lo) * 0x6AC690C5ull + uint64_t(hi);
    std::memcpy(g.seed + 0x20, &v, 8);
    return v;
  };
  int32_t tries = 0;
  int32_t s;
  do {
    s = unit[0];
    if (stackFlag == 0 && unit[1] != 0) {
      const uint64_t v = advance();
      if ((v & 1) == 0) s = unit[1];
    }
    if (unit[2] != 0 && (s == 0 || (advance() & 0xFFFFFFFFull) % 3 == 0)) {
      const int32_t b = I.ord10396(g.g3b8);
      if (b == 1) s = unit[2];
      else if (b < 2 || b > 3) s = unit[2] + 2;
      else s = unit[2] + 1;
    }
    s = I.choose_5EE70(s);
  } while (s == unit[4] && ++tries < 20);
  unit[4] = s;
  unit[5] = g.g97f8;
}

// ---------------------------------------------------------------------------------------------
// 1.13c D2Client+0xA2540 missile/effect emit. p1, p2, p3 are the stack words the decompile calls
// param_1..3 (record words [4], [0xB], [0xC]); p4 and p5 are the source and target words (record
// words [5], [6]). p5 may be replaced by the resolver at 0xA1340 before the record is built.
// ---------------------------------------------------------------------------------------------
struct MissileEmitImports {
  virtual int32_t resolve_A1340(int32_t* target) = 0;   // FUN_6fb51340 (RVA 0xA1340), may update target
  virtual uint32_t* emit_B30F0(const Rec* rec) = 0;     // FUN_6fb630f0 (RVA 0xB30F0), client object or null
};

// 1.13c D2Client+0xA2540
void missile_emit_A2540(MissileEmitImports& I, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, int32_t p5) {
  if ((p4 == 0 || p5 == 0) && I.resolve_A1340(&p5) == 0) return;
  Rec rec{};
  rec.w[0x0] = 1;
  rec.w[0x4] = p1;
  rec.w[0x5] = p4;
  rec.w[0x6] = static_cast<uint32_t>(p5);
  rec.w[0xB] = p2;
  rec.w[0xC] = p3;
  I.emit_B30F0(&rec);
}

// ---------------------------------------------------------------------------------------------
// 1.13c D2Client+0xB5540 client missile from a unit's path. unit is ECX; its path pointer (+0x2C) is
// passed in as pathOfUnit (the caller reads it). eax is the implicit input the decompile names in_EAX.
// ---------------------------------------------------------------------------------------------
struct MissilePathImports {
  virtual uint8_t ord10042(uint32_t path) = 0;                 // D2Common PATH_GetDirection
  virtual void ord10425(uint32_t path, uint8_t direction) = 0; // D2Common PATH_UpdateDirection
  virtual uint32_t* emit_B30F0(const Rec* rec) = 0;            // FUN_6fb630f0 (RVA 0xB30F0)
};

// 1.13c D2Client+0xB5540
uint32_t* ClientMissileFromUnitPath_B5540(MissilePathImports& I, uint32_t unit, uint32_t pathOfUnit, uint32_t eax) {
  if (pathOfUnit == 0) return nullptr;
  const uint8_t direction = I.ord10042(pathOfUnit);
  Rec rec{};
  rec.w[1] = unit;
  rec.w[2] = unit;
  rec.w[4] = 0x54;
  rec.w[0xC] = 1;
  uint32_t* m = I.emit_B30F0(&rec);
  if (m != nullptr) {
    m[0x11] = eax << 8;                                  // +0x44
    if (m[0xB] != 0) I.ord10425(m[0xB], direction);      // +0x2C of the new object
    m[0x31] = m[0x31] | 0x10000u;                        // +0xC4 flags
  }
  return m;
}

// ---------------------------------------------------------------------------------------------
// 1.13c D2Client+0xBF540 append a localized label and one digit to a wide string in place.
// dst = EDI (wide buffer, at most 500 code units). digit = the stack byte param_1. labelId is the
// register the decompile does not name (the D2Lang lookup at ordinal 10003 is given it here).
// ---------------------------------------------------------------------------------------------
struct LabelImports {
  virtual const uint16_t* ord10003(uint32_t labelId) = 0;  // D2Lang string lookup
  virtual void fatal(uint32_t msg) = 0;                    // ord 10265(msg) -> ord 10024 -> _exit(-1)
};

static uint32_t wlen(const uint16_t* s) {
  uint32_t n = 0;
  while (s[n] != 0) ++n;
  return n;
}
static void wcat(uint16_t* dst, const uint16_t* src) {
  std::memcpy(dst + wlen(dst), src, (wlen(src) + 1) * sizeof(uint16_t));
}

// 1.13c D2Client+0xBF540
void AppendNumberedUnicodeLabel_BF540(LabelImports& I, uint16_t* dst, uint8_t digit, uint32_t labelId) {
  if (wlen(dst) > 499) { I.fatal(0x36C); return; }
  wcat(dst, I.ord10003(labelId));
  // wsprintfA("%c", digit + '0') then win2Unicode of 2 bytes: one wide character plus the terminator.
  const uint16_t tail[2] = { static_cast<uint16_t>(static_cast<uint8_t>(digit + '0')), 0 };
  wcat(dst, tail);
}

}  // namespace d2client_implb
