// 1.13c BNClient+0x13250: CRC-32 over `count` bytes at `data`. The retail table at
// 0x1DCF0 is the reflected CRC-32 table (poly 0xEDB88320), built here at first use. The NULL-with-count path is the
// retail fatal exit (Ordinal_10024 log, _exit(-1)); it is not transcribed.
#include <cstdint>

namespace {
struct Crc32Table {
    uint32_t t[256];
    Crc32Table() {
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t c = i;
            for (int k = 0; k < 8; ++k) c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
            t[i] = c;
        }
    }
};
const Crc32Table g_crc;
}  // namespace

// 1.13c BNClient+0x13250
extern "C" uint32_t bn_crc32_13250(const uint8_t* data, uint32_t count) {
    uint32_t crc = 0xFFFFFFFFu;
    for (; count != 0; --count) crc = (crc >> 8) ^ g_crc.t[(*data++ ^ crc) & 0xFFu];
    return ~crc;
}
