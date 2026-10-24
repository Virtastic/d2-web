// D2Launch MSVC CRT: small-block heap, locale refcounts, _commit, codepage helpers.
// Retail addresses are 32-bit; here they are offsets into d2l_gbase (guest arena). OS services are host seams.
#include <cstdint>
#include <cstring>
#include <initializer_list>

struct D2LCrtHost {
  uint32_t (*virtualAlloc)(uint32_t addr, uint32_t size, uint32_t type, uint32_t prot);
  void (*virtualFree)(uint32_t addr, uint32_t size, uint32_t type);
  uint32_t (*heapAlloc)(uint32_t flags, uint32_t size);    // zeroed when flags&8
  uint32_t (*heapReAlloc)(uint32_t flags, uint32_t p, uint32_t size);
  void (*heapFree)(uint32_t p);
  void (*crtFree)(uint32_t p);                              // _free
  uint32_t (*locinfoSlot)(void);                            // guest addr of the calling thread's ptlocinfo slot
  uint32_t (*flushOs)(int fd);                              // FlushFileBuffers: 0 or GetLastError
  void (*setErrno)(int e);
  void (*setDosErrno)(uint32_t e);
  int (*fdOpen)(uint32_t fd);                               // FOPEN flag of the ioinfo entry
  void (*lock)(int n);
  void (*unlock)(int n);
};
D2LCrtHost d2l_host;
uint8_t* d2l_gbase;
uint32_t d2l_nfile;                                         // DAT_6fa66f84
uint32_t d2l_curLocinfo, d2l_defLocinfo;                    // PTR_DAT_6fa6075c, DAT_6fa60708
uint32_t d2l_lcDefTime, d2l_lcDefMon, d2l_lcDefNum, d2l_lcDefCtype, d2l_lcDefLconv;  // 6fa65148/66d18/66d1c/66d14/6fa6514c
uint8_t d2l_mbctype[257];                                   // DAT_6fa66d40
uint32_t d2l_mbcp, d2l_mbcpLast, d2l_mbcFlag, d2l_mbcA, d2l_mbcB, d2l_mbcC;  // 66e44 66d28 66d20 66e50 66e54 66e58

extern "C++" {
void FUN_6fa489fc(uint32_t);  // ___free_lconv_mon
void FUN_6fa4899d(uint32_t);  // ___free_lconv_num
void FUN_6fa4880d(uint32_t);  // ___free_lc_time
}
__attribute__((weak)) void FUN_6fa489fc(uint32_t) {}
__attribute__((weak)) void FUN_6fa4899d(uint32_t) {}
__attribute__((weak)) void FUN_6fa4880d(uint32_t) {}

static uint32_t& U(uint32_t a) { return *(uint32_t*)(d2l_gbase + a); }
static uint8_t& B(uint32_t a) { return d2l_gbase[a]; }
static uint32_t mn63(uint32_t v) { return v > 0x3f ? 0x3f : v; }

// sbh state: 6fa66f60 cur, f64 count, f68 regions, f70 start, f74 capacity, f78 group
static uint32_t sbhCur, sbhCount, sbhRegions, sbhStart, sbhCap, sbhGrp;

// region = 20 bytes {mask0,mask1,freeGroups,base,hdr}; hdr: +0 curgroup, +4.. per-bucket counts, +0x43 groupcount,
// +0x44/+0xc4 per-group bitmaps, +0x144 groups of 0x204 bytes (count + 64 buckets of {next,prev}).
static void clearBit(uint32_t reg, uint32_t hdr, uint32_t g, uint32_t i) {
  uint32_t m = ~(0x80000000u >> (i & 31));
  U(hdr + (i < 0x20 ? 0x44 : 0xc4) + g * 4) &= m;
  if (--B(hdr + 4 + i) == 0) U(reg + (i < 0x20 ? 0 : 4)) &= m;
}
static void setBit(uint32_t reg, uint32_t hdr, uint32_t g, uint32_t i) {
  uint32_t m = 0x80000000u >> (i & 31);
  if (B(hdr + 4 + i)++ == 0) U(reg + (i < 0x20 ? 0 : 4)) |= m;
  U(hdr + (i < 0x20 ? 0x44 : 0xc4) + g * 4) |= m;
}
static void unlinkN(uint32_t n) { U(U(n + 8) + 4) = U(n + 4); U(U(n + 4) + 8) = U(n + 8); }
static bool lone(uint32_t n) { return U(n + 4) == U(n + 8); }
static void linkN(uint32_t reg, uint32_t hdr, uint32_t g, uint32_t gb, uint32_t i, uint32_t n) {
  uint32_t h = gb + i * 8, t = U(h + 4);
  U(n + 8) = h; U(n + 4) = t; U(h + 4) = n; U(U(n + 4) + 8) = n;
  if (lone(n)) setBit(reg, hdr, g, i);
}

