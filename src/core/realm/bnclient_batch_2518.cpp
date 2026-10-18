// src/core/realm/bnclient_batch_2518.cpp -- one UNMAPPED 1.13c BNClient.dll alpha-lane row (Stage A:
// implementation + marker only, no spec, no vector), transcribed from
// build-wasm/ghidra-bnclient/decomp/0x2518.c (image base 0x6FF20000).
//
// Rows: __nh_malloc 0x2518 and _report_failure 0x3DF8, MSVC 2003 (VC7.1) Release CRT. __heap_alloc
// (0x249D) and __callnewh (0x49AE) are seams; both exist in this tree (src/core/realm/
// bnclient_batch_3fb6.cpp, bnclient_batch_030c3.cpp).
#include <cstdint>
#include <emscripten.h>

struct BnNhMallocEnv {
    void*   (*heap_alloc)(uint32_t size);   // 0x249D __heap_alloc
    int32_t (*callnewh)(uint32_t size);     // 0x49AE __callnewh
};

static void* bn_no_heap_alloc_2518(uint32_t) { return nullptr; }
static int32_t bn_no_callnewh_2518(uint32_t) { return 0; }

static const BnNhMallocEnv g_bn_nh_malloc_default = {bn_no_heap_alloc_2518, bn_no_callnewh_2518};

// 1.13c BNClient+0x02518 (44 B, caller 0x2544 __malloc): __nh_malloc(size, nhflag). Requests at or above
// 0xFFFFFFE1 fail without trying. Otherwise retry __heap_alloc; a null result with nhflag clear returns
// null, and with nhflag set asks __callnewh(size) for room, repeating while it reports success.
extern "C"
EMSCRIPTEN_KEEPALIVE void* bn_nh_malloc(uint32_t size, int32_t nhflag, const BnNhMallocEnv* env) {
    if (env == nullptr) env = &g_bn_nh_malloc_default;
    if (size >= 0xffffffe1u) return nullptr;
    for (;;) {
        void* p = env->heap_alloc(size);
        if (p != nullptr) return p;
        if (nhflag == 0) return nullptr;
        if (env->callnewh(size) == 0) return nullptr;
    }
}

// 1.13c BNClient+0x03DF8 (41 B, caller 0x3E29): _report_failure(): report code 1 through 0x72CA and
// ExitProcess(3). Same body as D2MCPClient 0x4794. Neither callee returns.
struct BnReportFailureEnv {
    void (*report)(uint32_t code);          // 0x72CA CRT security-failure reporter, noreturn
    void (*exit_process)(uint32_t code);    // IAT ExitProcess
};

extern "C"
EMSCRIPTEN_KEEPALIVE void bn_report_failure(const BnReportFailureEnv* env) {
    env->report(1);
    env->exit_process(3);
}
