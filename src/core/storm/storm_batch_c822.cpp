// Storm batch c822: MSVC CRT helpers statically linked into Storm.dll 1.13c.
// Retail globals/callees are seam params; 32-bit retail pointers are uint32_t.
#include <cstdint>
typedef uint32_t u32;
typedef int32_t i32;

// 1.13c Storm+0xC822: _write(fh, buf, n). Seams: fh table, errno/doserrno, lock, raw write.
struct StormWriteSeam {
  u32 nhandles;                       // DAT_6fc47b9c
  uint8_t* (*fhFlags)(int fh);        // &(table[fh>>5] + (fh&0x1f)*0x24 + 4)
  void (*lockFh)(int);                // __lock_fhandle
  void (*unlockFh)();                 // FUN_6fbfc8a9
  int* errnoPtr; unsigned long* doserrnoPtr;  // FUN_6fbfa062 / FUN_6fbfa06b
  int (*rawWrite)(int, const void*, u32);     // FUN_6fbfc666
};
int Storm_C822(const StormWriteSeam& s, int fh, const void* buf, u32 n) {
  if ((u32)fh < s.nhandles && (*s.fhFlags(fh) & 1)) {
    s.lockFh(fh);
    int r;
    if ((*s.fhFlags(fh) & 1) == 0) { *s.errnoPtr = 9; *s.doserrnoPtr = 0; r = -1; }
    else r = s.rawWrite(fh, buf, n);
    s.unlockFh();
    return r;
  }
  *s.errnoPtr = 9; *s.doserrnoPtr = 0;
  return -1;
}

// 1.13c Storm+0xC8CD: __getbuf(FILE*). 32-bit FILE: _ptr,_cnt,_base,_flag,_file,_charbuf,_bufsiz.
struct StormFile32 { u32 _ptr; i32 _cnt; u32 _base; i32 _flag; i32 _file; i32 _charbuf; i32 _bufsiz; };
// malloc returns a 32-bit address; fileAddr = address of f (for &_charbuf); DAT_6fc427bc is the counter.
void Storm_C8CD(StormFile32* f, u32 fileAddr, i32* bufCount, u32 (*malloc32)(u32)) {
  ++*bufCount;
  u32 p = malloc32(0x1000);
  f->_base = p;
  if (p == 0) { f->_flag |= 4; f->_base = fileAddr + 0x14; f->_bufsiz = 2; }
  else        { f->_flag |= 8; f->_bufsiz = 0x1000; }
  f->_cnt = 0;
  f->_ptr = f->_base;
}

// 1.13c Storm+0xCA9C: ___wctomb_mt(locinfo, dst, wc). lc: +4 codepage, +0x14 flag, +0x28 mb_cur_max.
struct StormWctombSeam {
  int* errnoPtr;
  int (*wideCharToMultiByte)(u32 cp, const uint16_t* w, char* dst, int dstLen, int* usedDefault);
};
int Storm_CA9C(const StormWctombSeam& s, const u32* lc /*dwords*/, char* dst, uint16_t wc) {
  if (!dst) return 0;
  if (lc[0x14 / 4] == 0) {
    if (wc < 0x100) { *dst = (char)wc; return 1; }
  } else {
    int used = 0;
    int n = s.wideCharToMultiByte(lc[4 / 4], &wc, dst, (int)lc[0x28 / 4], &used);
    if (n != 0 && used == 0) return n;
  }
  *s.errnoPtr = 0x2a;
  return -1;
}

// 1.13c Storm+0xCB30: __aulldvrm(a_lo,a_hi,b_lo,b_hi) -> {quot (EDX:EAX), rem (ECX:EBX) via out}.
// Decompile returns only the quotient and drops the remainder registers; remainder computed here
// from the same algorithm's contract (a - q*b).
uint64_t Storm_CB30(u32 lo, u32 hi, u32 dlo, u32 dhi, uint64_t* rem) {
  uint64_t a = ((uint64_t)hi << 32) | lo, b = ((uint64_t)dhi << 32) | dlo, q;
  if (dhi == 0) q = a / dlo;
  else {
    u32 d = dlo, dh = dhi, n = lo, nh = hi;
    do { d = (d >> 1) | ((dh & 1) << 31); dh >>= 1; n = (n >> 1) | ((nh & 1) << 31); nh >>= 1; } while (dh);
    q = (((uint64_t)nh << 32) | n) / d & 0xffffffffu;
    uint64_t prod = b * q;
    if ((prod >> 32) > hi || ((prod >> 32) == hi && (u32)prod > lo) || prod < (uint64_t)dlo * q) q--;
  }
  if (rem) *rem = a - q * b;
  return q;
}

