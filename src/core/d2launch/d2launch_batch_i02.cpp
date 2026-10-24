// src/core/d2launch/d2launch_batch_i02.cpp -- 1.13c D2Launch.dll VS2003 CRT rows (batch i02).
// Retail 32-bit layouts; Win32/CRT callees are seams (null until wired).
#include <cstdint>
#include <cstddef>
#include <cstring>

static void* (*i02_malloc)(size_t) = nullptr;
static void (*i02_free)(void*) = nullptr;
static int (*i02_callnewh)(size_t) = nullptr;                    // 0x7F43
static void* (*i02_sbh_alloc)(size_t) = nullptr;                 // ___sbh_alloc_block
static void (*i02_lock)(int) = nullptr;                          // 0x352E
static void (*i02_unlock)(int) = nullptr;                        // 0x3479
static void* (*i02_heap_alloc_win)(uint32_t flags, size_t) = nullptr;  // HeapAlloc(DAT_6fa66f7c,..)
static int32_t i02_sbh_mode = 0;                                 // DAT_6fa66f80 (3 = small-block heap)
static size_t i02_sbh_threshold = 0;                             // DAT_6fa66f6c
static int32_t i02_new_handler_set = 0;                          // DAT_6fa65124
static void (*i02_flush_all)(int) = nullptr;                     // 0x81C0
static void (*i02_exit_process)(uint32_t) = nullptr;             // ExitProcess

// 1.13c D2LAUNCH+0x49EA  (_calloc_impl)
void* i02_calloc(uint32_t n, uint32_t sz) {
  if (n != 0 && sz > 0xffffffe0u / n) return nullptr;
  uint32_t size = n * sz, req = size ? size : 1;
  for (;;) {
    void* p = nullptr;
    if (req < 0xffffffe1u) {
      if (i02_sbh_mode == 3) {
        req = (req + 0xf) & 0xfffffff0u;
        if (size <= i02_sbh_threshold) {
          i02_lock(4); p = i02_sbh_alloc(size); i02_unlock(4);
          if (p) { memset(p, 0, size); return p; }
        }
      }
      p = i02_heap_alloc_win(8, req);
    }
    if (p) return p;
    if (!i02_new_handler_set) return nullptr;
    if (!i02_callnewh(req)) return nullptr;
  }
}

// 1.13c D2LAUNCH+0x5951  (__heap_alloc)
void* i02_heap_alloc(size_t size) {
  if (i02_sbh_mode == 3 && size <= i02_sbh_threshold) {
    i02_lock(4); void* p = i02_sbh_alloc(size); i02_unlock(4);   // 0x59C3 is unlock(4)
    if (p) return p;
  }
  if (size == 0) size = 1;
  if (i02_sbh_mode != 1) size = (size + 0xf) & ~(size_t)0xf;
  return i02_heap_alloc_win(0, size);
}

// 1.13c D2LAUNCH+0x59C3
void i02_unlock4() { i02_unlock(4); }

// 1.13c D2LAUNCH+0x59CC  (__nh_malloc)
void* i02_nh_malloc(size_t size, int nhFlag) {
  if (size < 0xffffffe1u)
    do {
      void* p = i02_heap_alloc(size);
      if (p) return p;
      if (!nhFlag) return nullptr;
    } while (i02_callnewh(size));
  return nullptr;
}

// 1.13c D2LAUNCH+0x51F0  (__chkstk): stack probe, no-op under wasm.
void i02_chkstk() {}

// 1.13c D2LAUNCH+0x5A91  (_report_failure)
void i02_report_failure() { i02_flush_all(1); i02_exit_process(3); }

// ---- ioinfo table (__ioinit / __ioterm); entry = 0x24 bytes: osfhnd,flags,opencount,critsec[6]
struct I02Io { int32_t osfhnd; uint8_t osfile, pipech; uint16_t pad; int32_t opencount; uint8_t cs[24]; };
static I02Io* i02_pioinfo[32];                                   // DAT_6fa66fa0
static int32_t i02_nhandle = 0;                                  // DAT_6fa66f84
static void (*i02_get_startup)(uint32_t* cbReserved2, uint8_t** lpReserved2) = nullptr;
static uint32_t (*i02_file_type)(int32_t) = nullptr;
static int32_t (*i02_get_std)(uint32_t) = nullptr;
static int (*i02_init_cs)(void*, uint32_t) = nullptr;            // ___crtInitCritSecAndSpinCount
static void (*i02_del_cs)(void*) = nullptr;
static void (*i02_set_handle_count)(int32_t) = nullptr;

static void i02_ioblock_init(I02Io* p) {
  for (int i = 0; i < 32; i++) { p[i].osfhnd = -1; p[i].opencount = 0; p[i].osfile = 0; p[i].pipech = 10; }
}

