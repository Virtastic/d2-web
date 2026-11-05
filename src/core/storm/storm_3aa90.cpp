// 1.13c Storm+0x3AA90: 64-bit name hash. A null string sets the last error to
// ERROR_INVALID_PARAMETER and returns 0. A zero seed becomes 0x7FED7FED7FED7FED. Unless flags bit 0 is set, each
// character is upper-cased (a-z) and '/' becomes '\\' first. A zero result is returned as 1.
#include <cstdint>

static const uint64_t kNibble[16] = {
    0x486e26eedcaa16b3ull, 0xe1918eef202dafdbull, 0x341c7dc71c365303ull, 0x40ef2d3765fd5e49ull,
    0xd6057177904ece93ull, 0x1c38024f98fd323bull, 0xe3061ae7a39b0fa1ull, 0x9797f25fe4444563ull,
    0xcd2ec20c8dc1b898ull, 0x31759633799a306dull, 0x8c2063852e6e9627ull, 0x79237d9973922c66ull,
    0x8728628d28628824ull, 0x8f1f7e9625887795ull, 0x296e3281389c0d60ull, 0x6f4893ca61636542ull,
};

static uint32_t g_err3aa90;
extern "C" uint32_t* Storm_3AA90_LastError() { return &g_err3aa90; }

uint64_t Storm_3AA90(const uint8_t* s, uint32_t flags, uint32_t lo, uint32_t hi) {
    if (!s) { g_err3aa90 = 0x57; return 0; }
    uint64_t v = ((uint64_t)hi << 32) | lo, k = 0xEEEEEEEEEEEEEEEEull;
    if (v == 0) v = 0x7fed7fed7fed7fedull;
    for (uint32_t c; (c = *s) != 0; ++s) {
        if (!((uint8_t)flags & 1)) { if (c > 0x60 && c < 0x7b) c -= 0x20; if (c == 0x2f) c = 0x5c; }
        v = (kNibble[c >> 4] - kNibble[c & 15]) ^ (k + v);
        k = k * 0x21 + c + v + 3;
    }
    return v ? v : 1;
}
