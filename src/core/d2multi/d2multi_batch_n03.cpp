// D2Multi batch n03: statically linked MSVC CRT (mbcs tables, realloc, small-block heap, EH3 validation).
// Win32 services are reached through g_crt_host; retail's CRT globals are file-local state here.
// ponytail: small-block heap keeps retail's 32-bit in-heap layout (u32 links), so heap addresses must be < 4 GiB.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>

typedef uint32_t u32;
typedef uint8_t u8;
typedef uint16_t u16;

struct CrtCpInfo { u32 maxCharSize; u8 defaultChar[2]; u8 leadByte[12]; };
struct CrtMbi { uintptr_t base, allocBase; u32 allocProtect; size_t regionSize; u32 state, protect, type; };

struct CrtHost {
  bool (*getCPInfo)(u32 cp, CrtCpInfo*) = [](u32, CrtCpInfo*) { return false; };
  void (*getStringTypeA)(const char* s, int n, u16* out, u32 cp, u32 lcid) = nullptr;
  void (*lcMapStringA)(u32 lcid, u32 flags, const char* s, int n, char* d, int nd, u32 cp) = nullptr;
  int (*getLocaleInfoA)(u32 lcid, u32 type, char* buf, int n) = nullptr;
  int (*mbToWide)(u32 cp, const char* s, int n, u16* d, int nd) = nullptr;
  int (*wideToMb)(u32 cp, const u16* s, int n, char* d, int nd) = nullptr;
  u32 (*acp)() = []() -> u32 { return 1252; };
  u32 (*oemcp)() = []() -> u32 { return 437; };
  void* (*heapAlloc)(size_t) = [](size_t n) -> void* { return malloc(n); };
  void* (*heapReAlloc)(void*, size_t) = [](void* p, size_t n) -> void* { return realloc(p, n); };
  void (*heapFree)(void*) = [](void* p) { free(p); };
  void (*virtualFree)(uintptr_t, size_t, u32) = [](uintptr_t, size_t, u32) {};  // wasm: no page decommit
  size_t (*virtualQuery)(uintptr_t, CrtMbi*) = [](uintptr_t, CrtMbi*) -> size_t { return 0; };
  uintptr_t (*virtualAlloc)(uintptr_t, size_t, u32, u32) = [](uintptr_t, size_t, u32, u32) -> uintptr_t { return 0; };
  bool (*virtualProtect)(uintptr_t, size_t, u32, u32*) = [](uintptr_t, size_t, u32, u32*) { return false; };
  size_t (*pageSize)() = []() -> size_t { return 4096; };
  void (*messageBox)(const char* text, const char* title, u32 flags) = [](const char* t, const char*, u32) { fputs(t, stderr); };
  void (*exitProcess)(int) = [](int c) { exit(c); };
  const char* (*moduleFileName)() = []() -> const char* { return nullptr; };
  int (*callNewHandler)(size_t) = [](size_t) { return 0; };
  // 5 rows of 0x30 bytes at retail 0x6f9e99a8, and the 4 range masks at 0x6f9e99a0; supplied from retail's table data.
  const u8* cpTable = nullptr; u8 rangeMask[4] = {0, 0, 0, 0};
  // small-block-heap entry points owned by other rows
  int (*sbhResizeBlock)(u32* hdr, void* p, size_t n) = nullptr;
  void* (*sbhAllocBlock)(size_t n) = nullptr;
  uintptr_t stackLow = 0, stackHigh = 0;  // TIB StackLimit/StackBase for __ValidateEH3RN
};
CrtHost g_crt_host;

