// 1.13c D2Client+0xAB780 (67 B): append a fresh 0x1F4C-byte chunk to the record chain. Retail layout (32-bit links):
//   chunk = Fog alloc(ECX = 0x1F4C, EDX = file string, stack 0x35, 0 -- ret 8)
//   chunk[+0x1F40] = 0 (count), chunk[+0x1F44] = 0, old[+0x1F48] = chunk, chunk[+0x1F48] = 0, current (DAT 0x6FB8E694) = chunk
// Links are the low 32 bits of the chunk address (the vector convention for blob pointers); the old chunk is read from the global.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClientAb780Env {
    uint8_t* (*alloc)(uint32_t size, uint32_t line, uint32_t zero);   // Ordinal 10042 thunk +0xBF7A, ECX = size, EDX = file
};

extern "C" void d2client_ab780_port(uint32_t* current, const D2ClientAb780Env* env) {
    uint8_t* nu = env->alloc(0x1F4C, 0x35, 0);
    uint8_t* old = (uint8_t*)(uintptr_t)*current;
    const uint32_t z = 0, nu32 = (uint32_t)(uintptr_t)nu;
    std::memcpy(nu + 0x1F40, &z, 4);
    std::memcpy(nu + 0x1F44, &z, 4);
    std::memcpy(old + 0x1F48, &nu32, 4);
    std::memcpy(nu + 0x1F48, &z, 4);
    *current = nu32;
}