// 1.13c D2LAUNCH+0x4AC1  (__ioinit)
int i02_ioinit() {
  I02Io* b = (I02Io*)i02_malloc(0x480);
  if (!b) return -1;
  i02_nhandle = 0x20; i02_pioinfo[0] = b; i02_ioblock_init(b);
  uint32_t cb = 0; uint8_t* res = nullptr; i02_get_startup(&cb, &res);
  if (cb != 0 && res) {
    int32_t n = *(int32_t*)res; uint8_t* flags = res + 4; uint8_t* h = flags + n;
    if (n > 0x7ff) n = 0x800;
    int32_t cnt = n;
    for (int s = 1; i02_nhandle < n; s++) {
      I02Io* nb = (I02Io*)i02_malloc(0x480);
      if (!nb) { cnt = i02_nhandle; break; }
      i02_nhandle += 0x20; i02_pioinfo[s] = nb; i02_ioblock_init(nb);
    }
    for (int32_t i = 0; i < cnt; i++, h += 4, flags++) {
      int32_t hv = *(int32_t*)h;
      if (hv != -1 && (*flags & 1) && ((*flags & 8) || i02_file_type(hv) != 0)) {
        I02Io* e = &i02_pioinfo[i >> 5][i & 0x1f];
        e->osfhnd = hv; e->osfile = *flags;
        if (!i02_init_cs(e->cs, 4000)) return -1;
        e->opencount++;
      }
    }
  }
  for (int i = 0; i < 3; i++) {
    I02Io* e = &i02_pioinfo[0][i];
    if (e->osfhnd == -1) {
      e->osfile = 0x81;
      uint32_t id = i == 0 ? 0xfffffff6u : 0xfffffff5u - (i != 1);
      int32_t hf = i02_get_std(id); uint32_t t;
      if (hf == -1 || (t = i02_file_type(hf)) == 0) e->osfile |= 0x40;
      else {
        e->osfhnd = hf;
        if ((t & 0xff) == 2) e->osfile |= 0x40; else if ((t & 0xff) == 3) e->osfile |= 8;
        if (!i02_init_cs(e->cs, 4000)) return -1;
        e->opencount++;
      }
    } else e->osfile |= 0x80;
  }
  i02_set_handle_count(i02_nhandle);
  return 0;
}

// 1.13c D2LAUNCH+0x4CBF  (__ioterm)
void i02_ioterm() {
  for (int s = 0; s < 32; s++) {
    I02Io* b = i02_pioinfo[s];
    if (!b) continue;
    for (int i = 0; i < 32; i++) if (b[i].opencount) i02_del_cs(b[i].cs);
    i02_free(b); i02_pioinfo[s] = nullptr;
  }
}

// 1.13c D2LAUNCH+0x4FE0  (___crtGetEnvironmentStringsA)
static int32_t i02_wide_mode = 0;                                // DAT_6fa650b0: 0 unknown,1 W,2 A
static const uint16_t* (*i02_getenv_w)() = nullptr;
static void (*i02_freeenv_w)(const uint16_t*) = nullptr;
static const char* (*i02_getenv_a)() = nullptr;
static void (*i02_freeenv_a)(const char*) = nullptr;
static uint32_t (*i02_last_error)() = nullptr;
static int (*i02_w2mb)(const uint16_t*, int, char*, int) = nullptr;  // WideCharToMultiByte(0,0,...)
void* i02_crt_get_env_a() {
  const uint16_t* w = nullptr;
  if (i02_wide_mode == 0) {
    w = i02_getenv_w();
    if (w) i02_wide_mode = 1;
    else if (i02_last_error() == 0x78) i02_wide_mode = 2;
  }
  if (i02_wide_mode != 1) {
    if (i02_wide_mode != 2 && i02_wide_mode != 0) return nullptr;
    const char* a = i02_getenv_a(); if (!a) return nullptr;
    const char* p = a; while (*p) p += strlen(p) + 1;
    size_t n = (size_t)(p + 1 - a);
    void* d = i02_malloc(n); if (d) memcpy(d, a, n);
    i02_freeenv_a(a); return d;
  }
  if (!w && !(w = i02_getenv_w())) return nullptr;
  const uint16_t* p = w; while (*p) { while (*p) p++; p++; }
  int wn = (int)(p - w) + 1;
  int sz = i02_w2mb(w, wn, nullptr, 0);
  char* out = nullptr;
  if (sz != 0 && (out = (char*)i02_malloc(sz)) && i02_w2mb(w, wn, out, sz) == 0) { i02_free(out); out = nullptr; }
  i02_freeenv_w(w);
  return out;
}

// 1.13c D2LAUNCH+0x522D  (__XcptFilter) -- depends on per-thread _tiddata + exception table (seam)
static int (*i02_xcpt_filter)(uint32_t, void*) = nullptr;
int i02_XcptFilter(uint32_t num, void* ptrs) { return i02_xcpt_filter ? i02_xcpt_filter(num, ptrs) : 0; }

// 1.13c D2LAUNCH+0x5A0A  (___wctomb_mt); loc: +4 codepage, +0x14 is-multibyte flag, +0x28 mb_cur_max
struct I02Loc { int32_t pad0, cp, pad1[3], mb, pad2[5], mbmax; };
static int* (*i02_errno)() = nullptr;                            // 0x3AFA
static int (*i02_w2mb_cp)(uint32_t cp, const uint16_t*, int, char*, int, int* usedDefault) = nullptr;
int i02_wctomb_mt(I02Loc* loc, char* dst, uint16_t wc) {
  if (!dst) return 0;
  if (loc->mb == 0) { if (wc < 0x100) { *dst = (char)wc; return 1; } }
  else {
    int used = 0;
    int r = i02_w2mb_cp((uint32_t)loc->cp, &wc, 1, dst, loc->mbmax, &used);
    if (r != 0 && !used) return r;
  }
  *i02_errno() = 0x2a;
  return -1;
}

// 1.13c D2LAUNCH+0x5AD0  (__aulldvrm): unsigned 64-bit divide; quotient returned, remainder in *rem.
uint64_t i02_aulldvrm(uint64_t a, uint64_t b, uint64_t* rem) { *rem = a % b; return a / b; }
