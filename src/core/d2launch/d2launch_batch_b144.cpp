// D2Launch CRT batch b144. Retail 32-bit addresses stay uint32_t; callees and data are weak seams.
// Retail 32-bit guest pointers held in argument slots are resolved through b144_g2n.
#include <cstdint>
#include <cstddef>

#define B144_W __attribute__((weak))

struct B144File { char* ptr; int32_t cnt; char* base; uint32_t flag; int32_t file; int32_t charbuf; int32_t bufsiz; char* tmpfname; };

extern "C" {
B144_W void* b144_g2n(uint32_t guest);                        // guest 32-bit pointer -> native
B144_W void FUN_6fa41598(int32_t a, int32_t b, int32_t c);    // doexit
B144_W void FUN_6fa43479(int32_t lockNum);                    // _unlock
B144_W void FUN_6fa4352e(int32_t lockNum);                    // __lock
B144_W void FUN_6fa4440b(void);                               // unlock(4)
B144_W void FUN_6fa43c98(void);                               // unlock(1)
B144_W void FUN_6fa448b7(void);
B144_W void FUN_6fa448c3(void);
B144_W void* FUN_6fa46ceb(uint32_t p);                        // ___sbh_find_block
B144_W void FUN_6fa46d16(void* hdr, uint32_t p);              // ___sbh_free_block
B144_W int32_t* FUN_6fa43afa(void);                           // _errno
B144_W uint32_t* FUN_6fa43b03(void);                          // __doserrno
B144_W void FUN_6fa43b0c(uint32_t e);                         // __dosmaperr
B144_W intptr_t FUN_6fa46a24(int fh);                         // __get_osfhandle
B144_W void FUN_6fa469a5(int fh);                             // __free_osfhnd
B144_W void FUN_6fa46a65(int fh);                             // __lock_fhandle
B144_W void FUN_6fa44523(void);                               // unlock_fhandle
B144_W int FUN_6fa4391d(int idx, void* f);                    // __lock_file2
B144_W int FUN_6fa4348e(int lockNum);                         // __mtinitlocknum
B144_W void* FUN_6fa459f8(size_t n);                          // _malloc
B144_W int FUN_6fa4_crt_initcs(void* cs, uint32_t spin);      // ___crtInitCritSecAndSpinCount
B144_W void FUN_6fa4_entercs(void* cs);                       // EnterCriticalSection
B144_W void FUN_6fa47882(void* locinfo);                      // ___freetlocinfo
B144_W int FUN_6fa4_closehandle(intptr_t h);                  // CloseHandle
B144_W uint32_t FUN_6fa4_getlasterror(void);
B144_W void FUN_6fa4_heapfree(void* p);                       // HeapFree(DAT_6fa66f7c,0,p)
B144_W int FUN_6fa4_tlsalloc(void);                           // TlsAlloc
B144_W int FUN_6fa4_getversionex(uint32_t* osvi);             // GetVersionExA (0x94-byte OSVERSIONINFOA as u32[37])
B144_W char* FUN_6fa4_getcommandline(void);
B144_W void* FUN_6fa44fe0(void);                              // ___crtGetEnvironmentStringsA
B144_W int FUN_6fa448fb(void);                                // __mtinit
B144_W void FUN_6fa446e4(void);                               // __mtterm
B144_W void FUN_6fa44701(void* ptd, void* locale);            // __initptd
B144_W void* FUN_6fa449ea(size_t n, size_t sz);               // _calloc_crt
B144_W int FUN_6fa44ac1(void);                                // __ioinit
B144_W void FUN_6fa44cbf(void);                               // __ioterm
B144_W int FUN_6fa4511c(void);                                // __heap_init
B144_W void FUN_6fa4516d(void);                               // __heap_term
B144_W void FUN_6fa43651(void);                               // __RTC_Initialize
B144_W int FUN_6fa4152e(int);                                 // __cinit
B144_W int FUN_6fa44f3e(void);                                // __setargv
B144_W int FUN_6fa44d0b(void);                                // __setenvp
B144_W void FUN_6fa448cc(void* ptd);                          // __freeptd
B144_W uint32_t FUN_6fa4_getcurrentthreadid(void);

B144_W extern int32_t DAT_6fa66f80;  // sbh mode
B144_W extern uint32_t DAT_6fa66f84; // _nhandle
B144_W extern uint8_t* DAT_6fa66fa0[];
B144_W extern int32_t DAT_6fa680c0;  // stream table count
B144_W extern B144File** DAT_6fa670a0;
B144_W extern uint32_t DAT_6fa64de4, DAT_6fa64df0, DAT_6fa64df4, DAT_6fa64de8, DAT_6fa64dec;
B144_W extern char* DAT_6fa680c4;
B144_W extern void* DAT_6fa64e2c;
B144_W extern int32_t DAT_6fa64e28, DAT_6fa64e20;
B144_W extern void* DAT_6fa60560;       // initial locale
B144_W extern int (*DAT_6fa64f9c)(void);
B144_W extern int32_t* DAT_6fa66d24;
B144_W extern int32_t* PTR_DAT_6fa6075c;
B144_W extern int32_t DAT_6fa60708[];
B144_W extern uint8_t DAT_6fa60590[];
}

