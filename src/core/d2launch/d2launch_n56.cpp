// D2Launch MSVC CRT (VS2003) low-level I/O, per-thread data, calloc, env block, SEH filter.
// Retail globals are explicit state; every Win32/CRT service is a host function-pointer seam in D2L56Host.
#include <cstdint>
#include <cstring>

struct D2L56IoInfo { uint32_t osfhnd; uint8_t osfile; uint8_t pipech; uint8_t pad[2]; int32_t lockinit; uint8_t cs[24]; };  // 0x24
struct D2L56File { char* ptr; int32_t cnt; char* base; uint32_t flag; int32_t file; int32_t charbuf; int32_t bufsiz; char* tmpfname; };
struct D2L56Xcpt { uint32_t code; int32_t sig; uintptr_t act; };           // 12 bytes in retail
struct D2L56Ptd {                                                          // 0x8c in retail
  uint32_t tid; uint32_t thandle; int32_t terrno; uint32_t holdrand; void* initarg; D2L56Xcpt* initaddr; void* pxcptacttab; uint8_t rest[0x8c - 0x20];
};

struct D2L56Host {
  D2L56IoInfo* (*ioinfo)(uint32_t fh);                    // __pioinfo[fh>>5] + (fh&31)
  int (*isStdStream)(D2L56File*);                         // &_iob[1] or &_iob[2]
  int (*isatty)(int fh);
  void (*getbuf)(D2L56File*);
  int (*write)(int fh, const void* buf, uint32_t n);
  long (*lseek)(int fh, long off, int whence);
  int (*readFile)(uint32_t h, void* buf, uint32_t n, uint32_t* got);   // ReadFile, 0 on failure
  uint32_t (*getLastError)(void);
  void (*setLastError)(uint32_t);
  void (*dosmaperr)(uint32_t);
  void (*setErrnoDosErr)(int errnoV, uint32_t doserr);    // *_errno()=errnoV; *__doserrno()=doserr
  long long (*lseeki64lk)(int fh, long long off, int whence);
  void* (*tlsGet)(void);
  int (*tlsSet)(void*);
  void* (*calloc)(uint32_t n, uint32_t sz);
  uint32_t (*curThreadId)(void);
  void (*amsgExit)(int);
  int sbhMode; uint32_t sbhThreshold;                     // DAT_6fa66f80 (3 = small-block heap), DAT_6fa66f6c
  void (*lock)(int);  void (*unlock)(int);
  void* (*sbhAlloc)(uint32_t);
  void* (*heapAllocZero)(uint32_t);                       // HeapAlloc(heap, HEAP_ZERO_MEMORY, n)
  int newhMode; int (*callnewh)(uint32_t);                // DAT_6fa65124
  void (*deleteCS)(void*); void (*crtFree)(void*);
  void* (*crtMalloc)(uint32_t);
  int envState;                                           // DAT_6fa650b0
  uint16_t* (*getEnvW)(void); char* (*getEnvA)(void);
  void (*freeEnvW)(uint16_t*); void (*freeEnvA)(char*);
  int (*wide2mb)(const uint16_t*, int, char*, int);
  int (*unhandledFilter)(void*);
  int xcptIdxA, xcptCnt;                                  // DAT_6fa60614, DAT_6fa60608/0c
  int fpeFirst, fpeCount;
  D2L56Xcpt* defaultXcptTab;
};
D2L56Host d2l56;
static D2L56IoInfo* IO(int fh) { return d2l56.ioinfo((uint32_t)fh); }

// 1.13c D2LAUNCH+0x018FE
void d2l56_sub_18FE(D2L56File* f, void (*unlockFile)(D2L56File*)) { unlockFile(f); }

