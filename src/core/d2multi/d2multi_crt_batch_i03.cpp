// D2Multi CRT startup (VS2003 statically linked): 1EF5 3BA1 3D9F 3DEB 3EB2 401E 40C0 41FC 424D 42D0 430D.
// Globals are retail's, named by address. Win32 / CRT callees are declared extern.
// ponytail: decompiler hides register args of parse_cmdline (EAX=cmdline ECX=argbuf ESI=&nchars); modelled as explicit params.
#include <cstdint>
#include <cstring>
#include <cstdlib>

extern "C" {
typedef void* HANDLE;
typedef int32_t BOOL;
int32_t GetVersionExA(void*);
void* GetCommandLineA(void);
int32_t __mtinit(void);
void __mtterm(void);
void __RTC_Initialize(void);
int32_t __cinit(int32_t);
void __cexit(void);
void __freeptd(void*);
void __initptd(void*, void*);
uint32_t GetCurrentThreadId(void);
int32_t FUN_6f9d7ee0(void*);
void* FUN_6f9d3aca(int32_t, int32_t);
int ___initmbctable(void);
void FUN_6f9d51f0(void*, const void*);
void* GetStdHandle(uint32_t);
uint32_t GetFileType(void*);
void GetStartupInfoA(void*);
int32_t ___crtInitCritSecAndSpinCount(void*, uint32_t);
void DeleteCriticalSection(void*);
int32_t SetHandleCount(uint32_t);
uint32_t GetModuleFileNameA(void*, char*, uint32_t);
void* GetEnvironmentStringsW(void);
char* GetEnvironmentStrings(void);
int32_t FreeEnvironmentStringsA(void*);
int32_t FreeEnvironmentStringsW(void*);
uint32_t GetLastError(void);
int32_t WideCharToMultiByte(uint32_t, uint32_t, const uint16_t*, int32_t, char*, int32_t, const char*, int32_t*);
HANDLE HeapCreate(uint32_t, uint32_t, uint32_t);
int32_t HeapDestroy(HANDLE);
int32_t HeapFree(HANDLE, uint32_t, void*);
int32_t VirtualFree(void*, uint32_t, uint32_t);
int32_t ___heap_select(void);
int32_t ___sbh_heap_init(uint32_t);
int32_t UnhandledExceptionFilter(void*);
void* __getptd(void);

extern uint32_t DAT_6f9eba84, DAT_6f9eba90, DAT_6f9eba94, DAT_6f9eba88, DAT_6f9eba8c;
extern int32_t DAT_6f9ebac8, DAT_6f9ebac0, DAT_6f9ebd58, DAT_6f9eba98, DAT_6fa0b4a8, DAT_6fa0b4b4;
extern int32_t DAT_6fa0b38c, DAT_6fa0b388, DAT_6fa0a110, DAT_6f9e9544, DAT_6f9e9538, DAT_6f9e953c;
extern void *DAT_6fa0b4a0, *DAT_6f9ebacc, *DAT_6f9ebaa4, *DAT_6f9eba9c, *DAT_6f9ebab4, *DAT_6fa0b384, *DAT_6fa0a114, *DAT_6f9e91e4;
extern uint8_t DAT_6f9ebc50[], DAT_6f9ebd54, DAT_6fa0a141[];
extern uint32_t DAT_6fa0b3a0[32];  // ioinfo block table (0x6fa0b3a0..0x6fa0b4a0)
extern int32_t (*DAT_6fa0b4a4)(void*, int32_t, void*);
extern int32_t (*DAT_6f9ebaf0)(void);
}

static void ioinfo_fill(uint32_t* blk) {  // 32 entries of 0x24 bytes
  for (uint8_t* p = (uint8_t*)blk; p < (uint8_t*)blk + 0x480; p += 0x24) {
    *(int32_t*)p = -1; p[4] = 0; *(int32_t*)(p + 8) = 0; p[5] = 10;
  }
}

// 1.13c D2Multi+0x41FC
extern "C" int32_t __heap_init(int32_t mt) {
  DAT_6fa0b384 = HeapCreate(mt == 0, 0x1000, 0);
  if (!DAT_6fa0b384) return 0;
  DAT_6fa0b388 = ___heap_select();
  if (DAT_6fa0b388 == 3 && ___sbh_heap_init(0x3f8) == 0) { HeapDestroy(DAT_6fa0b384); return 0; }
  return 1;
}

