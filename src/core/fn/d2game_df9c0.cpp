// d2game+0xdf9c0 sub_DF9C0: probe. fastcall ecx = idx, edx = record blob (counter at +0x90+4*idx); the port reads it by address.
// The record's wasm64 address comes from the replay's probe-only pair r[14]/r[15] (r[1] holds only the low dword).
// The retail function is void but leaves the stored counter in eax, and the vector records eax as the result: r[4] mirrors it.
#include <emscripten.h>
#include <cstdint>
extern "C" void d2_fn_d2game_DF9C0(int32_t param1, void* param2);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_df9c0(uint32_t* r) {
    uintptr_t rec = (uintptr_t)r[14] | ((uintptr_t)r[15] << 32);
    d2_fn_d2game_DF9C0((int32_t)r[0], (void*)rec);
    r[4] = *(uint32_t*)(rec + (uint32_t)(0x90 + r[0] * 4));   // 32-bit offset wrap, as the retail address math
}
