// D2Launch MSVC CRT glue (mbcp, message box, isctype, chsize, memmove, locale frees).
// Host seams (GetCPInfo, GetStringType, LCMapString, MessageBox, lseek, free...) are passed as args.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>

struct CpInfo { uint32_t maxCharSize; uint8_t leadByte[12]; };
struct MbcState {  // DAT_6fa66d20.. block
  uint32_t codePage, isMulti, locale, ctypeFlag;  // 66e44 / 66d28 / 66d20 / 6511c
  uint16_t cp[5];                                  // 66e50..58
  uint8_t ctype[257];                              // 66d40, index+1
  uint8_t lower[256];                              // 66e60 (case map)
};
struct MbcHost {
  bool (*getCPInfo)(uint32_t cp, CpInfo* out);
  void (*getStringType)(const uint8_t* s, int n, uint16_t* out, uint32_t cp, uint32_t lcid);
  void (*mapString)(uint32_t lcid, uint32_t flags, const uint8_t* s, int n, uint8_t* out, uint32_t cp);
  uint32_t (*getLocale)(void);  // FUN_6fa47a4e
  void (*setSBCS)(void);
};
struct CpTableEntry { uint32_t cp; uint32_t pad; uint8_t ranges[4][8]; uint32_t w0, w1, w2; };  // 0x30 bytes

// 1.13c D2Launch+0x7AA6
void FUN_6fa47aa6(MbcState* s, const MbcHost* h) {
  CpInfo ci;
  if (h->getCPInfo(s->codePage, &ci)) {
    uint8_t buf[256];
    for (int i = 0; i < 256; i++) buf[i] = (uint8_t)i;
    buf[0] = ' ';
    for (const uint8_t* p = ci.leadByte; p[0] && p[1]; p += 2)
      for (uint32_t c = p[0]; c <= p[1]; c++) buf[c] = ' ';
    uint16_t t[256]; uint8_t up[256], lo[256];
    h->getStringType(buf, 256, t, s->codePage, s->locale);
    h->mapString(s->locale, 0x100, buf, 256, up, s->codePage);
    h->mapString(s->locale, 0x200, buf, 256, lo, s->codePage);
    for (int i = 0; i < 256; i++) {
      if (t[i] & 1) { s->ctype[i + 1] |= 0x10; s->lower[i] = up[i]; }
      else if (t[i] & 2) { s->ctype[i + 1] |= 0x20; s->lower[i] = lo[i]; }
      else s->lower[i] = 0;
    }
  } else {
    for (int i = 0; i < 256; i++) {
      if (i >= 0x41 && i <= 0x5a) { s->ctype[i + 1] |= 0x10; s->lower[i] = (uint8_t)(i + 0x20); }
      else if (i >= 0x61 && i <= 0x7a) { s->ctype[i + 1] |= 0x20; s->lower[i] = (uint8_t)(i - 0x20); }
      else s->lower[i] = 0;
    }
  }
}

// 1.13c D2Launch+0x7C41
int32_t FUN_6fa47c41(MbcState* s, const MbcHost* h, const CpTableEntry* tbl, uint32_t cp) {
  static const uint8_t kFlag[4] = {4, 4, 4, 4};  // DAT_6fa60898 per-range flags; seam: table data
  if (cp != 0) {
    for (int e = 0; e < 5; e++) {
      if (tbl[e].cp != cp) continue;
      memset(s->ctype, 0, sizeof s->ctype);
      for (int r = 0; r < 4; r++)
        for (const uint8_t* p = tbl[e].ranges[r]; p[0] && p[1]; p += 2)
          for (uint32_t c = p[0]; c <= p[1]; c++) s->ctype[c + 1] |= kFlag[r];
      s->codePage = cp; s->isMulti = 1; s->locale = h->getLocale();
      s->cp[0] = (uint16_t)tbl[e].w0; s->cp[1] = (uint16_t)tbl[e].w1; s->cp[2] = (uint16_t)tbl[e].w2;
      FUN_6fa47aa6(s, h);
      return 0;
    }
    CpInfo ci;
    if (h->getCPInfo(cp, &ci)) {
      memset(s->ctype, 0, sizeof s->ctype);
      s->codePage = cp; s->locale = 0;
      if (ci.maxCharSize < 2) s->isMulti = 0;
      else {
        for (const uint8_t* p = ci.leadByte; p[0] && p[1]; p += 2)
          for (uint32_t c = p[0]; c <= p[1]; c++) s->ctype[c + 1] |= 4;
        for (uint32_t c = 1; c < 0xff; c++) s->ctype[c + 1] |= 8;
        s->locale = h->getLocale();
        s->isMulti = 1;
      }
      s->cp[0] = s->cp[1] = s->cp[2] = 0;
      FUN_6fa47aa6(s, h);
      return 0;
    }
    if (s->ctypeFlag == 0) return -1;
  }
  h->setSBCS();
  FUN_6fa47aa6(s, h);
  return 0;
}

