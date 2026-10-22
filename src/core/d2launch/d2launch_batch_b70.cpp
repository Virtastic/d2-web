// D2Launch MSVC CRT pieces (batch b70). Externs are weak; hidden register inputs are leading params.
#include <cstdint>
#include <cstddef>
#include <initializer_list>

#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa680d8; WK extern uint8_t* DAT_6fa64e2c; WK extern uint32_t** DAT_6fa64e04;
WK extern int DAT_6fa680cc; WK extern uint8_t DAT_6fa66d40[]; WK extern char DAT_6fa64fa8[];
WK extern int DAT_6fa650ac; WK extern char* DAT_6fa64e14; WK extern int DAT_6fa64df8;
WK extern uint32_t** DAT_6fa64dfc; WK extern void* DAT_6fa66f7c; WK extern int DAT_6fa66f80;
WK extern int DAT_6fa66f64; WK extern uint8_t* DAT_6fa66f68; WK extern int DAT_6fa64e34;
WK extern int DAT_6fa64e38; WK extern void (*DAT_6fa650b4)(); WK extern uint32_t DAT_6fa66f84;
WK extern uint8_t* DAT_6fa66fa0[];
struct MsgEnt { int num; const char* text; };
WK extern MsgEnt DAT_6fa60618[19];
WK int ___initmbctable(); WK void* _malloc(size_t); WK void _free(void*); WK size_t _strlen(const char*);
WK char* FUN_6fa47f60(char*, const char*);  // strcpy
WK char* FUN_6fa47f70(char*, const char*);  // strcat
WK char* _strncpy(char*, const char*, size_t);
WK uint32_t GetModuleFileNameA(void*, char*, uint32_t);
WK void* GetStdHandle(uint32_t);
WK int WriteFile(void*, const void*, uint32_t, uint32_t*, void*);
WK uint32_t GetLastError();
WK void* HeapCreate(uint32_t, size_t, size_t);
WK int HeapDestroy(void*);
WK int HeapFree(void*, uint32_t, void*);
WK int VirtualFree(void*, size_t, uint32_t);
WK int ___heap_select();
WK int ___sbh_heap_init(uint32_t);
WK void ___crtMessageBoxA(const char*, const char*, uint32_t);
WK intptr_t __get_osfhandle(uint32_t);
WK uint32_t SetFilePointer(void*, int32_t, int32_t*, uint32_t);
WK void __dosmaperr(uint32_t);
WK int* FUN_6fa43afa();    // _errno
WK uint32_t* FUN_6fa43b03(); // __doserrno
WK void __lock_fhandle(int);
WK void FUN_6fa45658();    // unlock fhandle (lseek)
WK void FUN_6fa458bf();    // unlock fhandle (write)
WK int __lseeki64_lk(uint32_t, int32_t, int32_t, uint32_t);
}

// 1.13c D2Launch+0x04D0B  __setenvp
int sub_4D0B_setenvp() {
  if (DAT_6fa680d8 == 0) ___initmbctable();
  int n = 0;
  uint8_t* p = DAT_6fa64e2c;
  if (!p) return -1;
  for (; *p; p += _strlen((char*)p) + 1) if (*p != '=') n++;
  uint32_t** v = (uint32_t**)_malloc(n * 4 + 4);
  p = DAT_6fa64e2c; DAT_6fa64e04 = v;
  if (!v) return -1;
  for (;;) {
    if (!*p) { _free(DAT_6fa64e2c); DAT_6fa64e2c = nullptr; *v = nullptr; DAT_6fa680cc = 1; return 0; }
    size_t l = _strlen((char*)p);
    if (*p != '=') {
      char* s = (char*)_malloc(l + 1);
      *v = (uint32_t*)s;
      if (!s) { _free(DAT_6fa64e04); DAT_6fa64e04 = nullptr; return -1; }
      FUN_6fa47f60(s, (char*)p);
      v++;
    }
    p += l + 1;
  }
}

