// D2Launch CRT batch 5c18. Not ported (Win32 stack/SEH-image specific): 5CF0 __resetstkoflw, 62A6 __ValidateEH3RN.
#include <cstdint>
#include <cstddef>

struct CrtFile { char* ptr; int32_t cnt; char* base; uint32_t flag; int32_t file; int32_t charbuf; int32_t bufsiz; char* tmpfname; };

extern void __getbuf(CrtFile*);                     // 1.13c D2Launch+0x58E3
extern int32_t* FUN_6fa43afa();                     // _errno
extern void FUN_6fa43479(int32_t lockNum);          // _unlock
extern void __lock(int32_t);                        // 1.13c D2Launch+0x352E
extern void* ___sbh_find_block(void*);
extern void* ___sbh_alloc_block(uint32_t);
extern int32_t ___sbh_resize_block(void*, void*, uint32_t);
extern void ___sbh_free_block(void*, void*);
extern int32_t __callnewh(uint32_t);
extern void* _malloc(uint32_t);
extern void _free(void*);
extern void* _memcpy(void*, const void*, uint32_t);
extern int32_t MultiByteToWideChar(uint32_t, uint32_t, const char*, int32_t, uint16_t*, int32_t);
extern void* HeapAlloc(void*, uint32_t, uint32_t);
extern void* HeapReAlloc(void*, uint32_t, void*, uint32_t);
extern uint32_t HeapSize(void*, uint32_t, void*);
extern void RtlUnwind(void*, void*, void*, void*);
extern void* GetModuleHandleA(const char*);
extern void* GetProcAddress(void*, const char*);
extern void ___crtInitCritSecNoSpinCount_8(void*, uint32_t);
extern uint32_t DAT_6fa64de4, DAT_6fa66f80, DAT_6fa65124;
extern void* DAT_6fa66f7c;
extern uint32_t DAT_6fa66f6c;
extern uint32_t DAT_6fa606f4, DAT_6fa606f8, DAT_6fa606fc;
static void (*DAT_6fa650b8)(void*, uint32_t);

// 1.13c D2Launch+0x5C18  _ungetc
int32_t FID_conflict__ungetc(int32_t ch, CrtFile* f) {
  if (ch == -1) return -1;
  uint32_t fl = f->flag;
  if (!((fl & 1) || ((int8_t)fl < 0 && !(fl & 2)))) return -1;
  if (!f->base) __getbuf(f);
  if (f->ptr == f->base) { if (f->cnt) return -1; f->ptr++; }
  f->ptr--;
  if (!(f->flag & 0x40)) *f->ptr = (char)ch;
  else if (*f->ptr != (char)ch) { f->ptr++; return -1; }
  f->cnt++;
  f->flag = (f->flag & 0xffffffefu) | 1;
  return ch & 0xff;
}

// 1.13c D2Launch+0x5DD3  ___mbtowc_mt (p = locale info)
int32_t ___mbtowc_mt(uint8_t* p, uint16_t* wc, const uint8_t* s, uint32_t n) {
  if (!s || !n) return 0;
  uint8_t b = *s;
  if (!b) { if (wc) *wc = 0; return 0; }
  if (*(int32_t*)(p + 0x14) == 0) { if (wc) *wc = b; return 1; }
  uint32_t cp = *(uint32_t*)(p + 4);
  if (!(*(uint8_t*)(*(uintptr_t*)(p + 0x48) + 1 + b * 2) & 0x80)) {
    if (MultiByteToWideChar(cp, 9, (const char*)s, 1, wc, wc != nullptr)) return 1;
  } else {
    int32_t m = *(int32_t*)(p + 0x28);
    if ((m > 1 && m <= (int32_t)n && MultiByteToWideChar(cp, 9, (const char*)s, m, wc, wc != nullptr)) ||
        ((uint32_t)m <= n && s[1]))
      return m;
  }
  *FUN_6fa43afa() = 0x2a;
  return -1;
}