// Small-block-heap globals shared by CBC5 / CC38 (DAT_6fc4690c.. etc).
struct StormSbh {
  u32 heap;      // DAT_6fc47b94
  u32 regions;   // DAT_6fc4690c
  u32 cur;       // DAT_6fc46904 (region ptr)
  u32 count;     // DAT_6fc46908
  u32 sizeArg;   // DAT_6fc46910
  u32 end;       // DAT_6fc46914
  u32 cap;       // DAT_6fc46918
  u32 curGroup;  // DAT_6fc4691c
};
// 1.13c Storm+0xCBC5: ___sbh_heap_init(sizeArg)
int Storm_CBC5(StormSbh& g, u32 sizeArg, u32 (*heapAlloc)(u32 heap, u32 flags, u32 n)) {
  g.regions = heapAlloc(g.heap, 0, 0x140);
  if (g.regions == 0) return 0;
  g.cur = 0; g.count = 0; g.end = g.regions; g.sizeArg = sizeArg; g.cap = 0x10;
  return 1;
}

// 1.13c Storm+0xCC38: ___sbh_free_block(region, block). P(a) maps a 32-bit address to host memory.
struct StormFreeSeam {
  u32* (*P)(u32 addr);
  void (*virtualFree)(u32 addr, u32 size, u32 type);
  void (*heapFree)(u32 heap, u32 flags, u32 p);
  void (*memmove32)(u32 dst, u32 src, u32 n);
};
void Storm_CC38(StormSbh& g, const StormFreeSeam& s, u32 region, u32 block) {
  auto W = [&](u32 a) -> u32& { return *s.P(a); };
  auto B = [&](u32 a) -> uint8_t& { return *(uint8_t*)s.P(a); };
  u32 grp = W(region + 0x10);
  u32 hdr = block - 4;
  u32 idx = (block - W(region + 0xc)) >> 15;
  u32 pg = idx * 0x204 + 0x144 + grp;
  u32 sz = W(hdr) - 1;
  if (sz & 1) return;  // decompile: whole body is inside (local_8 & 1) == 0
  u32 nx = sz + hdr;
  u32 nxv = W(nx);
  u32 prevv = W(block - 8);
  auto unlinkBin = [&](u32 bin) {  // clear bitmap bit, drop count, clear region summary
    if (bin < 0x20) {
      u32 m = ~(0x80000000u >> (bin & 0x1f));
      W(grp + 0x44 + idx * 4) &= m;
      if (--B(bin + 4 + grp) == 0) W(region) &= m;
    } else {
      u32 m = ~(0x80000000u >> ((bin - 0x20) & 0x1f));
      W(grp + 0xc4 + idx * 4) &= m;
      if (--B(bin + 4 + grp) == 0) W(region + 4) &= m;
    }
  };
  if ((nxv & 1) == 0) {
    u32 bin = ((i32)nxv >> 4) - 1; if (bin > 0x3f) bin = 0x3f;
    if (W(nx + 4) == W(nx + 8)) unlinkBin(bin);
    sz += nxv;
    W(W(nx + 8) + 4) = W(nx + 4);
    W(W(nx + 4) + 8) = W(nx + 8);
  }
  u32 binN = ((i32)sz >> 4) - 1; if (binN > 0x3f) binN = 0x3f;
  u32 binP = region;  // decompile reuses the pointer var; only compared when prev is free
  if ((prevv & 1) == 0) {
    hdr -= prevv;
    binP = ((i32)prevv >> 4) - 1; if (binP > 0x3f) binP = 0x3f;
    sz += prevv;
    binN = ((i32)sz >> 4) - 1; if (binN > 0x3f) binN = 0x3f;
    if (binP != binN) {
      if (W(hdr + 4) == W(hdr + 8)) unlinkBin(binP);
      W(W(hdr + 8) + 4) = W(hdr + 4);
      W(W(hdr + 4) + 8) = W(hdr + 8);
    }
  }
  if ((prevv & 1) != 0 || binP != binN) {
    u32 head = pg + binN * 8;
    u32 t = W(head + 4);
    W(hdr + 8) = head; W(hdr + 4) = t;
    W(head + 4) = hdr;
    W(W(hdr + 4) + 8) = hdr;
    if (W(hdr + 4) == W(hdr + 8)) {
      uint8_t c = B(binN + grp + 4)++;
      u32 bit = binN < 0x20 ? 0x80000000u >> (binN & 0x1f) : 0x80000000u >> ((binN - 0x20) & 0x1f);
      if (binN < 0x20) { if (c == 0) W(region) |= bit; W(grp + 0x44 + idx * 4) |= bit; }
      else             { if (c == 0) W(region + 4) |= bit; W(grp + 0xc4 + idx * 4) |= bit; }
    }
  }
  W(hdr) = sz;
  W(sz - 4 + hdr) = sz;
  i32& used = *(i32*)s.P(pg);
  if (--used == 0) {
    if (g.cur != 0) {
      s.virtualFree(g.curGroup * 0x8000 + W(g.cur + 12), 0x8000, 0x4000);
      W(g.cur + 8) |= 0x80000000u >> (g.curGroup & 0x1f);
      W(W(g.cur + 16) + 0xc4 + g.curGroup * 4) = 0;
      if (--B(W(g.cur + 16) + 0x43) == 0) W(g.cur + 4) &= 0xfffffffe;
      if (W(g.cur + 8) == 0xffffffff) {
        s.virtualFree(W(g.cur + 12), 0, 0x8000);
        s.heapFree(g.heap, 0, W(g.cur + 16));
        s.memmove32(g.cur, g.cur + 20, (g.count * 0x14 - g.cur) - 0x14 + g.regions);
        g.count--;
        if (g.cur < region) region -= 5 * 4;  // param_1 - 5 uint* = -20 bytes
        g.end = g.regions;
      }
    }
    g.cur = region;
    g.curGroup = idx;
  }
}

