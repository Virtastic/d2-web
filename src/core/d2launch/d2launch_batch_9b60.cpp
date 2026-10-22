// src/core/d2launch/d2launch_batch_9b60.cpp -- UNMAPPED 1.13c D2Launch.dll alpha-lane rows (Stage A:
// implementation + marker only, no spec, no vector), transcribed from
// build-wasm/ghidra-d2launch/decomp/0x<RVA>.c (image base 0x6FA40000).
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <emscripten.h>

// 1.13c D2Launch+0x09B60 -- QueryInterface, ordinal 1, 6 bytes, caller 0x1FAE8. The retail body is
// `mov eax, 0x6FA64DC0; ret`: it hands the caller the address of the module interface table in .data.
// The table's first dword is the module init entry point (prior art: D2MOO Game/src/Main.cpp:350-351,
// `gpCurrentModuleInterface = pQueryInterface(); (*(ModuleInitPointer*)gpCurrentModuleInterface)(pCfg)`).
// The entries are unmapped module functions, so the table keeps its retail shape: pointer slots.
static void* g_launchModuleInterface[1];

extern "C"
void** d2launch_query_interface(void) {
    return g_launchModuleInterface;
}

// 1.13c D2Launch+0x01000 -- 27 bytes, caller 0x18E10. Register-argument leaf (retail uses ESI = the
// 16-bit word buffer and EDI = the word count; modelled with the retail pair in that order). It walks
// the buffer in place and stores each word byte-swapped: the big-endian (0xFFFE BOM) Unicode file
// path of 0x18E10 converting to native UTF-16LE.
extern "C"
void d2launch_sub_1000(uint16_t* buf, uint32_t count) {
    for (uint32_t i = 0; i < count; ++i) {
        const uint16_t w = buf[i];
        buf[i] = (uint16_t)((w >> 8) | (w << 8));
    }
}

// 1.13c D2Launch+0x0101B -- _sprintf, 88 bytes, 16 internal callers. VS2003 unbounded formatted
// print: a FILE is fixed at {_cnt = 0x7FFFFFFF, _flag = 0x42, _base = _ptr = dest}, the engine at
// 0x1D45 writes it, then the terminator runs (*ptr = 0 when _cnt stays positive, __flsbuf(0) when
// the count ran out). Destination NULL skips the terminator. Returns the engine's char count, NUL
// excluded. The host formatter stands in for 0x1D45; the count is what would be written.
extern "C"
EMSCRIPTEN_KEEPALIVE int d2launch_sprintf(char* dest, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsnprintf(dest, (size_t)-1, fmt, ap);
    va_end(ap);
    return n;
}

// 1.13c D2Launch+0x01073 -- _sscanf, 52 bytes, callers 0x12770/0x128D0/0x18900. Builds a read-only
// FILE over the source string {_flag = 0x49, _base = _ptr = src, _cnt = strlen(src)} and runs the
// engine at 0x2517. Returns the number of fields converted. The host formatter stands in for 0x2517.
extern "C"
EMSCRIPTEN_KEEPALIVE int d2launch_sscanf(const char* src, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsscanf(src, fmt, ap);
    va_end(ap);
    return n;
}

// 1.13c D2Launch+0x010B0 -- shortsort, 106 bytes, caller 0x1120 (_qsort, three-argument call shape
// plus hi in EAX: the retail register carries the last element). Straight selection sort: find the
// largest element in [lo, hi] by comp, byte-swap it with hi over `width` bytes, then hi -= width.
// Element size 0 never swaps (the width==0 guard), as in retail.
typedef int (*D2LaunchCompare)(const void*, const void*);

extern "C"
void d2launch_shortsort(uint8_t* lo, uint8_t* hi, uint32_t width, D2LaunchCompare comp) {
    while (lo < hi) {
        uint8_t* maxp = lo;
        for (uint8_t* p = lo + width; p <= hi; p += width) {
            if (comp(p, maxp) > 0) maxp = p;
        }
        if (maxp != hi && width != 0) {
            for (uint32_t i = 0; i < width; ++i) {
                const uint8_t t = hi[i];
                hi[i] = maxp[i];
                maxp[i] = t;
            }
        }
        hi -= width;
    }
}