// 1.13c D2Launch+0x167d  __cexit
void b144_cexit(void) { FUN_6fa41598(0, 0, 1); }

// 1.13c D2Launch+0x36dc  __SEH_prolog (x86 frame push; no wasm equivalent)
void b144_seh_prolog(void) {}

// 1.13c D2Launch+0x3717  __SEH_epilog (x86 frame pop; no wasm equivalent)
void b144_seh_epilog(void) {}

// 1.13c D2Launch+0x46db
void b144_tlsalloc(void) { FUN_6fa4_tlsalloc(); }

// 1.13c D2Launch+0x43b8  _free
void b144_free(void* mem) {
  if (!mem) return;
  if (DAT_6fa66f80 == 3) {
    FUN_6fa4352e(4);
    void* hdr = FUN_6fa46ceb((uint32_t)(uintptr_t)mem);
    if (hdr) FUN_6fa46d16(hdr, (uint32_t)(uintptr_t)mem);
    FUN_6fa4440b();
    if (hdr) return;
  }
  FUN_6fa4_heapfree(mem);
}

// 1.13c D2Launch+0x4429  __close_lk
int32_t b144_close_lk(uint32_t fh) {
  uint32_t err = 0;
  if (FUN_6fa46a24((int)fh) != -1) {
    bool skip = false;
    if (fh == 1 || fh == 2) skip = FUN_6fa46a24(1) == FUN_6fa46a24(2);
    if (!skip && !FUN_6fa4_closehandle(FUN_6fa46a24((int)fh))) err = FUN_6fa4_getlasterror();
  }
  FUN_6fa469a5((int)fh);
  DAT_6fa66fa0[(int32_t)fh >> 5][4 + (fh & 0x1f) * 0x24] = 0;
  if (!err) return 0;
  FUN_6fa43b0c(err);
  return -1;
}

// 1.13c D2Launch+0x44ac  __close
int32_t b144_close(int32_t fh) {
  if ((uint32_t)fh < DAT_6fa66f84) {
    uint8_t* fl = &DAT_6fa66fa0[fh >> 5][4 + (fh & 0x1f) * 0x24];
    if (*fl & 1) {
      FUN_6fa46a65(fh);
      int32_t r;
      if (!(*fl & 1)) { *FUN_6fa43afa() = 9; r = -1; }
      else r = b144_close_lk((uint32_t)fh);
      FUN_6fa44523();
      return r;
    }
  }
  *FUN_6fa43afa() = 9;
  *FUN_6fa43b03() = 0;
  return -1;
}

// 1.13c D2Launch+0x3b7f  __getstream
B144File* b144_getstream(void) {
  B144File* found = nullptr;
  FUN_6fa4352e(1);
  for (int32_t i = 0; i < DAT_6fa680c0; i++) {
    B144File* s = DAT_6fa670a0[i];
    if (!s) {
      s = (B144File*)FUN_6fa459f8(0x38);
      DAT_6fa670a0[i] = s;
      if (s) {
        if (!FUN_6fa4_crt_initcs((uint8_t*)s + 0x20, 4000)) { b144_free(s); DAT_6fa670a0[i] = nullptr; }
        else { FUN_6fa4_entercs((uint8_t*)s + 0x20); found = s; }
      }
      break;
    }
    if ((s->flag & 0x83) == 0) {
      if (i > 2 && i < 0x14 && !FUN_6fa4348e(i + 0x10)) break;
      FUN_6fa4391d(i, DAT_6fa670a0[i]);
      found = DAT_6fa670a0[i];
      if ((found->flag & 0x83) == 0) break;
      FUN_6fa4391d(i, found);   // retail relocks the busy stream
      found = nullptr;
    }
  }
  if (found) { found->cnt = 0; found->flag = 0; found->base = nullptr; found->ptr = nullptr; found->tmpfname = nullptr; found->file = -1; }
  FUN_6fa43c98();
  return found;
}

