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
    std::vector<uint32_t> cv, kv;  // 0x138b entries each (backing for the helper entry points)
    uint32_t *code, *key;          // the tables in use: the two SMemAlloc blocks (flag 8: zeroed) in Storm_Sub32D50
    uint32_t outn = 0;
    uint8_t buf[0x100 + 4] = {0};  // buf[0] = length, data from buf[1]
    Lzw() : cv(0x138b, 0), kv(0x138b, 0), code(cv.data()), key(kv.data()) {}
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
        std::fill(s.code, s.code + 0x138b, 0u);
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
extern "C" void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
extern "C" int32_t Storm_Sub32D50(const uint8_t* src, uint8_t* dst, int32_t n) {
    static const char* const kSbmp = "..\\3rdParty\\STORM\\SOURCE\\SBMP.CPP";
    Lzw s;
    auto* codeTab = (uint32_t*)Storm_Sub1C580(0x4e2c, kSbmp, 0x388, 8);
    auto* keyTab = (uint32_t*)Storm_Sub1C580(0x4e2c, kSbmp, 0x389, 8);
    s.code = codeTab; s.key = keyTab;
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
    Storm_Sub1B960(codeTab, kSbmp, 0x3a4);
    Storm_Sub1B960(keyTab, kSbmp, 0x3a5);
    return (int32_t)(d - dst);
}

// Raw-layout entry points for the two helpers (EAX/EBX = the 0x138-byte encoder state the caller owns):
// dwords [0]bits [1]mask [2]minBits [3]bitbuf [4]bitcnt [5]prefix [6]first [7]clear [8]end [9]next [10]code* [11]key* [12]outn, bytes from +0x34 = chunk buffer.
namespace {
void loadLzw(Lzw& s, const uint32_t* st, bool tables) {
    s.bits = st[0]; s.mask = st[1]; s.minBits = st[2]; s.bitbuf = st[3]; s.bitcnt = (int32_t)st[4]; s.prefix = st[5]; s.first = st[6];
    s.clear = st[7]; s.end = st[8]; s.next = st[9]; s.outn = st[12];
    memcpy(s.buf, (const uint8_t*)st + 0x34, sizeof s.buf);
    if (tables) {
        memcpy(s.code, (const void*)(uintptr_t)st[10], 0x138b * 4);
        memcpy(s.key, (const void*)(uintptr_t)st[11], 0x138b * 4);
    }
}
void storeLzw(const Lzw& s, uint32_t* st, bool tables) {
    st[0] = s.bits; st[1] = s.mask; st[3] = s.bitbuf; st[4] = (uint32_t)s.bitcnt; st[5] = s.prefix; st[6] = s.first; st[9] = s.next; st[12] = s.outn;
    memcpy((uint8_t*)st + 0x34, s.buf, sizeof s.buf);
    if (tables) {
        memcpy((void*)(uintptr_t)st[10], s.code, 0x138b * 4);
        memcpy((void*)(uintptr_t)st[11], s.key, 0x138b * 4);
    }
}
}  // namespace

extern "C" void Storm_Sub327E0(uint32_t* st, uint32_t c, uint8_t** pdst) {
    Lzw s; loadLzw(s, st, false);
    uint8_t* d = *pdst; emit(s, c, d); *pdst = d;
    storeLzw(s, st, false);
}
extern "C" void Storm_Sub32AD0(uint32_t* st, uint8_t** pdst, uint32_t b) {
    Lzw s; loadLzw(s, st, true);
    uint8_t* d = *pdst; addByte(s, d, b); *pdst = d;
    storeLzw(s, st, true);
}
