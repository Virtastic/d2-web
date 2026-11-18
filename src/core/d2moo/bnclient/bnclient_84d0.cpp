// 1.13c BNClient+0x084D0 -- key26 substitution, in place on four dwords.
// The 480-byte table at RVA 0x1A3F0 is 30 rows of 16 nibbles; it is held here as hex digits, one row per string,
// and checked byte-for-byte against the retail table. Walks positions 29..0, each one folding every other nibble
// through the table at offset position*16. Prior art used: bncsutil cdkeydecoder.cpp (table bytes only).
#include <cstdint>

namespace {
constexpr char substitutions[] =
    "947fda3b12c86e509b548f1e7032a6dc"
    "ce149fabd6087253b25ed3901f7ca648"
    "6245b8cedf71a03954ec76daf2910b83"
    "c78fb059da6e24313ae81b542fdc6790"
    "cd1f8e5b3a907246da7e16b8fc523049"
    "3e75bf8c1a4d0692b69418ad7e0cf235"
    "c78d3b0e6f94a152c6d9b012f734ae85"
    "3615bc80fe947ad2a7bf280dec169354"
    "abd4385910fc7e26b4df163e7ac89250"
    "96701ad23efc5b48de56198c2f37b40a"
    "9f4016ae237d5b8c3e1a2c84b7d0f695"
    "72c6a8b0f43e91d5c459a28d3f1e67b0"
    "a8ed9f30461c7b253c4a2fde705816b9"
    "ac109edb37f85246ea18765c2f0d3b49"
    "38e079fc16d25ab43ac4db9ef6172058"
    ;
uint32_t substitute(uint32_t index) {
    const char c = substitutions[index];
    return c <= '9' ? uint32_t(c - '0') : uint32_t(c - 'a' + 10);
}
uint32_t nibble(const uint32_t* words, uint32_t n) {
    return (words[3 - n / 8] >> ((n % 8) * 4)) & 15;
}
}

extern "C" void bn_key26_substitute(uint32_t* words) {
    for (int position = 29; position >= 0; --position) {
        const uint32_t offset = uint32_t(position) * 16;
        uint32_t value = nibble(words, uint32_t(position));
        for (int other = 29; other > position; --other)
            value = substitute(offset + (nibble(words, uint32_t(other)) ^ substitute(offset + value)));
        for (int other = position - 1; other >= 0; --other)
            value = substitute(offset + (nibble(words, uint32_t(other)) ^ substitute(offset + value)));
        const uint32_t shift = (uint32_t(position) % 8) * 4;
        uint32_t& word = words[3 - uint32_t(position) / 8];
        word = (word & ~(uint32_t(15) << shift)) | (substitute(offset + value) << shift);
    }
}
