// D2Net.dll MSVC 2003 CRT: locale-info refcount, lock stubs, string/memory leaves, SBH init, cookie check.
// No packet bytes. Win32/CRT calls are function-pointer seams. Rows without a cached decompile (marked
// "shape") follow the VS2003 CRT source shape, not a decompile.
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace d2net_n93 {

struct Seam {
  void (*lock)(int) = nullptr;                // __lock
  void (*unlock)(int) = nullptr;              // _mtunlock_
  void* (*heap_alloc)(size_t) = nullptr;      // HeapAlloc(crt heap, 0, n)
  void (*terminate)(unsigned) = nullptr;      // UnhandledExceptionFilter + TerminateProcess
  void (*freetlocinfo)(int*) = nullptr;       // 0x317B
};
static Seam g;

// ptiddata->ptlocinfo (_tfpecode slot) and the process-wide locinfo (PTR_DAT_6fbfa484) as int words.
struct Ptd { int* ptlocinfo; };

// 1.13c D2NET+0x0324B
int updatetlocinfo_lk(Ptd* ptd, int* cur, int* global_locinfo) {
  if (cur != global_locinfo) {
    if (cur) {
      int* p;
      cur[0]--;
      if ((p = (int*)cur[0xb])) (*p)--;
      if ((p = (int*)cur[0xd])) (*p)--;
      if ((p = (int*)cur[0xc])) (*p)--;
      if ((p = (int*)cur[0x10])) (*p)--;
      (*(int*)(cur[0x13] + 0xb4))--;
    }
    ptd->ptlocinfo = global_locinfo;
    global_locinfo[0]++;
    int* p;
    if ((p = (int*)global_locinfo[0xb])) (*p)++;
    if ((p = (int*)global_locinfo[0xd])) (*p)++;
    if ((p = (int*)global_locinfo[0xc])) (*p)++;
    if ((p = (int*)global_locinfo[0x10])) (*p)++;
    (*(int*)(global_locinfo[0x13] + 0xb4))++;
    if (cur && cur[0] == 0 && cur != /*static default locinfo*/ nullptr && g.freetlocinfo) g.freetlocinfo(cur);
  }
  return (int)(intptr_t)ptd->ptlocinfo;
}

// 1.13c D2NET+0x0330C
int updatetlocinfo(Ptd* ptd, int* cur, int* global_locinfo) {
  g.lock(0xC);  // _SETLOCALE_LOCK
  int r = updatetlocinfo_lk(ptd, cur, global_locinfo);
  g.unlock(0xC);
  return r;
}

// 1.13c D2NET+0x0333E
void unlock_333e() { g.unlock(0xC); }

// 1.13c D2NET+0x03815
void unlock_3815() { g.unlock(0xD); }  // _MB_CP_LOCK

// 1.13c D2NET+0x044E0 (shape)
size_t crt_strlen(const char* s) { size_t n = 0; while (s[n]) n++; return n; }

// 1.13c D2NET+0x04380 (shape)
void* crt_memset(void* d, int c, size_t n) { auto* p = (uint8_t*)d; while (n--) *p++ = (uint8_t)c; return d; }

// 1.13c D2NET+0x049B0 (shape)
char* crt_strncpy(char* d, const char* s, size_t n) {
  char* o = d;
  while (n && (*d++ = *s++)) n--;
  if (n) while (--n) *d++ = 0;
  return o;
}

// 1.13c D2NET+0x05881 (shape): leading whitespace, sign, decimal digits, wraps like int
long crt_atol(const char* s) {
  while (*s == ' ' || (*s >= 9 && *s <= 13)) s++;
  bool neg = *s == '-';
  if (*s == '-' || *s == '+') s++;
  unsigned long v = 0;
  while (*s >= '0' && *s <= '9') v = v * 10 + (unsigned)(*s++ - '0');
  return (long)(neg ? -v : v);
}

// 1.13c D2Net+0x04AD4 (41 B, caller 0x4B05): report code 1 via 0x55FC, then ExitProcess(3); hook = terminate(3)
void report_failure() { g.terminate(3); }

// 1.13c D2Net+0x04B05 (shape): cookie compare, mismatch reports
void security_check_cookie(uint32_t cookie, uint32_t expected) { if (cookie != expected) report_failure(); }

// 1.13c D2NET+0x0383C (shape): 0x140-byte header list allocation, 1 = ok
int sbh_heap_init(void** header_list) {
  *header_list = g.heap_alloc(0x140);
  return *header_list != nullptr;
}

// 1.13c D2NET+0x03884: SBH region table (DAT_6fbfb440 base, DAT_6fbfb43c count) of 0x14-byte entries,
// +0xC = region base address; returns the entry whose 1 MiB range holds p, or 0.
struct SbhRegion { uint32_t w[5]; };
SbhRegion* sbh_find_block(uint32_t p, SbhRegion* table, uint32_t count) {
  for (SbhRegion* e = table; e < table + count; e++)
    if (p - e->w[3] < 0x100000) return e;
  return nullptr;
}

}  // namespace d2net_n93

// FN probe entry (the retail capture tool/fn/d2net-4b05.txt): terminate seam reduced to the recorder.
extern "C" void d2net_security_check_cookie(uint32_t cookie, uint32_t expected, void (*fail)(unsigned)) {
  d2net_n93::g.terminate = fail;
  d2net_n93::security_check_cookie(cookie, expected);
}
