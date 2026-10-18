// src/core/d2client/d2client_batch_038a3.cpp -- twelve UNMAPPED 1.13c D2Client.dll rows reimplemented
// from the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Base 0x6FAB0000;
// every RVA below is retail's and every global is named at its retail RVA. These are MSVC 2003 Release
// CRT functions the linker pulled into the DLL. Callees that are not recovered yet (the scanf engine
// 0x61BC, __fclose_lk 0x3857, the CRT heap 0x7E9D/0x804F and the lock wrapper 0x350D) are host seams
// AFTER the retail arguments, which keep their order (the d2client_batch_1e70.cpp pattern).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

// ___ascii_stricmp (0x76F0) and ___ascii_strnicmp (0x7740), VERIFIED in crt_strings.cpp. The port's
// locale block is the zeroed one, so the locale branch in __stricmp/__strnicmp is never taken and the
// ASCII path is the only reachable one.
extern "C" int32_t d2c_stricmp(const uint8_t* a, const uint8_t* b);
extern "C" int32_t d2c_strnicmp(const uint8_t* a, const uint8_t* b, uint32_t n);
// 0x3F7C (__unlock(8) wrapper), d2client_batch_1e70.cpp.
extern "C" void d2_client_3f7c(void);

// The CRT's FILE fields these rows touch (retail: _ptr +0, _cnt +4, _base +8, _flag +0xC), in the
// tree's packed wasm64 form: every pointer field ahead of an offset widens it by sizeof(void*) - 4.
#pragma pack(push, 1)
struct D2cFile {
    const char* ptr;   // retail +0x00
    int32_t     cnt;   // retail +0x04
    const char* base;  // retail +0x08
    int32_t     flag;  // retail +0x0C
};
#pragma pack(pop)
static_assert(offsetof(D2cFile, ptr) == 0x00, "FILE._ptr");
static_assert(offsetof(D2cFile, cnt) == 0x04 + 1 * (sizeof(void*) - 4), "FILE._cnt");
static_assert(offsetof(D2cFile, base) == 0x08 + 1 * (sizeof(void*) - 4), "FILE._base");
static_assert(offsetof(D2cFile, flag) == 0x0C + 2 * (sizeof(void*) - 4), "FILE._flag");

// The ASCII C-locale isspace set the CRT ctype table at locale +0x48 encodes for byte < 0x100: 0x20
// and 0x09..0x0D.
static bool d2c_is_space(uint32_t c) {
    return c == 0x20 || (c >= 0x09 && c <= 0x0D);
}

// _token of the thread data block (PTD +0x18; _holdrand is at +0x14). The port is single-threaded, so
// this shared slot is the whole of the thread state _strtok keeps.
static char* g_d2c_token;

typedef int32_t (*D2cFcloseLk)(void* file);
// 1.13c D2Client+0x038A3 (70 B, caller 0xAC530, callee 0x3857 __fclose_lk): _fclose. When the FILE's
// _flag has bit 0x40 clear, take the file lock, run __fclose_lk, release the lock and return its
// result; when bit 0x40 is set, clear _flag and return -1 without touching the lock. The port has no
// lock objects (single-threaded), so __lock_file/__unlock_file are no-ops and __fclose_lk, still
// UNMAPPED, is the seam.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_38a3(void* file, D2cFcloseLk fclose_lk) {
    D2cFile* f = (D2cFile*)file;
    if ((f->flag & 0x40) == 0) {
        return fclose_lk(file);   // __lock_file(file) ... __unlock_file(file) around it
    }
    f->flag = 0;
    return -1;
}

typedef int32_t (*D2cScanfEngine)(void* file, const char* fmt, void* args);
// 1.13c D2Client+0x038F4 (52 B, caller 0x89830, callee 0x61BC): _sscanf builds a read-only FILE view of
// the source string -- _flag = 0x49, _base = _ptr = _Src, _cnt = strlen(_Src) -- and runs the shared
// scanf engine over it with the caller's argument slot. The engine (0x61BC, 3562 B) is still UNMAPPED,
// so it is the seam; the return is its field count.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_38f4(const char* src, const char* fmt, void* args,
                                            D2cScanfEngine engine) {
    D2cFile view;
    view.flag = 0x49;
    view.base = src;
    view.ptr = src;
    view.cnt = (int32_t)std::strlen(src);
    return engine(&view, fmt, args);
}

