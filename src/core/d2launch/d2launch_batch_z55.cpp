// D2Launch batch z55: MSVCRT 2003 statics (fsopen/fread/fclose/time/CRT_INIT/entry). CRT callees are seams.
// FILE* are 32-bit retail pointers (uint32_t); FILE fields accessed by retail offsets
// (_ptr 0, _cnt 4, _base 8, _flag 0xC, _file 0x10, _bufsiz 0x18, _tmpfname 0x1C).
#include <cstdint>
#include <cstring>

#define W __attribute__((weak)) extern
W uint32_t FUN_getstream(void);                                   // 0x3992 __getstream
W uint32_t FUN_openfile(uint32_t, uint32_t, int, uint32_t);       // 0x36DC-area __openfile
W int32_t* FUN_errno(void);                                       // 0x3AFA
W void CRT_unlock_file(uint32_t);                                 // 0x3940
W void CRT_lock_file(uint32_t);                                   // 0x36DC
W int CRT_filbuf(uint32_t);                                       // 0x4010
W int CRT_read(int, void*, uint32_t);                             // 0x3F5F
W int CRT_flush(uint32_t);                                        // 0x4572
W void CRT_freebuf(uint32_t);                                     // 0x43B8
W int CRT_close(int);                                             // 0x44AC
W void CRT_free(void*);                                           // 0x4547
W void CRT_doexit(int, int, int);                                 // 0x1598
W uint64_t CRT_aulldiv(uint32_t, uint32_t, uint32_t, uint32_t);   // 0x4350
W void CRT_GetSystemTimeAsFileTime(uint32_t out[2]);
W uint32_t CRT_cinit_all(uint32_t param_1, int reason);           // pieces of 0x1906 (heap/mt/ioinit/setargv/cinit)
W uint32_t DAT_6fa64e28, DAT_6fa64e20;
W uint32_t (*DAT_6fa680c8)(uint32_t, int, uint32_t);
W uint32_t FUN_6fa49b70(uint32_t);

static inline uint32_t& F(uint32_t f, int o) { return *(uint32_t*)(uintptr_t)(f + o); }

// 1.13c D2LAUNCH+0x167D  (__cexit)
void FUN_6fa4167d(void) { CRT_doexit(0, 0, 1); }

// 1.13c D2LAUNCH+0x16DE  (fsopen unlock thunk; FILE* in caller frame at ebp-0x1c)
void FUN_6fa416de(uint32_t file) { CRT_unlock_file(file); }

// 1.13c D2LAUNCH+0x168C  (__fsopen)
uint32_t FUN_6fa4168c(uint32_t name, uint32_t mode, int shflag) {
  uint32_t f = FUN_getstream();
  if (!f) { *FUN_errno() = 0x18; return 0; }
  f = FUN_openfile(name, mode, shflag, f);
  FUN_6fa416de(f);  // ponytail: retail unlocks the stream slot held in the SEH frame; same pointer
  return f;
}

// 1.13c D2LAUNCH+0x16E8  (_fopen)
uint32_t FUN_6fa416e8(uint32_t name, uint32_t mode) { return FUN_6fa4168c(name, mode, 0x40); }

// 1.13c D2LAUNCH+0x16FB  (__fread_lk)
uint32_t FUN_6fa416fb(uint8_t* dst, uint32_t size, uint32_t count, uint32_t fp) {
  uint32_t total = size * count, left, bufsz;
  if (!total) return 0;
  left = total;
  bufsz = (F(fp, 0xC) & 0x10c) ? F(fp, 0x18) : 0x1000;
  do {
    int32_t cnt = (int32_t)F(fp, 4);
    if (!(F(fp, 0xC) & 0x10c) || cnt == 0) {
      if (left < bufsz) {
        int c = CRT_filbuf(fp);
        if (c == -1) return (total - left) / size;
        *dst++ = (uint8_t)c;
        bufsz = F(fp, 0x18);
        left--;
      } else {
        uint32_t n = bufsz ? left - left % bufsz : left;
        int r = CRT_read((int)F(fp, 0x10), dst, n);
        if (r == 0) { F(fp, 0xC) |= 0x10; return (total - left) / size; }
        if (r == -1) { F(fp, 0xC) |= 0x20; return (total - left) / size; }
        left -= r; dst += r;
      }
    } else {
      uint32_t n = left < (uint32_t)cnt ? left : (uint32_t)cnt;
      memcpy(dst, (void*)(uintptr_t)F(fp, 0), n);
      left -= n; F(fp, 4) -= n; F(fp, 0) += n; dst += n;
    }
  } while (left);
  return count;
}