uintptr_t d2_multi_sub_687c(uintptr_t);
void d2_multi_sub_68a7(u32*, uintptr_t);
namespace {
u8 g_ctype[0x101];     // 0x6fa0a140 (index c+1)
u8 g_mapcase[0x100];   // 0x6fa0a260
u32 g_cp, g_mbcsFlag, g_lcid;  // a244, a130, a128
u16 g_ulinfo[6];       // a250
int g_userCp = 0, g_setByUser = 0;  // ebdb0, ebddc
u8* g_cpCache;         // a12c
int g_osPlatform = 2;  // eba84 (1 = Win9x)
void (*g_gsHandler)() = nullptr;  // ebde0
int g_newMode = 0;     // ebe38
int g_sbhMode = 0;     // b388 (3 = small-block heap)
// small-block heap
uintptr_t g_hdrs, g_hdrEnd_unused, g_cachedHdr, g_hdrCursor;  // a114, -, a10c, a11c
u32 g_hdrCount, g_threshold, g_a120, g_cachedGrp;             // a110, a118, a120, a124
// EH3 cache
u32 g_ehCount; u32 g_ehCache[16]; int g_ehLock;

inline u32* A(uintptr_t a) { return (u32*)a; }
inline u8* A8(uintptr_t a) { return (u8*)a; }
}

// 1.13c D2Multi+0x599E
static u32 d2_multi_sub_599e(u32 cp) {
  switch (cp) { case 0x3a4: return 0x411; case 0x3a8: return 0x804; case 0x3b5: return 0x412; case 0x3b6: return 0x404; }
  return 0;
}

static void setSBCS() {
  memset(g_ctype, 0, sizeof g_ctype);
  g_cp = g_mbcsFlag = g_lcid = 0;
  memset(g_ulinfo, 0, sizeof g_ulinfo);
}

// 1.13c D2Multi+0x59F6
void d2_multi_sub_59f6() {
  CrtCpInfo ci;
  if (g_crt_host.getCPInfo(g_cp, &ci)) {
    char src[256]; char lo[256], up[256]; u16 ty[256];
    for (int i = 0; i < 256; i++) src[i] = (char)i;
    src[0] = ' ';
    for (int i = 0; ci.leadByte[i] != 0; i += 2)
      for (u32 c = ci.leadByte[i]; c <= ci.leadByte[i + 1]; c++) src[c] = ' ';
    g_crt_host.getStringTypeA(src, 256, ty, g_cp, g_lcid);
    g_crt_host.lcMapStringA(g_lcid, 0x100, src, 256, lo, 256, g_cp);
    g_crt_host.lcMapStringA(g_lcid, 0x200, src, 256, up, 256, g_cp);
    for (int c = 0; c < 256; c++) {
      if (ty[c] & 1) { g_ctype[c + 1] |= 0x10; g_mapcase[c] = (u8)lo[c]; }
      else if (ty[c] & 2) { g_ctype[c + 1] |= 0x20; g_mapcase[c] = (u8)up[c]; }
      else g_mapcase[c] = 0;
    }
  } else {
    for (u32 c = 0; c < 256; c++) {
      if (c >= 0x41 && c <= 0x5a) { g_ctype[c + 1] |= 0x10; g_mapcase[c] = (u8)(c + 0x20); }
      else if (c > 0x60 && c < 0x7b) { g_ctype[c + 1] |= 0x20; g_mapcase[c] = (u8)(c - 0x20); }
      else g_mapcase[c] = 0;
    }
  }
}

// 1.13c D2Multi+0x5B91
int d2_multi_sub_5b91(u32 cp) {
  if (cp != 0) {
    for (int row = 0; row < 5; row++) {
      const u8* r = g_crt_host.cpTable + row * 0x30;
      u32 rcp; memcpy(&rcp, r, 4);
      if (rcp != cp) continue;
      memset(g_ctype, 0, sizeof g_ctype);
      for (int k = 0; k < 4; k++) {
        const u8* p = r + 0x10 + k * 8;
        for (; p[0] != 0 && p[1] != 0; p += 2)
          for (u32 c = p[0]; c <= p[1]; c++) g_ctype[c + 1] |= g_crt_host.rangeMask[k];
      }
      g_cp = cp; g_mbcsFlag = 1; g_lcid = d2_multi_sub_599e(cp);
      for (int i = 0; i < 3; i++) { u32 w; memcpy(&w, r + 4 + i * 4, 4); memcpy((u8*)g_ulinfo + i * 4, &w, 4); }
      d2_multi_sub_59f6();
      return 0;
    }
    CrtCpInfo ci;
    if (g_crt_host.getCPInfo(cp, &ci)) {
      memset(g_ctype, 0, sizeof g_ctype);
      g_cp = cp; g_lcid = 0;
      if (ci.maxCharSize < 2) g_mbcsFlag = 0;
      else {
        for (int i = 0; ci.leadByte[i] != 0 && ci.leadByte[i + 1] != 0; i += 2)
          for (u32 c = ci.leadByte[i]; c <= ci.leadByte[i + 1]; c++) g_ctype[c + 1] |= 4;
        for (u32 c = 1; c < 0xff; c++) g_ctype[c + 1] |= 8;
        g_lcid = d2_multi_sub_599e(cp);
        g_mbcsFlag = 1;
      }
      memset(g_ulinfo, 0, sizeof g_ulinfo);
      d2_multi_sub_59f6();
      return 0;
    }
    if (g_setByUser == 0) return -1;
  }
  setSBCS();
  d2_multi_sub_59f6();
  return 0;
}

