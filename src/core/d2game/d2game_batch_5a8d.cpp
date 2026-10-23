// src/core/d2game/d2game_batch_5a8d.cpp -- twelve UNMAPPED 1.13c D2Game.dll CRT rows reimplemented from
// the cached decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order: __unlock(10),
// __getptd()->_terrno, _free, _malloc, __NMSG_WRITE, __FF_MSGBANNER, _strlen,
// __security_check_cookie, __local_unwind2, _wctomb, __aulldvrm and __fptrap. Alpha batch lane
// (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// Host model follows src/core/d2game/d2game_batch_3e70.cpp: retail's module globals are the statics
// below at their retail RVA; the MSVC small-block heap and the per-thread CRT data block collapse onto
// the host's allocator and errno. Callees recovered elsewhere (0x3F76 __unlock, 0x69DC __NMSG_WRITE)
// are called through their extern "C" definitions. The 19-entry CRT message table is retail data and
// stays out of git: __NMSG_WRITE and __FF_MSGBANNER take it as an argument ({code, text} pairs, exactly
// the shape at 0x6FD1F7E8) from the caller, as src/core/d2client/d2client_batch_04860.cpp does.
// Not modelled, because the wasm host has no Win32 dialog or PE module path: __NMSG_WRITE's
// GetModuleFileNameA / truncated-name / MessageBoxA branch writes the same text to stderr instead, and
// __FF_MSGBANNER's _pfnmsg hook is a settable pointer. __local_unwind2's 0x78C6 (undecoded) is the
// abnormal-termination callee and arrives as a parameter.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <emscripten.h>
#include "crt_security_cookie_9bb0.h"

// Defined in src/core/d2game/d2game_batch_3e70.cpp.
extern "C" void d2_fn_d2game_3f76(int32_t lockNum);

// ---- CRT module state (retail RVA in the comment) ------------------------------------------------
static uint32_t g_stackCookie;              // 0x6FD1F980 __security_cookie
static int32_t   g_errorMode;                // 0x6FD2FD74 __error_mode
static int32_t   g_appType;                  // 0x6FD2FD78 __app_type
static void    (*g_pfnmsg)(void);            // 0x6FD2FFFC _pfnmsg

// 1.13c D2Game+0x04022 (sub_4022): release critical-section number 10.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_4022(void) {
    d2_fn_d2game_3f76(10);
}

// 1.13c D2Game+0x04E65 (sub_4E65, __getptd()->_terrno): the thread's errno slot. The host models the
// CRT thread data on the C library's own thread-local errno.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t* d2game_errno_ptr(void) {
    return &errno;
}

// 1.13c D2Game+0x05A8D (_free): the small-block heap (0x6FD3A210 == 3) is libc's allocator here; both
// its find_block path and the HeapFree fallback collapse to the host free. A null block returns.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2game_free_5a8d(void* mem) {
    if (mem != nullptr) std::free(mem);
}

// 1.13c D2Game+0x05F78 (_malloc): __nh_malloc(size, _newmode). The host allocator is the heap; a
// failure returns null (retail then calls the new handler, which the engine never installs).
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2game_malloc(size_t size) {
    return std::malloc(size);
}

// 1.13c D2Game+0x069DC (__NMSG_WRITE): find the code in the 19-entry {code, text} table and return
// when absent. With _error_mode 1, or _error_mode 0 and _app_type 1, write the text to stderr; else,
// for any code but 0xFC, show "Runtime Error!\n\nProgram: " + module name + "\n\n" + text. The module
// name and the modal box are Win32-only, so that branch writes its text to stderr (see the header).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2game_nmsg_write(int32_t code, const uint32_t* table) {
    uint32_t i = 0;
    while (i < 0x13 && code != (int32_t)table[i * 2]) ++i;
    if (i == 0x13) return;
    const char* text = (const char*)(uintptr_t)table[i * 2 + 1];
    if (g_errorMode == 1 || (g_errorMode == 0 && g_appType == 1)) {
        (void)!write(2, text, std::strlen(text));
    } else if (code != 0xFC) {
        static const char kPrefix[] = "Runtime Error!\n\nProgram: ";
        (void)!write(2, kPrefix, sizeof kPrefix - 1);
        (void)!write(2, text, std::strlen(text));
        (void)!write(2, "\n\n", 2);
    }
}

// 1.13c D2Game+0x06B54 __FF_MSGBANNER: when the error mode selects the banner, write message 0xFC,
// call the app hook when installed, then write message 0xFF.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2game_ff_msgbanner_6b54(const uint32_t* table) {
    if (g_errorMode == 1 || (g_errorMode == 0 && g_appType == 1)) {
        d2game_nmsg_write(0xFC, table);
        if (g_pfnmsg != nullptr) g_pfnmsg();
        d2game_nmsg_write(0xFF, table);
    }
}