// 1.13c D2Launch+0x7F1C
void FUN_6fa47f1c(void (*unlock)(int)) { unlock(0xd); }

// 1.13c D2Launch+0x7DD5
int32_t __setmbcp(MbcState* s, const MbcHost* h, const CpTableEntry* tbl, uint8_t** saved,
                  int32_t cp, uint32_t oemcp, uint32_t acp, uint32_t cachedAcp, void (*lock)(int),
                  void (*unlock)(int)) {
  int32_t r = -1;
  lock(0xd);
  s->ctypeFlag = 0;
  if (cp == -2) { s->ctypeFlag = 1; cp = (int32_t)oemcp; }
  else if (cp == -3) { s->ctypeFlag = 1; cp = (int32_t)acp; }
  else if (cp == -4) { s->ctypeFlag = 1; cp = (int32_t)cachedAcp; }
  if ((uint32_t)cp == s->codePage) r = 0;
  else {
    uint8_t* m = *saved;
    if (!m || *(int32_t*)m != 0) m = (uint8_t*)malloc(0x220);
    if (m && (r = FUN_6fa47c41(s, h, tbl, (uint32_t)cp)) == 0) {
      *(int32_t*)m = 0;
      ((uint32_t*)m)[1] = s->codePage; ((uint32_t*)m)[2] = s->isMulti; ((uint32_t*)m)[3] = s->locale;
      memcpy(m + 0x10, s->cp, 10);
      memcpy(m + 0x1c, s->ctype, 0x101);
      memcpy(m + 0x11d, s->lower, 0x100);
      *saved = m;
    }
    if (r == -1 && m != *saved) free(m);
  }
  FUN_6fa47f1c(unlock);
  return r;
}

// 1.13c D2Launch+0x8058
struct MsgBoxHost {
  int (*messageBox)(int hwnd, const char*, const char*, uint32_t);
  int (*getActiveWindow)(void);
  int (*getLastActivePopup)(int);
  int (*getProcessWindowStation)(void);                       // null unless NT
  int (*getUserObjectInformation)(int, int, uint8_t*, int, uint32_t*);
  uint32_t osMajor;  // DAT_6fa64df0
};
int ___crtMessageBoxA(const MsgBoxHost* h, const char* text, const char* cap, uint32_t type) {
  if (!h->messageBox) return 0;
  int owner = 0;
  uint8_t info[12]; uint32_t len;
  int ws;
  if (!h->getProcessWindowStation ||
      ((ws = h->getProcessWindowStation()) != 0 &&
       h->getUserObjectInformation(ws, 1, info, 0xc, &len) != 0 && (info[8] & 1))) {
    if (h->getActiveWindow && (owner = h->getActiveWindow()) != 0 && h->getLastActivePopup)
      owner = h->getLastActivePopup(owner);
  } else if (h->osMajor < 4) type |= 0x40000;
  else type |= 0x200000;
  return h->messageBox(owner, text, cap, type);
}

