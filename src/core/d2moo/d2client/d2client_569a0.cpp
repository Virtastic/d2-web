// 1.13c D2Client+0x569A0: ECX = unit, one ignored stack word (ret 4). Sets bits 1-3 of the unit flags at +0xC4; always 1.
#include <cstdint>
#include <cstring>
extern "C" uint32_t d2c_569a0(uint8_t* unit) {
    if (unit) { uint32_t f; std::memcpy(&f, unit + 0xc4, 4); f |= 0xeu; std::memcpy(unit + 0xc4, &f, 4); }
    return 1;
}