// 1.13c D2Client+0x0393C (136 B, caller 0xB7F6): _atol. Skip every leading byte the C-locale table
// marks as space (bit 8); then read one optional '-' or '+'; then accumulate digits with
// value = digit + value * 10 until a non-digit; a '-' makes the result negative. Ghidra's integer
// division/overflow wrapping is plain 32-bit arithmetic.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_393c(const char* s) {
    const uint8_t* p = (const uint8_t*)s;
    while (d2c_is_space(*p)) ++p;
    uint32_t first = *p;
    const uint8_t* digits = p + 1;
    if (first == 0x2D || first == 0x2B) {
        first = *digits;
        digits = p + 2;
    }
    int32_t value = 0;
    for (;;) {
        if (first < 0x30 || first > 0x39) break;
        value = (int32_t)first - 0x30 + value * 10;
        first = *digits++;
    }
    if (*p == 0x2D) value = -value;
    return value;
}

// 1.13c D2Client+0x039C9 (105 B, callers 0x10A60, 0x707F0, 0x70500): __stricmp. The CRT compares with
// the caller's thread locale; with lc_category[0].wlocale NULL it calls ___ascii_stricmp (0x76F0). The
// port's locale block is zeroed (the bn_updatetlocinfo convention), so that path is the only one.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_39c9(const char* a, const char* b) {
    return d2c_stricmp((const uint8_t*)a, (const uint8_t*)b);
}

// 1.13c D2Client+0x03A32 (127 B, caller 0x707F0): __strnicmp. A zero count returns 0 before the locale
// is considered; otherwise the same locale branch as __stricmp, ASCII path ___ascii_strnicmp (0x7740).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_3a32(const char* a, const char* b, size_t count) {
    if (count == 0) return 0;
    return d2c_strnicmp((const uint8_t*)a, (const uint8_t*)b, (uint32_t)count);
}

// 1.13c D2Client+0x03BE4 (191 B, callers 0x70700, 0x70500): unnamed CRT helper -- _strtok. Build the
// 32-byte (256-bit) delimiter set, NUL included. A null input resumes from the thread's _token. Skip
// leading delimiters (a NUL is not skipped); then scan to the next delimiter: on NUL store the pointer
// to it as the token and return NULL (the caller's start was the terminator); on a delimiter write a
// NUL over it, store the following byte as the token and return the start of the token.
extern "C"
EMSCRIPTEN_KEEPALIVE char* d2_client_3be4(char* s, const char* delim) {
    uint8_t set[32] = {};
    for (;;) {
        const uint8_t c = (uint8_t)*delim++;
        set[c >> 3] |= (uint8_t)(1u << (c & 7));
        if (c == 0) break;
    }
    if (s == nullptr) s = g_d2c_token;
    char* start = s;
    while (*start != '\0' &&
           (set[(uint8_t)*start >> 3] & (1u << ((uint8_t)*start & 7))) != 0) {
        ++start;
    }
    char* p = start;
    for (;;) {
        if (*p == 0) {
            g_d2c_token = p;
            return start != p ? start : nullptr;
        }
        if ((set[(uint8_t)*p >> 3] & (1u << ((uint8_t)*p & 7))) != 0) {
            *p = 0;
            g_d2c_token = p + 1;
            return start;
        }
        ++p;
    }
}

// 1.13c D2Client+0x03CA3 (201 B, caller 0x3D6C): ___toupper_mt(this, locale, c). The shipped fast path
// is taken when locale +0x14 (the LCMapString handle) is 0, or when locale +0x24 is set and c < 0x80;
// it folds 0x61..0x7A by -0x20 and returns every other byte unchanged. The else branch is LCMapStringA,
// which the port's zeroed locale (locale +0x14 == 0) never reaches.
// ponytail: ASCII fast path only; add the code-page path if a caller ever passes a non-zero LCMapString.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_3ca3(const void* locale, uint32_t c) {
    const bool fast = locale == nullptr || *(const int32_t*)((const uint8_t*)locale + 0x14) == 0 ||
                      (*(const int32_t*)((const uint8_t*)locale + 0x24) != 0 && c < 0x80);
    const int32_t ci = (int32_t)c;
    if (fast && ci > 0x60 && ci < 0x7B) return (uint32_t)(ci - 0x20);
    return c;
}

