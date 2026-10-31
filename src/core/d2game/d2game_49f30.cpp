// D2Game+0x49F30 -- reset four dwords of a buffer to -1, then hand it to #a41e.
// ecx = owner, edx = size, eax = buffer. size < 16 or a null owner is fatal (line 0x3be).
// #a41e(owner, buffer, &local) takes a 16-byte stack local the function never initialises.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game49F30Env {
    void (*fatal)(uint32_t line);
    void (*a41e)(uint32_t owner, uint32_t buf, uint32_t local16);
    uint32_t local16;   // address standing in for the uninitialised 16-byte stack local
};

// 1.13c D2Game+0x49F30
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_49f30(uint32_t owner, uint32_t size, uint32_t buf, const D2Game49F30Env* env)
{
    if (size < 16 || owner == 0) { env->fatal(0x3be); return; }
    const uint32_t m = 0xffffffffu;
    for (int i = 0; i < 4; ++i) std::memcpy((void*)(uintptr_t)(buf + i * 4), &m, 4);
    env->a41e(owner, buf, env->local16);
}
