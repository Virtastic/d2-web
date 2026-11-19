// 1.13c D2Client+0x14910 seam for the platform layer: the 9-byte packet [op, ecx dword, edx dword]. Returns the address of the
// packet bytes (9 of them); valid until the next call.
#include <cstdint>
#include <emscripten.h>
extern "C" uint32_t d2_client_14910(uint32_t op, uint32_t c, uint32_t d, uint8_t* o);
extern "C" EMSCRIPTEN_KEEPALIVE const uint8_t* d2_client_packet9(uint32_t op, uint32_t c, uint32_t d) {
    static uint8_t b[16];
    d2_client_14910(op, c, d, b);
    return b;
}
