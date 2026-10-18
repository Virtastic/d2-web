// src/core/d2game/d2game_batch_3261.cpp -- twelve 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order: the MSVC 2003 CRT routines
// the linker pulled into the DLL -- the two __unlock_file SEH fragments in _fwrite/_fclose, __fclose_lk,
// __ArrayUnwind, `eh vector destructor iterator' and its unwind fragment, __onexit_lk/___onexitinit/
// __onexit plus the __unlock(8) thunk and _atexit, and `eh vector constructor iterator'. Stage A batch
// lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// Host model: the CRT FILE is the host stdio stream, because retail's _iobuf only exists to drive
// __flush/__freebuf/__close/_free, none of which have a wasm64 analogue -- glibc carries the same
// observable buffer counters, EOF/error flags and fd position and locks internally (same collapse as
// d2game_batch_6820.cpp _fwrite/0x321F and d2game_batch_28ee.cpp __fread/__fwrite). The __onexit table
// globals are the statics below (retail 0x6FD3B350/0x6FD3B354); __msize's block size is tracked as a
// capacity in bytes because the host allocator's usable size is not portable. Register-only arguments and
// injected SEH frame operands (Ghidra's unaff_EBP/unaff_EDI) are explicit parameters here and are named
// at each function. The destructor/constructor callback of the array iterators is a seam on
// D2GameBatch19Env, not a raw retail address, because the probe cannot call an image address directly.
// A probe passes the env pointer in r[5]; the functions take retail arguments in r[0..3], return in r[4],
// per the retail capture tool/fn/README.md.
//
// Prior art: src/core/d2game/d2game_batch_28ee.cpp (the CRT leaf/host-stdio batch immediately before
// these RVAs), d2game_batch_2a6d.cpp (__SEH_prolog/__SEH_epilog and the CRT lock table) and
// src/core/d2client/d2client_batch_77a1.cpp (the _unlock(n) thunk).
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <emscripten.h>

// ---- module state --------------------------------------------------------------------------------
// __onexit's growable table (retail 0x6FD3B354 = block base, 0x6FD3B350 = next free slot). The capacity
// stands in for __msize(base) and is always the size the last malloc/realloc asked for.
static uint32_t* g_onexitTable    = nullptr;
static uint32_t* g_onexitNext     = nullptr;
static size_t    g_onexitCapacity = 0;   // bytes

// Seams for the callbacks the array iterators invoke. In retail these are __thiscall function pointers
// passed as the last argument(s); a probe cannot call an image address, so they live on the env.
struct D2GameBatch19Env {
    void (*callOne)(void* element);   // 0x3308 param_4 / 0x3366 param_4 / 0x34B8 param_4
    void (*unwindOne)(void* element); // 0x34B8 param_5 (cleanup destructor, EH path only)
};
static const D2GameBatch19Env kNoEnv = {};

static const D2GameBatch19Env& d2game_env(const uint32_t* r) {
    return r[5] ? *(const D2GameBatch19Env*)(uintptr_t)r[5] : kNoEnv;
}

// ---- CRT leaves ----------------------------------------------------------------------------------
// 0x4CAB __unlock_file(stream): leave the stream's critical section (the lock-table arm for a _iob entry,
// stream+1 otherwise). Host stdio owns its lock for the whole call and never exposes it, so nothing is
// left to do; both SEH fragments below call this one function.
static void d2game_file_unlock(FILE* stream) { (void)stream; }

// 0x28E5 __lock(8) / 0x3F76 __unlock(8): Enter/LeaveCriticalSection on the CRT lock table. The compat
// critical section (src/core/d2moo/compat/Windows.h) is a no-op, so the lock is the index.
static void d2game_crt_lock(uint32_t index)   { (void)index; }
static void d2game_crt_unlock(uint32_t index) { (void)index; }

// __ArrayUnwind rule: walk `count` elements from `ptr`, calling the callback with each and advancing by
// `size` bytes; the decompiled EDI is that running element pointer.
static void d2game_array_unwind(void* ptr, uint32_t size, int32_t count, void (*callOne)(void*)) {
    uint8_t* element = (uint8_t*)ptr;
    while (--count >= 0) {
        if (callOne != nullptr) callOne(element);
        element += size;
    }
}

// __onexit_lk rule: the table holds 4-byte function addresses; when the next slot would pass the block
// size, grow by min(size, 0x800) bytes (the retail both-directions growth), and if that realloc fails
// retry with 0x10 more bytes; only a failing retry gives up and returns NULL. The registered address is
// the result.
static uint32_t d2game_onexit_lk(uint32_t func) {
    const size_t usedBytes = (size_t)(g_onexitNext - g_onexitTable) * sizeof(uint32_t);
    if (g_onexitCapacity < usedBytes + sizeof(uint32_t)) {
        const size_t grow = g_onexitCapacity < 0x800 ? g_onexitCapacity : 0x800;
        uint32_t* grown = (uint32_t*)std::realloc(g_onexitTable, g_onexitCapacity + grow);
        if (grown == nullptr) {
            grown = (uint32_t*)std::realloc(g_onexitTable, g_onexitCapacity + 0x10);
            if (grown == nullptr) return 0;
            g_onexitCapacity += 0x10;
        } else {
            g_onexitCapacity += grow;
        }
        g_onexitTable = grown;
        g_onexitNext = grown + usedBytes / sizeof(uint32_t);
    }
    *g_onexitNext++ = func;
    return func;
}