// 1.13c D2LAUNCH+0x01B9E
int d2l56_flsbuf(int ch, D2L56File* f) {
  uint32_t fl = f->flag; int fh = f->file;
  if (((fl & 0x82) == 0) || (fl & 0x40)) { f->flag = fl | 0x20; return -1; }
  if (fl & 1) {
    f->cnt = 0;
    if (!(fl & 0x10)) { f->flag = fl | 0x20; return -1; }
    f->ptr = f->base; f->flag = fl & ~1u;
  }
  fl = f->flag; f->cnt = 0;
  int wrote = 0, want;
  f->flag = (fl & ~0x10u) | 2;
  if (((fl & 0x10c) == 0) && ((!d2l56.isStdStream(f)) || !d2l56.isatty(fh))) d2l56.getbuf(f);
  if ((f->flag & 0x108) == 0) {
    char c = (char)ch; want = 1; wrote = d2l56.write(fh, &c, 1);
  } else {
    char* b = f->base; char* p = f->ptr; f->ptr = b + 1; want = (int)(p - b); f->cnt = f->bufsiz - 1;
    if (want < 1) { if (fh != -1 && (IO(fh)->osfile & 0x20)) d2l56.lseek(fh, 0, 2); }
    else wrote = d2l56.write(fh, b, (uint32_t)want);
    *f->base = (char)ch;
  }
  if (wrote == want) return ch & 0xff;
  f->flag |= 0x20; return -1;
}

// 1.13c D2LAUNCH+0x03639
void d2l56_sub_3639(void (*fn_150D)(void)) { fn_150D(); }

// 1.13c D2LAUNCH+0x03D82
int d2l56_read_lk(uint32_t fh, char* buf, uint32_t cnt) {
  if (!cnt) return 0;
  D2L56IoInfo* io = IO((int)fh);
  if (io->osfile & 2) return 0;
  char* dst = buf; uint32_t pre = 0, got = 0;
  if ((io->osfile & 0x48) && io->pipech != '\n') { cnt--; *buf = (char)io->pipech; dst = buf + 1; pre = 1; io->pipech = '\n'; }
  if (!d2l56.readFile(io->osfhnd, dst, cnt, &got)) {
    uint32_t e = d2l56.getLastError();
    if (e == 5) d2l56.setErrnoDosErr(9, 5);
    else { if (e == 0x6d) return 0; d2l56.dosmaperr(e); }
    return -1;
  }
  if (!(io->osfile & 0x80)) return (int)(pre + got);
  if (got == 0 || *buf != '\n') io->osfile &= 0xfb; else io->osfile |= 4;
  char* end = buf + pre + got; char* p = buf; char* o = buf;
  while (p < end) {
    char c = *p;
    if (c == 0x1a) { if (!(io->osfile & 0x40)) io->osfile |= 2; break; }
    if (c == '\r') {
      char* nx = p + 1;
      if (p < end - 1) {
        if (p[1] == '\n') { p = p + 2; *o++ = '\n'; continue; }
        *o++ = '\r'; p++; continue;
      }
      char t; uint32_t n2 = 0;
      if ((!d2l56.readFile(io->osfhnd, &t, 1, &n2) && d2l56.getLastError() != 0) || n2 == 0) { *o++ = '\r'; p = nx; continue; }
      if (!(io->osfile & 0x48)) {
        if (o == buf && t == '\n') { p = nx; *o++ = '\n'; continue; }
        d2l56.lseeki64lk((int)fh, -1, 1);
        if (t == '\n') { p = nx; continue; }
        *o++ = '\r'; p = nx; continue;
      }
      if (t == '\n') { p = nx; *o++ = '\n'; } else { *o++ = '\r'; io->pipech = (uint8_t)t; p = nx; }
      continue;
    }
    *o++ = c; p++;
  }
  return (int)(o - buf);
}

// 1.13c D2LAUNCH+0x03FE6
void d2l56_sub_3FE6(int fh, void (*unlockFhandle)(int)) { unlockFhandle(fh); }

// 1.13c D2LAUNCH+0x04523
void d2l56_sub_4523(int fh, void (*unlockFhandle)(int)) { unlockFhandle(fh); }

// 1.13c D2LAUNCH+0x05658
void d2l56_sub_5658(int fh, void (*unlockFhandle)(int)) { unlockFhandle(fh); }
