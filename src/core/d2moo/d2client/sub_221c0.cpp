// 1.13c D2Client+0x221C0: map a mode/type nibble to a string id.
#include <stdint.h>
#include <emscripten.h>
// fastcall(ecx=v), eax live-in = pointer (null -> fatal error + _exit(-1), reported as 0xFFFFFFFF). Returns 0x18e / 0x191 / 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_221c0(uint32_t v, uint32_t eax) {
    if (!eax) return 0xFFFFFFFFu;
    uint32_t n = v & 0xf;
    return (n >= 1 && n <= 9) ? 0x18e : (n == 10 || n == 11) ? 0x191 : 0;
}