// 1.13c D2Launch+0x5F04  ___crtInitCritSecAndSpinCount
void ___crtInitCritSecAndSpinCount(void* cs, uint32_t spin) {
  if (!DAT_6fa650b8) {
    if (DAT_6fa64de4 != 1) {
      void* m = GetModuleHandleA("kernel32.dll");
      if (m) DAT_6fa650b8 = (void (*)(void*, uint32_t))GetProcAddress(m, "InitializeCriticalSectionAndSpinCount");
    }
    if (!DAT_6fa650b8) DAT_6fa650b8 = ___crtInitCritSecNoSpinCount_8;
  }
  DAT_6fa650b8(cs, spin);
}

// 1.13c D2Launch+0x5F90  __global_unwind2
void __global_unwind2(void* frame) { RtlUnwind(frame, nullptr, nullptr, nullptr); }

// 1.13c D2Launch+0x6066  (EBP-frame capture for the unwind handler)
void FUN_6fa46066(uint32_t eax, uint32_t* ebp) { DAT_6fa606f8 = ebp[2]; DAT_6fa606f4 = eax; DAT_6fa606fc = (uint32_t)(uintptr_t)ebp; }

// 1.13c D2Launch+0x5FD2  __local_unwind2 (exception-list link omitted; host has no TIB chain)
void __local_unwind2(uint8_t* rec, int32_t stop) {
  for (;;) {
    uint32_t* tbl = *(uint32_t**)(rec + 8);
    int32_t lvl = *(int32_t*)(rec + 0xc);
    if (lvl == -1 || lvl == stop) break;
    *(uint32_t*)(rec + 0xc) = tbl[lvl * 3];
    if (tbl[lvl * 3 + 1] == 0) {
      FUN_6fa46066(0, (uint32_t*)rec);
      ((void (*)())(uintptr_t)tbl[lvl * 3 + 2])();
    }
  }
}

// 1.13c D2Launch+0x61E6  unlock(4)
void FUN_6fa461e6() { FUN_6fa43479(4); }
// 1.13c D2Launch+0x629D  unlock(4)
void FUN_6fa4629d() { FUN_6fa43479(4); }

// 1.13c D2Launch+0x607E  _realloc
void* FUN_6fa4607e(void* p, uint32_t n) {
  if (!p) return _malloc(n);
  if (!n) { _free(p); return nullptr; }
  for (;;) {
    if (n > 0xffffffe0u) { __callnewh(n); return nullptr; }
    void* r = nullptr;
    if (DAT_6fa66f80 == 3) {
      __lock(4);
      void* blk = ___sbh_find_block(p);
      if (blk) {
        uint32_t old = ((uint32_t*)p)[-1] - 1;
        if (n <= DAT_6fa66f6c) {
          if (___sbh_resize_block(blk, p, n)) r = p;
          else if ((r = ___sbh_alloc_block(n))) {
            _memcpy(r, p, n <= old ? n : old);
            ___sbh_free_block(___sbh_find_block(p), p);
          }
        }
        if (!r) {
          n = (n ? n : 1) + 0xf & 0xfffffff0u;
          if ((r = HeapAlloc(DAT_6fa66f7c, 0, n))) { _memcpy(r, p, n <= old ? n : old); ___sbh_free_block(blk, p); }
        }
      }
      FUN_6fa461e6();
      if (!blk) { n = (n ? n : 1) + 0xf & 0xfffffff0u; r = HeapReAlloc(DAT_6fa66f7c, 0, p, n); }
    } else {
      r = HeapReAlloc(DAT_6fa66f7c, 0, p, n ? n : 1);
    }
    if (r) return r;
    if (!DAT_6fa65124 || !__callnewh(n)) return nullptr;
  }
}

// 1.13c D2Launch+0x6230  __msize
uint32_t __msize(void* p) {
  if (DAT_6fa66f80 == 3) {
    __lock(4);
    void* blk = ___sbh_find_block(p);
    uint32_t r = blk ? ((uint32_t*)p)[-1] - 9 : 0;
    FUN_6fa4629d();
    if (blk) return r;
  }
  return HeapSize(DAT_6fa66f7c, 0, p);
}
