// 1.13c Fog+0x163C0 -- build a zeroed 0x2CC-byte CONTEXT (ContextFlags=0x10007) and hand it with the
// current-thread handle to the stack walker Fog+0x15F50. No threads/CONTEXT on wasm: the walker is a seam.
#include <cstdint>
#include <cstring>

#include <emscripten.h>

extern "C" void fog_sub_15f50(void* param, uint32_t thread, void* ctx);  // seam: stack walker, Fog RVA 0x15F50

extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_163c0(void* param_1) {  // 1.13c Fog+0x163C0
    uint32_t ctx[0xb3];
    memset(ctx, 0, sizeof ctx);
    ctx[0] = 0x10007;
    fog_sub_15f50(param_1, 0xfffffffeu /* GetCurrentThread pseudo-handle */, ctx);
}