// 1.13c Storm+0xD924: __ValidateEH3RN(frame). Self = security-cookie-era module range at Self+4/+8;
// cache = DAT_6fc427d0[16] (page bases), cacheN = DAT_6fc427c8, lock = DAT_6fc42810.
// NOTE: decompile lists the PE-header read through local_24.AllocationBase; ported as seam peHdr().
struct StormEh3Seam {
  u32 selfLo, selfHi;                       // *(Self+8), *(Self+4)
  u32* cache; i32* cacheN; volatile i32* lock;
  u32 (*rd)(u32 addr);                      // 32-bit memory read
  int (*virtualQuery)(u32 addr, u32* allocBase, u32* type, u32* protect);
  int (*xchg)(volatile i32*, i32);
};
int Storm_D924(const StormEh3Seam& s, u32 frame) {
  u32 tbl = s.rd(frame + 8);
  if ((tbl & 3) || (s.selfLo <= tbl && tbl < s.selfHi)) return 0;
  u32 top = s.rd(frame + 0xc);
  if (top == 0xffffffff) return 1;
  u32 i = 0, nz = 0, p = tbl;
  do {
    u32 v = s.rd(p);
    if (v != 0xffffffff && i <= v) return 0;
    if (s.rd(p + 4) != 0) nz++;
    i++; p += 12;
  } while (i <= top);
  if (nz) { u32 prev = s.rd(frame - 8); if (prev < s.selfLo || frame <= prev) return 0; }
  u32 page = tbl & 0xfffff000;
  int k = 0;
  auto promote = [&](int upto) {  // move cache[upto] to the front (rotate)
    for (int j = 0; j <= upto; j++) { u32 t = s.cache[j]; s.cache[j] = page; page = t; }
  };
  for (; k < *s.cacheN; k++) {
    if (s.cache[k] != page) continue;
    if (k < 1) return 1;
    if (s.xchg(s.lock, 1) != 0) return 1;
    if (s.cache[k] == page) { promote(k); s.xchg(s.lock, 0); return 1; }
    int m = *s.cacheN - 1;
    while (m >= 0 && s.cache[m] != page) m--;
    if (m < 0) { if (*s.cacheN < 0x10) ++*s.cacheN; m = *s.cacheN - 1; }
    else if (m == 0) { s.xchg(s.lock, 0); return 1; }
    promote(m); s.xchg(s.lock, 0); return 1;
  }
  u32 ab, type, prot;
  if (!s.virtualQuery(tbl, &ab, &type, &prot)) return -1;
  if (type != 0x1000000) return -1;
  if (prot & 0xcc) {
    u32 nt = s.rd(ab + 0x3c) + ab;
    if ((s.rd(ab) & 0xffff) != 0x5a4d || s.rd(nt) != 0x4550 || (s.rd(nt + 0x18) & 0xffff) != 0x10b) return -1;
    u32 opt = s.rd(nt + 0x14) & 0xffff;
    if ((s.rd(nt + 4) >> 16) == 0) return -1;  // NumberOfSections (short at +6)
    u32 rva = s.rd(nt + opt + 0x24), len = s.rd(nt + opt + 0x20);
    if (rva <= tbl - ab && tbl - ab < len + rva && (s.rd(nt + opt + 0x3c) >> 24 & 0x80)) return 0;
  }
  if (s.xchg(s.lock, 1) != 0) return 1;
  int n = *s.cacheN, q = n;
  while (q > 0 && s.cache[q - 1] != page) q--;  // &cache[-1+n]: DAT_6fc427cc + n*4
  if (q == 0) {
    int lim = n < 0x10 ? n : 0xf;
    for (int j = 0; j <= lim; j++) { u32 t = s.cache[j]; s.cache[j] = page; page = t; }
    if (n < 0x10) *s.cacheN = n + 1;
  }
  s.xchg(s.lock, 0);
  return 1;
}