// 1.13c D2Launch+0x6D16
void FUN_6fa46d16(uint32_t reg, uint32_t blk) {
  uint32_t hdr = U(reg + 16), n = blk - 4, g = (blk - U(reg + 12)) >> 15, gb = g * 0x204 + 0x144 + hdr;
  uint32_t sz = U(n) - 1;
  if (sz & 1) return;
  uint32_t nx = sz + n, nsz = U(nx), psz = U(blk - 8);
  if (!(nsz & 1)) {
    uint32_t i = mn63(((int32_t)nsz >> 4) - 1);
    if (lone(nx)) clearBit(reg, hdr, g, i);
    sz += nsz;
    unlinkN(nx);
  }
  uint32_t a = mn63(((int32_t)sz >> 4) - 1), pb = reg;  // pb: sentinel (retail reuses param_1)
  if (!(psz & 1)) {
    n -= psz;
    pb = mn63(((int32_t)psz >> 4) - 1);
    sz += psz;
    a = mn63(((int32_t)sz >> 4) - 1);
    if (pb != a) {
      if (lone(n)) clearBit(reg, hdr, g, pb);
      unlinkN(n);
    }
  }
  if ((psz & 1) || pb != a) linkN(reg, hdr, g, gb, a, n);
  U(n) = sz;
  U(sz - 4 + n) = sz;
  if (--U(gb) == 0) {
    if (sbhCur) {
      d2l_host.virtualFree(sbhGrp * 0x8000 + U(sbhCur + 12), 0x8000, 0x4000);
      U(sbhCur + 8) |= 0x80000000u >> (sbhGrp & 31);
      uint32_t ch = U(sbhCur + 16);
      U(ch + 0xc4 + sbhGrp * 4) = 0;
      if (--B(ch + 0x43) == 0) U(sbhCur + 4) &= ~1u;
      if (U(sbhCur + 8) == 0xffffffffu) {
        d2l_host.virtualFree(U(sbhCur + 12), 0, 0x8000);
        d2l_host.heapFree(U(sbhCur + 16));
        memmove(d2l_gbase + sbhCur, d2l_gbase + sbhCur + 20, sbhCount * 20 - sbhCur - 20 + sbhRegions);
        sbhCount--;
        if (sbhCur < reg) reg -= 20;
        sbhStart = sbhRegions;
      }
    }
    sbhCur = reg;
    sbhGrp = g;
  }
}

// 1.13c D2Launch+0x702E
uint32_t FUN_6fa4702e(void) {
  if (sbhCount == sbhCap) {
    uint32_t p = d2l_host.heapReAlloc(0, sbhRegions, (sbhCap * 5 + 0x50) * 4);
    if (!p) return 0;
    sbhCap += 0x10;
    sbhRegions = p;
  }
  uint32_t r = sbhRegions + sbhCount * 20;
  uint32_t h = d2l_host.heapAlloc(8, 0x41c4);
  U(r + 16) = h;
  if (h) {
    uint32_t v = d2l_host.virtualAlloc(0, 0x100000, 0x2000, 4);
    U(r + 12) = v;
    if (v) {
      U(r + 8) = 0xffffffffu; U(r) = 0; U(r + 4) = 0;
      sbhCount++;
      U(U(r + 16)) = 0xffffffffu;
      return r;
    }
    d2l_host.heapFree(U(r + 16));
  }
  return 0;
}

// 1.13c D2Launch+0x70E5
int32_t FUN_6fa470e5(uint32_t reg) {
  uint32_t hdr = U(reg + 16);
  int32_t g = 0;
  for (int32_t m = (int32_t)U(reg + 8); m >= 0; m <<= 1) g++;
  uint32_t gb = g * 0x204 + 0x144 + hdr, a = gb;
  for (int k = 0; k < 0x3f; k++, a += 8) { U(a + 8) = a; U(a + 4) = a; }
  uint32_t la = g * 0x8000 + U(reg + 12);
  if (!d2l_host.virtualAlloc(la, 0x8000, 0x1000, 4)) return -1;
  for (uint32_t p = la + 0x10, k = 0; k < 8; k++, p += 0x1000) {
    U(p - 8) = 0xffffffffu; U(p + 0xfec) = 0xffffffffu;
    U(p) = p + 0xffc; U(p - 4) = 0xff0;
    U(p + 4) = p - 0x1004; U(p + 0xfe8) = 0xff0;
  }
  U(gb + 0x1fc) = la + 0xc; U(la + 0x14) = gb + 0x1f8;
  U(gb + 0x200) = la + 0x700c; U(la + 0x7010) = gb + 0x1f8;
  U(hdr + 0x44 + g * 4) = 0; U(hdr + 0xc4 + g * 4) = 1;
  if (B(hdr + 0x43)++ == 0) U(reg + 4) |= 1;
  U(reg + 8) &= ~(0x80000000u >> (g & 31));
  return g;
}