// 1.13c D2Launch+0x4785  __freefls@4
void b144_freefls(uint8_t* p) {
  if (!p) return;
  static const int off[] = {0x24, 0x2c, 0x34, 0x3c, 0x44, 0x48};
  for (int o : off) { void* q = *(void**)(p + o); if (q) b144_free(q); }
  // retail 32-bit pointer slots; here native-width seam via b144_g2n
  if (*(uint8_t**)(p + 0x54) != DAT_6fa60590) b144_free(*(void**)(p + 0x54));
  FUN_6fa4352e(0xd);
  int32_t* li = *(int32_t**)(p + 0x60);
  if (li && --*li == 0 && li != DAT_6fa66d24) b144_free(li);
  FUN_6fa448b7();
  FUN_6fa4352e(0xc);
  int32_t** ci = (int32_t**)(p + 0x64);
  int32_t* c = *ci;
  if (c) {
    --c[0];
    int32_t** pp = (int32_t**)c;
    if (pp[0xb]) --*pp[0xb];
    if (pp[0xd]) --*pp[0xd];
    if (pp[0xc]) --*pp[0xc];
    if (pp[0x10]) --*pp[0x10];
    --*(int32_t*)((uint8_t*)pp[0x13] + 0xb4);
    if (c != PTR_DAT_6fa6075c && c != DAT_6fa60708 && c[0] == 0) FUN_6fa47882(c);
  }
  FUN_6fa448c3();
  b144_free(p);
}

// 1.13c D2Launch+0x1906  __CRT_INIT@12
uint32_t b144_crt_init(void* hinst, int32_t reason) {
  (void)hinst;
  if (reason == 1) {
    uint32_t v[37] = {0x94};
    if (!FUN_6fa4_getversionex(v)) return 0;
    DAT_6fa64de4 = v[4]; DAT_6fa64df0 = v[1]; DAT_6fa64df4 = v[2];
    DAT_6fa64de8 = v[3] & 0x7fff;
    if (v[4] != 2) DAT_6fa64de8 |= 0x8000;
    DAT_6fa64dec = v[1] * 0x100 + v[2];
    if (!FUN_6fa4511c()) return 0;
    if (!FUN_6fa448fb()) { FUN_6fa4516d(); return 0; }
    FUN_6fa43651();
    DAT_6fa680c4 = FUN_6fa4_getcommandline();
    DAT_6fa64e2c = FUN_6fa44fe0();
    if (FUN_6fa44ac1() >= 0) {
      if (FUN_6fa44f3e() >= 0 && FUN_6fa44d0b() >= 0 && FUN_6fa4152e(0) == 0) { DAT_6fa64e28++; return 1; }
      FUN_6fa44cbf();
    }
    FUN_6fa446e4();
    FUN_6fa4516d();
    return 0;
  }
  if (reason == 0) {
    if (DAT_6fa64e28 > 0) {
      DAT_6fa64e28--;
      if (DAT_6fa64e20 == 0) b144_cexit();
      FUN_6fa44cbf(); FUN_6fa446e4(); FUN_6fa4516d();
      return 1;
    }
    return 0;
  }
  if (reason == 2) {
    void* ptd = FUN_6fa449ea(1, 0x8c);
    if (ptd) {
      void* loc = DAT_6fa60560;
      if (DAT_6fa64f9c()) {
        FUN_6fa44701(ptd, loc);
        *(uint32_t*)((uint8_t*)ptd + 4) = 0xffffffffu;                     // _thandle
        *(uint32_t*)((uint8_t*)ptd + 0) = FUN_6fa4_getcurrentthreadid();   // _tid
        return 1;
      }
      b144_free(ptd);
    }
    return 0;
  }
  if (reason == 3) FUN_6fa448cc(nullptr);
  return 1;
}