// 1.13c D2Multi+0x5D25
int d2_multi_setmbcp(int cp) {  // __setmbcp; the _lock(0xd) pair is dropped (single-threaded wasm)
  int ret = -1;
  g_setByUser = 0;
  if (cp == -2) { g_setByUser = 1; cp = (int)g_crt_host.oemcp(); }
  else if (cp == -3) { g_setByUser = 1; cp = (int)g_crt_host.acp(); }
  else if (cp == -4) { g_setByUser = 1; cp = g_userCp; }
  if ((u32)cp == g_cp) return 0;
  u8* mem = g_cpCache;
  if (!mem || *(int*)mem != 0) mem = (u8*)malloc(0x220);
  if (mem && (ret = d2_multi_sub_5b91((u32)cp)) == 0) {
    memset(mem, 0, 4);
    memcpy(mem + 4, &g_cp, 4); memcpy(mem + 8, &g_mbcsFlag, 4); memcpy(mem + 12, &g_lcid, 4);
    memcpy(mem + 0x10, g_ulinfo, 10);
    memcpy(mem + 0x1c, g_ctype, 0x101);
    g_cpCache = mem;
    memcpy(mem + 0x11d, g_mapcase, 0x100);
  }
  if (ret == -1 && mem != g_cpCache) free(mem);
  return ret;
}

// 1.13c D2Multi+0x5EF9
void d2_multi_sub_5ef9(int kind) {  // __report_gsfailure-style fatal path
  if (g_gsHandler) { g_gsHandler(); g_crt_host.exitProcess(3); return; }
  const char* head = kind == 1 ? "Buffer overrun detected!" : "Unknown security failure detected!";
  const char* desc = kind == 1
    ? "A buffer overrun has been detected which has corrupted the program's\ninternal state.  The program cannot safely continue execution and must\nnow be terminated.\n"
    : "A security error of unknown cause has been detected which has\ncorrupted the program's internal state.  The program cannot safely\ncontinue execution and must now be terminated.\n";
  char prog[0x105] = {0};
  const char* mf = g_crt_host.moduleFileName();
  strncpy(prog, mf ? mf : "<program name unknown>", 0x104);
  char* p = prog;
  size_t n = strlen(p);
  if (n + 0xb > 0x3c) { p = prog + n - 0x31; strncpy(p, "...", 3); }
  std::string s = std::string(head) + "\n\nProgram: " + p + "\n\n" + desc;
  g_crt_host.messageBox(s.c_str(), "Microsoft Visual C++ Runtime Library", 0x12010);
  g_crt_host.exitProcess(3);
}

