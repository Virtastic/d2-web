// D2Multi batch z32: CRT fragments (lock table, mtinit, cookie, allmul). Retail globals/callees are seams, named by address.
// ponytail: 32-bit links held as uint32_t; guessed: none; __mtinitlocknum SEH unwind elided (seam only).
#include <cstdint>
extern "C" {
extern uint32_t z32_DAT_6f9e91e4;            // FLS/TLS index
extern uint32_t (*z32_FlsAlloc)(void*);      // DAT_6f9ebae8
extern void*    (*z32_FlsGetValue)(uint32_t);// DAT_6f9ebaec
extern int      (*z32_FlsSetValue)(uint32_t, void*); // DAT_6f9ebaf0
extern uint32_t z32_DAT_6f9e91f0;            // security cookie
extern uint32_t z32_lock_tab[0x24 * 2];      // DAT_6f9e91f8: {crit ptr, static flag}
extern uint8_t  z32_lock_pool[];             // DAT_6f9ebaf8, 0x18 per lock
extern uint8_t  z32_DAT_6f9e94c0[];
void z32_sub_31f1(int);                      // LeaveCriticalSection(lock n)
void z32_freefls(void*);                     // __freefls@4 (2DC8-adjacent)
int  z32_mtinitlocks_cs(uint32_t, int);      // ___crtInitCritSecAndSpinCount
void z32_mtterm(void);
void* z32_crt_calloc(int, int);              // 3ACA
void* z32_malloc(int);
void z32_free(void*);
int* z32_errno(void);                        // 39D4
void z32_lock_enter_cs(uint32_t);            // EnterCriticalSection
void z32_delete_cs(uint32_t);
void z32_amsg_exit(int);
void z32_report_exit(int);                   // 5EF9
void z32_ExitProcess(int);
uint32_t z32_GetCurrentThreadId(void);
void z32_unlock(int);
void z32_lock(int);
int z32_mtinitlocknum(int);

// 1.13c D2Multi+0x2FA4
void z32_sub_2fa4(void) { z32_sub_31f1(0xd); }
// 1.13c D2Multi+0x2FB0
void z32_sub_2fb0(void) { z32_sub_31f1(0xc); }

// 1.13c D2Multi+0x2FB9
void z32_freeptd(void* p) {
  if (z32_DAT_6f9e91e4 != 0xFFFFFFFFu) {
    if (!p) p = z32_FlsGetValue(z32_DAT_6f9e91e4);
    z32_freefls(p);
    z32_FlsSetValue(z32_DAT_6f9e91e4, nullptr);
  }
}

// 1.13c D2Multi+0x2FE8
int z32_mtinit(void) {
  extern int z32_mtinitlocks(void);
  extern void z32_mtinit_resolve_fls(void); // GetProcAddress fill of the four Fls* slots, TLS fallback
  if (!z32_mtinitlocks()) { z32_mtterm(); return 0; }
  z32_mtinit_resolve_fls();
  z32_DAT_6f9e91e4 = z32_FlsAlloc((void*)z32_freefls);
  if (z32_DAT_6f9e91e4 != 0xFFFFFFFFu) {
    uint32_t* d = (uint32_t*)z32_crt_calloc(1, 0x8c);
    if (d && z32_FlsSetValue(z32_DAT_6f9e91e4, d)) {
      d[0x15] = (uint32_t)(uintptr_t)z32_DAT_6f9e94c0;
      d[5] = 1;
      d[1] = 0xFFFFFFFFu;
      d[0] = z32_GetCurrentThreadId();
      return 1;
    }
  }
  z32_mtterm();
  return 0;
}

// 1.13c D2Multi+0x30E0
int64_t z32_allmul(uint32_t a_lo, int32_t a_hi, uint32_t b_lo, int32_t b_hi) {
  uint64_t p = (uint64_t)a_lo * b_lo;
  if (!a_hi && !b_hi) return (int64_t)p;
  uint32_t hi = (uint32_t)(p >> 32) + (uint32_t)a_hi * b_lo + a_lo * (uint32_t)b_hi;
  return (int64_t)(((uint64_t)hi << 32) | (uint32_t)p);
}

// 1.13c D2Multi+0x3114
[[noreturn]] void z32_report_failure(void) { z32_report_exit(1); z32_ExitProcess(3); for (;;) {} }

// 1.13c D2Multi+0x3145
void z32_security_check_cookie(uint32_t c) { if (c != z32_DAT_6f9e91f0) z32_report_failure(); }

// 1.13c D2Multi+0x3153
int z32_mtinitlocks(void) {
  uint8_t* pool = z32_lock_pool;
  for (int i = 0; i < 0x24; i++) {
    if (z32_lock_tab[i * 2 + 1] == 1) {
      z32_lock_tab[i * 2] = (uint32_t)(uintptr_t)pool;
      pool += 0x18;
      if (!z32_mtinitlocks_cs(z32_lock_tab[i * 2], 4000)) { z32_lock_tab[i * 2] = 0; return 0; }
    }
  }
  return 1;
}

// 1.13c D2Multi+0x319C
void z32_mtdeletelocks(void) {
  for (int i = 0; i < 0x24; i++)
    if (z32_lock_tab[i * 2] && z32_lock_tab[i * 2 + 1] != 1) {
      z32_delete_cs(z32_lock_tab[i * 2]);
      z32_free((void*)(uintptr_t)z32_lock_tab[i * 2]);
      z32_lock_tab[i * 2] = 0;
    }
  for (int i = 0; i < 0x24; i++)
    if (z32_lock_tab[i * 2] && z32_lock_tab[i * 2 + 1] == 1) z32_delete_cs(z32_lock_tab[i * 2]);
}

// 1.13c D2Multi+0x3206
int z32_mtinitlocknum(int n) {
  uint32_t* slot = &z32_lock_tab[n * 2];
  if (*slot) return 1;
  void* m = z32_malloc(0x18);
  if (!m) { *z32_errno() = 0xc; return 0; }
  z32_lock(10);
  if (!*slot) {
    if (!z32_mtinitlocks_cs((uint32_t)(uintptr_t)m, 4000)) {
      z32_free(m); *z32_errno() = 0xc; return 0; // retail unwinds via __local_unwind2 without unlock
    }
    *slot = (uint32_t)(uintptr_t)m;
  } else z32_free(m);
  z32_unlock(10);
  return 1;
}

// 1.13c D2Multi+0x329D
void z32_sub_329d(void) { z32_sub_31f1(10); }

// 1.13c D2Multi+0x32A6
void z32_lock(int n) {
  if (!z32_lock_tab[n * 2] && !z32_mtinitlocknum(n)) z32_amsg_exit(0x11);
  z32_lock_enter_cs(z32_lock_tab[n * 2]);
}
}