// 1.13c D2Multi+0x424D
extern "C" void __heap_term(void) {
  if (DAT_6fa0b388 == 3) {
    uint32_t* p = (uint32_t*)((uint8_t*)DAT_6fa0a114 + 0xc);
    for (int32_t i = 0; i < DAT_6fa0a110; i++, p += 5) {
      VirtualFree((void*)p[0], 0x100000, 0x4000);
      VirtualFree((void*)p[0], 0, 0x8000);
      HeapFree(DAT_6fa0b384, 0, (void*)p[1]);
    }
    HeapFree(DAT_6fa0b384, 0, DAT_6fa0a114);
  }
  HeapDestroy(DAT_6fa0b384);
}

// 1.13c D2Multi+0x42D0
// Stack probe: touches each page of the frame. Wasm has no guard-page stack, so nothing to probe.
extern "C" void __chkstk(void) {}

// 1.13c D2Multi+0x3BA1
extern "C" int32_t __ioinit(void) {
  uint32_t* b = (uint32_t*)malloc(0x480);
  if (!b) return -1;
  DAT_6fa0b38c = 0x20;
  DAT_6fa0b3a0[0] = (uint32_t)(uintptr_t)b;
  ioinfo_fill(b);
  uint8_t si[0x44];
  GetStartupInfoA(si);
  uint16_t cbReserved2 = *(uint16_t*)(si + 0x32);
  uint8_t* lpReserved2 = *(uint8_t**)(si + 0x34);
  if (cbReserved2 && lpReserved2) {
    int32_t n = *(int32_t*)lpReserved2;
    uint8_t* flags = lpReserved2 + 4;
    uint8_t* hnd = flags + n;
    if (n > 0x7ff) n = 0x800;
    int32_t cnt = n;
    while (DAT_6fa0b38c < n) {
      uint32_t* nb = (uint32_t*)malloc(0x480);
      cnt = DAT_6fa0b38c;
      if (!nb) break;
      DAT_6fa0b3a0[DAT_6fa0b38c >> 5] = (uint32_t)(uintptr_t)nb;
      DAT_6fa0b38c += 0x20;
      ioinfo_fill(nb);
      cnt = n;
    }
    for (int32_t i = 0; i < cnt; i++, hnd += 4, flags++) {
      if (*(int32_t*)hnd != -1 && (*flags & 1) && ((*flags & 8) || GetFileType(*(void**)hnd) != 0)) {
        uint32_t* e = (uint32_t*)(uintptr_t)(DAT_6fa0b3a0[i >> 5] + (i & 0x1f) * 0x24);
        e[0] = *(uint32_t*)hnd;
        *(uint8_t*)(e + 1) = *flags;
        if (!___crtInitCritSecAndSpinCount(e + 3, 4000)) return -1;
        e[2]++;
      }
    }
  }
  for (int32_t i = 0; i < 3; i++) {
    uint32_t* e = (uint32_t*)(uintptr_t)(DAT_6fa0b3a0[0] + i * 0x24);
    uint8_t* fl = (uint8_t*)(e + 1);
    if ((int32_t)e[0] == -1) {
      *fl = 0x81;
      uint32_t id = i == 0 ? 0xfffffff6 : 0xfffffff5 - (i != 1);
      void* h = GetStdHandle(id);
      uint32_t t;
      if (h == (void*)-1 || (t = GetFileType(h)) == 0) *fl |= 0x40;
      else {
        e[0] = (uint32_t)(uintptr_t)h;
        if ((t & 0xff) == 2) *fl |= 0x40; else if ((t & 0xff) == 3) *fl |= 8;
        if (!___crtInitCritSecAndSpinCount(e + 3, 4000)) return -1;
        e[2]++;
      }
    } else *fl |= 0x80;
  }
  SetHandleCount(DAT_6fa0b38c);
  return 0;
}

// 1.13c D2Multi+0x3D9F
extern "C" void __ioterm(void) {
  for (int i = 0; i < 32; i++) {
    uint8_t* b = (uint8_t*)(uintptr_t)DAT_6fa0b3a0[i];
    if (!b) continue;
    for (uint8_t* e = b; e < b + 0x480; e += 0x24)
      if (*(int32_t*)(e + 8)) DeleteCriticalSection(e + 0xc);
    free(b);
    DAT_6fa0b3a0[i] = 0;
  }
}

// 1.13c D2Multi+0x3DEB
extern "C" int32_t __setenvp(void) {
  if (DAT_6fa0b4b4 == 0) ___initmbctable();
  char* s = (char*)DAT_6f9ebacc;
  if (!s) return -1;
  int32_t n = 0;
  for (char* p = s; *p; p += strlen(p) + 1) if (*p != '=') n++;
  char** out = (char**)malloc(n * 4 + 4);
  DAT_6f9ebaa4 = out;
  if (!out) return -1;
  for (char* p = s; *p; p += strlen(p) + 1) {
    if (*p == '=') continue;
    char* c = (char*)malloc(strlen(p) + 1);
    *out = c;
    if (!c) { free(DAT_6f9ebaa4); DAT_6f9ebaa4 = 0; return -1; }
    strcpy(c, p);
    out++;
  }
  free(DAT_6f9ebacc);
  DAT_6f9ebacc = 0;
  *out = 0;
  DAT_6fa0b4a8 = 1;
  return 0;
}

