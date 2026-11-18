// 1.13c D2Client+0x01F90: stdcall wrapper (ret 0xC). Reloads the three stack words and calls 0x24B0 with
// ESI = 1 (a non-null "self"), so after the allocation 0x2400(1, 0) runs.
#include <cstdint>
#include <emscripten.h>
struct D2ClientBatch2190Env;
extern "C" int32_t d2_client_24b0(uint32_t, int32_t, uint32_t, void*, const D2ClientBatch2190Env*);
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_1f90(uint32_t param_1, int32_t param_2, uint32_t param_3,
                                             const D2ClientBatch2190Env* env) {
    return d2_client_24b0(param_1, param_2, param_3, (void*)(uintptr_t)1, env);
}