// 1.13c D2Launch+0x01120 -- _qsort, 643 bytes, caller 0x15B60. The MSVC 2003 CRT median-of-three
// quicksort with the size<=8 insertion cutoff whose shortsort fallback is 0x10B0 (see above). The
// port already carries that algorithm element for element as compat/msvc_qsort.cpp, whose equal-key
// order the shipped table sorts were measured against; this row delegates to it rather than keeping
// a second copy.
extern "C" void d2web_msvc_qsort(void* base, size_t num, size_t width, D2LaunchCompare cmp);

extern "C"
EMSCRIPTEN_KEEPALIVE void d2launch_qsort(void* base, size_t num, size_t width, D2LaunchCompare cmp) {
    d2web_msvc_qsort(base, num, width, cmp);
}

// 1.13c D2Launch+0x013B0 -- _strncpy, 292 bytes, callers 0x053AC/0x081C0/0x0C720. VS2003 strncpy:
// copy source bytes up to the first NUL or count, then pad the rest of the count with NUL; when the
// source is at least count long no terminator is stored. count 0 returns dest untouched. The retail
// body is the usual aligned dword copy; a byte loop produces the same bytes and return value.
extern "C"
char* d2launch_strncpy(char* dest, const char* src, size_t count) {
    char* const ret = dest;
    while (count != 0 && *src != '\0') {
        *dest++ = *src++;
        --count;
    }
    while (count-- != 0) *dest++ = '\0';
    return ret;
}

// 1.13c D2Launch+0x014D4 -- ___crtExitProcess, 47 bytes, caller 0x01598 (__exit). Rule: hModule =
// GetModuleHandleA("mscoree.dll"); if non-null, p = GetProcAddress(hModule, "CorExitProcess") and, if
// that resolves, (*p)(code); then ExitProcess(code). No path returns in retail; the three kernel32
// imports are host seams, in retail argument order (the indirect CorExitProcess call takes the code).
// Cross-check: the byte-identical D2Client copy at 0x34DD (src/core/d2client/crt_chsize_bb34.cpp).
struct D2LaunchExitEnv {
    void* (*getModuleHandleA)(const char* name);
    void* (*getProcAddress)(void* module, const char* name);
    void (*exitProcess)(int32_t code);
};

extern "C"
void d2launch_crt_exit_process(int32_t code, D2LaunchExitEnv env) {
    void* const mod = env.getModuleHandleA("mscoree.dll");
    if (mod != nullptr) {
        auto corExit = reinterpret_cast<void (*)(int32_t)>(env.getProcAddress(mod, "CorExitProcess"));
        if (corExit != nullptr) corExit(code);
    }
    env.exitProcess(code);
}

// 1.13c D2Launch+0x01504 -- 9 bytes, caller 0x03607: the __lock(8) thunk (push 8, call __lock at
// 0x0352E, ret). __lock itself is not part of this row; it is the host seam, called in retail order.
extern "C"
void d2launch_sub_1504(void (*lock)(int)) { lock(8); }

// 1.13c D2Launch+0x0150D -- 9 bytes, caller 0x03639: the __unlock(8) thunk (push 8, call 0x03479,
// which is LeaveCriticalSection on the CRT lock table, ret). The lock release is the host seam.
extern "C"
void d2launch_sub_150d(void (*unlock)(int)) { unlock(8); }

// 1.13c D2Launch+0x01516 -- __initterm, 24 bytes, caller 0x01598 (doexit). Walks the callback table
// [begin, end) (retail carries begin in EAX and end on the stack) and calls every non-null entry with
// no arguments; the entries are not cleared afterwards.
extern "C"
void d2launch_initterm(void (**begin)(void), void (**end)(void)) {
    for (void (**p)(void) = begin; p < end; ++p) {
        if (*p != nullptr) (*p)();
    }
}

// 1.13c D2Launch+0x0152E -- __cinit, 106 bytes, caller 0x01906 (__CRT_INIT). Runs the dynamic-init
// hook at 0x6FA680DC (when set) with param, then walks the first initializer table
// [0x6FA60018, 0x6FA60028) -- 4 slots -- returning the first nonzero entry status. On a clean table it
// registers the exit thunk at 0x6FA43695 (RVA 0x3695) through _atexit and walks the second table
// [0x6FA60000, 0x6FA60014) -- 5 slots -- whose statuses are ignored, returning 0. Host build: no
// retail initializer is linked, so both tables stay empty and only the walk runs; the hook, entry
// invocation (by retail RVA) and _atexit are seams.
static uint32_t g_launchInitTable_6FA60018[4];
static uint32_t g_launchInitTable_6FA60000[5];

