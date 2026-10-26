// src/core/d2launch/d2launch_batch_z35.cpp -- UNMAPPED 1.13c D2Launch.dll alpha-lane rows (batch z35).
// VS2003 CRT. Retail 32-bit layouts; globals and callees are seams (uint32_t links).
#include <cstdint>
#include <cstddef>
#include <cstring>

// Seams
static void* (*z35_getptd)() = nullptr;                                   // 0x4714
static void* (*z35_updatetlocinfo)() = nullptr;                           // 0x7A13
static int (*z35_isctype_mt)(void*, void*, int, int) = nullptr;           // 0x830B
static int (*z35_mbtowc_mt)(void*, uint16_t*, const uint8_t*, size_t) = nullptr;  // 0x5DD3
static void (*z35_report_failure)() = nullptr;                            // 0x5A91
static void (*z35_InitializeCriticalSection)(uint32_t) = nullptr;
static int32_t* (*z35_errno_ptr)() = nullptr;                             // 0x3AFA
static uint32_t* (*z35_doserrno_ptr)() = nullptr;                         // 0x3B03
static int (*z35_vfprintf_core)(void* file, const uint8_t* fmt, void* args) = nullptr;  // 0x1D45
static int (*z35_flsbuf)(int, void* file) = nullptr;
static int (*z35_input_core)(void* file, const uint8_t* fmt, void* args) = nullptr;     // 0x2517
static size_t (*z35_strlen)(const char*) = nullptr;                       // 0x3350

static uint32_t z35_security_cookie = 0;                                  // DAT_6fa606d0
static void* z35_ptloci_cur = nullptr;                                    // PTR_DAT_6fa6075c
static uint32_t z35_nhandle = 0;                                          // DAT_6fa66f84
static uint8_t* z35_pioinfo[64] = {};                                     // DAT_6fa66fa0 (entries 0x24 bytes)
static uint8_t z35_QueryInterface_tbl[1] = {};                            // PTR_LAB_6fa64dc0 (table at that VA)

// Guessed locinfo layout: ptd->_tfpecode = locinfo*; Ghidra lc_category[1].refcount = mb_cur_max,
// lc_category[3].refcount = ctype table pointer (uint16_t[]).
struct Z35Loc { uint8_t pad0[0x1C]; int32_t mb_cur_max; uint8_t pad1[0x0C]; const uint16_t* pctype; };
struct Z35Ptd { uint8_t pad[0x4C]; Z35Loc* locinfo; };                    // guessed offset (as z31)
struct Z35File { char* ptr; int32_t cnt; char* base; int32_t flag; int32_t file; int32_t charbuf; int32_t bufsiz; char* tmpfname; };

static Z35Loc* z35_curloc() {
    Z35Ptd* p = (Z35Ptd*)z35_getptd();
    Z35Loc* l = p->locinfo;
    if ((void*)l != z35_ptloci_cur) l = (Z35Loc*)z35_updatetlocinfo();
    return l;
}
static int z35_ctype(int c, int mask) {
    Z35Loc* l = z35_curloc();
    if (l->mb_cur_max > 1) return z35_isctype_mt(nullptr, l, c, mask);
    return l->pctype[c] & mask;
}

// 1.13c D2Launch+0x5AC2  (__security_check_cookie, ECX = cookie)
void z35_security_check_cookie(uintptr_t cookie) {
    if (cookie == z35_security_cookie) return;
    z35_report_failure();
}

// 1.13c D2Launch+0x5B65
int z35_isdigit(int c) { return z35_ctype(c, 4); }

// 1.13c D2Launch+0x5B9F
int z35_isxdigit(int c) { return z35_ctype(c, 0x80); }

// 1.13c D2Launch+0x5BDE
int z35_isspace(int c) { return z35_ctype(c, 8); }

// 1.13c D2Launch+0x5C90  (memset, word-fill)
void* z35_memset(void* dst, int val, size_t n) {
    if (n == 0) return dst;
    uint8_t* p = (uint8_t*)dst;
    uint32_t v = (uint32_t)val & 0xff;
    if (n > 3) {
        uint32_t a = (uint32_t)(-(int32_t)(uintptr_t)dst) & 3;
        size_t rest = n;
        if (a) { rest -= a; while (a--) *p++ = (uint8_t)val; }
        v *= 0x1010101;
        n = rest & 3;
        for (uint32_t w = (uint32_t)(rest >> 2); w; w--) { memcpy(p, &v, 4); p += 4; }
        if (rest >> 2 && n == 0) return dst;
    }
    do { *p++ = (uint8_t)v; } while (--n);
    return dst;
}

// 1.13c D2Launch+0x5E93
int z35_mbtowc(uint16_t* dst, const char* src, size_t n) {
    return z35_mbtowc_mt(z35_curloc(), dst, (const uint8_t*)src, n);
}

// 1.13c D2Launch+0x5EF4  (___crtInitCritSecNoSpinCount@8)
uint32_t z35_crtInitCritSecNoSpinCount(uint32_t cs) {
    z35_InitializeCriticalSection(cs);
    return 1;
}

// 1.13c D2Launch+0x6A24  (__get_osfhandle)
intptr_t z35_get_osfhandle(int fh) {
    if ((uint32_t)fh < z35_nhandle) {
        uint8_t* e = z35_pioinfo[fh >> 5] + (fh & 0x1f) * 0x24;
        if (e[4] & 1) return (intptr_t)*(int32_t*)e;
    }
    *z35_errno_ptr() = 9;
    *z35_doserrno_ptr() = 0;
    return -1;
}

// 1.13c D2Launch+0x9B60  (QueryInterface, ordinal 1) -- returns address of static table
void* z35_QueryInterface() { return z35_QueryInterface_tbl; }

// 1.13c D2Launch+0x1000  (register-arg: ESI = wchar buffer, EDI = count; byte-swap each u16)
void z35_swap16_array(uint16_t* buf, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) buf[i] = (uint16_t)((buf[i] << 8) | (buf[i] >> 8));
}

// 1.13c D2Launch+0x101B  (sprintf; va_list passed as pointer to variadic args)
int z35_sprintf(char* dest, const char* fmt, void* va) {
    Z35File f = {};
    f.cnt = 0x7fffffff; f.flag = 0x42; f.base = dest; f.ptr = dest;
    int r = z35_vfprintf_core(&f, (const uint8_t*)fmt, va);
    if (dest) {
        if (--f.cnt < 0) z35_flsbuf(0, &f); else *f.ptr = 0;
    }
    return r;
}

// 1.13c D2Launch+0x1073  (sscanf)
int z35_sscanf(const char* src, const char* fmt, void* va) {
    Z35File f = {};
    f.flag = 0x49; f.base = (char*)src; f.ptr = (char*)src;
    f.cnt = (int32_t)z35_strlen(src);
    return z35_input_core(&f, (const uint8_t*)fmt, va);
}
