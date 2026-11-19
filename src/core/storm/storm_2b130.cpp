// 1.13c Storm+0x2B130 (EAX = the value, ECX = the byte stream {cap @0, count @4, buffer @8, step @0xC}): appends the value to the
// stream as base-255 digits, least significant first (each digit is value % 255, then value /= 255 while nonzero), then the 0xFF
// terminator. Every byte first checks that count+1 fits in cap; past it the stream grows: the step is @0xC when set, else the 0x3D00
// step (need >= 0x100 stores 0x100 into @0xC; below, the highest power of two <= need, 1 for 0). The new capacity is need rounded up to
// the step and 0x3D40 reallocates the buffer (ESI = the stream, EAX = the new capacity).
#include <cstdint>

extern "C" void Storm13c_3D40(uint32_t* stream, uint32_t newcap);  // 1.13c Storm+0x3D40

extern "C" uint32_t Storm13c_3D00(uint32_t n, uint32_t* obj);  // 1.13c Storm+0x3D00: the step for a stream without one

namespace {
uint32_t step_for(uint32_t* s, uint32_t need) { return Storm13c_3D00(need, s); }

// Grows the stream so that need bytes fit (need > cap on entry).
void grow(uint32_t* s, uint32_t need) {
    uint32_t step = s[3];
    if (step == 0) step = step_for(s, need);
    const uint32_t rem = need % step;
    Storm13c_3D40(s, rem != 0 ? need - rem + step : need);
}
}  // namespace

extern "C" void Storm13c_2B130(uint32_t value, uint32_t* s) {
    uint32_t v = value;
    while (v != 0) {
        if (s[1] + 1 > s[0]) grow(s, s[1] + 1);
        const uint32_t i = s[1];
        s[1] = i + 1;
        reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(s[2]))[i] = (uint8_t)(v % 0xFFu);
        v /= 0xFFu;
    }
    if (s[1] + 1 > s[0]) grow(s, s[1] + 1);
    const uint32_t i = s[1];
    s[1] = i + 1;
    reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(s[2]))[i] = 0xFF;
}