// 1.13c D2Multi+0x3EB2
// argv: out pointer array or null; buf: out chars or null; nchars: running char count; argc: running count.
extern "C" void parse_cmdline(uint8_t* cmd, uint32_t* argv, uint8_t* buf, int32_t* nchars, int32_t* argc) {
  bool inq = false;
  *nchars = 0; *argc = 1;
  if (argv) *argv++ = (uint32_t)(uintptr_t)buf;
  uint8_t c;
  uint8_t* p;
  for (;;) {  // program name: quotes toggle, no escapes
    if (*cmd == 0x22) { inq = !inq; p = cmd + 1; c = 0x22; }
    else {
      (*nchars)++;
      if (buf) *buf++ = *cmd;
      c = *cmd; p = cmd + 1;
      if (DAT_6fa0a141[c] & 4) { (*nchars)++; if (buf) *buf++ = *p; p = cmd + 2; }
      if (c == 0) { p--; goto args; }
    }
    cmd = p;
    if (!inq && (c == 0x20 || c == 9)) break;
  }
  if (buf) buf[-1] = 0;
args:
  inq = false;
  while (*p) {
    while (*p == 0x20 || *p == 9) p++;
    if (!*p) break;
    if (argv) *argv++ = (uint32_t)(uintptr_t)buf;
    (*argc)++;
    for (;;) {
      bool copy = true;
      uint32_t bs = 0;
      while (*p == 0x5c) { p++; bs++; }
      if (*p == 0x22) {
        if ((bs & 1) == 0) {
          if (!inq || p[1] != 0x22) copy = false; else p++;
          inq = !inq;
        }
        bs >>= 1;
      }
      while (bs--) { if (buf) *buf++ = 0x5c; (*nchars)++; }
      c = *p;
      if (c == 0 || (!inq && (c == 0x20 || c == 9))) break;
      if (copy) {
        if (DAT_6fa0a141[c] & 4) { if (buf) *buf++ = c; p++; (*nchars)++; }
        if (buf) *buf++ = *p;
        (*nchars)++;
      }
      p++;
    }
    if (buf) *buf++ = 0;
    (*nchars)++;
  }
  if (argv) *argv = 0;
  (*argc)++;
}

// 1.13c D2Multi+0x401E
extern "C" int32_t __setargv(void) {
  int32_t nchars, argc;
  if (DAT_6fa0b4b4 == 0) ___initmbctable();
  DAT_6f9ebd54 = 0;
  GetModuleFileNameA(0, (char*)DAT_6f9ebc50, 0x104);
  DAT_6f9ebab4 = DAT_6f9ebc50;
  uint8_t* cmd = (uint8_t*)(DAT_6fa0b4a0 && *(char*)DAT_6fa0b4a0 ? DAT_6fa0b4a0 : DAT_6f9ebc50);
  parse_cmdline(cmd, 0, 0, &nchars, &argc);
  uint8_t* mem = (uint8_t*)malloc(nchars + argc * 4);
  if (!mem) return -1;
  parse_cmdline(cmd, (uint32_t*)mem, mem + argc * 4, &nchars, &argc);
  DAT_6f9eba98 = argc - 1;
  DAT_6f9eba9c = mem;
  return 0;
}

// 1.13c D2Multi+0x40C0
extern "C" void* ___crtGetEnvironmentStringsA(void) {
  uint16_t* w = 0;
  if (DAT_6f9ebd58 == 0) {
    w = (uint16_t*)GetEnvironmentStringsW();
    if (w) DAT_6f9ebd58 = 1;
    else if (GetLastError() == 0x78) DAT_6f9ebd58 = 2;
  }
  if (DAT_6f9ebd58 != 1) {
    if (DAT_6f9ebd58 != 2 && DAT_6f9ebd58 != 0) return 0;
    char* a = GetEnvironmentStrings();
    if (!a) return 0;
    char* p = a;
    while (*p) p += strlen(p) + 1;
    size_t n = p + 1 - a;
    void* d = malloc(n);
    if (d) memcpy(d, a, n);
    FreeEnvironmentStringsA(a);
    return d;
  }
  if (!w && !(w = (uint16_t*)GetEnvironmentStringsW())) return 0;
  uint16_t* p = w;
  while (*p) { while (*p) p++; p++; }
  int32_t wn = (int32_t)(p - w) + 1;
  int32_t n = WideCharToMultiByte(0, 0, w, wn, 0, 0, 0, 0);
  char* out = 0;
  if (n) {
    char* m = (char*)malloc(n);
    if (m) {
      out = m;
      if (WideCharToMultiByte(0, 0, w, wn, m, n, 0, 0) == 0) { free(m); out = 0; }
    }
  }
  FreeEnvironmentStringsW(w);
  return out;
}