// 1.13c D2Multi+0x60DF
void* d2_multi_sub_60df(void* old, size_t size) {  // _realloc
  CrtHost& h = g_crt_host;
  if (!old) return h.heapAlloc(size);
  if (size == 0) { h.heapFree(old); return nullptr; }
  if (g_sbhMode == 3) {
    for (;;) {
      uintptr_t out = 0;
      if (size > 0xffffffe0u) { h.callNewHandler(size); return nullptr; }
      uintptr_t op = (uintptr_t)old;
      uintptr_t hdr = d2_multi_sub_687c(op);
      if (hdr) {
        if (size <= g_threshold) {
          if (h.sbhResizeBlock((u32*)hdr, old, size)) out = op;
          else {
            out = (uintptr_t)h.sbhAllocBlock(size);
            if (out) {
              size_t n = A(op)[-1] - 1; if (size <= n) n = size;
              memcpy((void*)out, old, n);
              d2_multi_sub_68a7((u32*)d2_multi_sub_687c(op), op);
            }
          }
        }
        if (!out) {
          size = (size + 0xf) & ~(size_t)0xf;
          out = (uintptr_t)h.heapAlloc(size);
          if (out) {
            size_t n = A(op)[-1] - 1; if (size <= n) n = size;
            memcpy((void*)out, old, n);
            d2_multi_sub_68a7((u32*)hdr, op);
          }
        }
      } else {
        size = (size + 0xf) & ~(size_t)0xf;
        out = (uintptr_t)h.heapReAlloc(old, size);
      }
      if (out) return (void*)out;
      if (!g_newMode) return nullptr;
      if (!h.callNewHandler(size)) return nullptr;
    }
  }
  for (;;) {
    if (size > 0xffffffe0u) { h.callNewHandler(size); return nullptr; }
    if (!size) size = 1;
    if (void* r = h.heapReAlloc(old, size)) return r;
    if (!g_newMode) return nullptr;
    if (!h.callNewHandler(size)) return nullptr;
  }
}

// 1.13c D2Multi+0x6307
int d2_multi_validate_eh3rn(void* reg) {  // __ValidateEH3RN
  u8* r = (u8*)reg;
  uintptr_t scope = A((uintptr_t)r)[2];
  u32 page, tryLevel = A((uintptr_t)r)[3];
  int i, idx, j;
  u32 cnt = 0, n = 0, prev;
  if ((scope & 3) != 0) return 0;
  if (g_crt_host.stackLow <= scope && scope < g_crt_host.stackHigh) return 0;
  if (tryLevel == 0xffffffff) return 1;
  {
    u32* e = A(scope);
    for (u32 lvl = 0; lvl <= tryLevel; lvl++, e += 3) {
      if (e[0] != 0xffffffff && lvl <= e[0]) return 0;
      if (e[1] != 0) cnt++;
    }
  }
  if (cnt != 0) { uintptr_t c = A((uintptr_t)r)[(size_t)-2]; if (c < g_crt_host.stackLow || (uintptr_t)r <= c) return 0; }
  page = (u32)(scope & 0xfffff000);
  idx = 0;
  for (i = 0; i < (int)g_ehCount; i++) {
    if (g_ehCache[i] != page) continue;
    if (i < 1) return 1;
    if (g_ehLock) return 1;
    g_ehLock = 1;
    idx = i;
    goto L6502;
  }
  {
    CrtMbi mbi;
    if (g_crt_host.virtualQuery(scope, &mbi) == 0) return -1;
    if (mbi.type != 0x1000000) return -1;
    if ((mbi.protect & 0xcc) != 0) {
      const u8* pe = (const u8*)mbi.allocBase;
      if (*(const u16*)pe != 0x5a4d) return -1;
      const u8* nt = pe + *(const u32*)(pe + 0x3c);
      if (*(const u32*)nt != 0x4550 || *(const u16*)(nt + 0x18) != 0x10b) return -1;
      u32 opt = *(const u16*)(nt + 0x14);
      if (*(const u16*)(nt + 6) == 0) return -1;
      u32 va = *(const u32*)(nt + opt + 0x24), sz = *(const u32*)(nt + opt + 0x20);
      u32 off = (u32)(scope - mbi.allocBase);
      if (va <= off && off < sz + va && (nt[opt + 0x3f] & 0x80)) return 0;
    }
  }
  if (g_ehLock) return 1;
  g_ehLock = 1;
  {
    u32 cur = g_ehCount; int k = (int)cur;
    for (; k > 0; k--) if (g_ehCache[k - 1] == page) break;  // retail scans &ebdec[count*4] downward
    if (k == 0) {
      int top = cur < 0x10 ? (int)cur : 0xf;
      for (j = 0; j <= top; j++) { prev = g_ehCache[j]; g_ehCache[j] = page; page = prev; }
      if (cur < 0x10) g_ehCount = cur + 1;
    }
  }
  g_ehLock = 0;
  return 1;
L6502:
  for (j = 0; j <= idx; j++) { prev = g_ehCache[j]; g_ehCache[j] = page; page = prev; }
  g_ehLock = 0;
  (void)n; (void)cnt;
  return 1;
}

