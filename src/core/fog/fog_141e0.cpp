// 1.13c Fog+0x141E0 (bits_init; stdcall buf, data, bytes): point the buffer at data, nBits = bytes*8, cursor and full
// cleared. Host-layout copy of bits_init in fog_batch_13c40.cpp; the null-buffer fatal path is not vectored.
#include <cstdint>

namespace {
struct Bits { uint8_t* p; int32_t nBits, pos, bit, full; };  // host layout (8-byte pointer)
}  // namespace

extern "C" void fog_sub_141E0(void* buf, uint8_t* data, int32_t bytes) {  // 1.13c Fog+0x141E0
    Bits* b = static_cast<Bits*>(buf);
    b->p = data; b->nBits = bytes << 3; b->pos = 0; b->bit = 0; b->full = 0;
}