struct D2LaunchCinitEnv {
    void (*dynamicInit)(int32_t param);            // called through 0x6FA680DC
    int32_t (*invokeInitializer)(uint32_t rva);    // call a non-null initializer-table entry
    int32_t (*atexit)(uint32_t rva);               // 0x0363F _atexit
};

extern "C"
int32_t d2launch_cinit(int32_t param, D2LaunchCinitEnv env) {
    if (env.dynamicInit != nullptr) env.dynamicInit(param);
    int32_t result = 0;
    for (size_t i = 0; i < 4; ++i) {
        if (result != 0) return result;
        const uint32_t entry = g_launchInitTable_6FA60018[i];
        if (entry != 0 && env.invokeInitializer != nullptr) result = env.invokeInitializer(entry);
    }
    if (result == 0) {
        if (env.atexit != nullptr) env.atexit(0x3695);
        for (size_t i = 0; i < 5; ++i) {
            const uint32_t entry = g_launchInitTable_6FA60000[i];
            if (entry != 0 && env.invokeInitializer != nullptr) env.invokeInitializer(entry);
        }
        result = 0;
    }
    return result;
}

// ---- CRT glue (b47 batch). Retail FILE/_iobuf, the stream table and the per-stream locks do not exist
// in wasm64; the wrappers keep the retail call shape over the host libc stream, which has the same
// observable contract (open/read/close, EOF and error flags, NULL/-1 on failure).
#include <cstdlib>
#include <ctime>

// 1.13c D2Launch+0x0166C __exit(code): doexit(code, quick=1, retain=0). doexit (0x1598) is STALLED
// (CRT-private atexit tables), so the quick-exit path is the host terminate.
extern "C" void d2launch_crt_exit(unsigned code) { _Exit((int)code); }

// 1.13c D2Launch+0x0168C __fsopen: take a free stream slot (EMFILE 0x18 when none), open, unlock.
extern "C" FILE* d2launch_crt_fsopen(const char* name, const char* mode, int /*shflag*/) {
    return fopen(name, mode);
}

// 1.13c D2Launch+0x016DE / 0x01826 / 0x018FE SEH-tail thunks: __unlock_file(stream); single-threaded here.
extern "C" void d2launch_crt_unlock_tail(FILE*) {}

// 1.13c D2Launch+0x016E8 fopen(name, mode) = __fsopen(name, mode, _SH_DENYNO 0x40).
extern "C" FILE* d2launch_crt_fopen(const char* name, const char* mode) {
    return d2launch_crt_fsopen(name, mode, 0x40);
}

// 1.13c D2Launch+0x016FB __fread_lk and 0x017E4 _fread: count*size bytes; return whole elements read
// (0 when the product is 0); retail sets _IOEOF 0x10 / _IOERR 0x20, which libc keeps in the stream.
extern "C" size_t d2launch_crt_fread_lk(void* dst, size_t size, size_t count, FILE* f) {
    if (size * count == 0) return 0;
    return fread(dst, size, count, f);
}
extern "C" size_t d2launch_crt_fread(void* dst, size_t size, size_t count, FILE* f) {
    return d2launch_crt_fread_lk(dst, size, count, f);
}

// 1.13c D2Launch+0x01869 __fclose_lk and 0x018B5 _fclose: flush, free buffer, close handle, drop temp
// name; -1 on failure, 0 on success.
extern "C" int d2launch_crt_fclose_lk(FILE* f) { return fclose(f) == 0 ? 0 : -1; }
extern "C" int d2launch_crt_fclose(FILE* f) { return f ? d2launch_crt_fclose_lk(f) : -1; }

// 1.13c D2Launch+0x01830 _time32(out): FILETIME - 1970 epoch offset, /10^7; stored when out != NULL.
extern "C" int32_t d2launch_crt_time32(int32_t* out) {
    const int32_t t = (int32_t)time(nullptr);
    if (out) *out = t;
    return t;
}