// 1.13c D2Multi+0x430D
extern "C" int32_t __XcptFilter(uint32_t num, void* ptrs) {
  struct Ptd { uint8_t pad[0x24]; uint32_t* initaddr; void* initarg; void* pxcptacttab_pad[0]; };
  uint8_t* ptd = (uint8_t*)__getptd();
  uint32_t* tab = *(uint32_t**)(ptd + 0x24);   // _initaddr (xcptacttab)
  void** initarg = (void**)(ptd + 0x28);
  void** acttab = (void**)(ptd + 0x24 + 0);    // placeholder replaced below
  (void)acttab;
  uint32_t* e = tab;
  do { if (*e == num) break; e += 3; } while (e < tab + DAT_6f9e9544 * 3);
  if (tab + DAT_6f9e9544 * 3 <= e || *e != num) e = 0;
  typedef void (*Sig)(int32_t, ...);
  Sig h = e ? (Sig)(uintptr_t)e[2] : 0;
  if (!e || !h) return UnhandledExceptionFilter(ptrs);
  if ((uintptr_t)h == 5) { e[2] = 0; return 1; }
  if ((uintptr_t)h != 1) {
    void* saved = *initarg;
    *initarg = ptrs;
    if (e[1] == 8) {
      for (int32_t i = DAT_6f9e9538; i < DAT_6f9e953c + DAT_6f9e9538; i++)
        *(uint32_t*)((uint8_t*)*(uint32_t**)(ptd + 0x24) + i * 12 + 8) = 0;
      uint32_t* fpe = (uint32_t*)(ptd + 0x2c);  // _pxcptacttab slot: holds the FPE sub-code
      uint32_t old = *fpe;
      switch (*e) {
        case 0xc000008e: *fpe = 0x83; break;
        case 0xc0000090: *fpe = 0x81; break;
        case 0xc0000091: *fpe = 0x84; break;
        case 0xc0000093: *fpe = 0x85; break;
        case 0xc000008d: *fpe = 0x82; break;
        case 0xc000008f: *fpe = 0x86; break;
        case 0xc0000092: *fpe = 0x8a; break;
      }
      h(8, *fpe);
      *fpe = old;
    } else { e[2] = 0; h((int32_t)e[1]); }
    *initarg = saved;
  }
  return -1;
}

// 1.13c D2Multi+0x1EF5
extern "C" int32_t __CRT_INIT_12(void* inst, int32_t reason) {
  if (reason == 1) {
    uint32_t v[37] = {0x94};
    if (!GetVersionExA(v)) return 0;
    DAT_6f9eba84 = v[4]; DAT_6f9eba90 = v[1]; DAT_6f9eba94 = v[2];
    DAT_6f9eba88 = v[3] & 0x7fff;
    if (v[4] != 2) DAT_6f9eba88 |= 0x8000;
    DAT_6f9eba8c = v[1] * 0x100 + v[2];
    if (!__heap_init(1)) return 0;
    if (__mtinit()) {
      __RTC_Initialize();
      DAT_6fa0b4a0 = GetCommandLineA();
      DAT_6f9ebacc = ___crtGetEnvironmentStringsA();
      if (__ioinit() >= 0) {
        if (__setargv() >= 0 && __setenvp() >= 0 && __cinit(0) == 0) { DAT_6f9ebac8++; return 1; }
        __ioterm();
      }
      __mtterm();
    }
    __heap_term();
    return 0;
  }
  if (reason == 0) {
    if (DAT_6f9ebac8 <= 0) return 0;
    DAT_6f9ebac8--;
    if (DAT_6f9ebac0 == 0) __cexit();
    __ioterm(); __mtterm(); __heap_term();
    return 1;
  }
  if (reason == 2) {
    uint8_t* ptd = (uint8_t*)FUN_6f9d3aca(1, 0x8c);
    if (!ptd) return 0;
    if (DAT_6f9ebaf0()) {
      __initptd(ptd, DAT_6f9e91e4);
      *(uint32_t*)(ptd + 4) = GetCurrentThreadId();  // _tid
      *(uint32_t*)(ptd + 8) = 0xffffffff;            // _thandle
      return 1;
    }
    free(ptd);
    return 0;
  }
  if (reason == 3) __freeptd(0);
  return 1;
}

