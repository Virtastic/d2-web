// Bit-packed server packet unpack, 9 bytes. Retail: EAX = packet bytes, ESI = destination record, plain ret.
// A 20-byte stack bit reader is opened on the packet (Fog 10126, thunk 0xBF9E) and each field is read in order
// (Fog 10130, thunk 0xBFB0); a byte field keeps AL, a word field AX, stored as each read returns.
// 1.13c D2Client+0xAC140
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cBitReader {
    void (*open)(void* bb, const uint8_t* src, int32_t bytes);   // Fog 10126 (stdcall, 3 words)
    uint32_t (*read)(void* bb, int32_t bits);                    // Fog 10130 (stdcall, 2 words)
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_ac140(const uint8_t* src, uint8_t* dst, const D2cBitReader* f) {
    uint8_t bb[0x14];
    uint16_t v;
    f->open(bb, src, 9);
    dst[0x0] = (uint8_t)f->read(bb, 8);
    v = (uint16_t)f->read(bb, 15); std::memcpy(dst + 0x1, &v, 2);
    v = (uint16_t)f->read(bb, 16); std::memcpy(dst + 0x3, &v, 2);
    v = (uint16_t)f->read(bb, 16); std::memcpy(dst + 0x5, &v, 2);
    dst[0x7] = (uint8_t)f->read(bb, 8);
    dst[0x8] = (uint8_t)f->read(bb, 8);
}