// ---------------------------------------------------------------------------------------------
// printf core (_output) and scanf core (_input). Argument lists are arrays of 4-byte slots.
extern "C" {
B144_W void d2launch_write_char(uint8_t ch, void* f, int32_t* charsout);
B144_W void d2launch_write_multi_char(uint8_t ch, void* f, int32_t* charsout, int32_t num);
B144_W uint32_t d2launch_inc(void* f);
B144_W int d2launch_ungetc(int c, void* f);                  // FID_conflict__ungetc (5C18)
B144_W int d2launch_isspace(int c);
B144_W int d2launch_isdigit(int c);
B144_W int d2launch_isxdigit(int c);
B144_W int d2launch_wctomb(char* mb, uint16_t wc);
B144_W int d2launch_mbtowc(uint16_t* dst, const char* src, size_t n);
B144_W void* d2launch_memset(void* d, int v, size_t n);
B144_W size_t d2launch_strlen(const char* s);
B144_W void* d2launch_malloc(size_t n);
B144_W void d2launch_aulldvrm(uint32_t lo, uint32_t hi, uint32_t dlo, uint32_t dhi, uint64_t* q, uint32_t* r);
B144_W extern const int8_t DAT_6fa5c4d8[];   // char-class table (retail address-biased, indexed by char)
B144_W extern const int8_t DAT_6fa5c4f8[];   // state table
B144_W extern const uint16_t* PTR_DAT_6fa606c8;  // ctype table (_pctype)
B144_W extern void (*PTR___fptrap_6fa606b0)(void* dbl, char* buf, int fmt, int prec, int caps);  // _cfltcvt
B144_W extern void (*PTR___fptrap_6fa606b4)(char* buf);   // _cropzeros
B144_W extern void (*PTR___fptrap_6fa606bc)(char* buf);   // _forcdecpt
B144_W extern void (*PTR___fptrap_6fa606b8)(int size, void* dst, char* buf);  // _fltin
B144_W extern char DAT_6fa606e4;     // decimal point
B144_W extern size_t DAT_6fa606e0;   // mb_cur_max
}

