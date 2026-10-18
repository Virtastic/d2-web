// src/core/d2client/d2client_batch_057ae.cpp -- two UNMAPPED 1.13c D2Client.dll rows reimplemented
// from the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane, OBJECTIVE v8.3:
// implementation + marker, no spec, no vector). Base 0x6FAB0000; every RVA below is retail's.
//
// The other ten rows this file used to carry (__dosmaperr, __flush, _fclose's tail, _memcpy, __close_lk,
// __close, __freebuf, ___isctype_mt, ___freetlocinfo, ___updatetlocinfo_lk) are already implemented by
// main's d2client_batch_b56.cpp, which owns those RVAs; their duplicates were dropped from this tree.
//
// Callees already recovered elsewhere are called directly: __lock 0x4860 (batch_04860) and its
// _unlock(12) wrapper 0x72B5 (batch_4857), __mtdeletelocks 0x4756 (batch_03f82), and
// ___updatetlocinfo_lk 0x071C2 (d2client_batch_b56.cpp). The rest are host seams AFTER the retail
// arguments, which keep their order (the src/core/d2client/d2client_batch_1e70.cpp pattern).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"

// Callees recovered elsewhere in the tree. Their env structs belong to those TUs: here they are opaque
// pointers forwarded unchanged (only the pointer crosses the C ABI).
struct D2cGetptdEnv;             // d2client_batch_03f82.cpp / d2client_batch_908f.cpp
struct D2ClientBatch04860Env;    // d2client_batch_04860.cpp
extern "C" void     d2_client_4860(int32_t locknum, const D2ClientBatch04860Env* env); // 0x04860
extern "C" void     d2_client_72b5(void);                                             // 0x072B5 _unlock(12)
extern "C" void     d2_client_4756(D2cHost h);                                        // 0x04756
extern "C" uint32_t d2_client_getptd_index(void);                                     // +0x0DB774
extern "C" void     d2_client_getptd_pin(uint32_t tls_index);                         // sets it

// d2client_batch_b56.cpp's own env shape, repeated so the TUs agree at the C ABI (only `ptd`, the
// last field, is read by 0x071C2; the 24 pointers ahead of it are never touched here).
struct D2ClientBatchB56Env {
    void*               callees[24];
    const D2cGetptdEnv* ptd;
};
extern "C" uint32_t d2_client_71c2(const D2ClientBatchB56Env* env);   // 0x071C2

struct D2ClientBatch057AEEnv {
    const D2cGetptdEnv*          ptd;          // 0x72F7's env
    const D2ClientBatch04860Env* lock;         // 0x4860's env
    D2cHost                      free_host;    // __mtdeletelocks 0x4756
    void     (*tls_free)(uint32_t index);      // [0x0FAA6C] slot
};

// 1.13c D2Client+0x07283 (___updatetlocinfo): CRT lock 12 around ___updatetlocinfo_lk, released by
// the _unlock(12) wrapper at 0x072B5.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_7283(const D2ClientBatch057AEEnv* env) {
    d2_client_4860(0xC, env->lock);
    D2ClientBatchB56Env owner = {};
    owner.ptd = env->ptd;
    uint32_t locale = d2_client_71c2(&owner);
    d2_client_72b5();
    return locale;
}

// 1.13c D2Client+0x072C7 (__mtterm): release the thread-data TLS index (through the TlsFree slot the
// MULTITHREAD init installed) and mark it free, then tear down the CRT's lock table.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_72c7(const D2ClientBatch057AEEnv* env) {
    uint32_t index = d2_client_getptd_index();
    if (index != 0xFFFFFFFFu) {
        env->tls_free(index);
        d2_client_getptd_pin(0xFFFFFFFFu);
    }
    d2_client_4756(env->free_host);
}
