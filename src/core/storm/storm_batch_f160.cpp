// Prior art used: D2MOO Storm.cpp exposes the imported Storm API but does not
// implement these internal compression/network leaves. Algorithms checked against
// the cached 1.13c decompiles; Stage A only, no retail parity claim.
#include <cstdint>
#include <cstring>

// 1.13c Storm+0x0F160
// Expand low-byte prefix codes into a 256-entry decoding table. Later writes
// from lower symbol indices deliberately replace entries filled earlier.
extern "C" void Storm_SubF160(int32_t count, const uint8_t* widths,
                               const uint8_t* codes, uint8_t* table) {
    for (int64_t symbol = int64_t(count) - 1; symbol >= 0; --symbol) {
        uint32_t slot = codes[symbol];
        const uint32_t step = uint32_t{1} << (widths[symbol] & 31);
        do {
            table[slot] = static_cast<uint8_t>(symbol);
            slot += step;
        } while (slot < 256);
    }
}

// 1.13c Storm+0x0F2A0
// Preserve retail's forward DWORD-copy ordering, including overlapping ranges.
extern "C" void Storm_SubF2A0(void* destination, const void* source, uint32_t bytes) {
    auto* out = static_cast<uint8_t*>(destination);
    const auto* in = static_cast<const uint8_t*>(source);
    for (uint32_t words = bytes >> 2; words; --words, out += 4, in += 4) {
        uint32_t word;
        std::memcpy(&word, in, sizeof(word));
        std::memcpy(out, &word, sizeof(word));
    }
    for (uint32_t tail = bytes & 3; tail; --tail) *out++ = *in++;
}

// 1.13c Storm+0x0FBD0
extern "C" void Storm_SubFBD0(void* destination, uint8_t value, uint32_t bytes) {
    // Retail stores repeated-byte DWORDs followed by zero to three single bytes.
    // memset has exactly that final state and accepts unaligned storage.
    if (bytes) std::memset(destination, value, bytes);
}

// 1.13c Storm+0x0FF40
extern "C" uint16_t Storm_SubFF40(uint32_t /*unusedEcx*/, uint32_t bytes,
                                  const uint8_t* data) {
    uint32_t sum = 0;
    uint32_t accumulated = 0;
    while (bytes) {
        sum += data[--bytes];
        if (sum > 254) sum -= 255;
        accumulated += sum; // x86 accumulation wraps before the final modulus.
    }
    return static_cast<uint16_t>((sum << 8) | (accumulated % 255));
}

// 1.13c Storm+0x0FF80
// The required capabilities arrive in EAX in retail, supplied explicitly here.
// These are numeric records, not native pointers: retain their DWORD layout.
extern "C" uint32_t Storm_SubFF80(const uint32_t* available,
                                  const uint32_t* requiredEax) {
    if (!available || !requiredEax) return 1;
    return (available[1] & requiredEax[1]) == requiredEax[1]
        && requiredEax[2] <= available[2]
        && requiredEax[3] <= available[3]
        && requiredEax[4] <= available[4]
        && requiredEax[5] <= available[5]
        && (requiredEax[6] == 0 || available[6] <= requiredEax[6]);
}