// 1.13c D2Game+0x07460 (_strlen): the decompile's word-at-a-time scan is the C string length.
extern "C"
EMSCRIPTEN_KEEPALIVE size_t d2_fn_d2game_7460(const char* s) {
    return std::strlen(s);
}

// 1.13c D2Game+0x0751c (__security_check_cookie): fastcall, the expected cookie in ECX. A mismatch
// goes report_failure (0x74EB): 0x9C16(1) then ExitProcess(3), whose observable is the exit code.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_751c(uintptr_t cookie) {
    if (cookie != g_stackCookie) _exit(3);
}

// Prior art used: D2Client's VC7.1 cookie initializer; local 1.13c D2Game
// decompile confirms identical XOR rule and the shared +0xFF980 owner used above.
// 1.13c D2Game+0x09BB0: initialize the actual module cookie read by 0x0751C.
// The native OS boundary supplies FILETIME and performance-counter halves in call order.
extern "C" void d2_game_security_init_cookie_9bb0(const D2GameCookieEnv* env) {
    constexpr uint32_t initial = 0xBB40E64Eu;
    if (g_stackCookie != 0 && g_stackCookie != initial) return;
    const auto time = env->system_time_filetime();
    const uint32_t process = env->process_id();
    const uint32_t thread = env->thread_id();
    const uint32_t tick = env->tick_count();
    const auto counter = env->performance_counter();
    const uint32_t value = time.low ^ time.high ^ process ^ thread ^ tick ^
                           counter.low ^ counter.high;
    g_stackCookie = value == 0 ? initial : value;
}

// 1.13c D2Game+0x07832 (__local_unwind2): walk frame+8's table from index frame+0xC until -1 or the
// target. Each 12-byte record is {next index, filter, handler}: advance frame+0xC to the next index,
// and when the filter is zero run the abnormal-termination callee (0x78C6, passed in) then the handler.
// The SEH chain head is saved and restored around the walk.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_7832(uint8_t* frame, int32_t target, void (*abnormalTermination)(void)) {
    static uint32_t g_exceptionList;          // FS:[0] equivalent, single-threaded here
    uint32_t saved = g_exceptionList;
    g_exceptionList = (uint32_t)(uintptr_t)frame;
    for (;;) {
        uint32_t table = *(uint32_t*)(frame + 8);
        uint32_t idx = *(uint32_t*)(frame + 0xc);
        if (idx == 0xFFFFFFFFu || (int32_t)idx == target) break;
        uint8_t* rec = (uint8_t*)(uintptr_t)(table + idx * 12);
        *(uint32_t*)(frame + 0xc) = *(uint32_t*)rec;
        if (*(uint32_t*)(rec + 4) == 0) {
            abnormalTermination();
            ((void (*)(void))(uintptr_t)*(uint32_t*)(rec + 8))();
        }
    }
    g_exceptionList = saved;
}

// 1.13c D2Game+0x08284 (_wctomb): __getptd, refresh the thread locale if stale, then ___wctomb_mt
// (0x8224). With no per-thread locale and no code page (D2 never selects a DBCS locale), that is the
// C-locale path: a null output answers the stateful-encoding query with 0; a value below 0x100 stores
// its low byte and returns 1; anything wider fails with -1 (retail also sets errno = EILSEQ).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_8284(char* mbch, uint16_t wch) {
    if (mbch == nullptr) return 0;
    if (wch < 0x100) { *mbch = (char)wch; return 1; }
    return -1;
}

// 1.13c D2Game+0x082B0 (__aulldvrm): unsigned 128/64 -> 64-bit quotient. EDX:EAX is the dividend
// (hi=param_2, lo=param_1), the divisor is param_4:param_3, and (as with 0x3E70) both decompile limbs
// are the restored divide whose result is the plain 64-bit quotient.
extern "C"
EMSCRIPTEN_KEEPALIVE uint64_t d2_fn_d2game_82b0(uint32_t lo, uint32_t hi, uint32_t dlo, uint32_t dhi) {
    return ((uint64_t)hi << 32 | lo) / ((uint64_t)dhi << 32 | dlo);
}

// 1.13c D2Game+0x09D61 (__fptrap): __amsg_exit(2), i.e. print the "floating point not loaded" runtime
// message and terminate with exit code 0xFF. The message is the only side effect and is not modelled.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9d61(void) {
    _exit(0xFF);
}

// Scenario setup for the retail CRT state the rows above branch on.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2game_b39_crt_pin(int32_t errorMode, int32_t appType, void (*pfnmsg)(void)) {
    g_errorMode = errorMode;
    g_appType = appType;
    g_pfnmsg = pfnmsg;
}

// Scenario state for 0x751C: the stack cookie the check compares against.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2game_b39_cookie_pin(uintptr_t cookie) {
    g_stackCookie = cookie;
}
