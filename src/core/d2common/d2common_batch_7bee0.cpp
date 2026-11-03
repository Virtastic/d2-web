// D2Common 1.13c batch: unit post-update dispatch.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Common7bc60Env;

extern "C" {
__attribute__((weak)) void d2_fn_d2common_68400();
__attribute__((weak)) void d2_fn_d2common_bf70(uint8_t* u);
__attribute__((weak)) void D2Common_7BC60_LinkTail(uint8_t* u, const D2Common7bc60Env* env);

// 1.13c D2Common+0x7BEE0: unit u (ECX). Oddity kept: 0x68400 is called with no visible argument.
EMSCRIPTEN_KEEPALIVE void D2Common_7BEE0_PostUpdate(uint8_t* u, const D2Common7bc60Env* env) {
    uint32_t flags, mode;
    std::memcpy(&flags, u + 0x28, 4);
    if (!(flags & 0x1000000) && d2_fn_d2common_68400) d2_fn_d2common_68400();
    std::memcpy(&flags, u + 0x28, 4);  // retail re-reads after the call
    std::memcpy(&mode, u + 0x48, 4);
    if (mode == 2 && !(flags & 0x2000000) && d2_fn_d2common_bf70) d2_fn_d2common_bf70(u);
    if (u[0x44] > 3 && D2Common_7BC60_LinkTail) D2Common_7BC60_LinkTail(u, env);
}
}
