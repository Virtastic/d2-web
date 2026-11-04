// 1.13c Fog+0x163C0 -- build a zeroed 0xB3-dword CONTEXT (ContextFlags=0x10007) and hand it with the
// current-thread handle to the stack walker Fog+0x15F50. No threads/CONTEXT on wasm: the walker is a seam.
// The frame words are the ones retail's call-next/pop trick leaves (ctx Ebp = its frame pointer, Eip = the module address of the
// label after the call, Esp = its stack pointer). The module address is held as the 0xB2000000|RVA tag the vectors use (no image base on wasm).
#include <cstdint>
#include <cstring>

#include <emscripten.h>

extern "C" void fog_sub_15f50(void* param, uint32_t thread, void* ctx);  // seam: stack walker, Fog RVA 0x15F50

extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_163c0(void* param_1) {  // 1.13c Fog+0x163C0
    uint32_t ctx[0xb3];
    memset(ctx, 0, sizeof ctx);
    ctx[0] = 0x10007;
    uint32_t local = 0;
    ctx[45] = (uint32_t)(uintptr_t)__builtin_frame_address(0);   // Ebp
    ctx[46] = 0xB20163EBu;                                       // Eip: 0x163EB, the label after the call
    ctx[49] = (uint32_t)(uintptr_t)&local;                       // Esp
    fog_sub_15f50(param_1, 0xfffffffeu /* GetCurrentThread pseudo-handle */, ctx);
}
