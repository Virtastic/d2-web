#include <cstdint>
#include <cstddef>
// Storm CRT batch 9ede. Retail globals/callees are seam params; 32-bit links are uint32_t.

// 1.13c Storm+0x9EDE: __validdrive. getDriveType = kernel32 GetDriveTypeA seam.
int Storm_9EDE(uint32_t drive, uint32_t (*getDriveType)(const char*)) {
  if (drive != 0) {
    char root[4] = {(char)(drive + '@'), ':', '\\', 0};
    uint32_t t = getDriveType(root);
    if (t == 0 || t == 1) return 0;
  }
  return 1;
}

// 1.13c Storm+0xA9F8: ___init_monetary. Seams: calloc (9198), malloc, free, getLocaleInfo (DEAA),
// freeLconvMon (A91F); globals: locUser/locAlt (DAT_..277c/2780), lcid (..27a8), curLconv (PTR_..07b4),
// defaultLconv (the 12-dword table at ..0784), outMon (..2748), outRef (..7b8c), outRef2 (..7b88).
struct StormMonetary {
  int *locUser, *locAlt;
  uint32_t lcid;
  uint32_t** curLconv;       // PTR_PTR_6fc407b4 (points at current lconv, 3 dwords read)
  uint32_t* defaultLconv;    // 12 dwords
  uint32_t** outMon;         // DAT_6fc42748
  uint32_t** outRef;         // DAT_6fc47b8c
  uint32_t** outRef2;        // DAT_6fc47b88
  void* (*calloc_)(uint32_t, uint32_t);
  void* (*malloc_)(uint32_t);
  void (*free_)(void*);
  int (*getLocaleInfo)(int isString, uint32_t lcid, uint32_t type, char* out);
  void (*freeLconvMon)(void*);
};
int Storm_A9F8(StormMonetary* g) {
  if (*g->locUser == 0 && *g->locAlt == 0) {
    *g->outRef2 = 0; *g->outRef = 0;
    *g->curLconv = g->defaultLconv;
    *g->outMon = 0;
    return 0;
  }
  uint32_t* m = (uint32_t*)g->calloc_(1, 0x30);
  if (!m) return 1;
  uint32_t* ref = (uint32_t*)g->malloc_(4);
  if (!ref) { g->free_(m); return 1; }
  *ref = 0;
  if (*g->locUser == 0) {
    for (int i = 0; i < 12; i++) m[i] = g->defaultLconv[i];
    *g->outRef2 = 0;
  } else {
    uint32_t* ref2 = (uint32_t*)g->malloc_(4);
    *g->outRef2 = ref2;
    if (!ref2) { g->free_(m); g->free_(ref); return 1; }
    *ref2 = 0;
    char* b = (char*)m;
    uint32_t l = g->lcid;
    int r = 0;
    r |= g->getLocaleInfo(1, l, 0x15, b + 12);
    r |= g->getLocaleInfo(1, l, 0x14, b + 16);
    r |= g->getLocaleInfo(1, l, 0x16, b + 20);
    r |= g->getLocaleInfo(1, l, 0x17, b + 24);
    r |= g->getLocaleInfo(1, l, 0x18, b + 28);
    r |= g->getLocaleInfo(1, l, 0x50, b + 32);
    r |= g->getLocaleInfo(1, l, 0x51, b + 36);
    r |= g->getLocaleInfo(0, l, 0x1a, b + 40);
    r |= g->getLocaleInfo(0, l, 0x19, b + 0x29);
    r |= g->getLocaleInfo(0, l, 0x54, b + 0x2a);
    r |= g->getLocaleInfo(0, l, 0x55, b + 0x2b);
    r |= g->getLocaleInfo(0, l, 0x56, b + 44);
    r |= g->getLocaleInfo(0, l, 0x57, b + 0x2d);
    r |= g->getLocaleInfo(0, l, 0x52, b + 0x2e);
    r |= g->getLocaleInfo(0, l, 0x53, b + 0x2f);
    if (r) { g->freeLconvMon(m); g->free_(m); g->free_(ref); return 1; }
    // the retail check is "all zero"; r is the OR of the returns so identical
    char* p = (char*)(uintptr_t)m[7];   // 32-bit link
    while (*p) {
      if (*p >= '0' && *p <= '9') { *p -= '0'; p++; }
      else if (*p != ';') p++;
      else for (char* q = p; (*q = q[1]); q++) {}
    }
  }
  m[0] = (*g->curLconv)[0]; m[1] = (*g->curLconv)[1]; m[2] = (*g->curLconv)[2];
  *g->curLconv = m;
  *g->outMon = m;
  *g->outRef = ref;
  return 0;
}