// 1.13c D2Client+0x03D6C (34 B, caller 0x6EFB0): _toupper(c) -- fetch the thread locale and fold
// through ___toupper_mt. No locale table is carried here, so the fast path decides.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_3d6c(int32_t c) {
    return (int32_t)d2_client_3ca3(nullptr, (uint32_t)c);
}

// 1.13c D2Client+0x03D8E (253 B, callers 0x69350, 0x69970): __strlwr. With lc_category[0].wlocale NULL
// the ASCII path folds each '@' < c < '[' ('A'..'Z') by +0x20 in place and returns the string; the
// LCMapString path needs code-page tables the port does not carry and is unreachable with the zeroed
// locale (the bn_updatetlocinfo convention).
extern "C"
EMSCRIPTEN_KEEPALIVE char* d2_client_3d8e(char* s) {
    for (char* p = s; *p != '\0'; ++p) {
        if (*p > '@' && *p < '[') *p = (char)(*p + ' ');
    }
    return s;
}

typedef size_t (*D2cMsize)(void* block);
typedef void* (*D2cRealloc)(void* block, size_t size);
// Onexit table state: +0x124AD4 DAT_6fbd4ad4 (base), +0x124AD0 DAT_6fbd4ad0 (tail).
static uintptr_t g_onexit_base;
static uintptr_t g_onexit_tail;

// 1.13c D2Client+0x03EA2 (128 B, caller 0x3F4A): __onexit_lk. The function pointer retail passes in
// EDI (our `func`) is appended to the onexit table. The block size is __msize(base); when it lacks
// room for one 4-byte slot (size < tail + 4 - base), grow it by min(size, 0x800) through _realloc,
// and on failure retry once with size + 0x10; a second failure returns 0 without storing. The tail is
// rebased to new_base + (tail - base); then store func at the tail, advance by 4 and return it.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_3ea2(uint32_t func, D2cMsize msize, D2cRealloc realloc_) {
    const uintptr_t base = g_onexit_base;
    const uintptr_t tail = g_onexit_tail;
    const size_t size = msize((void*)base);
    if (size < (size_t)(tail + 4 - base)) {
        const size_t grow = size < 0x800 ? size : 0x800;
        void* grown = realloc_((void*)base, grow + size);
        if (grown == nullptr) {
            grown = realloc_((void*)base, size + 0x10);
            if (grown == nullptr) return 0;
        }
        g_onexit_tail = (uintptr_t)grown + (tail - base);
        g_onexit_base = (uintptr_t)grown;
    }
    *(uint32_t*)g_onexit_tail = func;
    g_onexit_tail += 4;
    return func;
}

typedef void* (*D2cMalloc)(size_t size);
// 1.13c D2Client+0x03F22 (40 B, caller 0x3537): ___onexitinit. _malloc(0x80); NULL returns 0x18
// (ENOMEM) and leaves the globals alone; otherwise the first dword is zeroed (NULL terminator) and
// both globals point at the block. Returns 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_3f22(D2cMalloc malloc_) {
    void* table = malloc_(0x80);
    if (table == nullptr) return 0x18;
    g_onexit_base = (uintptr_t)table;
    *(uint32_t*)table = 0;
    g_onexit_tail = g_onexit_base;
    return 0;
}

typedef void (*D2cLock8)(void);
// 1.13c D2Client+0x03F4A (50 B, caller 0x3F82): __onexit(Func) -- __lock(8) through the 0x350D
// wrapper (still UNMAPPED, so a seam), register Func through __onexit_lk with the function pointer in
// EDI, __unlock(8) through 0x3F7C, and return __onexit_lk's result.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_3f4a(uint32_t func, D2cLock8 lock8, D2cMsize msize,
                                             D2cRealloc realloc_) {
    lock8();
    const uint32_t result = d2_client_3ea2(func, msize, realloc_);
    d2_client_3f7c();
    return result;
}