// 1.13c Storm+0xDB4D: ___crtInitCritSecNoSpinCount@8
int Storm_DB4D(void* cs, void (*initCS)(void*)) { initCS(cs); return 1; }

// 1.13c Storm+0xDCE1: _abort()
void Storm_DCE1(void (*sub9BA1)(int), int (*raise)(int), void (*exit_)(int)) {
  sub9BA1(10); raise(0x16); exit_(3);
}

// 1.13c Storm+0xDFDA: _atol. Locale ctype path seams: isspace(c)->bool, digit test is inline.
long Storm_DFDA(const char* s, bool (*isspace_)(int)) {
  while (isspace_((uint8_t)*s)) s++;
  u32 c = (uint8_t)*s; const uint8_t* p = (const uint8_t*)s + 1; u32 sign = c;
  if (c == '-' || c == '+') { c = *p; p++; }
  i32 v = 0;
  while (c >= '0' && c <= '9') { v = (i32)(c - '0') + v * 10; c = *p++; }
  return sign == '-' ? -v : v;
}

// 1.13c Storm+0xE50A: _raise(sig). Seam carries the handler slots and thread data accessors.
// The decompile does not show siglookup()'s internals, __lock(0)/FUN_6fbfe645 (unlock) bodies, nor
// the _XCPT_ACTION table walk contents beyond the zeroing loop; they are seams here.
struct StormRaiseSeam {
  void (**slot)(int, int);                 // per-signal handler slot: SIGINT f0, SIGBREAK f4, SIGABRT f8, SIGTERM fc
  void (**(*siglookup)(int))(int, int);    // siglookup(sig)+8 (SIGSEGV/ILL/FPE)
  void (*lock)(int); void (*unlock)(int);
  void (*exit_)(int);
  void** initarg; void** pxcptacttab;      // _tiddata fields (thread data)
  void (*zeroFpeActions)();                // for (i=DAT_6fc40480; i<DAT_6fc40484+DAT_6fc40480; i++) tab[i*0xc+8]=0
  void* slotInt[4];
};
// Handler is called as h(sig) or h(8, fpecode); void(*)(int,int) stands in for both.
int Storm_E50A(const StormRaiseSeam& s, int sig, void (**slotInt)(int, int), void (**slotBrk)(int, int),
               void (**slotAbrt)(int, int), void (**slotTerm)(int, int), void (**slotXcpt)(int, int)) {
  bool lockIt = false; void (**slot)(int, int); void (*h)(int, int);
  if (sig == 2) { slot = slotInt; lockIt = true; }
  else if (sig != 4 && sig != 8 && sig != 0xb) {
    if (sig == 0xf) slot = slotTerm;
    else if (sig == 0x15) slot = slotBrk;
    else if (sig == 0x16) slot = slotAbrt;
    else return -1;
    lockIt = true;
  } else slot = slotXcpt;  // siglookup(sig)+8
  h = *slot;
  if ((uintptr_t)h == 1) return 0;
  if (!h) { s.exit_(3); return 0; }
  if (lockIt) s.lock(0);
  void* savedInit = nullptr; void* savedAct = nullptr;
  if (sig == 8 || sig == 0xb || sig == 4) {
    savedInit = *s.initarg; *s.initarg = nullptr;
    if (sig == 8) { savedAct = *s.pxcptacttab; *s.pxcptacttab = (void*)0x8c; s.zeroFpeActions(); }
    else *slot = nullptr;
  } else *slot = nullptr;
  s.unlock(0);
  if (sig == 8) h(8, (int)(intptr_t)*s.pxcptacttab);
  else { h(sig, 0); if (sig != 0xb && sig != 4) return 0; }
  *s.initarg = savedInit;
  if (sig == 8) *s.pxcptacttab = savedAct;
  return 0;
}

// 1.13c Storm+0xE683: _isdigit(c). Single-byte-locale path: ctype table word & 4; multibyte path via seam.
int Storm_E683(int c, int mbCurMax, const uint16_t* ctype, int (*isctype_mt)(int c, int mask)) {
  if (mbCurMax > 1) return isctype_mt(c, 4);
  return ctype[c] & 4;
}