// 1.13c D2Launch+0x1d45  _output (printf core): stream, format, argument slots
int32_t b144_output(void* f, const uint8_t* fmt, const uint32_t* args) {
  static const char kNull[] = "(null)";
  static const uint16_t kWNull[] = {'(', 'n', 'u', 'l', 'l', ')', 0};
  char buf[0x210];
  uint8_t c = *fmt;
  int32_t state = 0, charsout = 0;
  void* heapBuf = nullptr;
  int32_t prec = -1, width = 0, wantSkip = 0, base10hex = 0, fltcaps = 0;
  int32_t prefixLen = 0, nSkip = 0;
  uint32_t flags = 0;
  int32_t wide = 0, hexAdj = 0;
  size_t len = 0;
  const void* text = nullptr;
  char prefix[2] = {0, 0};
  (void)base10hex; (void)wantSkip;
  for (;;) {
    if (c == 0 || charsout < 0) return charsout;
    const uint8_t* next = fmt + 1;
    uint32_t cls = (((int8_t)c < ' ') || ((int8_t)c > 'x')) ? 0 : (DAT_6fa5c4d8[(int8_t)c] & 0xf);
    state = DAT_6fa5c4f8[cls * 8 + state] >> 4;
    const uint32_t* nargs = args;
    switch (state) {
      case 0:
      normal:
        wide = 0;
        if (PTR_DAT_6fa606c8[c] & 0x8000) {
          d2launch_write_char(c, f, &charsout);
          next = fmt + 2;
          c = fmt[1];
        }
        d2launch_write_char(c, f, &charsout);
        break;
      case 1:
        prec = -1; fltcaps = 0; nSkip = 0; width = 0; prefixLen = 0; flags = 0; wide = 0;
        break;
      case 2:
        if (c == ' ') flags |= 2; else if (c == '#') flags |= 0x80; else if (c == '+') flags |= 1;
        else if (c == '-') flags |= 4; else if (c == '0') flags |= 8;
        break;
      case 3:
        if (c == '*') {
          width = (int32_t)*args; nargs = args + 1;
          if (width < 0) { flags |= 4; width = -width; }
        } else width = (int8_t)c - '0' + width * 10;
        break;
      case 4: prec = 0; break;
      case 5:
        if (c == '*') {
          prec = (int32_t)*args; nargs = args + 1;
          if (prec < 0) prec = -1;
        } else prec = (int8_t)c - '0' + prec * 10;
        break;
      case 6:
        if (c == 'I') {
          uint8_t b = *next;
          if (b == '6' && fmt[2] == '4') { next = fmt + 3; flags |= 0x8000; }
          else if (b == '3' && fmt[2] == '2') { next = fmt + 3; flags &= ~0x8000u; }
          else if (b != 'd' && b != 'i' && b != 'o' && b != 'u' && b != 'x' && b != 'X') { state = 0; goto normal; }
        } else if (c == 'h') flags |= 0x20;
        else if (c == 'l') flags |= 0x10;
        else if (c == 'w') flags |= 0x800;
        break;
      case 7: {
        uint8_t t = c;
        int doEmit = 1, kind = 0;   // kind: 1 char, 2 string, 3 float, 4 integer
        int radix = 0;
        if (t == 'C') { if (!(flags & 0x830)) flags |= 0x800; kind = 1; }
        else if (t == 'c') kind = 1;
        else if (t == 'S') { if (!(flags & 0x830)) flags |= 0x800; kind = 2; }
        else if (t == 's') kind = 2;
        else if (t == 'Z') kind = 5;
        else if (t == 'E' || t == 'G') { fltcaps = 1; t += 0x20; kind = 3; }
        else if (t == 'e' || t == 'f' || t == 'g') kind = 3;
        else if (t == 'd' || t == 'i') { flags |= 0x40; radix = 10; kind = 4; }
        else if (t == 'u') { radix = 10; kind = 4; }
        else if (t == 'o') { radix = 8; if (flags & 0x80) flags |= 0x200; kind = 4; }
        else if (t == 'X' || t == 'p' || t == 'x') {
          if (t == 'p') prec = 8;
          hexAdj = (t == 'x') ? 0x27 : 7; radix = 16; kind = 4;
          if (flags & 0x80) { prefix[1] = (char)(hexAdj + 'Q'); prefix[0] = '0'; prefixLen = 2; }
        } else if (t == 'n') {
          nargs = args + 1;
          void* dst = b144_g2n(*args);
          if (flags & 0x20) *(int16_t*)dst = (int16_t)charsout; else *(int32_t*)dst = charsout;
          nSkip = 1; doEmit = 0;
        }
        if (kind == 1) {
          nargs = args + 1;
          if (!(flags & 0x810)) { buf[0] = (char)*args; len = 1; }
          else { len = (size_t)d2launch_wctomb(buf, (uint16_t)*args); if ((int)len < 0) nSkip = 1; }
          text = buf;
        } else if (kind == 5) {
          nargs = args + 1;
          uint8_t* s = (uint8_t*)b144_g2n(*args);
          const uint32_t* tp = s ? (const uint32_t*)(s + 4) : nullptr;
          void* tb = (s && tp) ? b144_g2n(*tp) : nullptr;
          if (!s || !tb) { text = kNull; len = 6; }
          else {
            text = tb; len = *(int16_t*)s;
            if (flags & 0x800) { len = (int32_t)len / 2; wide = 1; } else wide = 0;
          }
        } else if (kind == 2) {
          int32_t lim = prec == -1 ? 0x7fffffff : prec;
          nargs = args + 1;
          const void* s = b144_g2n(*args);
          if (!(flags & 0x810)) {
            const char* p = s ? (const char*)s : kNull;
            const char* q = p;
            for (; lim && (lim--, *q); q++) {}
            text = p; len = (size_t)(q - p);
          } else {
            const uint16_t* p = s ? (const uint16_t*)s : kWNull;
            wide = 1;
            const uint16_t* q = p;
            for (; lim && (lim--, *q); q++) {}
            text = p; len = (size_t)(q - p);
          }
        } else if (kind == 3) {
          flags |= 0x40;
          char* out = buf;
          if (prec < 0) prec = 6;
          else if (prec == 0) { if (t == 'g') prec = 1; }
          else {
            if (prec > 0x200) prec = 0x200;
            if (prec > 0xa3) { heapBuf = d2launch_malloc((size_t)prec + 0x15d); if (heapBuf) out = (char*)heapBuf; else prec = 0xa3; }
          }
          uint32_t dbl[2] = {args[0], args[1]};
          nargs = args + 2;
          PTR___fptrap_6fa606b0(dbl, out, (int8_t)t, prec, fltcaps);
          uint32_t alt = flags & 0x80;
          if (alt && prec == 0) PTR___fptrap_6fa606bc(out);
          if (t == 'g' && !alt) PTR___fptrap_6fa606b4(out);
          if (out[0] == '-') { flags |= 0x100; out++; }
          text = out; len = d2launch_strlen(out);
        } else if (kind == 4) {
          uint32_t lo, hi;
          if (!(flags & 0x8000)) {
            nargs = args + 1;
            lo = *args;
            if (flags & 0x20) lo = (flags & 0x40) ? (uint32_t)(int32_t)(int16_t)lo : (uint32_t)(uint16_t)lo;
            hi = (flags & 0x40) ? (uint32_t)((int32_t)lo >> 31) : 0;
          } else { lo = args[0]; hi = args[1]; nargs = args + 2; }
          if ((flags & 0x40) && (int32_t)hi < 0) {
            bool nz = lo != 0; lo = -lo; hi = -(hi + nz); flags |= 0x100;
          }
          if (!(flags & 0x8000)) hi = 0;
          if (prec < 0) prec = 1; else { flags &= ~8u; if (prec > 0x200) prec = 0x200; }
          if (lo == 0 && hi == 0) prefixLen = 0;
          int32_t i = 0x1ff;
          uint64_t v = ((uint64_t)hi << 32) | lo;
          for (;;) {
            if (prec < 1 && v == 0) break;
            prec--;
            uint32_t d = (uint32_t)(v % (uint32_t)radix); v /= (uint32_t)radix;
            int ch = (int)d + '0';
            if (ch > '9') ch += hexAdj;
            buf[i--] = (char)ch;
          }
          len = (size_t)(0x1ff - i); text = buf + i + 1;
          if ((flags & 0x200) && (*(const char*)text != '0' || len == 0)) { buf[i] = '0'; len++; text = buf + i; }
        }
        if (doEmit && !nSkip) {
          if (flags & 0x40) {
            if (flags & 0x100) prefix[0] = '-';
            else if (flags & 1) prefix[0] = '+';
            else if (flags & 2) prefix[0] = ' ';
            else goto noSign;
            prefixLen = 1;
          }
        noSign:
          int32_t pad = (width - prefixLen) - (int32_t)len;
          if (!(flags & 0xc)) d2launch_write_multi_char(' ', f, &charsout, pad);
          for (int k = 0; k < prefixLen; k++) d2launch_write_char((uint8_t)prefix[k], f, &charsout);
          if ((flags & 8) && !(flags & 4)) d2launch_write_multi_char('0', f, &charsout, pad);
          if (!wide || (int)len < 1) {
            for (size_t k = 0; k < len; k++) d2launch_write_char(((const uint8_t*)text)[k], f, &charsout);
          } else {
            const uint16_t* w = (const uint16_t*)text;
            for (size_t k = 0; k < len; k++) {
              int n = d2launch_wctomb(buf + 0x200, w[k]);
              if (n < 1) break;
              for (int j = 0; j < n; j++) d2launch_write_char((uint8_t)buf[0x200 + j], f, &charsout);
            }
          }
          if (flags & 4) d2launch_write_multi_char(' ', f, &charsout, pad);
        }
        if (heapBuf) { b144_free(heapBuf); heapBuf = nullptr; }
        break;
      }
    }
    c = *next; fmt = next; args = nargs;
  }
}

