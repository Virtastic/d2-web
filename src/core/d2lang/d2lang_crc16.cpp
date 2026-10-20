// Prior art used: D2MOO D2Lang/src/D2Crc16.cpp (CRC polynomial and range contract),
// cross-checked against the cached retail 1.13c routine. No module state is needed.
#include <cstdint>
#include <cstdlib>

// 1.13c D2Lang+0x092F0: CRC-16/CCITT, initial 0xffff, over [first,last).
// Retail reports the failed range assertion through Fog and exits with -1.
extern "C" void d2_fn_d2lang_92f0(uint16_t* result, const uint8_t* bytes,
                                  uint32_t first, uint32_t last)
{
    if (last <= first) std::exit(-1);
    *result = 0xffff;
    do {
        // Compute the table entry rather than carrying a retail-derived table.
        uint16_t entry = static_cast<uint16_t>((bytes[first++] ^ (*result >> 8)) << 8);
        for (unsigned bit = 0; bit != 8; ++bit)
            entry = static_cast<uint16_t>((entry << 1) ^ ((entry & 0x8000) ? 0x1021 : 0));
        *result = static_cast<uint16_t>((*result << 8) ^ entry);
    } while (first < last);
}
