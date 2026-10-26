// BNClient.dll alpha-lane batch 1 (rows s3636-s3645): the module's BnetLog writers (both are thin wrappers
// over Storm.dll's SLog* exports) and the MSVC 2003 Release CRT leaves it statically links. Every rule below
// is transcribed from the cached Ghidra decompile and cross-checked against the retail image
// retail/D2/D2SE/CORES/1.13c/Bnclient.dll (base 0x6FF20000). Host services retail imports (Storm, the
// Win32 thread/CRT seams) are arguments or bound function pointers, never invented behaviour.
#include <emscripten.h>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// --- BnetLog: 0x17420 and 0x17480 -----------------------------------------------------------------
// Both take the module's critical section DAT_6FF3F898 and call the Storm logging exports through the
// thunks at 0x800C/0x8012/0x8024/0x802A: SLogClose #541, SLogCreate #542, SLogWrite #548,
// SLogCriticalLog #550 (reference/D2MOO/source/Storm/definitions/Storm.1.10f.def).
struct BnStormLogEnv {
    void (*slog_close)(uint32_t handle);                             // Storm #541
    int  (*slog_create)(const char* name, int use_log_dir, uint32_t* out);  // Storm #542
    void (*slog_write)(uint32_t handle, const char* text);           // Storm #548
    void (*slog_critical)(uint32_t handle, uint32_t arg);            // Storm #550
    void (*enter_cs)(void* cs);                                      // KERNEL32 EnterCriticalSection
    void (*leave_cs)(void* cs);                                      // KERNEL32 LeaveCriticalSection
};

static BnStormLogEnv g_bn_log;
static uint32_t g_bn_log_handle;   // DAT_6FF3F8B0, the HLOG SLogCreate fills
static void*    g_bn_log_cs;       // &DAT_6FF3F898, the module critical section

extern "C"
EMSCRIPTEN_KEEPALIVE void bn_bnet_log_bind(const BnStormLogEnv* env, uint32_t* handle, void* cs) {
    g_bn_log = *env;
    g_bn_log_handle = *handle;
    g_bn_log_cs = cs;
}

// 1.13c BNClient+0x17420 (94 B): BnetLog printf. Take DAT_6FF3F898, format the varargs into a 0x200-byte
// stack buffer with __vsnprintf(buf, 0x200, fmt, ap), force the byte after the buffer to zero, forward the
// text to SLogWrite(DAT_6FF3F8B0, buf), release the section. Void.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_bnet_log_printf(const char* fmt, ...) {
    char buf[0x200];
    va_list ap;
    va_start(ap, fmt);
    g_bn_log.enter_cs(g_bn_log_cs);
    std::vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    buf[sizeof buf - 1] = '\0';                  // retail's `local_1 = 0`, the byte at buffer end
    g_bn_log.slog_write(g_bn_log_handle, buf);
    g_bn_log.leave_cs(g_bn_log_cs);
}

// 1.13c BNClient+0x17480 (141 B): BnetLog control. Under DAT_6FF3F898:
//   action 1: SLogClose(handle); handle = 0; SLogCreate("BnetLog.txt", 0, &handle); return 1.
//   action 2: SLogCriticalLog(handle, arg); return 0 (action 0, and any other action, also returns 0).
// The return value is the local initialised to 0 and set to 1 only on the create path.
extern "C"
EMSCRIPTEN_KEEPALIVE int bn_bnet_log_control(int action, uint32_t arg) {
    int result = 0;
    g_bn_log.enter_cs(g_bn_log_cs);
    if (action == 1) {
        g_bn_log.slog_close(g_bn_log_handle);
        g_bn_log_handle = 0;
        g_bn_log.slog_create("BnetLog.txt", 0, &g_bn_log_handle);
        result = 1;
    } else if (action == 2) {
        g_bn_log.slog_critical(g_bn_log_handle, arg);
    }
    g_bn_log.leave_cs(g_bn_log_cs);
    return result;
}

// --- CRT leaves -----------------------------------------------------------------------------------

