// D2Launch CRT batch b68. Skipped (SEH/CRT-lock/direct-Win32): 3B7F 3D82 43B8 49EA 4AC1 4CBF.
#include <cstdint>
#include <initializer_list>

struct CrtFile { char* ptr; int32_t cnt; char* base; uint32_t flag; int32_t file; int32_t charbuf; int32_t bufsiz; char* tmpfname; };
struct CrtPtd { uint32_t pad[2]; int32_t terrno; uint32_t tdoserrno; };  // _tiddata head: errno / doserrno slots

extern CrtPtd* __getptd();                          // 1.13c D2Launch+0x4714
extern uint32_t g_errnoTable[45][2];                // DAT_6fa603f0: {doserr, errno} x 0x2d
extern void FUN_6fa43479(int32_t lockNum);          // _unlock
extern uint8_t* g_ioInfo[];                         // DAT_6fa66fa0: ioinfo blocks, 0x24-byte entries
extern uint32_t g_nhandle;                          // DAT_6fa66f84
extern uint8_t g_ioInfoStdDummy[];                  // DAT_6fa60564
extern void __getbuf(CrtFile*);                     // 1.13c D2Launch+0x58E3
extern int32_t __read(int32_t, void*, uint32_t);    // 1.13c D2Launch+0x3F5F
extern int32_t* FUN_6fa43afa();                     // _errno
extern uint32_t* FUN_6fa43b03();                    // __doserrno
extern void __lock_fhandle(int32_t);                // 1.13c D2Launch+0x6A65
extern void FUN_6fa43fe6();                         // unlock_fhandle
extern int32_t FUN_6fa43d82(uint32_t, char*, char*);// 1.13c D2Launch+0x3D82

// 1.13c D2Launch+0x3B0C  __dosmaperr
void __dosmaperr(uint32_t e) {
  __getptd()->tdoserrno = e;
  for (uint32_t i = 0; i < 0x2d; i++)
    if (e == g_errnoTable[i][0]) { __getptd()->terrno = (int32_t)g_errnoTable[i][1]; return; }
  int32_t v = 0x16;
  if (e > 0x12 && e < 0x25) v = 0xd;
  else if (e > 0xbb && e < 0xcb) v = 8;
  __getptd()->terrno = v;
}

// 1.13c D2Launch+0x3C98  unlock(1) (stream list)
void FUN_6fa43c98() { FUN_6fa43479(1); }
// 1.13c D2Launch+0x440B  unlock(4) (heap)
void FUN_6fa4440b() { FUN_6fa43479(4); }
// 1.13c D2Launch+0x4AB0  unlock(4) (heap)
void FUN_6fa44ab0() { FUN_6fa43479(4); }

// 1.13c D2Launch+0x3CA1  __filbuf
int32_t __filbuf(CrtFile* f) {
  uint32_t fl = f->flag;
  if ((fl & 0x83) == 0 || (fl & 0x40)) return -1;
  if (fl & 2) { f->flag = fl | 0x20; return -1; }
  f->flag = fl | 1;
  if ((fl & 0x10c) == 0) __getbuf(f); else f->ptr = f->base;
  int32_t n = __read(f->file, f->base, (uint32_t)f->bufsiz);
  f->cnt = n;
  if (n == 0 || n == -1) {
    f->flag |= n != 0 ? 0x20u : 0x10u;  // ERR : EOF
    f->cnt = 0;
    return -1;
  }
  if ((f->flag & 0x82) == 0) {
    uint32_t h = (uint32_t)f->file;
    uint8_t* e = h == 0xffffffffu ? g_ioInfoStdDummy : g_ioInfo[(int32_t)h >> 5] + (h & 0x1f) * 0x24;
    if ((e[4] & 0x82) == 0x82) f->flag |= 0x2000;
  }
  if (f->bufsiz == 0x200 && (f->flag & 8) && !(f->flag & 0x400)) f->bufsiz = 0x1000;
  f->cnt = n - 1;
  return (uint8_t)*f->ptr++;
}

// 1.13c D2Launch+0x3F5F  __read (SEH prolog dropped)
int32_t __read(int32_t h, void* dst, uint32_t n) {
  if ((uint32_t)h < g_nhandle && (g_ioInfo[h >> 5][(h & 0x1f) * 0x24 + 4] & 1)) {
    __lock_fhandle(h);
    int32_t r;
    if (!(g_ioInfo[h >> 5][(h & 0x1f) * 0x24 + 4] & 1)) { *FUN_6fa43afa() = 9; *FUN_6fa43b03() = 0; r = -1; }
    else r = FUN_6fa43d82((uint32_t)h, (char*)dst, (char*)(uintptr_t)n);
    FUN_6fa43fe6();
    return r;
  }
  *FUN_6fa43afa() = 9; *FUN_6fa43b03() = 0;
  return -1;
}
