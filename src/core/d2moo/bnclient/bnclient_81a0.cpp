// 1.13c BNClient+0x081A0 -- key26 spread. cdecl (key, digits): reads 26 key bytes,
// writes 52 digit bytes. The retail translation table (RVA 0x1A5D0) maps every byte to its key26 alphabet index
// (0..24) or 255, with both letter cases mapping alike; the alphabet below reproduces that table byte-for-byte.
// Cursor steps by 1973 modulo 52 from 33. Prior art used: bncsutil cdkeydecoder.cpp (base-5 split only).
#include <cstdint>

extern "C" void bn_key26_spread(const char* key, uint8_t* digits) {
    constexpr char alphabet[] = "246789BCDEFGHJKMNPRTVWXYZ";
    uint32_t b = 33;
    for (uint32_t i = 0; i != 26; ++i) {
        uint8_t c = uint8_t(key[i]);
        if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
        uint8_t value = 255;
        for (uint32_t j = 0; j != 25; ++j)
            if (c == uint8_t(alphabet[j])) value = uint8_t(j);
        const uint32_t a = (b + 1973) % 52;
        b = (a + 1973) % 52;
        digits[a] = value / 5;
        digits[b] = value % 5;
    }
}
