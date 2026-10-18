// src/core/d2client/d2client_2b40.cpp -- 1.13c D2Client.dll RVA 0x2B40.
// Base 0x6FAB0000. The unrecovered callee 0x2B90 is a host seam AFTER the retail arguments, which
// keep their order (the d2client_batch_1de0.cpp pattern). The SEH frame (ExceptionList, handler
// LAB_6FB7D7E5) is compiler/host state and has no wasm counterpart.
#include <cstdint>
#include <emscripten.h>

struct D2Client2b40Env {
    void (*sub_2B90)(void* param_1, void* self); // 0x2B90, called as (arg, this)
};

// 1.13c D2Client+0x02B40 (68 B): null-checked forwarder. When the argument is non-null it calls 0x2B90
// with the argument first and `self` second; a null argument is a no-op.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2b40(void* self, void* arg, const D2Client2b40Env* env) {
    if (arg != nullptr)
        env->sub_2B90(arg, self);
}
