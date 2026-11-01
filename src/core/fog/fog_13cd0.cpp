// 1.13c Fog+0x13CD0 (bits_read; ESI = bit buffer, n on the stack): read n bits LSB first. An over-read sets full
// and reads only the bits that exist. Host-layout copy of the bits_read in fog_batch_13c40.cpp (anon namespace there).
#include <cstdint>

namespace {
struct Bits { uint8_t* p; int32_t nBits, pos, bit, full; };  // host layout (8-byte pointer)
int32_t lowmask(int32_t n) { return (1 << n) - 1; }          // table at Fog+0x6FF76898 (n = 0..8)
}  // namespace

extern "C" int32_t fog_sub_13CD0(void* buf, int32_t n) {  // 1.13c Fog+0x13CD0
    Bits* b = static_cast<Bits*>(buf);
    int32_t over = b->pos * 8 - b->nBits + n + b->bit;
    if (over < 0) over = 0;
    int32_t left = n - over;
    if (over) b->full = 1;
    int32_t r = 0, sh = 0;
    while (left > 0) {
        int32_t k = 8 - b->bit; if (left < k) k = left;
        int32_t o = b->bit; left -= k;
        r += (int32_t)((uint32_t)(((uint8_t)(0xFFu << o) & *b->p) >> o) & (uint32_t)lowmask(k)) << (sh & 31);
        sh += k; b->bit = o + k;
        if (o + k == 8) { b->p++; b->bit = 0; b->pos++; }
    }
    return r;
}