// 1.13c BNClient+0x0159A (_sprintf, 88 B): VS2003 unbounded formatted print. The FILE is fixed at
// {_cnt = 0x7FFFFFFF, _flag = 0x42, _base = _ptr = dest}, the format engine writes and counts, then the
// caller-side terminator runs: _cnt -= 1; if negative __flsbuf(0, &file) else *ptr = 0. Destination NULL
// skips the terminator. Returns the engine's count (chars, NUL excluded). The host formatter stands in for
// FUN_6FF22854 and keeps the bounded/unbounded contract: the count is what would be written.
extern "C"
EMSCRIPTEN_KEEPALIVE int bn_sprintf(char* dest, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsnprintf(dest, (size_t)-1, fmt, ap);
    va_end(ap);
    return n;
}

// 1.13c BNClient+0x01C76 (___tolower_mt, 200 B): lowercase one character through the thread locale. The
// shipped fast path (no multibyte code page, or c < 0x80) folds only 0x41..0x5A by +0x20 and returns every
// other byte unchanged; the LCMapString path below it needs the CRT locale tables our port does not carry.
// ponytail: ASCII-only, the locale fast path; add the code-page tables if a caller ever passes >= 0x80.
extern "C"
EMSCRIPTEN_KEEPALIVE int bn_tolower_mt(const void* locale, int c) {
    (void)locale;
    return (c > 0x40 && c < 0x5B) ? c + 0x20 : c;
}

// 1.13c BNClient+0x03255 (__getptd, 113 B): the current thread's _ptiddata, created on first use. Retail
// saves GetLastError, reads the TLS slot DAT_6FF3D1B0, and on a miss allocates 0x8C bytes (calloc(1,0x8C)),
// stores it back, then seeds _initaddr +0x54 = the retail VA 0x6FF3D4D8 (RVA 0x1D4D8), _holdrand +0x14 = 1,
// _thandle +0x04 = -1 and _tid +0x00 = GetCurrentThreadId(); a failed allocation/store runs
// __amsg_exit(0x10). The saved error is restored before returning.
// Our engine is single-threaded, so the TLS slot is this one static block and _tid is the engine's only
// thread (1). _initaddr holds the module-relative RVA, the port's convention for retail VAs.
static uint8_t g_bn_ptd[0x8C];
static bool    g_bn_ptd_ready;

extern "C"
EMSCRIPTEN_KEEPALIVE void* bn_getptd(void) {
    if (!g_bn_ptd_ready) {
        std::memset(g_bn_ptd, 0, sizeof g_bn_ptd);
        *(uint32_t*)(g_bn_ptd + 0x54) = 0x1D4D8;        // _initaddr, retail 0x6FF3D4D8
        *(uint32_t*)(g_bn_ptd + 0x14) = 1;              // _holdrand
        *(uint32_t*)(g_bn_ptd + 0x04) = 0xFFFFFFFFu;    // _thandle
        *(uint32_t*)(g_bn_ptd + 0x00) = 1;              // _tid
        g_bn_ptd_ready = true;
    }
    return g_bn_ptd;
}

// 1.13c BNClient+0x037D6 (_get_doserrno, 9 B): __getptd() then &ptd->_tdoserrno. _tdoserrno is the
// VS2003 _ptiddata long at +0x0C (the same offset the module's __lseek/__write paths write).
extern "C"
EMSCRIPTEN_KEEPALIVE int* bn_get_doserrno(void) {
    return (int*)((uint8_t*)bn_getptd() + 0x0C);
}

static uint8_t bn_ascii_lower(uint8_t c) { return (uint8_t)(c - 'A') < 26 ? (uint8_t)(c + 0x20) : c; }

// 1.13c BNClient+0x035A0 (___ascii_strnicmp, 97 B): compare up to n bytes. A NUL in the first string ends
// the comparison with -1 unless the second is also NUL (0); a NUL in the second returns 1. Otherwise fold
// 0x41..0x5A on both sides and return -1/1 on the first difference. n equal bytes return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int bn_ascii_strnicmp(const char* a, const char* b, size_t n) {
    for (; n != 0; --n, ++a, ++b) {
        const uint8_t x = (uint8_t)*a, y = (uint8_t)*b;
        if (x == 0) return y != 0 ? -1 : 0;
        if (y == 0) return 1;
        const uint8_t fx = bn_ascii_lower(x), fy = bn_ascii_lower(y);
        if (fx != fy) return fx < fy ? -1 : 1;
    }
    return 0;
}

