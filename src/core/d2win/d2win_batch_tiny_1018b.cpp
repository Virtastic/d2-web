// src/core/d2win/d2win_batch_tiny_1018b.cpp -- five 1.13c D2Win.dll rows transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2win/decomp/0x<RVA>.c) and checked against the retail bytes
// (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base 0x6F8E0000). Alpha batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector.
//
// Rows: sub_1000 0x01000 (31 B), __initterm 0x010B9 (24 B), _exit 0x011FE (17 B), __exit 0x0120F
// (17 B), __cexit 0x01220 (15 B). All are MSVC 2003 (VC7.1) Release CRT leaves.
//
// 0x01000 is the out-of-line loop the compiler emitted for `Unicode` array construction (0x0CFC0
// calls it once per row of 0x100 elements): count in EAX, the advancing value in ECX, the callback in
// EBX and the stride as the one stack argument (`ret 4`). Each call passes the running value in ECX.
//
// 0x010B9 is __initterm: begin in EAX, end on the stack; call every non-NULL dword of [begin, end) in
// ascending order. The 1.13c tables hold retail code addresses, so the entry is a seam, the same
// wiring as src/core/realm/bnclient_batch_2558.cpp.
//
// 0x011FE / 0x0120F / 0x01220 are the three _doexit wrappers. Their 0x0113B target (181 B) is still a
// row of its own, so it is modelled here only to the point the wrappers need: the 0x6F901024 /
// 0x6F901020 / 0x6F90101C globals, the two C tables emptied to the shipped slots, no onexit list and
// process exit for the terminating paths. The tables were read from retail .data:
// [0x6F8FF02C, 0x6F8FF034) = {0, 0x6F8E3DEA} and [0x6F8FF038, 0x6F8FF03C) = {0}; the onexit head and
// tail at 0x6F9ABC50 / 0x6F9ABC4C are unbacked .data, both zero. The lock/unlock pair (0x0244B /
// 0x02396) is a no-op in this single-threaded port.
//
// Prior art: src/core/realm/mcp_exit.cpp (doexit flag semantics) and bnclient_batch_2558.cpp
// (__initterm seam).
#include <cstdint>
#include <unistd.h>
#include <emscripten.h>

// Host seam for the retail addresses an initterm table can hold; NULL selects the defaults.
struct D2WinBatchTinyEnv {
    void (*initterm_entry)(uint32_t rva);   // one retail initialiser / terminator address
};

static void d2win_noop_entry(uint32_t) {}
static const D2WinBatchTinyEnv g_d2win_batch_tiny_default = { d2win_noop_entry };

// 1.13c D2Win+0x01000 (31 B, caller 0x0CFC0): repeat a callback over an advancing value. count in EAX,
// value in ECX, callback in EBX, stride as the single stack argument. `dec eax; js` skips a count <= 0;
// otherwise the callback runs count times, each time with ECX = value + i * stride.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_repeat_call(uint32_t count, uint32_t value, uint32_t stride,
                                            void (*callback)(uint32_t)) {
    if ((int32_t)count <= 0) return;
    for (uint32_t i = 0; i < count; ++i) {
        callback(value);
        value += stride;
    }
}

// 1.13c D2Win+0x010B9 (24 B, caller 0x0113B doexit): __initterm(begin, end). begin arrives in EAX and
// end is the stack argument; walk [begin, end) in ascending dword order and call every non-NULL entry
// with no arguments. Each entry is a retail initialiser address, hence the seam.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_initterm(const uint32_t* begin, const uint32_t* end,
                                         const D2WinBatchTinyEnv* env) {
    if (env == nullptr) env = &g_d2win_batch_tiny_default;
    for (const uint32_t* p = begin; p < end; ++p)
        if (*p != 0) env->initterm_entry(*p);
}

// doexit (0x0113B) support, not a row of this batch. Retail: __lock(8); when _C_Exit_Done is already
// 1, TerminateProcess(code); _C_Termination_Done = 1; _exitflag = (byte)retcaller; when quick == 0 run
// the onexit list backwards (empty here) and __initterm the C table, then always __initterm the
// terminator table; if retcaller != 0, __unlock(8) and return; otherwise _C_Exit_Done = 1 and
// ___crtExitProcess(code), which never returns.
static uint32_t g_d2win_C_Exit_Done;           // 0x6F901024
static uint32_t g_d2win_C_Termination_Done;    // 0x6F901020
static uint8_t  g_d2win_exitflag;              // 0x6F90101C; also read by 0x6F8E3DEA
static const uint32_t kD2WinCInitTable[2] = { 0, 0x3DEA };   // 0x6F8FF02C, 0x6F8FF030
static const uint32_t kD2WinCTermTable[1] = { 0 };           // 0x6F8FF038

static void d2win_doexit(uint32_t code, int32_t quick, int32_t retcaller,
                         const D2WinBatchTinyEnv* env) {
    if (env == nullptr) env = &g_d2win_batch_tiny_default;
    if (g_d2win_C_Exit_Done == 1) _exit((int)code);   // TerminateProcess(GetCurrentProcess(), code)
    g_d2win_C_Termination_Done = 1;
    g_d2win_exitflag = (uint8_t)retcaller;
    if (quick == 0) {
        // Onexit list empty (head 0x6F9ABC50 == 0): the backwards walk calls nothing.
        d2win_initterm(kD2WinCInitTable, kD2WinCInitTable + 2, env);
    }
    d2win_initterm(kD2WinCTermTable, kD2WinCTermTable + 1, env);
    if (retcaller != 0) return;                       // 0x011EA: __unlock(8), a no-op here
    g_d2win_C_Exit_Done = 1;
    _exit((int)code);                                 // ___crtExitProcess(0x01077), never returns
}

// 1.13c D2Win+0x011FE (17 B, many callers): _exit(code) = doexit(code, quick=0, retcaller=0): run the
// exit lists, then terminate the process with code.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_exit(uint32_t code) {
    d2win_doexit(code, 0, 0, nullptr);
}

// 1.13c D2Win+0x0120F (17 B, callers 0x01962, 0x06CF4): __exit(code) = doexit(code, quick=1,
// retcaller=0): skip the onexit walk, run the terminator table, then terminate with code.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_exit_quick(uint32_t code) {
    d2win_doexit(code, 1, 0, nullptr);
}

// 1.13c D2Win+0x01220 (15 B, caller 0x016FD): __cexit() = doexit(0, quick=0, retcaller=1): run the
// exit lists and return instead of terminating.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_cexit(void) {
    d2win_doexit(0, 0, 1, nullptr);
}