// __onexit rule: __lock(8), register, __unlock(8) (0x34A0).
static uint32_t d2game_onexit(uint32_t func) {
    d2game_crt_lock(8);
    const uint32_t result = d2game_onexit_lk(func);
    d2game_crt_unlock(8);
    return result;
}

extern "C" {
// 1.13c D2Game+0x3261
// SEH fragment in _fwrite (0x321F): __unlock_file(*(FILE**)(EBP + 0x14)), i.e. the fourth argument. A
// probe passes the stream in r[5].
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3261(uint32_t* r) {
    d2game_file_unlock((FILE*)(uintptr_t)r[5]);
    r[4] = 0;
}

// 1.13c D2Game+0x326B
// __fclose_lk(stream): -1; a stream with _flag & 0x83 set is flushed, its buffer freed, its fd closed
// and any _tmpfname freed; _flag is cleared either way. A close failure forces -1. Host stdio holds the
// flag/buffer/temp-name state internally and returns EOF on failure, so the close is the whole effect.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_326b(uint32_t* r) {
    r[4] = (uint32_t)(int32_t)std::fclose((FILE*)(uintptr_t)r[0]);
}

// 1.13c D2Game+0x3300
// SEH fragment in _fclose (0x32B7): __unlock_file(unaff_ESI), the stream held in ESI. A probe passes the
// stream in r[5].
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3300(uint32_t* r) {
    d2game_file_unlock((FILE*)(uintptr_t)r[5]);
    r[4] = 0;
}

// 1.13c D2Game+0x3308
// __ArrayUnwind(ptr, size, count, callback): destroy `count` elements at `size` stride. r[3] carries the
// retail callback address and is not called; the callback is env.callOne (see the env comment).
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3308(uint32_t* r) {
    d2game_array_unwind((void*)(uintptr_t)r[0], r[1], (int32_t)r[2], d2game_env(r).callOne);
    r[4] = 0;
}

// 1.13c D2Game+0x3366
// `eh vector destructor iterator'(ptr, size, count, callback): destroy each element in turn; the trailing
// FUN_6fc233ae is the injected SEH epilogue handler and runs only when a destructor unwinds.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3366(uint32_t* r) {
    d2game_array_unwind((void*)(uintptr_t)r[0], r[1], (int32_t)r[2], d2game_env(r).callOne);
    r[4] = 0;
}

// 1.13c D2Game+0x33AE
// Unwind handler fragment of __ArrayUnwind: the operand at EBP-0x1c being 0 selects the handler, which
// then unwinds the parent's arguments at EBP+8/+0xc/+0x10/+0x14 -- ptr, size, count, callback. Those are
// r[1..3] here and env.callOne; r[0] is that selector word.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_33ae(uint32_t* r) {
    if ((int32_t)r[0] == 0)
        d2game_array_unwind((void*)(uintptr_t)r[1], r[2], (int32_t)r[3], d2game_env(r).callOne);
    r[4] = 0;
}

// 1.13c D2Game+0x33C6
// __onexit_lk(func): append to the __onexit table, growing it as needed; the registered address (NULL
// only when both growth attempts fail) is the result. The func arrives in EDI -> r[0].
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_33c6(uint32_t* r) {
    r[4] = d2game_onexit_lk(r[0]);
}

// 1.13c D2Game+0x3446
// ___onexitinit: allocate the 0x80-byte table, seed its first slot with 0 and point the free slot at its
// base; a failed allocation reports 0x18, success 0.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3446(uint32_t* r) {
    g_onexitTable = (uint32_t*)std::malloc(0x80);
    if (g_onexitTable == nullptr) {
        r[4] = 0x18;
        return;
    }
    *g_onexitTable = 0;
    g_onexitNext = g_onexitTable;
    g_onexitCapacity = 0x80;
    r[4] = 0;
}

// 1.13c D2Game+0x346E
// __onexit(func): __lock(8), register on the table, __unlock(8); the registered address is the result.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_346e(uint32_t* r) {
    r[4] = d2game_onexit(r[0]);
}

// 1.13c D2Game+0x34A0
// __unlock(8) thunk (via 0x28EE): the compat critical section is a no-op.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_34a0(uint32_t* r) {
    d2game_crt_unlock(8);
    r[4] = 0;
}

// 1.13c D2Game+0x34A6
// _atexit(func): register through __onexit and return 0 on success, -1 when registration failed.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_34a6(uint32_t* r) {
    r[4] = (uint32_t)(int32_t)((d2game_onexit(r[0]) != 0) - 1);
}

// 1.13c D2Game+0x34B8
// `eh vector constructor iterator'(ptr, size, count, ctor, dtor): construct each element in turn; the
// trailing FUN_6fc23502 is the injected SEH epilogue handler and runs only when a constructor unwinds
// (it would unwind with env.unwindOne). r[3] is the retail ctor address; env.callOne is called instead.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_34b8(uint32_t* r) {
    const D2GameBatch19Env& env = d2game_env(r);
    uint8_t* element = (uint8_t*)(uintptr_t)r[0];
    for (int32_t i = 0; i < (int32_t)r[2]; ++i) {
        if (env.callOne != nullptr) env.callOne(element);
        element += r[1];
    }
    r[4] = 0;
}
}