// 1.13c D2LAUNCH+0x1826  (fread unlock thunk)
void FUN_6fa41826(uint32_t file) { CRT_unlock_file(file); }

// 1.13c D2LAUNCH+0x17E4  (_fread)
uint32_t FUN_6fa417e4(void* dst, uint32_t size, uint32_t count, uint32_t fp) {
  CRT_lock_file(fp);
  uint32_t r = FUN_6fa416fb((uint8_t*)dst, size, count, fp);
  FUN_6fa41826(fp);
  return r;
}

// 1.13c D2LAUNCH+0x1830  (_time)
int32_t FUN_6fa41830(int32_t* out) {
  uint32_t ft[2];
  CRT_GetSystemTimeAsFileTime(ft);
  uint64_t q = CRT_aulldiv(ft[0] + 0x2ac18000u, ft[1] + 0xfe624e21u + (ft[0] > 0xd53e7fffu), 10000000, 0);
  if (out) *out = (int32_t)q;
  return (int32_t)q;
}

// 1.13c D2LAUNCH+0x1869  (__fclose_lk)
int FUN_6fa41869(uint32_t fp) {
  int r = -1;
  if (F(fp, 0xC) & 0x83) {
    r = CRT_flush(fp);
    CRT_freebuf(fp);
    if (CRT_close((int)F(fp, 0x10)) < 0) r = -1;
    else if (F(fp, 0x1C)) { CRT_free((void*)(uintptr_t)F(fp, 0x1C)); F(fp, 0x1C) = 0; }
  }
  F(fp, 0xC) = 0;
  return r;
}

// 1.13c D2LAUNCH+0x18B5  (_fclose; 0x18FE unlock thunk = CRT_unlock_file)
int FUN_6fa418b5(uint32_t fp) {
  if (F(fp, 0xC) & 0x40) { F(fp, 0xC) = 0; return -1; }
  CRT_lock_file(fp);
  int r = FUN_6fa41869(fp);
  CRT_unlock_file(fp);
  return r;
}

// 1.13c D2LAUNCH+0x1906  (__CRT_INIT@12) PARTIAL: the per-reason CRT bring-up/teardown
// (GetVersionEx globals, heap/mtinit/ioinit/setargv/setenvp/cinit, TLS ptd alloc) is one seam.
uint32_t FUN_6fa41906(uint32_t hinst, int reason) { return CRT_cinit_all(hinst, reason); }

// 1.13c D2LAUNCH+0x1A87  (DLL entry)
int FUN_6fa41a87(uint32_t hinst, int reason, uint32_t reserved) {
  int r = 1;
  if (reason == 0 && DAT_6fa64e28 == 0) return 0;
  if (reason == 1 || reason == 2) {
    if (DAT_6fa680c8) r = DAT_6fa680c8(hinst, reason, reserved);
    if (r == 0 || FUN_6fa41906(hinst, reason) == 0) return 0;
  }
  r = FUN_6fa49b70(hinst);
  if (reason == 1 && r == 0) FUN_6fa41906(hinst, 0);
  if (reason == 0 || reason == 3) {
    if (FUN_6fa41906(hinst, reason) == 0) r = 0;
    if (r != 0 && DAT_6fa680c8) r = DAT_6fa680c8(hinst, reason, reserved);
  }
  return r;
}
