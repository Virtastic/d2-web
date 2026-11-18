// Prior art used: D2MOO Storm.cpp contains export declarations only. StormLib's
// SBaseCommon.cpp DecryptMpqBlock cross-checks the MPQ key recurrence. The limb
// scans/comparison below follow the cached 1.13c SBig helpers, including their
// offset-relative return values and zero result for an all-zero operand.
#include "storm_big_types.h"
#include <cstdint>
#include <cstring>

// 1.13c Storm+0x2AD10
uint32_t Storm_2AD10(const Storm2fVector* value) {
    uint32_t position = value->offset;
    const uint32_t length = value->count - position;
    for (uint32_t relative = 0; relative < length; ++relative, ++position) {
        if (position < value->count) {
            const uint32_t word = value->data[position];
            if (word) {
                for (uint32_t bit = 0; bit < 32; ++bit)
                    if (word & (uint32_t{1} << bit)) return relative * 32 + bit;
            }
        }
    }
    return 0;
}

// 1.13c Storm+0x2ADC0
uint32_t Storm_2ADC0(const Storm2fVector* value) {
    uint32_t remaining = value->count - value->offset;
    uint32_t position = value->offset + remaining;
    while (remaining) {
        --remaining;
        --position;
        if (position < value->count) {
            const uint32_t word = value->data[position];
            if (word) {
                for (uint32_t bit = 32; bit; ) {
                    --bit;
                    if (word & (uint32_t{1} << bit)) return remaining * 32 + bit;
                }
            }
        }
    }
    return 0;
}

// 1.13c Storm+0x2AE30
int32_t Storm_2AE30(const Storm2fVector* left, const Storm2fVector* right) {
    uint32_t a = left->offset, b = right->offset;
    int32_t order = 0;
    for (;;) {
        uint32_t x;
        if (a < left->count) x = left->data[a];
        else {
            if (right->count <= b) return order;
            x = 0;
        }
        const uint32_t y = b < right->count ? right->data[b] : 0;
        // More significant limbs replace the result from lower limbs. Trailing
        // zero limbs compare equal to the implicit zeros beyond either count.
        if (x != y) order = x > y ? 1 : -1;
        ++a;
        ++b;
    }
}

// 1.13c Storm+0x25F30
// cryptTable is the caller's live DAT_6FC43120 table (five blocks of 256
// DWORDs). bytes is retail EAX, explicitly supplied; only whole DWORDs change.
extern "C" void Storm_Sub25F30(uint32_t key, void* buffer, uint32_t bytes,
                                const uint32_t* cryptTable) {
    auto* data = static_cast<uint8_t*>(buffer);
    uint32_t seed = 0xeeeeeeeeu;
    for (uint32_t count = bytes >> 2; count; --count, data += 4) {
        seed += cryptTable[0x400 + (key & 0xff)];
        uint32_t plain;
        std::memcpy(&plain, data, sizeof(plain));
        plain ^= seed + key;
        seed = seed * 33 + 3 + plain;
        std::memcpy(data, &plain, sizeof(plain));
        key = (key >> 11) | ((~key * 0x200000u) + 0x11111111u);
    }
}