// 1.13c D2Launch+0x81C0
void FUN_6fa481c0(int code, void (*handler)(void), const char* module, void (*msg)(const char*, const char*, uint32_t),
                  void (*exit_)(int)) {
  if (handler) handler();
  else {
    char b[0x200];
    const char* what = code == 1 ? "Buffer overrun detected!" : "Unknown security failure detected!";
    char name[0x104];
    strncpy(name, module ? module : "<program name unknown>", sizeof name - 1); name[sizeof name - 1] = 0;
    const char* n = name;
    if (strlen(n) + 11 > 0x3c) { n = name + strlen(name) - 0x3c + 11 + 3; memcpy((char*)n - 3 + 0, "...", 3); n -= 3; }
    snprintf(b, sizeof b, "%s\n\nProgram: %s\n\n%s", what, n, "");
    msg(b, "Microsoft Visual C++ Runtime Library", 0x12010);
  }
  exit_(3);
}

// 1.13c D2Launch+0x830B
uint32_t ___isctype_mt(const uint16_t* ctypeTab, const uint8_t* lead, uint32_t cp, uint32_t lcid, int c, uint32_t mask,
                       bool (*getStringType)(const uint8_t*, int, uint16_t*, uint32_t, uint32_t)) {
  uint16_t t;
  if ((uint32_t)(c + 1) < 0x101) t = ctypeTab[c];
  else {
    uint8_t s[2]; int n;
    if (lead[(c >> 8) & 0xff] & 0x80) { s[0] = (uint8_t)(c >> 8); s[1] = (uint8_t)c; n = 2; }
    else { s[0] = (uint8_t)c; s[1] = 0; n = 1; }
    if (!getStringType(s, n, &t, cp, lcid)) return 0;
  }
  return t & mask;
}

// 1.13c D2Launch+0x8382
struct FileHost {
  int32_t (*lseek)(int fd, int32_t off, int whence);
  int (*setEof)(int fd);
  int (*write)(int fd, const void*, uint32_t);
  int (*setmode)(int fd, int mode);
  int* errnoP; uint32_t* doserrno; uint32_t (*lastError)(void);
};
int FUN_6fa48382(const FileHost* h, int fd, int32_t size) {
  int r = 0;
  int32_t cur = h->lseek(fd, 0, 1), end;
  if (cur == -1 || (end = h->lseek(fd, 0, 2)) == -1) return -1;
  int32_t d = size - end;
  if (d < 1) {
    if (d < 0) {
      h->lseek(fd, size, 0);
      r = h->setEof(fd) ? 0 : -1;
      if (r) { *h->errnoP = 0xd; *h->doserrno = h->lastError(); }
    }
  } else {
    uint8_t z[0x1000]; memset(z, 0, sizeof z);
    int om = h->setmode(fd, 0x8000);
    do {
      int n = h->write(fd, z, d < 0x1000 ? d : 0x1000);
      if (n == -1) { if (*h->doserrno == 5) *h->errnoP = 0xd; r = -1; break; }
      d -= n;
    } while (d > 0);
    h->setmode(fd, om);
  }
  h->lseek(fd, cur, 0);
  return r;
}

// 1.13c D2Launch+0x84D0
void* _memmove(void* dst, const void* src, size_t n) { return memmove(dst, src, n); }

// 1.13c D2Launch+0x880D
void ___free_lc_time(void** p) {
  static const uint8_t order[43] = {1,2,3,4,5,6,0,8,9,10,11,12,13,7,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42};
  if (p) for (int i = 0; i < 43; i++) free(p[order[i]]);
}

// 1.13c D2Launch+0x899D
void ___free_lconv_num(void** p, void** cLocale, void** defaults) {
  if (p) for (int i = 0; i < 3; i++) if (p[i] != cLocale[i] && p[i] != defaults[i]) free(p[i]);
}

// 1.13c D2Launch+0x89FC
void ___free_lconv_mon(void** p, void** cLocale, void** defaults) {
  if (p) for (int i = 3; i < 10; i++) if (p[i] != cLocale[i] && p[i] != defaults[i]) free(p[i]);
}
