// 1.13c Fog+0xFB90 (ordinal 10058): reset the performance accumulator. While the index (module global +0x56E7C) is nonzero, zero the
// first two dwords of each 6-dword (0x18 byte) entry of the array at +0x32F48; then store GetTickCount() at +0x4AF40. The tail is not
// conditioned on the index. The state is passed in, so the probe and the play path share one body.
#include <cstdint>
#include <cstring>
extern "C" void fog_perf_reset_fb90(uint32_t index, uint8_t* acc, uint32_t* tickSlot, uint32_t now) {
    for (; index != 0; --index, acc += 0x18) memset(acc, 0, 8);
    *tickSlot = now;
}
