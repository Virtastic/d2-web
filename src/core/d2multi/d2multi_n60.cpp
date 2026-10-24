// D2Multi n60: 448C 475C.
// Retail globals are passed in; host calls are function-pointer seams.
// ponytail: decompiler hides register args (EAX/EBX/EDI); roles taken from the bodies, low confidence on seam arg lists.
#include <cstdint>
#include <cstring>
#include <cstdio>

namespace d2multi_n60 {
typedef void (*Fn0)();
typedef void (*FnP)(void*);
typedef void (*FnS)(const char*);

// game-list row: 0x3b bytes: +1 str(16) +0x11 str(32) +0x31 u16 +0x33 u16 +0x37 u32
enum { ROW = 0x3b };

// 1.13c D2MULTI+0x0448C
// CRT _FF_MSGBANNER-style runtime error. tbl: {code, msg*} x 0x13. mode: DAT_6f9ebad4/ad8.
struct RtErr { int32_t code; const char* msg; };
void crt_rterr(int32_t code, const RtErr* tbl, int32_t m4, int32_t m8, void (*writeStdErr)(const char*, size_t),
               const char* (*moduleName)(), void (*msgBox)(const char*, const char*, uint32_t)) {
  int i = 0;
  while (i < 0x13 && tbl[i].code != code) i++;
  if (i == 0x13 && tbl[0x12].code != code) return;
  if (i == 0x13) i = 0x12;
  if (m4 == 1 || (m4 == 0 && m8 == 1)) { writeStdErr(tbl[i].msg, strlen(tbl[i].msg)); return; }
  if (code == 0xfc) return;
  char prog[0x105] = {0};
  const char* n = moduleName();
  strncpy(prog, n ? n : "<program name unknown>", 0x104);
  const char* p = prog;
  char tail[0x40];
  if (strlen(p) + 1 > 0x3c) { strcpy(tail, "..."); strncat(tail, p + strlen(p) - 0x3b + 3, 0x3b); p = tail; }
  char buf[0x400];
  snprintf(buf, sizeof buf, "Runtime Error!\n\nProgram: %s\n\n%s", p, tbl[i].msg);
  msgBox(buf, "Microsoft Visual C++ Runtime Library", 0x12010);
}

// 1.13c D2MULTI+0x0475C
// CRT _write_lk: text-mode LF->CRLF translation through a 1 KiB buffer. Returns bytes written (minus added CRs), -1 on error.
// fl: ioinfo flags (0x20 append, 0x80 text, 0x40 ctrl-z). wf returns bytes written or -1 with *err set.
int32_t crt_write(uint32_t fd, const char* s, uint32_t n, uint8_t fl, int32_t (*seekEnd)(uint32_t),
                  int32_t (*wf)(const void*, uint32_t, uint32_t* wrote, uint32_t* err), void (*setErr)(int32_t, uint32_t),
                  void (*dosmaperr)(uint32_t)) {
  if (n == 0) return 0;
  if (fl & 0x20) seekEnd(fd);
  uint32_t total = 0, err = 0, wrote = 0; int32_t crs = 0;
  if (!(fl & 0x80)) {
    if (!wf(s, n, &wrote, &err)) ; else { total = wrote; err = 0; }
  } else {
    const char* p = s; char b[0x400 + 2];
    do {
      int32_t k = 0; char* o = b;
      while ((uint32_t)(p - s) < n && k < 0x400) {
        char c = *p++;
        if (c == '\n') { crs++; *o++ = '\r'; k++; }
        *o++ = c; k++;
      }
      if (!wf(b, (uint32_t)(o - b), &wrote, &err)) break;
      total += wrote;
      if ((int32_t)wrote < o - b) break;
    } while ((uint32_t)(p - s) < n);
  }
  if (total) return (int32_t)total - crs;
  if (err) {
    if (err == 5) { setErr(9, 5); return -1; }
    dosmaperr(err); return -1;
  }
  if ((fl & 0x40) && *s == 0x1a) return 0;
  setErr(0x1c, 0);
  return -1;
}

}  // namespace d2multi_n60
