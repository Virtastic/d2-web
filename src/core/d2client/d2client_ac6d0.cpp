// 1.13c D2Client+0xAC6D0: packet handler, ECX = packet (slot byte +1, word +2, dword +4). With limit = the data tables' skill-row count (+0xBA0), writes
// the 12-bit index (or -1 past the limit, signed compare) to D2Client+0x119D30[slot], the dword to +0x119DD0[slot] and bit 15 of the word to +0x119E20[slot];
// EAX is that flag.
#include <cstdint>
#include <cstring>
extern "C" uint32_t d2c_ac6d0(int32_t limit, uint8_t* a /* +0x119D30 */, uint8_t* b /* +0x119DD0 */, uint8_t* c /* +0x119E20 */, const uint8_t* pkt) {
    uint16_t w; std::memcpy(&w, pkt + 2, 2); uint32_t d; std::memcpy(&d, pkt + 4, 4);
    int32_t idx = w & 0xfff; if (idx > limit) idx = -1;
    const uint32_t flag = (w & 0x8000) != 0, slot = pkt[1];
    std::memcpy(a + 4 * slot, &idx, 4); std::memcpy(b + 4 * slot, &d, 4); std::memcpy(c + 4 * slot, &flag, 4);
    return flag;
}
