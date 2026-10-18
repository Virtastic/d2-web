// src/core/d2game/d2game_batch_28ee.cpp -- twelve 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order: the MSVC 2003 CRT routines
// the linker pulled into the DLL -- the __unlock(8) thunk, __cinit, __exit/__cexit, _memmove, __strrev,
// __fsopen and its two unlock fragments, __fread_lk/_fread and __fwrite_lk. Stage A batch lane
// (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// Host model: retail's 32-bit module globals are the statics below (retail address noted); the CRT FILE
// is the host stdio stream, because retail's _iobuf only exists to drive __getstream/__openfile/_read/
// __write, none of which have a wasm64 analogue -- glibc carries the same observable buffer counters,
// EOF/error flags and fd position and locks internally. Register-only arguments and injected SEH frame
// operands (Ghidra's unaff_EBP) are explicit parameters here and are named at each function. The seams
// whose implementations live outside this batch are function pointers on D2GameBatch18Env in retail
// argument order; a probe passes it in r[5]. The functions themselves take the retail arguments in
// r[0..3] and return in r[4], per the retail capture tool/fn/README.md.
//
// Prior art: src/core/d2client/d2client_batch_77a1.cpp (0x031A0 _memmove, 0x0885D _unlock(n) thunk) and
// src/core/d2game/d2game_batch_6820.cpp (the host-stdio collapse of _fwrite/0x321F and fopen/0x2FD0).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <emscripten.h>

// ---- module state --------------------------------------------------------------------------------
// __cinit's initializer tables (0x6FD1F000..0x6FD1F01C, 7 entries, then 0x6FD1F020..0x6FD1F034,
// 5 entries). In the host build no retail init function is linked, so both tables stay empty and only
// the table walk itself -- the rule -- is exercised.
static uint32_t g_initTable_6FD1F000[7];
static uint32_t g_initTable_6FD1F020[5];
static constexpr uint32_t kCinitExitThunk = 0x240A0;   // LAB_6fc240a0, the thunk __cinit registers

// Seams for the callees this batch does not implement (address = retail RVA).
struct D2GameBatch18Env {
    void    (*fpmath)(int32_t);                       // 0xF5704 __fpmath, called through 0x6FD2FA08
    int32_t (*invokeInitializer)(uint32_t);           // call a non-NULL initializer-table address
    int32_t (*atexit)(uint32_t);                      // 0x34A6 _atexit
    void    (*doexit)(uint32_t code, int32_t skipOnexit, int32_t keepProcess); // 0x2979 doexit
};
static const D2GameBatch18Env kNoEnv = {};

static const D2GameBatch18Env& d2game_env(const uint32_t* r) {
    return r[5] ? *(const D2GameBatch18Env*)(uintptr_t)r[5] : kNoEnv;
}

// ---- CRT leaves ----------------------------------------------------------------------------------
// 0x3F76 __unlock(index): LeaveCriticalSection((LPCRITICAL_SECTION)(&DAT_6fd1f210)[index * 2]).
// The compat critical section (src/core/d2moo/compat/Windows.h) is a no-op, so the lock is the index.
static void d2game_crt_unlock(uint32_t index) { (void)index; }

// 0x4CAB __unlock_file(stream): leave the stream's critical section (the lock-table arm for a _iob entry,
// stream+1 otherwise). Host stdio owns its lock for the whole call and never exposes it, so nothing is
// left to do; both SEH fragments below call this one function.
static void d2game_file_unlock(FILE* stream) { (void)stream; }

// _memmove rule: an overlap-safe byte copy. Same-pointer and size 0 return the destination untouched;
// a destination below the source copies forward, otherwise backwards. Result is the destination.
static void* d2game_memmove(void* dst, const void* src, size_t size) {
    uint8_t* d = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;
    if (d == s || size == 0) return dst;
    if ((uintptr_t)d < (uintptr_t)s) {
        for (size_t i = 0; i < size; ++i) d[i] = s[i];
    } else {
        for (size_t i = size; i != 0; --i) d[i - 1] = s[i - 1];
    }
    return dst;
}

// _strrev rule: the decompiled scan first computes the length (the -1/-2 counter is retail's loop
// unrolling), then swaps the bytes in place, stopping before the middle. The NUL stays where it was.
static char* d2game_strrev(char* str) {
    char* end = str;
    while (*end != '\0') ++end;
    if (end == str) return str;                         // empty string: the iVar2 == -2 path
    for (char* lo = str, *hi = end - 1; lo < hi; ++lo, --hi) {
        const char c = *lo;
        *lo = *hi;
        *hi = c;
    }
    return str;
}

// __fsopen rule: __getstream() then __openfile(name, mode, share, stream), with errno = 0x18 when the
// stream table is exhausted. Host stdio has neither a stream table nor a share mode: fopen fills the
// host stream and reports the open failure by returning NULL, so that is the whole observable result
// (same collapse that 0x2FD0 -> D2Game_CrtFopen already makes).
static FILE* d2game_fsopen(const char* filename, const char* mode, int32_t shareFlag) {
    (void)shareFlag;
    return std::fopen(filename, mode);
}

// __fread_lk rule: fill the destination in stream-sized pieces, from the buffer while _cnt > 0 and
// otherwise through __read/_filbuf, setting _IOEOF/_IOERR at end/error. Return is elements read, and 0
// when size*count is 0 (retail zeroes the count in that branch). The host stream keeps the same counters
// and flags, so std::fread is that whole rule.
static size_t d2game_fread_lk(void* dst, size_t size, size_t count, FILE* stream) {
    return std::fread(dst, size, count, stream);
}

