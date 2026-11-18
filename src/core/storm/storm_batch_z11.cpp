#include <cstdint>
// Seams: retail globals/callees are passed in. Log records are 32-bit-linked: [0]=id [1]=next [2]=handle [3]=len [4]=mark [5],[6]=user, text at +0x1c.
struct StormZ11Log {
  void (*enter)(uint32_t bucket);   // EnterCriticalSection(&DAT_6fc42f60 + bucket*0x18)
  void (*leave)(uint32_t bucket);
  uint32_t* heads;                  // DAT_6fc42fc0[4]
  void (*write)(uint32_t handle, const void* p, uint32_t n); // WriteFile + FlushFileBuffers
};
static uint32_t* z11_find(const StormZ11Log& s, uint32_t id) {
  for (uint32_t* r = (uint32_t*)(uintptr_t)s.heads[id & 3]; r; r = (uint32_t*)(uintptr_t)r[1]) if (r[0] == id) return r;
  return 0;
}
// 1.13c Storm+0x34610 (ord 413): message pump; idle(n) returns nonzero while it has work.
uintptr_t Storm_34610(int (*idle)(int), int param2, int (*peek)(void*), int (*get)(void*), void (*dispatch)(void*), uintptr_t (*wparam)(void*), void (*sub15ba0)(), void* msg) {
  int n = 0;
  for (;;) {
    if (peek(msg) == 0 && idle) { int r = idle(n); n++; if (r) continue; }
    n = 0;
    if (get(msg) == 0) { if (param2) sub15ba0(); return wparam(msg); }
    dispatch(msg);
  }
}
// 1.13c Storm+0x348A0 (ord 543): set record field [6].
void Storm_348A0(const StormZ11Log& s, uint32_t id, uint32_t v) {
  if (!id) return;
  s.enter(id & 3);
  if (uint32_t* r = z11_find(s, id)) r[6] = v;
  s.leave(id & 3);
}
// 1.13c Storm+0x34970 (ord 549): exchange record field [5], returns old.
uint32_t Storm_34970(const StormZ11Log& s, uint32_t id, uint32_t v) {
  uint32_t old = 0;
  if (!id) return 0;
  s.enter(id & 3);
  if (uint32_t* r = z11_find(s, id)) { old = r[5]; r[5] = v; }
  s.leave(id & 3);
  return old;
}
// 1.13c Storm+0x34A50 (ord 545): flush buffered text to the file.
void Storm_34A50(const StormZ11Log& s, uint32_t id) {
  if (!id) return;
  s.enter(id & 3);
  if (uint32_t* r = z11_find(s, id)) if (r[3]) { s.write(r[2], r + 7, r[3]); r[3] = 0; r[4] = 0; }
  s.leave(id & 3);
}
// 1.13c Storm+0x34E70 (ord 547): printf-style line. fmt: vsprintf into text at r[3]=r[4] after 0x34C40(r,1) (ensure room); then CRLF suffix (DAT_6fc36a94/96, 3 bytes copied).
void Storm_34E70(const StormZ11Log& s, uint32_t id, const char* fmt, void (*ensure)(uint32_t*, int),
                 int (*fmtfn)(char*, const char*, void*), void* args, const char* crlf) {
  if (!id) return;
  s.enter(id & 3);
  if (uint32_t* r = z11_find(s, id)) {
    r[3] = r[4];
    ensure(r, 1);
    char* t = (char*)r + 0x1c + r[3];
    int n = fmtfn(t, fmt, args);   // 0x3AFD0 strlen of result
    r[3] += n;
    t = (char*)r + 0x1c + r[3];
    t[0] = crlf[0]; t[1] = crlf[1]; t[2] = crlf[2];
    r[3] += 2;
  }
  s.leave(id & 3);
}
// 1.13c Storm+0x34F20 (ord 544): hex dump of len bytes, 8 per line "%04x: " + hex (blank-pad, extra ' ' every 4) + chars (non-printable -> '.', NUL -> ' ' pad; extra ' ' after char 3).
void Storm_34F20(const StormZ11Log& s, uint32_t id, const uint8_t* data, uint32_t len, void (*ensure)(uint32_t*, int),
                 int (*print)(char*, const char*, uint32_t), const char* pad3, const char* sp1, const char* dot, const char* sp, const char* crlf,
                 int debugOut, void (*dbg)(const char*)) {
  if (!id) return;
  s.enter(id & 3);
  if (uint32_t* r = z11_find(s, id)) {
    auto at = [&] { return (char*)r + 0x1c + r[3]; };
    auto cat = [&](const char* t) { char* d = at(); int n = 0; while ((d[n] = t[n])) n++; return n; };
    uint32_t o = 0;
    if (len) do {
      ensure(r, 0);
      r[3] += print(at(), "%04x: ", o);
      uint32_t e = o + 8;
      for (uint32_t i = o; i < e; i++) {
        int n = i < len ? print(at(), "%02x ", data[i]) : cat(pad3);
        r[3] += n;
        if ((i & 3) == 3) r[3] += cat(sp1);
      }
      for (; o < e; o++) {
        int n;
        if (o < len) {
          char c = (char)data[o];
          if (c < ' ' || c == 0x7f) n = cat(c == 0 ? sp : dot);
          else n = print(at(), "%c", (uint32_t)(int)c);
        } else n = cat(sp);
        r[3] += n;
        if ((o & 7) == 3) r[3] += cat(sp);
      }
      char* t = at(); t[0] = crlf[0]; t[1] = crlf[1]; t[2] = crlf[2];
      r[3] += 2;
    } while (o < len);
    if (debugOut) dbg((char*)r + 0x1c + r[4]);
    r[4] = r[3];
    if (r[3] > 0xbfff) { s.write(r[2], r + 7, r[3]); r[3] = 0; r[4] = 0; }
  }
  s.leave(id & 3);
}
// 1.13c Storm+0x35230: wide->byte via reverse lookup in a 256-entry ushort table; n = EAX count. Returns bytes written (incl. NUL if room).
int Storm_35230(const uint16_t* src, const uint16_t* tab, uint8_t* dst, int n) {
  if (n == 0) return 0;
  uint8_t* d = dst;
  uint16_t c = *src;
  while (c != 0) {
    if (n == 0) return (int)(d - dst);
    if (c < 0x100 && c == tab[c]) *d = (uint8_t)*src;
    else { int i = 255; while (i > 0 && tab[i] != c) i--; *d = i ? (uint8_t)i : 0x3f; }
    c = *++src; d++; n--;
  }
  if (n) *d++ = 0;
  return (int)(d - dst);
}
// 1.13c Storm+0x35310 (ord 906): codepoint -> UTF-8 (up to 6 bytes), NUL-terminated; over 0x7fffffff writes only NUL.
void Storm_35310(uint32_t c, uint8_t* p) {
  if (!p) return;
  if (c > 0x7f) {
    if (c < 0x800) { p[0] = (c >> 6) | 0xc0; p[1] = (c & 0x3f) | 0x80; p[2] = 0; return; }
    uint8_t b;
    if (c < 0x10000) b = (c >> 12) | 0xe0;
    else if (c < 0x200000) { *p++ = (c >> 18) | 0xf0; b = ((c >> 12) & 0x3f) | 0x80; }
    else if (c < 0x400000) { *p++ = (uint8_t)(c >> 24) | 0xf8; *p++ = ((c >> 18) & 0x3f) | 0x80; b = ((c >> 12) & 0x3f) | 0x80; }
    else {
      if (c > 0x7fffffff) { *p = 0; return; }
      uint8_t t = c >> 24; *p++ = (t >> 6) | 0xfc; *p++ = (t & 0x3f) | 0x80; *p++ = ((c >> 18) & 0x3f) | 0x80; b = ((c >> 12) & 0x3f) | 0x80;
    }
    *p++ = b; *p++ = ((c >> 6) & 0x3f) | 0x80; c = (c & 0x3f) | 0x80;
  }
  *p++ = (uint8_t)c; *p = 0;
}
// 1.13c Storm+0x35410 (ord 905): UTF-8 decode one codepoint; *used counts bytes consumed. -1 = NUL/null, 0x80000000 = malformed.
uint32_t Storm_35410(const uint8_t* p, int* used) {
  if (used) *used = 0;
  if (!p || !*p) return 0xffffffff;
  uint32_t c = *p; uint8_t b = *p; int n;
  if (used) ++*used;
  if ((b & 0xfe) == 0xfc) { c &= 1; n = 5; }
  else if ((b & 0xfc) == 0xf8) { c &= 3; n = 4; }
  else if ((b & 0xf8) == 0xf0) { c &= 7; n = 3; }
  else if ((b & 0xf0) == 0xe0) { c &= 0xf; n = 2; }
  else if ((b & 0xe0) == 0xc0) { c &= 0x1f; n = 1; }
  else return (int8_t)b >= 0 ? c : 0x80000000;
  while (n--) {
    b = *++p;
    if (!b) return 0xffffffff;
    if (used) ++*used;
    if ((b & 0xc0) != 0x80) return 0x80000000;
    c = (b & 0x3f) | (c << 6);
  }
  return c;
}
// 1.13c Storm+0x35BB0 (ord 911): wide -> byte via table DAT_6fc35ab0 (tab); count in EAX at call, passed as n.
void Storm_35BB0(uint8_t* dst, const uint16_t* src, const uint16_t* tab_6fc35ab0, int n) { Storm_35230(src, tab_6fc35ab0, dst, n); }
// 1.13c Storm+0x35BD0 (ord 910): same with table DAT_6fc358b0.
void Storm_35BD0(uint8_t* dst, const uint16_t* src, const uint16_t* tab_6fc358b0, int n) { Storm_35230(src, tab_6fc358b0, dst, n); }
// 1.13c Storm+0x35BF0 (ord 909): same with table DAT_6fc356b0.
void Storm_35BF0(uint8_t* dst, const uint16_t* src, const uint16_t* tab_6fc356b0, int n) { Storm_35230(src, tab_6fc356b0, dst, n); }