// dup of D2Multi+0x6530 (bound in b8)
long d2_multi_sub_6530(u32 lcid) {
  char buf[7] = {0};
  if (g_crt_host.getLocaleInfoA(lcid, 0x1004, buf, 6) == 0) return -1;
  return atol(buf);
}

// dup of D2Multi+0x6577 (bound in b8)
char* d2_multi_sub_6577(u32 cpIn, u32 cpOut, const char* src, u32* pLen, char* out, int outLen) {
  CrtHost& h = g_crt_host;
  char* res = nullptr;
  u32 cb = *pLen;
  if (cpIn == cpOut) return nullptr;
  bool sbcs = false;
  CrtCpInfo ci;
  if (h.getCPInfo(cpIn, &ci) && ci.maxCharSize == 1 && h.getCPInfo(cpOut, &ci) && ci.maxCharSize == 1) sbcs = true;
  int nw;
  if (sbcs) nw = (cb == 0xffffffff) ? (int)strlen(src) + 1 : (int)cb;
  else if ((nw = h.mbToWide(cpIn, src, (int)cb, nullptr, 0)) == 0) return nullptr;
  std::vector<u16> wide((size_t)nw, 0);
  if (h.mbToWide(cpIn, src, (int)cb, wide.data(), nw) == 0) return nullptr;
  if (!out) {
    int nb = nw;
    if (!sbcs && (nb = h.wideToMb(cpOut, wide.data(), nw, nullptr, 0)) == 0) return nullptr;
    res = (char*)calloc(1, (size_t)nb);
    if (!res) return nullptr;
    int got = h.wideToMb(cpOut, wide.data(), nw, res, nb);
    if (got == 0) { free(res); res = nullptr; }
    else if (cb != 0xffffffff) *pLen = (u32)got;
  } else if (h.wideToMb(cpOut, wide.data(), nw, out, outLen) != 0) res = out;
  return res;
}

// 1.13c D2Multi+0x6751
int d2_multi_resetstkoflw(uintptr_t sp) {  // __resetstkoflw; sp = current stack address (retail used &local)
  CrtHost& h = g_crt_host;
  CrtMbi mbi;
  if (h.virtualQuery(sp, &mbi) == 0) return 0;
  size_t pg = h.pageSize();
  uintptr_t guard = (sp & ~(uintptr_t)(pg - 1)) - pg;
  uintptr_t floor_ = mbi.allocBase + (g_osPlatform != 1 ? 2 : 0x11) * pg;
  if (floor_ > guard) return 0;
  uintptr_t at = guard;
  if (g_osPlatform != 1) {
    at = mbi.allocBase;
    do {
      if (h.virtualQuery(at, &mbi) == 0) return 0;
      at += mbi.regionSize;
    } while ((mbi.state & 0x1000) == 0);
    at = mbi.base;
    if ((mbi.protect >> 8) & 1) return 1;
    if (guard < mbi.base) return 0;
    if (mbi.base < floor_) at = floor_;
    h.virtualAlloc(at, pg, 0x1000, 4);
  }
  u32 old;
  return h.virtualProtect(at, pg, g_osPlatform != 1 ? 0x104 : 1, &old);
}

// 1.13c D2Multi+0x6834
int d2_multi_sbh_heap_init(u32 threshold) {  // ___sbh_heap_init
  g_hdrs = (uintptr_t)g_crt_host.heapAlloc(0x140);
  if (!g_hdrs) return 0;
  g_cachedHdr = 0; g_hdrCount = 0; g_hdrCursor = g_hdrs; g_threshold = threshold; g_a120 = 0x10;
  return 1;
}

// 1.13c D2Multi+0x687C
uintptr_t d2_multi_sub_687c(uintptr_t p) {  // ___sbh_find_block
  for (uintptr_t h = g_hdrs; h < g_hdrs + g_hdrCount * 0x14; h += 0x14)
    if ((u32)(p - A(h)[3]) < 0x100000) return h;
  return 0;
}