// 1.13c BNClient+0x03792 (___updatetlocinfo, 50 B): __lock(0xC); refresh the calling thread's locale info
// through ___updatetlocinfo_lk; run the helper at 0x37C4 (unlock/cleanup); return the locale pointer.
// Retail keeps the process locale tables in the CRT; our port classifies with the ASCII rules above, so the
// locale object is a stable zeroed block whose address is the identity the callers compare.
static uint8_t g_bn_locale[0x50];

extern "C"
EMSCRIPTEN_KEEPALIVE void* bn_updatetlocinfo(void) {
    return g_bn_locale;
}

// 1.13c BNClient+0x014A6 (_doexit, 181 B): the module's CRT shutdown. __lock(8); if _C_Exit_Done
// (0x1E364) is already 1, TerminateProcess(GetCurrentProcess(), code) (never returns);
// _C_Termination_Done (0x1E360) = 1; _exitflag (0x1E35C) = (byte)retcaller; when quick == 0 walk the
// __onexit list backwards from tail (0x20D14) to head (0x20D18) calling each non-NULL entry, then
// __initterm(0x1D03C, 0x1D044); always __initterm(0x1D048, 0x1D050); if retcaller != 0 return;
// _C_Exit_Done = 1; ___crtExitProcess(code).
// The shipped image's init/terminator tables each hold one retail callback -- RVA 0x5FE4 in
// [0x1D03C,0x1D044) and RVA 0x1835D in [0x1D048,0x1D050) (read from the retail .data) -- so the walks are
// exposed as one env call rather than silently dropped. Our engine registers no __onexit entries.
// ponytail: this is a shim (as retail does): flags and the retcaller branch only;
// TerminateProcess / ___crtExitProcess have no wasm analogue and arriving here is an engine bug.
struct BnDoexitEnv {
    void (*initterm_entry)(uint32_t rva);   // one retail C-init/terminator callback
    void (*exit_process)(uint32_t code);    // ___crtExitProcess, terminal
};

static uint32_t g_bn_C_Termination_Done;   // +0x1E360
static uint32_t g_bn_C_Exit_Done;          // +0x1E364
static uint8_t  g_bn_exitflag;             // +0x1E35C

extern "C"
EMSCRIPTEN_KEEPALIVE void bn_doexit(uint32_t code, int quick, int retcaller, const BnDoexitEnv* env) {
    if (g_bn_C_Exit_Done == 1) { env->exit_process(code); return; }   // retail never returns here
    g_bn_C_Termination_Done = 1;
    g_bn_exitflag = (uint8_t)retcaller;
    if (quick == 0) env->initterm_entry(0x5FE4);      // [0x1D03C, 0x1D044), one entry
    env->initterm_entry(0x1835D);                     // [0x1D048, 0x1D050), one entry
    if (retcaller != 0) return;
    g_bn_C_Exit_Done = 1;
    env->exit_process(code);
}

static int bn_isspace_tbl(uint8_t c) { return c == 0x20 || (c >= 0x09 && c <= 0x0D); }
static int bn_isdigit_tbl(uint8_t c) { return c >= '0' && c <= '9'; }
static int bn_isalpha_tbl(uint8_t c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }

// 1.13c BNClient+0x016D9 (_strtoxl, 447 B): the signed/unsigned integer parser behind the DLL's _strtol
// (0x1898) and _strtoul (0x18AF). Signature from the decompile:
//   strtoxl(const char* s, const char** endptr, uint32_t base, uint32_t flags)
// steps: skip _isspace bytes; '-' sets flags|=2 (negative) and '+' is accepted, byte after the sign becomes
// the first digit; invalid base ((int)base < 0, base == 1, base > 36) sets *endptr = s and returns 0; base 0
// auto-detects ("0x"/"0X" -> 16, leading "0" -> 8, else 10); a 0x/0X prefix is consumed for base 16; digits
// are the _ctype-digit bytes or letters folded by toupper(c)-0x37, rejected when >= base; the value
// accumulates with a /%-base overflow guard (flag bit 4 = overflow, bit 8 = any digit consumed); at the end
// the cursor backs up one byte, a value with no digits returns 0 with *endptr = s, overflow sets errno =
// ERANGE (0x22) and clamps to INT_MAX/INT_MIN (0x7FFFFFFF / 0xFFFFFFFF then negated), the sign flag makes
// the result negative. Flags bit 1 = unsigned (no ERANGE for the sign), bit 2 = negative, bit 4 = range
// overflow, bit 8 = digit seen.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t bn_strtoxl(const char* s, const char** endptr, uint32_t base,
                                         uint32_t flags) {
    const char* p = s;
    uint8_t c = (uint8_t)*p;
    while (bn_isspace_tbl(c)) c = (uint8_t)*++p;

    const char* next = p + 1;
    if (c == '-') {
        flags |= 2;
        c = (uint8_t)*next;
        next = p + 2;
    } else if (c == '+') {
        c = (uint8_t)*next;
        next = p + 2;
    }

    if ((int32_t)base < 0 || base == 1 || base > 36) {
        if (endptr != nullptr) *endptr = s;
        return 0;
    }
    if (base == 0) {
        if (c != '0') {
            base = 10;
        } else if (*next != 'x' && *next != 'X') {
            base = 8;
        } else {
            base = 16;
        }
    }
    if (base == 16 && c == '0' && (*next == 'x' || *next == 'X')) {
        c = (uint8_t)next[1];
        next += 2;
    }

    const uint32_t limit = 0xFFFFFFFFu / base;
    uint32_t value = 0;
    for (;;) {
        uint32_t digit;
        if (bn_isdigit_tbl(c)) {
            digit = (uint32_t)c - '0';
        } else if (bn_isalpha_tbl(c)) {
            digit = (uint32_t)((c >= 'a' && c <= 'z' ? c - 0x20 : (int)c) - 0x37);   // toupper(c) - 0x37
        } else {
            break;
        }
        if (base <= digit) break;
        if (value < limit || (value == limit && digit <= 0xFFFFFFFFu % base)) {
            value = value * base + digit;
            flags |= 8;
        } else {
            flags |= 0xC;
        }
        c = (uint8_t)*next++;
    }

    --next;                                     // retail backs the cursor off the rejected byte
    if ((flags & 8) == 0) {
        next = s;                               // no digits: endptr is the original string
        value = 0;
    } else if ((flags & 4) != 0 ||
               ((flags & 1) == 0 &&
                (((flags & 2) != 0 && value > 0x80000000u) ||
                 ((flags & 2) == 0 && value > 0x7FFFFFFFu)))) {
        // errno = ERANGE; the host errno is not observable through this interface, but the clamp is.
        if ((flags & 1) == 0) value = ((flags & 2) != 0 ? 1u : 0u) + 0x7FFFFFFFu;
        else value = 0xFFFFFFFFu;
    }
    if (endptr != nullptr) *endptr = next;
    return (flags & 2) != 0 ? 0u - value : value;
}

// 1.13c BNClient+0x0138C (__vsnprintf, 86 B): VS2003 bounded formatted print. FILE = {_cnt = count,
// _flag = 0x42, _base = _ptr = dest}; the format engine writes and returns its count; then if dest != NULL,
// _cnt -= 1, and either __flsbuf(0, &file) (count was 0, nothing left for the terminator) or *ptr = 0.
// The host vsnprintf stands in for FUN_6FF22854 and terminates within count.
extern "C"
EMSCRIPTEN_KEEPALIVE int bn_vsnprintf(char* dest, size_t count, const char* fmt, va_list ap) {
    return std::vsnprintf(dest, count, fmt, ap);
}