// 1.13c Storm+0xAE80: _strncpy (copy, then zero-pad to count).
char* Storm_AE80(char* dst, const char* src, size_t n) {
  size_t i = 0;
  for (; i < n && src[i]; i++) dst[i] = src[i];
  for (; i < n; i++) dst[i] = 0;
  return dst;
}

// 1.13c Storm+0xAFB0: _memset.
void* Storm_AFB0(void* d, int v, size_t n) {
  uint8_t* p = (uint8_t*)d;
  while (n--) *p++ = (uint8_t)v;
  return d;
}

// 1.13c Storm+0xB010: _memcpy (retail is overlap-safe: backward copy when src < dst < src+n).
void* Storm_B010(void* d, const void* s, size_t n) {
  uint8_t* dp = (uint8_t*)d; const uint8_t* sp = (const uint8_t*)s;
  if (sp < dp && dp < sp + n) { while (n--) dp[n] = sp[n]; }
  else for (size_t i = 0; i < n; i++) dp[i] = sp[i];
  return d;
}

// 1.13c Storm+0xB542: _LcidFromHexString (fastcall; ecx unused).
int Storm_B542(const char* s) {
  int v = 0;
  for (char c; (c = *s); s++) {
    if (c >= 'a' && c <= 'f') c -= 0x27;
    else if (c > '@' && c < 'G') c -= 7;
    v = (int)((uint32_t)v * 16u - 0x30u + (uint32_t)(int)c);   // (v + 0xffffffd) * 16 + c
  }
  return v;
}

// 1.13c Storm+0xB577: _GetPrimaryLen (fastcall; ecx unused): count of leading ASCII letters.
int Storm_B577(const char* s) {
  int n = 0;
  for (;; s++, n++) { char c = *s; if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) return n; }
}

// 1.13c Storm+0xBC60: _strlen.
size_t Storm_BC60(const char* s) { size_t n = 0; while (s[n]) n++; return n; }

