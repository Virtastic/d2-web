// 1.13c Storm+0x25ED0/0x27FC0/0x25E50: MPQ name hash, crypt table fill, entry widening.
#include <cstdint>
#include <cstring>
// 1.13c Storm+0x25ED0: hash of s (ASCII-uppercased, signed-char index) under table slice `type`
uint32_t Storm_Sub25ED0(const uint32_t* tbl, int32_t type, const char* s) {
    uint32_t h = 0x7fed7fed; int32_t seed = -0x11111112;
    for (; s && *s; s++) {
        int8_t c = (int8_t)((*s >= 'a' && *s <= 'z') ? *s - 32 : *s);
        h = (h + (uint32_t)seed) ^ tbl[type * 0x100 + c];
        seed = (int32_t)((uint32_t)seed * 0x21 + (uint32_t)(int32_t)c + 3 + h);
    }
    return h;
}
// 1.13c Storm+0x27FC0: fill 0x500-dword table from seed; returns final state / 0x2aaaab
uint32_t Storm_Sub27FC0(uint32_t* t, uint32_t seed) {
    uint32_t a = seed, b = 0, nx = 0;
    for (int i = 0; i < 0x100; i++) {
        for (int k = 0; k < 5; k++) {
            a = (a * 0x7d + 3) % 0x2aaaab;
            b = (a * 0x7d + 3) % 0x2aaaab;
            t[i + 0x100 * k] = a << 16 | (b & 0xffff);
            nx = b * 0x7d + 3;
            a = nx % 0x2aaaab;
        }
    }
    return nx / 0x2aaaab;
}
// 1.13c Storm+0x25E50: widen n 16-byte entries to 28-byte in place (tail first), zeroing +0x10..+0x1b
void Storm_Sub25E50(uint8_t* base, uint32_t n) {
    for (uint32_t i = n; i-- > 0;) {
        uint8_t* d = base + i * 0x1c;
        std::memmove(d, base + i * 0x10, 0x10);
        std::memset(d + 0x10, 0, 12);
    }
}