// 1.13c D2Launch+0x04DD2  parse_cmdline; hidden: EAX=cmdline, ECX=arg buffer, ESI=&numchars
void sub_4DD2_parse_cmdline(uint8_t* cmd, uint8_t* buf, int* numchars, uint8_t** argv, int* numargs) {
  bool inq = false, copy;
  uint8_t c;
  *numchars = 0; *numargs = 1;
  if (argv) *argv++ = buf;
  do {
    if (*cmd == 0x22) { inq = !inq; c = 0x22; cmd++; }
    else {
      ++*numchars;
      if (buf) *buf++ = *cmd;
      c = *cmd++;
      if (DAT_6fa66d40[c + 1] & 4) { ++*numchars; if (buf) *buf++ = *cmd; cmd++; }
      if (c == 0) { cmd--; goto rest; }
    }
  } while (inq || (c != 0x20 && c != 9));
  if (buf) buf[-1] = 0;
rest:
  inq = false;
  while (*cmd) {
    while (*cmd == 0x20 || *cmd == 9) cmd++;
    if (!*cmd) break;
    if (argv) *argv++ = buf;
    ++*numargs;
    for (;;) {
      copy = true;
      unsigned bs = 0;
      while (*cmd == 0x5c) { cmd++; bs++; }
      if (*cmd == 0x22) {
        if (!(bs & 1)) {
          if (inq && cmd[1] == 0x22) cmd++; else copy = false;
          inq = !inq;
        }
        bs >>= 1;
      }
      while (bs--) { if (buf) *buf++ = 0x5c; ++*numchars; }
      c = *cmd;
      if (!c || (!inq && (c == 0x20 || c == 9))) break;
      if (copy) {
        if (buf) {
          if (DAT_6fa66d40[c + 1] & 4) { *buf++ = c; cmd++; ++*numchars; }
          *buf++ = *cmd;
        } else if (DAT_6fa66d40[c + 1] & 4) { cmd++; ++*numchars; }
        ++*numchars;
      }
      cmd++;
    }
    if (buf) *buf++ = 0;
    ++*numchars;
  }
  if (argv) *argv = nullptr;
  ++*numargs;
}

// 1.13c D2Launch+0x04F3E  __setargv
int sub_4F3E_setargv() {
  if (DAT_6fa680d8 == 0) ___initmbctable();
  DAT_6fa650ac = 0;
  GetModuleFileNameA(nullptr, DAT_6fa64fa8, 0x104);
  DAT_6fa64e14 = DAT_6fa64fa8;
  int nargs = 0, nchars = 0;
  // hidden: parse_cmdline reads the command line from a global the decompile does not expose
  sub_4DD2_parse_cmdline((uint8_t*)DAT_6fa64e14, nullptr, &nchars, nullptr, &nargs);
  uint32_t** a = (uint32_t**)_malloc(nchars + nargs * 4);
  if (!a) return -1;
  sub_4DD2_parse_cmdline((uint8_t*)DAT_6fa64e14, (uint8_t*)(a + nargs), &nchars, (uint8_t**)a, &nargs);
  DAT_6fa64df8 = nargs - 1;
  DAT_6fa64dfc = a;
  return 0;
}

// 1.13c D2Launch+0x0511C  __heap_init(mtflag)
int sub_511C_heap_init(int mtflag) {
  DAT_6fa66f7c = HeapCreate(mtflag == 0, 0x1000, 0);
  if (!DAT_6fa66f7c) return 0;
  DAT_6fa66f80 = ___heap_select();
  if (DAT_6fa66f80 == 3 && ___sbh_heap_init(0x3f8) == 0) { HeapDestroy(DAT_6fa66f7c); return 0; }
  return 1;
}

// 1.13c D2Launch+0x0516D  __heap_term
void sub_516D_heap_term() {
  if (DAT_6fa66f80 == 3) {
    uint32_t* p = (uint32_t*)(DAT_6fa66f68 + 0xc);
    for (int i = 0; i < DAT_6fa66f64; i++, p += 5) {
      VirtualFree((void*)(uintptr_t)p[0], 0x100000, 0x4000);
      VirtualFree((void*)(uintptr_t)p[0], 0, 0x8000);
      HeapFree(DAT_6fa66f7c, 0, (void*)(uintptr_t)p[1]);
    }
    HeapFree(DAT_6fa66f7c, 0, DAT_6fa66f68);
  }
  HeapDestroy(DAT_6fa66f7c);
}

// 1.13c D2Launch+0x053AC  _NMSG_WRITE
void sub_53AC_nmsg_write(int num) {
  uint32_t i = 0;
  while (i < 0x13 && DAT_6fa60618[i].num != num) i++;
  if (DAT_6fa60618[i].num != num) return;
  const char* msg = DAT_6fa60618[i].text;
  if (DAT_6fa64e34 == 1 || (DAT_6fa64e34 == 0 && DAT_6fa64e38 == 1)) {
    uint32_t w;
    WriteFile(GetStdHandle(0xfffffff4), msg, (uint32_t)_strlen(msg), &w, nullptr);
  } else if (num != 0xfc) {
    char prog[0x104 + 8]; prog[0x100 + 0x1c] = 0;
    if (GetModuleFileNameA(nullptr, prog, 0x104) == 0) FUN_6fa47f60(prog, "<program name unknown>");
    char* name = prog;
    if (_strlen(name) + 1 > 0x3c) { name += _strlen(name) - 0x3c + 3 + 0; _strncpy(name, "...", 3); }
    char out[0x400];
    FUN_6fa47f60(out, "Runtime Error!\n\nProgram: ");
    FUN_6fa47f70(out, name);
    FUN_6fa47f70(out, "\n\n");
    FUN_6fa47f70(out, msg);
    ___crtMessageBoxA(out, "Microsoft Visual C++ Runtime Library", 0x12010);
  }
}

