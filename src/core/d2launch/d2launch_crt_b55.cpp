// src/core/d2launch/d2launch_crt_b55.cpp -- MSVC CRT leaves of 1.13c D2Launch.dll (batch b55-lau-a).
// State and callees are reached through seams (struct / function pointers), no Win32 calls.
#include <cstdint>
#include <cstddef>

struct d2launch_b55_Env {
    // ctype locale view (ptd->ptlocinfo)
    int32_t  mbCurMax;            // locinfo->mb_cur_max
    const uint16_t* ctype1;       // locinfo->pctype table
    int (*isctypeMt)(int c, int mask);   // ___isctype_mt
    int (*wctombMt)(char* mb, uint16_t wc);                    // ___wctomb_mt
    int (*mbtowcMt)(uint16_t* dst, const uint8_t* src, size_t n); // ___mbtowc_mt
    int (*nhMalloc)(size_t size, int32_t newmode, void** out);  // __nh_malloc
    int32_t newMode;              // DAT_6fa65124
    int32_t osVersionKind;        // DAT_6fa64de4
    uint32_t winMajor;            // DAT_6fa64df0
    uint32_t nhandle;             // DAT_6fa66f84
    uint8_t** osfileTable;        // DAT_6fa66fa0 (blocks of 32 entries, 0x24 bytes each)
    int32_t* errnoPtr;            // FUN_6fa43afa()
    uint32_t* doserrnoPtr;        // FUN_6fa43b03()
    uintptr_t securityCookie;     // DAT_6fa606d0
    void (*reportFailure)();      // 0x5A91
    void (*initCritSec)(void* cs);// InitializeCriticalSection
};
static d2launch_b55_Env d2launch_b55_env;
extern "C" d2launch_b55_Env* d2launch_b55_getEnv() { return &d2launch_b55_env; }

// 1.13c D2Launch+0x05EC0 -- __allmul
extern "C" int64_t d2launch_allmul(uint32_t aLo, int32_t aHi, uint32_t bLo, int32_t bHi) {
    uint64_t p = (uint64_t)aLo * bLo;
    if (aHi == 0 && bHi == 0) return (int64_t)p;
    uint32_t hi = (uint32_t)(p >> 32) + (uint32_t)aHi * bLo + aLo * (uint32_t)bHi;
    return (int64_t)(((uint64_t)hi << 32) | (uint32_t)p);
}

// 1.13c D2Launch+0x05C90 -- _memset (byte-wise result)
extern "C" void* d2launch_memset(void* dst, int val, size_t n) {
    uint8_t* p = (uint8_t*)dst;
    while (n--) *p++ = (uint8_t)val;
    return dst;
}

// 1.13c D2Launch+0x05102 -- ___heap_select
extern "C" int32_t d2launch_heap_select() {
    d2launch_b55_Env& e = d2launch_b55_env;
    return (e.osVersionKind == 2 && 4 < (int32_t)e.winMajor) ? 1 : 3;
}

// 1.13c D2Launch+0x059F8 -- _malloc
extern "C" void* d2launch_malloc(size_t size) {
    d2launch_b55_Env& e = d2launch_b55_env;
    void* r = nullptr;
    e.nhMalloc(size, e.newMode, &r);
    return r;
}

// 1.13c D2Launch+0x06A24 -- __get_osfhandle
extern "C" intptr_t d2launch_get_osfhandle(int fh) {
    d2launch_b55_Env& e = d2launch_b55_env;
    if ((uint32_t)fh < e.nhandle) {
        uint8_t* ent = e.osfileTable[fh >> 5] + (fh & 0x1f) * 0x24;
        if (ent[4] & 1) return *(intptr_t*)ent;
    }
    *e.errnoPtr = 9;       // EBADF
    *e.doserrnoPtr = 0;
    return -1;
}

// 1.13c D2Launch+0x05AC2 -- __security_check_cookie
extern "C" void d2launch_security_check_cookie(uintptr_t cookie) {
    d2launch_b55_Env& e = d2launch_b55_env;
    if (cookie == e.securityCookie) return;
    e.reportFailure();
}

// 1.13c D2Launch+0x05EF4 -- ___crtInitCritSecNoSpinCount@8
extern "C" int32_t d2launch_crtInitCritSecNoSpinCount(void* cs) {
    d2launch_b55_env.initCritSec(cs);
    return 1;
}

static int d2launch_b55_ctype(int c, int mask) {
    d2launch_b55_Env& e = d2launch_b55_env;
    if (e.mbCurMax > 1) return e.isctypeMt(c, mask);
    return e.ctype1[c] & mask;
}

// 1.13c D2Launch+0x05B65 -- _isdigit
extern "C" int d2launch_isdigit(int c) { return d2launch_b55_ctype(c, 4); }
// 1.13c D2Launch+0x05B9F -- _isxdigit
extern "C" int d2launch_isxdigit(int c) { return d2launch_b55_ctype(c, 0x80); }
// 1.13c D2Launch+0x05BDE -- _isspace
extern "C" int d2launch_isspace(int c) { return d2launch_b55_ctype(c, 8); }

// 1.13c D2Launch+0x05A6A -- _wctomb
extern "C" int d2launch_wctomb(char* mb, uint16_t wc) { return d2launch_b55_env.wctombMt(mb, wc); }

// 1.13c D2Launch+0x05E93 -- _mbtowc
extern "C" int d2launch_mbtowc(uint16_t* dst, const char* src, size_t n) {
    return d2launch_b55_env.mbtowcMt(dst, (const uint8_t*)src, n);
}
