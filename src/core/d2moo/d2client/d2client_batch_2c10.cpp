// src/core/d2client/d2client_batch_2c10.cpp -- 1.13c D2Client.dll 0x02C10 reimplemented from
// build-wasm/ghidra-d2client/decomp/0x2C10.c. Base 0x6FAB0000; the RVA below is retail's.
// Unrecovered callees are host seams (the src/core/d2client/cursor_init_166c0.cpp pattern): the
// function takes a D2ClientBatch2c10Env AFTER its retail arguments, which keep their order.
#include <cstdint>
#include <emscripten.h>

struct D2ClientBatch2c10Env {
    void (*destroy)(void* obj);                                              // 0x3F360
    void (*opDelete)(void* p, const char* file, int32_t line, int32_t kind); // 0x0BE6C, Ordinal_403
};

// 1.13c D2Client+0x02C10
// Scalar deleting destructor: 0x3F360(this), then for flags bit 0 (and this non-NULL) free `this`
// through operator delete(this, "delete", -1, 0). Returns `this` in EAX either way.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_2c10(void* obj, uint32_t nFlags, const D2ClientBatch2c10Env* env) {
    env->destroy(obj);
    if ((nFlags & 1) && obj) env->opDelete(obj, "delete", -1, 0);
    return obj;
}
