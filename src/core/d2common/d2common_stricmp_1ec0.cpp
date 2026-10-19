#include <cstdint>

namespace {
uint8_t asciiLower(uint8_t value)
{
    return value >= 'A' && value <= 'Z' ? uint8_t(value + 32) : value;
}
}

// 1.13c D2Common+0x01EC0
// The game uses the initial C locale. A non-null mapping represents the already
// resolved CRT locale's byte-to-lowercase results; no host locale state is read.
extern "C" int32_t D2Common_Stricmp_1ec0(const char* left, const char* right,
                                        const int32_t* lowercase = nullptr)
{
    for (;;) {
        const auto a = static_cast<uint8_t>(*left++);
        const auto b = static_cast<uint8_t>(*right++);
        const int32_t foldedA = lowercase ? lowercase[a] : asciiLower(a);
        const int32_t foldedB = lowercase ? lowercase[b] : asciiLower(b);
        if (foldedA != foldedB)
            return lowercase ? foldedA - foldedB : (foldedA < foldedB ? -1 : 1);
        if (foldedA == 0)
            return 0;
    }
}