// _fread rule: __lock_file(stream); count = __fread_lk(...); __unlock_file(stream); return count.
static size_t d2game_fread(void* dst, size_t size, size_t count, FILE* stream) {
    return d2game_fread_lk(dst, size, count, stream);
}

// __fwrite_lk rule: write through the buffer while it has room (_cnt), otherwise __flsbuf for one byte
// or __write for the bulk; a short write sets _IOERR. Return is elements written, 0 when size*count is 0.
// Host stdio buffers identically and returns the same element count.
static size_t d2game_fwrite_lk(const void* src, size_t size, size_t count, FILE* stream) {
    return std::fwrite(src, size, count, stream);
}

// __cinit rule: run __fpmath when the module pointer at 0x6FD2FA08 is set, then walk the first
// initializer table, stopping at the first nonzero status; when that table finished cleanly, register
// the CRT exit thunk and run the second table, whose statuses are ignored. A zero-status table walk is
// the success result 0.
static int32_t d2game_cinit(int32_t param, const D2GameBatch18Env& env) {
    if (env.fpmath != nullptr) env.fpmath(param);
    int32_t result = 0;
    for (size_t i = 0; i < 5; ++i) {
        if (result != 0) return result;
        const uint32_t entry = g_initTable_6FD1F020[i];
        if (entry != 0 && env.invokeInitializer != nullptr) result = env.invokeInitializer(entry);
    }
    if (result == 0) {
        if (env.atexit != nullptr) env.atexit(kCinitExitThunk);
        for (size_t i = 0; i < 7; ++i) {
            const uint32_t entry = g_initTable_6FD1F000[i];
            if (entry != 0 && env.invokeInitializer != nullptr) env.invokeInitializer(entry);
        }
        result = 0;
    }
    return result;
}

extern "C" {
// 1.13c D2Game+0x28EE
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_28ee(uint32_t* r) {
    d2game_crt_unlock(8);                               // sub_28EE: __unlock(8), caller 0x34A0
    r[4] = 0;
}

// 1.13c D2Game+0x290F
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_290f(uint32_t* r) {
    r[4] = (uint32_t)d2game_cinit((int32_t)r[0], d2game_env(r));
}

// 1.13c D2Game+0x2A4D
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_2a4d(uint32_t* r) {
    const D2GameBatch18Env& env = d2game_env(r);
    if (env.doexit != nullptr) env.doexit(r[0], 1, 0);  // __exit(_Code) == doexit(_Code, 1, 0)
    r[4] = 0;
}

// 1.13c D2Game+0x2A5E
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_2a5e(uint32_t* r) {
    const D2GameBatch18Env& env = d2game_env(r);
    if (env.doexit != nullptr) env.doexit(0, 0, 1);     // __cexit() == doexit(0, 0, 1)
    r[4] = 0;
}

// 1.13c D2Game+0x2AD0
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_2ad0(uint32_t* r) {
    r[4] = (uint32_t)(uintptr_t)d2game_memmove((void*)(uintptr_t)r[0], (const void*)(uintptr_t)r[1],
                                               (size_t)r[2]);
}

// 1.13c D2Game+0x2E10
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_2e10(uint32_t* r) {
    r[4] = (uint32_t)(uintptr_t)d2game_strrev((char*)(uintptr_t)r[0]);
}

// 1.13c D2Game+0x2F74
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_2f74(uint32_t* r) {
    r[4] = (uint32_t)(uintptr_t)d2game_fsopen((const char*)(uintptr_t)r[0],
                                              (const char*)(uintptr_t)r[1], (int32_t)r[2]);
}

// 1.13c D2Game+0x2FC6
// SEH fragment in __fsopen: __unlock_file(*(FILE**)(EBP - 0x1C)). The caller's frame operand is not an
// argument, so a probe passes the stream in r[5].
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_2fc6(uint32_t* r) {
    d2game_file_unlock((FILE*)(uintptr_t)r[5]);
    r[4] = 0;
}

// 1.13c D2Game+0x2FE3
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_2fe3(uint32_t* r) {
    r[4] = (uint32_t)d2game_fread_lk((void*)(uintptr_t)r[0], (size_t)r[1], (size_t)r[2],
                                     (FILE*)(uintptr_t)r[3]);
}

// 1.13c D2Game+0x30CC
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_30cc(uint32_t* r) {
    r[4] = (uint32_t)d2game_fread((void*)(uintptr_t)r[0], (size_t)r[1], (size_t)r[2],
                                  (FILE*)(uintptr_t)r[3]);
}

// 1.13c D2Game+0x310E
// SEH fragment in _fread: __unlock_file(*(FILE**)(EBP + 0x14)), i.e. the fourth argument. A probe passes
// the stream in r[5].
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_310e(uint32_t* r) {
    d2game_file_unlock((FILE*)(uintptr_t)r[5]);
    r[4] = 0;
}

// 1.13c D2Game+0x3118
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3118(uint32_t* r) {
    r[4] = (uint32_t)d2game_fwrite_lk((const void*)(uintptr_t)r[0], (size_t)r[1], (size_t)r[2],
                                      (FILE*)(uintptr_t)r[3]);
}
}