// 1.13c D2Launch+0x71EB
uint32_t FUN_6fa471eb(uint32_t reg, uint32_t blk, int32_t req) {
  uint32_t hdr = U(reg + 16), g = (blk - U(reg + 12)) >> 15, gb = g * 0x204 + 0x144 + hdr;
  uint32_t sz = (req + 0x17u) & 0xfffffff0u;
  int32_t cur = (int32_t)U(blk - 4) - 1;
  uint32_t nx = U(blk - 4) - 5 + blk, nsz = U(nx);
  if (cur < (int32_t)sz) {
    if ((nsz & 1) || (int32_t)(nsz + cur) < (int32_t)sz) return 0;
    uint32_t i = mn63(((int32_t)nsz >> 4) - 1);
    if (lone(nx)) clearBit(reg, hdr, g, i);
    unlinkN(nx);
    int32_t rem = nsz + (cur - sz);
    if (rem > 0) {
      uint32_t node = blk - 4 + sz;
      linkN(reg, hdr, g, gb, mn63((rem >> 4) - 1), node);
      U(node) = rem; U(rem - 4 + node) = rem;
    }
    U(blk - 4) = sz + 1; U(blk - 8 + sz) = sz + 1;
  } else if ((int32_t)sz < cur) {
    uint32_t d = cur - sz, node = blk - 4 + sz;
    U(blk - 4) = sz + 1; U(node - 4) = sz + 1;
    if (!(nsz & 1)) {
      uint32_t i = mn63(((int32_t)nsz >> 4) - 1);
      if (lone(nx)) clearBit(reg, hdr, g, i);
      unlinkN(nx);
      d += nsz;
    }
    linkN(reg, hdr, g, gb, mn63((d >> 4) - 1), node);
    U(node) = d; U(d - 4 + node) = d;
  }
  return 1;
}

// 1.13c D2Launch+0x74CA
uint32_t FUN_6fa474ca(uint32_t req) {
  uint32_t size = (req + 0x17u) & 0xfffffff0u;
  int32_t idx = ((int32_t)(req + 0x17u) >> 4) - 1;
  uint32_t end = sbhRegions + sbhCount * 5 * 4, lo, hi;
  if (idx < 0x20) { lo = 0xffffffffu >> (idx & 31); hi = 0xffffffffu; }
  else { lo = 0; hi = 0xffffffffu >> ((idx - 0x20) & 31); }
  uint32_t r = sbhStart;
  for (; r < end && (U(r + 4) & hi) == 0 && (U(r) & lo) == 0; r += 20) {}
  if (r == end) {
    uint32_t q = sbhRegions;
    for (; q < sbhStart && (U(q + 4) & hi) == 0 && (U(q) & lo) == 0; q += 20) {}
    r = q;
    if (q == sbhStart) {
      for (; q < end && U(q + 8) == 0; q += 20) {}
      uint32_t q2 = sbhRegions;
      r = q;
      if (q == end) {
        for (; q2 < sbhStart && U(q2 + 8) == 0; q2 += 20) {}
        r = q2;
        if (q2 == sbhStart && (r = FUN_6fa4702e()) == 0) return 0;
      }
      int32_t ng = FUN_6fa470e5(r);
      U(U(r + 16)) = ng;
      if ((int32_t)U(U(r + 16)) == -1) return 0;
    }
  }
  uint32_t hdr = U(r + 16);
  int32_t lg = (int32_t)U(hdr);
  if (lg == -1 || ((U(hdr + (lg + 0x31) * 4) & hi) == 0 && (U(hdr + (lg + 0x11) * 4) & lo) == 0)) {
    lg = 0;
    uint32_t p = hdr + 0x44;
    if ((U(hdr + 0xc4) & hi) == 0 && (U(p) & lo) == 0) {
      uint32_t c;
      do { c = p + 0x80; lg++; p += 4; } while ((U(c) & hi) == 0 && (U(p) & lo) == 0);
    }
  }
  uint32_t gb = hdr + lg * 0x204 + 0x144;
  int32_t b = 0;
  uint32_t m = U(hdr + 0x44 + lg * 4) & lo;
  if (m == 0) { m = U(hdr + 0xc4 + lg * 4) & hi; b = 0x20; }
  for (; (int32_t)m >= 0; m <<= 1) b++;
  uint32_t n = U(gb + b * 8 + 4);
  int32_t rest = (int32_t)(U(n) - size);
  uint32_t nb = mn63((rest >> 4) - 1);
  sbhStart = r;
  if (nb != (uint32_t)b) {
    if (lone(n)) clearBit(r, hdr, lg, b);
    unlinkN(n);
    if (rest != 0) linkN(r, hdr, lg, gb, nb, n);
  }
  if (rest != 0) { U(n) = rest; U(rest - 4 + n) = rest; }
  n += rest;
  U(n) = size + 1;
  U(n + size - 4) = size + 1;
  if (U(gb)++ == 0 && r == sbhCur && (uint32_t)lg == sbhGrp) sbhCur = 0;
  U(hdr) = lg;
  return n + 4;
}