// 1.13c D2Multi+0x68A7
void d2_multi_sub_68a7(u32* hdr, uintptr_t blk) {  // ___sbh_free_block
  CrtHost& h = g_crt_host;
  uintptr_t reg = hdr[4];
  uintptr_t pb = blk - 4;
  u32 grp = (u32)(blk - hdr[3]) >> 15;
  uintptr_t gp = grp * 0x204 + 0x144 + reg;
  u32 sz = *A(pb) - 1;
  if (sz & 1) return;
  uintptr_t nx = sz + pb;
  u32 nsz = *A(nx), psz = *A(blk - 8);
  auto clear = [&](u32 i) {  // drop one entry from size-class i; mask words follow retail's two halves
    u8* c = A8(i + 4 + reg);
    if (i < 0x20) { u32 m = ~(0x80000000u >> (i & 31)); *A(reg + 0x44 + grp * 4) &= m; if (--*c == 0) hdr[0] &= m; }
    else { u32 m = ~(0x80000000u >> ((i - 0x20) & 31)); *A(reg + 0xc4 + grp * 4) &= m; if (--*c == 0) hdr[1] &= m; }
  };
  auto cls = [](u32 s) { u32 i = ((int)s >> 4) - 1; return i > 0x3f ? 0x3fu : i; };
  if (!(nsz & 1)) {
    u32 i = cls(nsz);
    if (A(nx)[1] == A(nx)[2]) clear(i);
    sz += nsz;
    *A(A(nx)[2] + 4) = A(nx)[1];
    *A(A(nx)[1] + 8) = A(nx)[2];
  }
  u32 i2 = cls(sz), i1 = 0xffffffff;
  if (!(psz & 1)) {
    pb -= psz;
    i1 = cls(psz);
    sz += psz;
    i2 = cls(sz);
    if (i1 != i2) {
      if (A(pb)[1] == A(pb)[2]) clear(i1);
      *A(A(pb)[2] + 4) = A(pb)[1];
      *A(A(pb)[1] + 8) = A(pb)[2];
    }
  }
  if ((psz & 1) || i1 != i2) {
    uintptr_t head = gp + i2 * 8;
    u32 last = A(head)[1];
    A(pb)[2] = (u32)head; A(pb)[1] = last;
    A(head)[1] = (u32)pb;
    *A(A(pb)[1] + 8) = (u32)pb;
    if (A(pb)[1] == A(pb)[2]) {
      u8 c = *A8(i2 + reg + 4); *A8(i2 + reg + 4) = c + 1;
      if (i2 < 0x20) { u32 m = 0x80000000u >> (i2 & 31); if (c == 0) hdr[0] |= m; *A(reg + 0x44 + grp * 4) |= m; }
      else { u32 m = 0x80000000u >> ((i2 - 0x20) & 31); if (c == 0) hdr[1] |= m; *A(reg + 0xc4 + grp * 4) |= m; }
    }
  }
  *A(pb) = sz;
  *A(sz - 4 + pb) = sz;
  if (--*A(gp) == 0) {
    if (g_cachedHdr) {
      u32* c = A(g_cachedHdr);
      h.virtualFree(g_cachedGrp * 0x8000 + c[3], 0x8000, 0x4000);
      c[2] |= 0x80000000u >> (g_cachedGrp & 31);
      *A(c[4] + 0xc4 + g_cachedGrp * 4) = 0;
      u8* cnt = A8(c[4] + 0x43); --*cnt;
      if (*cnt == 0) c[1] &= 0xfffffffe;
      if (c[2] == 0xffffffff) {
        h.virtualFree(c[3], 0, 0x8000);
        h.heapFree((void*)(uintptr_t)c[4]);
        memmove(c, c + 5, g_hdrCount * 0x14 - (g_cachedHdr - g_hdrs) - 0x14);
        g_hdrCount--;
        if (g_cachedHdr < (uintptr_t)hdr) hdr -= 5;
        g_hdrCursor = g_hdrs;
      }
    }
    g_cachedHdr = (uintptr_t)hdr;
    g_cachedGrp = grp;
  }
}
