// 1.13c Storm+0x32D50: SBMP variable-width LZW encoder, with its
// helpers 0x327E0 (emit code) and 0x32AD0 (add byte). Output is 255-byte-max length-prefixed chunks.
#include <cstdint>
#include <algorithm>
#include <cstring>
#include <vector>

namespace {
struct Lzw {
    uint32_t bits = 9, mask = 0x1ff, minBits = 9, bitbuf = 0;
    int32_t bitcnt = 0;
    uint32_t prefix = 0, first = 1, clear = 0x100, end = 0x101, next = 0x102;
    std::vector<uint32_t> code, key;  // 0x138b entries each
    uint32_t outn = 0;
    uint8_t buf[0x100 + 4] = {0};  // buf[0] = length, data from buf[1]
    Lzw() : code(0x138b, 0), key(0x138b, 0) {}
};

// 1.13c Storm+0x327E0
void emit(Lzw& s, uint32_t c, uint8_t*& dst) {
    int32_t nb = s.bitcnt + (int32_t)s.bits;
    s.bitbuf |= c << (s.bitcnt & 31);
    s.bitcnt = nb;
    while (nb > 7) {
        s.buf[++s.outn] = (uint8_t)s.bitbuf;
        if (s.outn > 0xfe) {
            uint32_t n = s.outn;
            s.buf[0] = (uint8_t)n;
            memcpy(dst, s.buf, n + 1);
            dst += n + 1;
            s.outn = 0;
        }
        s.bitbuf >>= 8;
        nb = s.bitcnt -= 8;
    }
    if ((int32_t)s.mask < (int32_t)s.next) {
        if (++s.bits == 0xc) { s.mask = 0x1000; return; }
        s.mask = (1u << s.bits) - 1;
    }
}

// 1.13c Storm+0x32AD0
void addByte(Lzw& s, uint8_t*& dst, uint32_t b) {
    if (s.first) { s.prefix = b; s.first = 0; return; }
    int32_t i = (int32_t)(b * 0x10 + s.prefix);
    if (i > 0x138a) i -= 0x138b;
    uint32_t k = (s.prefix << 8) | b;
    if (s.code[i]) {
        if (s.key[i] == k) { s.prefix = s.code[i]; return; }
        int32_t step = i == 0 ? 1 : 0x138b - i;
        for (;;) {
            i -= step;
            if (i < 0) i += 0x138b;
            if (!s.code[i]) break;
            if (s.key[i] == k) { s.prefix = s.code[i]; return; }
        }
    }
    emit(s, s.prefix, dst);
    if (s.next > 0xfff) {
        std::fill(s.code.begin(), s.code.end(), 0u);
        s.next = s.clear + 2;
        emit(s, s.clear, dst);
        s.bits = s.minBits;
        s.mask = (1u << s.bits) - 1;
        s.prefix = b;
        return;
    }
    s.code[i] = s.next++;
    s.key[i] = k;
    s.prefix = b;
}
}  // namespace

// 1.13c Storm+0x32D50: compress n bytes of src into dst; returns bytes written.
extern "C" int32_t Storm_Sub32D50(const uint8_t* src, uint8_t* dst, int32_t n) {
    Lzw s;
    uint8_t* d = dst;
    emit(s, s.clear, d);
    for (; n > 0; --n) addByte(s, d, *src++);
    if (!s.first) emit(s, s.prefix, d);
    emit(s, s.end, d);
    if (s.bitcnt > 0) s.buf[++s.outn] = (uint8_t)s.bitbuf;
    if (s.outn) {
        s.buf[0] = (uint8_t)s.outn;
        memcpy(d, s.buf, ++s.outn);
        d += s.outn;
    }
    return (int32_t)(d - dst);
}