// 1.13c D2Launch+0x77C6  (_commit; locks are no-ops, wasm is single threaded: ponytail)
uint32_t FUN_6fa477c6(uint32_t fd) {
  if (fd >= d2l_nfile || !d2l_host.fdOpen(fd)) { d2l_host.setErrno(9); return 0xffffffffu; }
  uint32_t err = d2l_host.flushOs((int)fd);
  if (err == 0) return 0;
  d2l_host.setDosErrno(err);
  d2l_host.setErrno(9);
  return 0xffffffffu;
}

// 1.13c D2Launch+0x7882
void FUN_6fa47882(uint32_t li) {
  uint32_t lc = U(li + 0x3c);
  if (lc != d2l_lcDefLconv && lc != 0 && U(U(li + 0x2c)) == 0) {
    uint32_t p = U(li + 0x34);
    if (p && U(p) == 0 && p != d2l_lcDefMon) { d2l_host.crtFree(p); FUN_6fa489fc(U(li + 0x3c)); }
    p = U(li + 0x30);
    if (p && U(p) == 0 && p != d2l_lcDefNum) { d2l_host.crtFree(p); FUN_6fa4899d(U(li + 0x3c)); }
    d2l_host.crtFree(U(li + 0x2c));
    d2l_host.crtFree(U(li + 0x3c));
  }
  uint32_t p = U(li + 0x40);
  if (p != d2l_lcDefCtype && p && U(p) == 0) { d2l_host.crtFree(p); d2l_host.crtFree(U(li + 0x44) - 0xfe); }
  p = U(li + 0x50);
  if (p != d2l_lcDefTime && p && U(p + 0xb4) == 0) { FUN_6fa4880d(p); d2l_host.crtFree(U(li + 0x50)); }
  d2l_host.crtFree(li);
}

// 1.13c D2Launch+0x7952
uint32_t FUN_6fa47952(void) {
  uint32_t slot = d2l_host.locinfoSlot(), old = U(slot), cur = d2l_curLocinfo;
  if (old != cur) {
    if (old) {
      --U(old);
      for (uint32_t o : {0x2cu, 0x34u, 0x30u, 0x40u}) if (U(old + o)) --U(U(old + o));
      --U(U(old + 0x4c) + 0xb4);
    }
    U(slot) = cur;
    ++U(cur);
    for (uint32_t o : {0x2cu, 0x34u, 0x30u, 0x40u}) if (U(cur + o)) ++U(U(cur + o));
    ++U(U(cur + 0x4c) + 0xb4);
    if (old && U(old) == 0 && old != d2l_defLocinfo) FUN_6fa47882(old);
  }
  return U(slot);
}

// 1.13c D2Launch+0x7A45
void FUN_6fa47a45(void) { d2l_host.unlock(0xc); }

// 1.13c D2Launch+0x7A13
uint32_t FUN_6fa47a13(void) {
  d2l_host.lock(0xc);
  uint32_t r = FUN_6fa47952();
  FUN_6fa47a45();
  return r;
}

// 1.13c D2Launch+0x7A4E  (codepage in EAX -> LANGID)
uint32_t FUN_6fa47a4e(int32_t cp) {
  switch (cp) { case 0x3a4: return 0x411; case 0x3a8: return 0x804; case 0x3b5: return 0x412; case 0x3b6: return 0x404; }
  return 0;
}

// 1.13c D2Launch+0x7A7D
void FUN_6fa47a7d(void) {
  memset(d2l_mbctype, 0, sizeof d2l_mbctype);
  d2l_mbcp = 0; d2l_mbcpLast = 0; d2l_mbcFlag = 0; d2l_mbcA = 0; d2l_mbcB = 0; d2l_mbcC = 0;
}
