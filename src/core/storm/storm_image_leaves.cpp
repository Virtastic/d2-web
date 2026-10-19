#include "storm_image_leaves.h"

// Prior art used: D2MOO source/Storm exports have no SBMP implementation;
// PvPGN bniutils/tga.h cross-checks header offsets, mapped/truecolor type IDs,
// attribute and interleave bits. StormLib only recognizes PCX file signatures.
// The cached 1.13c decompiles supply these exact acceptance and run rules.
// These leaves own no globals or host callbacks. Arguments passed in retail
// registers are explicit native parameters; only image bytes remain serialized.

// 1.13c Storm+0x31F60
extern "C" void Storm_Sub31F60(uint32_t count, const uint8_t** input, uint8_t** output) {
    const uint8_t* source = *input;
    uint8_t* destination = *output;
    // Retail's caller supplies a positive scanline width. Preserve the do/while:
    // zero is outside the contract, rather than an invented successful empty row.
    do {
        const uint8_t value = *source++;
        --count;
        uint32_t run = 1;
        while (count != 0 && run < 63 && *source == value) {
            ++source;
            --count;
            ++run;
        }
        if (run >= 2 || value >= 0xc0) {
            *destination++ = static_cast<uint8_t>(0xc0 | run);
        }
        *destination++ = value;
    } while (count != 0);
    *input = source;
    *output = destination;
}

// 1.13c Storm+0x32690
extern "C" uint32_t Storm_Sub32690(const uint8_t* header) {
    const uint8_t type = header[2];
    if (type != 1 && type != 2 && type != 9 && type != 10) return 0;
    if ((header[17] & 0xc0) != 0) return 0;
    const uint32_t attributes = header[17] & 15;
    if (header[1] == 0) {
        return (header[16] == 24 && attributes == 0) ||
               (header[16] == 32 && attributes == 8);
    }
    const uint32_t first = uint32_t(header[3]) | (uint32_t(header[4]) << 8);
    const uint32_t entries = uint32_t(header[5]) | (uint32_t(header[6]) << 8);
    return header[1] == 1 && header[16] == 8 && first + entries < 257 &&
           (header[7] == 24 || header[7] == 32);
}
