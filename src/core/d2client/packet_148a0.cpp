#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2Client+0x148A0: packet = [op:1][a:4][b:4][c:4][d:4] (17 bytes), then sub_143E0(buf, len=17).
// op arrives in AL, a=ECX, b=EDX, c,d on the stack.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_148a0_build(uint8_t* out, uint32_t op, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
    out[0] = (uint8_t)op; memcpy(out + 1, &a, 4); memcpy(out + 5, &b, 4); memcpy(out + 9, &c, 4); memcpy(out + 13, &d, 4);
    return 17;
}
// 1.13c D2Client+0x148E0: packet = [op:1][a:4][b:4][c:4] (13 bytes); op=AL, a=ECX, b=EDX, c on stack.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_148e0_build(uint8_t* out, uint32_t op, uint32_t a, uint32_t b, uint32_t c) {
    out[0] = (uint8_t)op; memcpy(out + 1, &a, 4); memcpy(out + 5, &b, 4); memcpy(out + 9, &c, 4);
    return 13;
}
