// 1.13c Fog+0x14120 (bits_write; stdcall buf, v, n): write the low n bits of v LSB first. Past nBits it sets full,
// parks the cursor at nBits/8 and writes nothing. Host-layout copy of bits_write in fog_batch_13c40.cpp.
#include <cstdint>

namespace {
struct Bits { uint8_t* p; int32_t nBits, pos, bit, full; };  // host layout (8-byte pointer)
int32_t lowmask(int32_t n) { return (1 << n) - 1; }
}  // namespace

extern "C" void fog_sub_14120(void* buf, int32_t v, int32_t n) {  // 1.13c Fog+0x14120
    Bits* b = static_cast<Bits*>(buf);
    if (b->nBits < b->bit + b->pos * 8 + n) { b->full = 1; b->pos = b->nBits / 8; b->bit = 0; return; }
    if (b->bit == 0) *b->p = 0;
    while (n >= 1) {
        int32_t k = 8 - b->bit; if (n < k) k = n;
        *b->p |= (uint8_t)((lowmask(k) & (uint8_t)v) << (b->bit & 31));
        int32_t o = b->bit; b->bit = o + k;
        if (o + k != 8) break;
        b->p++; b->pos++; b->bit = 0;
        if (b->pos * 8 < b->nBits) *b->p = 0;
        n -= k; v >>= k;
    }
}
