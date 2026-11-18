// 1.13c D2Client+0x6EB70 (EAX = in, EBX = out; no stack arguments): cut a string at its first '*'. Unless in[0] is '*', 0xBF02 (strchr, stdcall 2)
// looks for the star: a hit copies the bytes before it to out and NUL-terminates; otherwise (leading star, or none) 0xBE96 (bounded copy, stdcall 3,
// max 0x7FFFFFFF) copies the whole string.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2c6EB70Env {
    uint32_t (*findChar)(uint32_t s, uint32_t c);                          // 0xBF02
    void (*copyBounded)(uint32_t dst, uint32_t src, uint32_t max);         // 0xBE96
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_port_6eb70(uint32_t in, uint32_t out, const D2c6EB70Env* env) {
    if (*(const uint8_t*)(uintptr_t)in != 0x2a) {
        const uint32_t p = env->findChar(in, 0x2a);
        if (p) {
            const uint32_t n = p - in;
            std::memcpy((void*)(uintptr_t)out, (const void*)(uintptr_t)in, n);
            *(uint8_t*)(uintptr_t)(out + n) = 0;
            return;
        }
    }
    env->copyBounded(out, in, 0x7fffffffu);
}