// 1.13c D2Launch+0x05524  __FF_MSGBANNER
void sub_5524_ff_msgbanner() {
  if (DAT_6fa64e34 == 1 || (DAT_6fa64e34 == 0 && DAT_6fa64e38 == 1)) {
    sub_53AC_nmsg_write(0xfc);
    if (DAT_6fa650b4) DAT_6fa650b4();
    sub_53AC_nmsg_write(0xff);
  }
}

// 1.13c D2Launch+0x0555D  __lseek_lk
uint32_t sub_555D_lseek_lk(uint32_t fh, int32_t off, uint32_t origin) {
  void* h = (void*)__get_osfhandle(fh);
  if (h == (void*)0xffffffff) { *FUN_6fa43afa() = 9; return 0xffffffff; }
  uint32_t r = SetFilePointer(h, off, nullptr, origin), err = 0;
  if (r == 0xffffffff) err = GetLastError();
  if (err == 0) DAT_6fa66fa0[(int)fh >> 5][4 + (fh & 0x1f) * 0x24] &= 0xfd;
  else { __dosmaperr(err); r = 0xffffffff; }
  return r;
}

// 1.13c D2Launch+0x055D1  _lseek (SEH frame omitted)
long sub_55D1_lseek(int fh, long off, int origin) {
  if ((uint32_t)fh < DAT_6fa66f84 && (DAT_6fa66fa0[fh >> 5][4 + (fh & 0x1f) * 0x24] & 1)) {
    long r;
    __lock_fhandle(fh);
    if (!(DAT_6fa66fa0[fh >> 5][4 + (fh & 0x1f) * 0x24] & 1)) { *FUN_6fa43afa() = 9; *FUN_6fa43b03() = 0; r = -1; }
    else r = (long)sub_555D_lseek_lk(fh, off, origin);
    FUN_6fa45658();
    return r;
  }
  *FUN_6fa43afa() = 9; *FUN_6fa43b03() = 0;
  return -1;
}

// 1.13c D2Launch+0x0567C  _write_lk
int sub_567C_write_lk(uint32_t fh, const char* buf, uint32_t cnt) {
  uint32_t written = 0, err = 0, w;
  int lf = 0;
  if (cnt == 0) return 0;
  uint8_t** tab = &DAT_6fa66fa0[(int)fh >> 5];
  uint8_t* ent = *tab + (fh & 0x1f) * 0x24;
  if (ent[4] & 0x20) __lseeki64_lk(fh, 0, 0, 2);
  void* h = *(void**)ent;
  if (!(ent[4] & 0x80)) {
    if (WriteFile(h, buf, cnt, &w, nullptr)) written = w; else err = GetLastError();
  } else {
    char tmp[0x400 + 4];
    const char* p = buf;
    do {
      int n = 0;
      char* d = tmp;
      uint32_t used = (uint32_t)(p - buf);
      while (used < cnt && n < 0x400) {
        char c = *p++; used++;
        if (c == '\n') { lf++; *d++ = '\r'; n++; }
        *d++ = c; n++;
      }
      if (!WriteFile(h, tmp, (uint32_t)(d - tmp), &w, nullptr)) { err = GetLastError(); break; }
      written += w;
      if ((int)w < d - tmp) break;
    } while ((uint32_t)(p - buf) < cnt);
  }
  if (written) return (int)(written - lf);
  if (err) {
    if (err == 5) { *FUN_6fa43afa() = 9; *FUN_6fa43b03() = 5; return -1; }
    __dosmaperr(err);
    return -1;
  }
  if ((ent[4] & 0x40) && *buf == 0x1a) return 0;
  *FUN_6fa43afa() = 0x1c; *FUN_6fa43b03() = 0;
  return -1;
}

// 1.13c D2Launch+0x05838  _write (SEH frame omitted)
int sub_5838_write(int fh, const void* buf, uint32_t cnt) {
  if ((uint32_t)fh < DAT_6fa66f84 && (DAT_6fa66fa0[fh >> 5][4 + (fh & 0x1f) * 0x24] & 1)) {
    int r;
    __lock_fhandle(fh);
    if (!(DAT_6fa66fa0[fh >> 5][4 + (fh & 0x1f) * 0x24] & 1)) { *FUN_6fa43afa() = 9; *FUN_6fa43b03() = 0; r = -1; }
    else r = sub_567C_write_lk(fh, (const char*)buf, cnt);
    FUN_6fa458bf();
    return r;
  }
  *FUN_6fa43afa() = 9; *FUN_6fa43b03() = 0;
  return -1;
}