// 1.13c Storm+0xBDA8: ___crtGetStringTypeA. Seams: dat2790 (DAT_..2790 state), codePageDef (..2778),
// charTypeDef (..2788), and the kernel32 / CRT callees.
struct StormStrType {
  int* state; uint32_t codePageDef; uint32_t charTypeDef;
  int (*getStringTypeW)(uint32_t info, const uint16_t* s, int n, uint16_t* out);
  uint32_t (*getLastError)();
  int (*getStringTypeA)(uint32_t lcid, uint32_t info, const char* s, int n, uint16_t* out);
  int (*multiByteToWideChar)(uint32_t cp, uint32_t flags, const char* s, int n, uint16_t* out, int outN);
  uint32_t (*d703)(uint32_t lcid);                                  // Storm+0xD703
  char* (*d74a)(uint32_t from, uint32_t to, const char* s, int* n); // Storm+0xD74A (alloc'd copy or null)
  void* (*calloc_)(uint32_t, uint32_t);
  void (*free_)(void*);
};
int Storm_BDA8(StormStrType* g, uint32_t infoType, const char* src, int cch, uint16_t* out,
               uint32_t codePage, int bError, uint32_t charType) {
  static const uint16_t kEmpty[1] = {0};
  if (*g->state == 0) {
    uint16_t t[2];
    if (g->getStringTypeW(1, kEmpty, 1, t) == 0) { if (g->getLastError() == 0x78) *g->state = 2; }
    else *g->state = 1;
  }
  if (*g->state == 2 || *g->state == 0) {
    uint32_t loc = codePage ? codePage : g->codePageDef;
    uint32_t ct = charType ? charType : g->charTypeDef;
    uint32_t cp = g->d703(loc);
    if (cp == 0xffffffffu) return 0;
    char* conv = nullptr;
    if (cp != ct) {
      int n = cch;
      conv = g->d74a(ct, cp, src, &n);
      if (!conv) return 0;
      src = conv; cch = n;
    }
    int r = g->getStringTypeA(loc, infoType, src, cch, out);
    if (conv) g->free_(conv);
    return r;
  }
  if (*g->state != 1) return 0;
  if (!charType) charType = g->charTypeDef;
  int n = g->multiByteToWideChar(charType, (bError != 0) * 8 + 1, src, cch, nullptr, 0);
  if (!n) return 0;
  uint16_t* w = (uint16_t*)g->calloc_(2, n);   // retail: stack buffer, heap fallback; same contents
  if (!w) return 0;
  int res = 0;
  if (g->multiByteToWideChar(charType, 1, src, cch, w, n)) res = g->getStringTypeW(infoType, w, n, out);
  g->free_(w);
  return res;
}

// 1.13c Storm+0xC031: __NLG_Notify1 (fastcall ecx = param; eax/ebp are the caller's).
void Storm_C031(uint32_t param, uint32_t eax, uint32_t ebp, uint32_t* d407d8, uint32_t* d407d4, uint32_t* d407dc) {
  *d407d8 = param; *d407d4 = eax; *d407dc = ebp;
}

// 1.13c Storm+0xC547: __lseek_lk. Seams: getOsfhandle (E149), setFilePointer, getLastError,
// dosmaperr (A074), setErrnoEBADF (A062 -> errno=9), pioinfo flags byte for this fd (4 + (fd&0x1f)*0x24).
int32_t Storm_C547(uint32_t fd, int32_t off, uint32_t whence, uint8_t* flagsByte,
                   intptr_t (*getOsfhandle)(uint32_t), uint32_t (*setFilePointer)(intptr_t, int32_t, uint32_t),
                   uint32_t (*getLastError)(), void (*dosmaperr)(uint32_t), int* errnoP) {
  intptr_t h = getOsfhandle(fd);
  if (h == -1) { *errnoP = 9; return -1; }
  uint32_t r = setFilePointer(h, off, whence);
  uint32_t err = (r == 0xffffffffu) ? getLastError() : 0;
  if (err == 0) { *flagsByte &= 0xfd; return (int32_t)r; }
  dosmaperr(err);
  return -1;
}

// 1.13c Storm+0xC5BB: __lseek. Seams: fdCount (DAT_..7b9c), flagsOf(fd) -> &pioinfo flags byte,
// lockFhandle (8A1C-style __lock_fhandle), unlock (C642), lseekLk (C547), errno (A062), doserrno (A06B).
int32_t Storm_C5BB(int fd, int32_t off, int origin, uint32_t fdCount, uint8_t* (*flagsOf)(int),
                   void (*lockFhandle)(int), void (*unlock)(), int32_t (*lseekLk)(int, int32_t, int),
                   int* errnoP, uint32_t* doserrnoP) {
  if ((uint32_t)fd < fdCount && (*flagsOf(fd) & 1)) {
    int32_t r;
    lockFhandle(fd);
    if ((*flagsOf(fd) & 1) == 0) { *errnoP = 9; *doserrnoP = 0; r = -1; }
    else r = lseekLk(fd, off, origin);
    unlock();
    return r;
  }
  *errnoP = 9; *doserrnoP = 0;
  return -1;
}