// 1.13c D2Launch+0x2517  _input (scanf core): stream, format, argument slots (guest pointers)
int32_t b144_input(void* f, const uint8_t* fmt, const uint32_t* args) {
  uint8_t table[32];
  uint32_t ch = 0;               // local_194
  int32_t charsread = 0, assigned = 0, started = 0;
  auto INC = [&]() { charsread++; ch = d2launch_inc(f); };
  auto UNG = [&](uint32_t c) { if (c != 0xffffffffu) d2launch_ungetc((int)c, f); };

  while (*fmt) {
    if (d2launch_isspace(*fmt)) {
      charsread--;
      uint32_t u;
      do { charsread++; u = d2launch_inc(f); } while (d2launch_isspace((int)u));
      UNG(u);
      do { fmt++; } while (d2launch_isspace(*fmt));
      continue;
    }
    if (*fmt != '%') {
      const uint8_t* p = fmt;
      INC(); fmt = p + 1;
      if (*p != ch) goto fail_unget;
      if (PTR_DAT_6fa606c8[ch & 0xff] & 0x8000) {
        charsread++;
        uint32_t c2 = d2launch_inc(f);
        fmt = p + 2;
        if (p[1] != c2) { UNG(c2); goto fail_unget; }
        charsread--;
      }
      goto next;
    }
    {
      int32_t width = 0, value = 0, digits = 0, widthSeen = 0, wrem = 0;
      uint64_t v64 = 0;
      uint8_t prevch = 0, xormask = 0;
      int neg = 0, suppress = 0, stop = 0, wide = 0, wstate = 0, size = 1, is64 = 0;
      const uint8_t* p = fmt;
      const uint32_t* savedArgs = args;
      void* dest = nullptr;
      do {
        const uint8_t* p16 = p + 1;
        uint32_t c = *p16;
        const uint8_t* pn = p16;
        if (d2launch_isdigit((int)c)) { widthSeen++; width = (int32_t)(c - '0') + width * 10; }
        else if (c < 0x4f) {
          if (c != 'N') {
            if (c == '*') suppress++;
            else if (c != 'F') {
              if (c == 'I') {
                uint8_t b = p[2];
                if (b == '6' && p[3] == '4') { is64++; v64 = 0; pn = p + 3; }
                else if (b == '3' && p[3] == '2') pn = p + 3;
                else if (b != 'd' && b != 'i' && b != 'o' && b != 'x' && b != 'X') stop++;
              } else if (c == 'L') size++;
              else stop++;
            }
          }
        } else if (c == 'h') { size--; wstate--; }
        else {
          if (c == 'l') size++;
          else if (c != 'w') { stop++; goto modDone; }
          wstate++;
        }
      modDone:
        p = pn;
      } while (!stop);
      if (!suppress) { dest = b144_g2n(*args); args++; }
      stop = 0;
      const bool prefixed = (*p | 0x20) == 'i' || (*p | 0x20) == 'x';
      uint32_t u;
      if (wstate == 0 && (*p == 'S' || (wstate = -1, *p == 'C'))) wstate = 1;
      uint32_t type = *p | 0x20;
      fmt = p;
      wrem = width;
      if (type != 'n') {
        if (type == 'c' || type == '{') INC();
        else do { INC(); } while (d2launch_isspace((int)ch));
      }
      if (widthSeen && width == 0) goto fail_unget;

      // dispatch
      if (type == 'c' || type == 's' || type == '{') {
        if (type == 'c') { if (!widthSeen) { widthSeen = 1; wrem++; } if (wstate > 0) wide = 1; }
        else if (type == 's') { if (wstate > 0) wide = 1; }
        else {
          if (wstate > 0) wide = 1;
          const uint8_t* q = p + 1;
          if (*q == '^') { q = p + 2; xormask = 0xff; }
          d2launch_memset(table, 0, 32);
          uint8_t b = prevch;
          if (*q == ']') { table[0xb] = 0x20; q++; b = ']'; }
          for (;;) {
            uint8_t cc = *q;
            if (cc == ']') break;
            if (cc == '-' && b != 0 && q[1] != ']') {
              uint8_t lo = q[1], hi = b;
              if (b < q[1]) { lo = b; hi = q[1]; }
              if (lo <= hi) for (uint32_t k = lo; k <= hi; k++) table[k >> 3] |= (uint8_t)(1u << (k & 7));
              q += 2; b = 0; continue;
            }
            prevch = cc; table[cc >> 3] |= (uint8_t)(1u << (cc & 7)); q++; b = cc;
          }
          fmt = q;
        }
        // 42acd
        charsread--;
        UNG(ch);
        int32_t consumed = 0;
        uint8_t* dp = (uint8_t*)dest;
        bool exhausted = false;
        for (;;) {
          if (widthSeen && wrem-- == 0) { exhausted = true; break; }
          INC();
          if (ch == 0xffffffffu) break;
          bool isSp = (ch > 8 && ch < 0xe) || ch == 0x20;
          if (type != 'c' && (type != 's' || isSp) && (type != '{' || ((((int8_t)(table[(int32_t)ch >> 3] ^ xormask)) & (1 << (ch & 7))) == 0))) break;
          if (!suppress) {
            if (!wide) *dp++ = (uint8_t)ch;
            else {
              uint8_t mb[2] = {(uint8_t)ch, 0};
              if (PTR_DAT_6fa606c8[ch & 0xff] & 0x8000) { INC(); mb[1] = (uint8_t)ch; }
              uint16_t wc; d2launch_mbtowc(&wc, (const char*)mb, DAT_6fa606e0);
              *(uint16_t*)dp = wc; dp += 2;
            }
          }
          consumed++;
        }
        if (!exhausted) { charsread--; UNG(ch); }
        if (consumed == 0) goto done;
        if (!suppress) {
          assigned++;
          if (type != 'c') { if (!wide) *dp = 0; else *(uint16_t*)dp = 0; }
        }
        goto complete;
      }
      if (type == 'e' || type == 'f' || type == 'g') {
        char fb[0x170];
        char* pc = fb;
        if (ch == '-') { fb[0] = '-'; pc++; goto fsign; }
        if (ch == '+') {
        fsign:
          wrem--; INC();
        }
        if (!widthSeen || wrem > 0x15d) wrem = 0x15d;
        for (;;) {
          if (!d2launch_isdigit((int)ch) || wrem-- == 0) break;
          digits++; *pc++ = (char)ch; INC();
        }
        if ((uint8_t)DAT_6fa606e4 == (uint8_t)ch && wrem-- != 0) {
          INC();
          *pc = DAT_6fa606e4;
          for (;;) {
            pc++;
            if (!d2launch_isdigit((int)ch) || wrem-- == 0) break;
            digits++; *pc = (char)ch; INC();
          }
        }
        char* p2 = pc;
        if (digits && (ch == 'e' || ch == 'E') && wrem-- != 0) {
          *pc = 'e'; p2 = pc + 1; INC();
          if (ch == '-' || ch == '+') {
            if (ch == '-') { *p2 = '-'; p2 = pc + 2; }
            bool more = wrem != 0; wrem--;
            if (more) INC(); else wrem = 0;
          }
          while (d2launch_isdigit((int)ch) && wrem-- != 0) { digits++; *p2++ = (char)ch; INC(); }
        }
        charsread--; UNG(ch);
        if (!digits) goto done;
        if (!suppress) { assigned++; *p2 = 0; PTR___fptrap_6fa606b8(size - 1, dest, fb); }
        goto complete;
      }
      if (type == 'n') {
        if (suppress) goto complete;
        value = charsread;
        goto store;
      }
      if (type == 'o' || type == 'd' || type == 'u' || type == 'i' || type == 'x' || type == 'p') {
        if (type == 'p') size = 1;
        if (type == 'i') type = 'd';
        if (ch == '-') neg = 1;
        if (ch == '-' || ch == '+') {
          wrem--;
          if (wrem == 0 && widthSeen) stop = 1; else INC();
        }
        if (prefixed) {   // 4286f path: i and x only
          if (ch == '0') {
            INC();
            if ((char)ch == 'x' || (char)ch == 'X') {
              INC();
              if (widthSeen && (wrem -= 2) < 1) stop++;
              type = 'x';
            } else {
              digits = 1;
              if (type == 'x') { charsread--; UNG(ch); ch = '0'; }
              else { if (widthSeen && --wrem == 0) stop++; type = 'o'; }
            }
          }
        }
        u = ch;
        while (!stop) {
          uint32_t orig = u;
          bool bad = false;
          if (type == 'x' || type == 'p') {
            if (d2launch_isxdigit((int)u)) {
              if (is64) v64 <<= 4; else value <<= 4;
              if (!d2launch_isdigit((int)u)) u = (u & ~0x20u) - 7;
            } else bad = true;
          } else if (!d2launch_isdigit((int)u)) bad = true;
          else if (type == 'o') {
            if ((int32_t)u > 0x37) bad = true;
            else if (is64) v64 <<= 3; else value <<= 3;
          } else if (is64) v64 *= 10; else value *= 10;
          if (!bad) {
            digits++;
            if (is64) v64 += (int64_t)((int32_t)u - 0x30); else value += (int32_t)u - 0x30;
            if (!widthSeen || --wrem != 0) { charsread++; u = d2launch_inc(f); } else stop = 1;
          } else {
            stop++; charsread--; u = orig; UNG(u);
          }
        }
        ch = u;
        if (neg) { if (is64) v64 = (uint64_t)(-(int64_t)v64); else value = -value; }
        if (type == 'F') digits = 0;
        if (!digits) goto done;
        if (suppress) goto complete;
        assigned++;
      store:
        if (!is64) { if (size == 0) *(int16_t*)dest = (int16_t)value; else *(int32_t*)dest = value; }
        else { ((uint32_t*)dest)[0] = (uint32_t)v64; ((uint32_t*)dest)[1] = (uint32_t)(v64 >> 32); }
        goto complete;
      }
      // literal percent or unknown conversion
      if (*p != ch) goto fail_unget;
      started--;
      if (!suppress) args = savedArgs;
    }
  complete:
    started++;
    fmt++;
  next:
    if (ch == 0xffffffffu && (fmt[0] != '%' || fmt[1] != 'n')) goto done;
    continue;
  }
  goto done;
fail_unget:
  UNG(ch);
done:
  if (ch == 0xffffffffu && assigned == 0 && started == 0) assigned = -1;
  return assigned;
}
