// src/core/d2client/d2client_batch_4857.cpp -- ten UNMAPPED 1.13c D2Client.dll rows reimplemented from
// the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Base 0x6FAB0000; every RVA
// below is retail's, and every global this file owns is a static named at its retail RVA.
//
// All are MSVC 2003 Release CRT glue: the 9-byte `_unlock(10)`, `_unlock(4)`,
// `_unlock(1)` and `_unlock(12)` wrappers around the already-recovered _unlock (0x47AB); `_errno` and
// `_doserrno` are __getptd() + their _ptiddata offsets; `__inc` is the stdio getc fast path; the TlsAlloc
// thunk has the FlsAlloc signature and ignores its callback. Callees that are not recovered yet
// (__nh_malloc, __report_gsfailure, __filbuf, the TlsAlloc import) are host seams AFTER the retail
// arguments (the src/core/d2client/d2client_batch_1e70.cpp pattern).
#include <cstdint>
#include <cstddef>
#include <emscripten.h>
#include "crt_security_init_cookie_b645.h"

// 1.13c D2Client+0x047AB: _unlock(idx), recovered in src/core/d2client/d2client_batch_3664.cpp.
extern "C" void d2_client_47ab(uint32_t idx);

struct D2ClientBatch4857Env {
    void* (*nh_malloc)(uint32_t size, uint32_t mode);   // 0x7D33 __nh_malloc(size, _newmode)
    void  (*report_failure)(void);                      // 0x77A1 __report_gsfailure, noreturn
};

// Retail module state, all zero-initialised.
static uint32_t g_newMode;      // +0x0FAAC64  DAT_6fbaac64: _newmode, __nh_malloc's second argument
uint32_t d2_client_stack_cookie_db780;  // +0x0DB780: shared module __security_cookie
static uint8_t  g_ptd[0x40];    // __getptd (0x72F7) per-thread block; _terrno sits at +8

// 1.13c D2Client+0x04857: `push 0xA; call 0x47AB` -- release CRT lock 10. The critical section is kernel
// (host) state, so the callee is the whole rule.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_4857(void (*unlock)(uint32_t)) {   // unlock: callee seam (null = 0x47AB), for the FN probe
    (unlock ? unlock : d2_client_47ab)(10);
}

// 1.13c D2Client+0x048E4: `push 4; call 0x47AB` -- release CRT lock 4. Same shape as 0x4857; the index
// is the only difference.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_48e4(void (*unlock)(uint32_t)) {   // unlock: callee seam (null = 0x47AB)
    (unlock ? unlock : d2_client_47ab)(4);
}

// 1.13c D2Client+0x0579C (_errno): `call __getptd (0x72F7); add eax, 8; ret`. The 1.13c CRT has no
// _ptiddata under wasm, so the block is a static and the rule is the offset: &_getptd()->_terrno == ptd + 8.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t* d2_client_579c(void) {
    return (int32_t*)(g_ptd + 8);
}

// 1.13c D2Client+0x07D5F (_malloc): cdecl `void *__cdecl _malloc(size_t)`. Forwards to
// __nh_malloc(size, _newmode) -- the allocation policy (heap, return-NULL-on-failure vs the new-handler)
// lives in that callee, which is unrecovered, hence the seam.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_7d5f(uint32_t size, const D2ClientBatch4857Env* env) {
    return env->nh_malloc(size, g_newMode);
}

// 1.13c D2Client+0x077D2 (__security_check_cookie): fastcall (the cookie arrives in ECX). Equal to
// __security_cookie -> return; otherwise __report_gsfailure(), which never returns in retail.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_77d2(uint32_t cookie, const D2ClientBatch4857Env* env) {
    if (cookie == d2_client_stack_cookie_db780) return;
    env->report_failure();
}

// --- b55 batch: three more CRT leaves and two more _unlock sites -------------------------------
// Seams for the two callees these rows reach that are not recovered yet. The wasm module has no PE
// import table, so the kernel32 TlsAlloc import is a host seam too.
struct D2ClientBatchB55Env {
    uint32_t (*tls_alloc)(void);        // IAT 0xFAA5C TlsAlloc
    int32_t  (*filbuf)(void* stream);   // 0x80C5 __filbuf(FILE*)
};

// 1.13c D2Client+0x0593a: `push 1; call 0x47AB` -- release CRT lock 1 (the __getstream lock, 0x5821).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_593a(void) {
    d2_client_47ab(1);
}

// 1.13c D2Client+0x072b5: `push 0xC; call 0x47AB` -- release CRT lock 12 (the ___updatetlocinfo
// lock, 0x7283).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_72b5(void) {
    d2_client_47ab(0xC);
}

// 1.13c D2Client+0x072be (9 B): the TlsAlloc thunk __mtinit (0x74DE) installs in the FlsAlloc slot.
// It has FlsAlloc's signature (one callback argument, `ret 4`), ignores the callback, and returns the
// new TLS index; -1 on failure is what __mtinit tests. The index space is host state, hence the seam.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_72be(void* callback, const D2ClientBatchB55Env* env) {
    (void)callback;
    return env->tls_alloc();
}

// 1.13c D2Client+0x057A5 (_doserrno accessor): `call __getptd (0x72F7); add eax, 0xC; ret` -- the
// address of the current thread's _ptiddata._tdoserrno (VS2003 offset 0xC, right after _terrno at +8).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t* d2_client_57a5(void) {
    return (int32_t*)(g_ptd + 0xC);
}

// 1.13c D2Client+0x061A6 (__inc): the stdio getc fast path. `--_cnt >= 0` returns the byte at _ptr and
// advances it; an exhausted buffer defers to __filbuf. Fields in the tree's packed wasm64 FILE form
// (_ptr at +0, _cnt at +4 + sizeof(void*) - 4) -- the src/core/d2client/d2client_batch_03f82.cpp layout.
#pragma pack(push, 1)
struct D2cFileInc {
    char*   ptr;    // +0x00
    int32_t cnt;    // +0x04
};
#pragma pack(pop)
static_assert(offsetof(D2cFileInc, cnt) == 0x04 + 1 * (sizeof(void*) - 4), "FILE._cnt");

// 1.13c D2Client+0x061a6
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_61a6(D2cFileInc* f, const D2ClientBatchB55Env* env) {
    if (--f->cnt >= 0) return (uint8_t)*f->ptr++;
    return env->filbuf(f);
}
