// Fog batch: bit-field helpers (+0x13xxx/+0x14xxx), Huffman-style symbol coder (+0x1Cxxx/+0x1Dxxx),
// string helpers (+0x1F7xx..+0x1F9xx). Decompile-derived from build-wasm/ghidra-fog/decomp.
// +0x140C0 and +0x13CA0 are marked in src/core/d2moo/src/Fog/BitManip.cpp (ReadSigned, GoToNextByte).
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>

void fog_error_report(int32_t type, const char* file, uint32_t line, const char* format, ...) __attribute__((weak));
[[noreturn]] void fog_exit_process(int32_t code) __attribute__((weak));
uint8_t fog_fold_byte_ext(uint8_t c) __attribute__((weak));

namespace {
// Retail bit buffer (D2BitBufferStrc layout, host pointer): data, bit capacity, byte pos, bit pos, full.
struct Bits { uint8_t* p; int32_t nBits, pos, bit, full; };

[[noreturn]] void fatal(uint32_t line) {
    if (fog_error_report) fog_error_report(6, "", line, "Unrecoverable internal error %08x");
    if (fog_exit_process) fog_exit_process(-1);
    __builtin_trap();
}
int32_t lowmask(int32_t n) { return (1 << n) - 1; }  // table at Fog+0x6FF76898 (n = 0..8)

void bits_init(Bits* b, uint8_t* data, int32_t bytes) {  // 1.13c Fog+0x141E0 (internal)
    if (!b) fatal(0x13c);
    b->p = data; b->nBits = bytes << 3; b->pos = 0; b->bit = 0; b->full = 0;
}
void bits_write(Bits* b, int32_t v, int32_t n) {  // 1.13c Fog+0x14120 (internal)
    if (b->nBits < b->bit + b->pos * 8 + n) { b->full = 1; b->pos = b->nBits / 8; b->bit = 0; return; }
    if (b->bit == 0) *b->p = 0;
    while (n >= 1) {
        int32_t k = 8 - b->bit; if (n < k) k = n;
        *b->p |= (uint8_t)((lowmask(k) & (uint8_t)v) << (b->bit & 31));
        int32_t o = b->bit; b->bit = o + k;
        if (o + k != 8) break;
        b->p++; b->pos++; b->bit = 0;
        if (b->pos * 8 < b->nBits) *b->p = 0;
        n -= k; v >>= k;
    }
}
int32_t bits_read(Bits* b, int32_t n) {  // 1.13c Fog+0x13CD0 (internal)
    int32_t over = b->pos * 8 - b->nBits + n + b->bit;
    if (over < 0) over = 0;
    int32_t left = n - over;
    if (over) b->full = 1;
    int32_t r = 0, sh = 0;
    while (left > 0) {
        int32_t k = 8 - b->bit; if (left < k) k = left;
        int32_t o = b->bit; left -= k;
        r += (int32_t)((uint32_t)(((uint8_t)(0xFFu << o) & *b->p) >> o) & (uint32_t)lowmask(k)) << (sh & 31);
        sh += k; b->bit = o + k;
        if (o + k == 8) { b->p++; b->bit = 0; b->pos++; }
    }
    return r;
}
// 1.13c Fog+0x1D100 (internal): code width for a symbol count 1..0x100.
int8_t code_width(int32_t n) {
    if (n < 1 || n > 0x100) fatal(0x1a);
    if (n > 0x10) return n < 0x41 ? (int8_t)((n > 0x20) + 5) : (int8_t)((n > 0x80) + 7);
    if (n < 5) return (int8_t)((n > 2) + 1);
    return (int8_t)((n > 8) + 3);
}
}  // namespace

// 1.13c Fog+0x13E40: dst = ~src over (bits+7)/8 bytes.
extern "C" void fog_sub_13E40(const uint8_t* src, uint8_t* dst, int32_t bits) {  // 1.13c Fog+0x13E40
    int32_t n = (bits + 7) / 8;
    for (int32_t i = 0; i < n; ++i) dst[i] = (uint8_t)~src[i];
}

// 1.13c Fog+0x13DA0: dst = a ^ b over (bits+7)/8 bytes.
extern "C" void fog_sub_13DA0(const uint8_t* a, const uint8_t* b, uint8_t* dst, int32_t bits) {  // 1.13c Fog+0x13DA0
    int32_t n = (bits + 7) / 8;
    for (int32_t i = 0; i < n; ++i) dst[i] = a[i] ^ b[i];
}

// 1.13c Fog+0x14240: write table[idx] = {value, bitCount} (32-bit pair) to the buffer.
extern "C" void fog_sub_14240(void* buf, const int32_t* table, int32_t idx) {  // 1.13c Fog+0x14240
    bits_write(static_cast<Bits*>(buf), table[idx * 2], table[idx * 2 + 1]);
}

// 1.13c Fog+0x13C40: walk a binary tree table one bit at a time until a non-negative entry.
extern "C" int32_t fog_sub_13C40(void* buf, const int32_t* table) {  // 1.13c Fog+0x13C40
    Bits* b = static_cast<Bits*>(buf);
    int32_t base = 0;
    for (;;) {
        int32_t o = b->bit;
        uint32_t bit = ((1u << o) & *b->p) >> o;
        b->bit = o + 1;
        if (o + 1 == 8) { b->p++; b->bit = 0; b->pos++; }
        int32_t i = (int32_t)bit + base;
        if (table[i] >= 0) return table[i];
        base = i * 2 + 2;
    }
}

// 1.13c Fog+0x1D190: build the coder (width at +0, symbol->index [4+c], index->symbol [0x103+i]).
extern "C" void fog_sub_1D190(uint8_t* h, uint8_t maxBits, const char* syms) {  // 1.13c Fog+0x1D190
    if (!h) fatal(0x41);
    if (*syms == 0) fatal(0x42);
    std::memset(h, 0, 0x204);
    uint8_t n = 0;
    for (const char* s = syms; *s; ++s) {
        if (h[(int8_t)*s + 4] == 0) {
            ++n;
            if ((1 << (maxBits & 31)) <= (int32_t)n) fatal(0x52);
            h[(int8_t)*s + 4] = n;
            h[n + 0x103] = (uint8_t)*s;
        }
    }
    int32_t w = code_width(n);
    std::memcpy(h, &w, 4);
}

// 1.13c Fog+0x1CFA0: encode a NUL-terminated string into out[outLen] with the coder.
extern "C" void fog_sub_1CFA0(const uint8_t* h, const char* str, uint8_t* out, int32_t outLen, int32_t terminate) {  // 1.13c Fog+0x1CFA0
    if (!h) fatal(0x6e);
    if (!str) fatal(0x6f);
    if (!out) fatal(0x70);
    int32_t w; std::memcpy(&w, h, 4);
    if (w == 0) fatal(0x71);
    Bits b; bits_init(&b, out, outLen);
    int32_t lim = (w * outLen + 7) >> 3;
    if (terminate) --lim;
    int32_t cnt = 0;
    char c = *str;
    while (c != 0 && (++cnt, cnt < lim)) {
        bits_write(&b, (int8_t)h[c + 4], w);
        c = *++str;
    }
    if (terminate || cnt < lim) bits_write(&b, 0, w);
}

// 1.13c Fog+0x1CE50: decode in[inLen] into out[outCap] (NUL-terminated).
extern "C" void fog_sub_1CE50(const uint8_t* h, char* out, uint8_t* in, int32_t inLen, int32_t outCap) {  // 1.13c Fog+0x1CE50
    if (!h) fatal(0x97);
    if (!out) fatal(0x98);
    if (!in) fatal(0x99);
    int32_t w; std::memcpy(&w, h, 4);
    if (w == 0) fatal(0x9a);
    int32_t i = 0;
    int32_t cap = outCap - 1;
    int32_t m = (inLen * 8 + 7) / w;
    if (m < cap) cap = m;
    Bits b; bits_init(&b, in, inLen);
    if (cap > 0) {
        do {
            int8_t idx = (int8_t)bits_read(&b, w);
            out[i++] = (char)h[idx + 0x103];
            if (idx == 0) return;
        } while (i < cap);
    }
    out[i] = 0;
}

// 1.13c Fog+0x1F910: capitalise the first letter of each space-separated word, lower-case the rest.
extern "C" void fog_sub_1F910(char* s) {  // 1.13c Fog+0x1F910
    if (!s) return;
    bool start = true;
    int32_t len = (int32_t)std::strlen(s);
    for (int32_t i = 0; i < len; ++i) {
        if (i > 0x3ff) fatal(0x78);
        char c = s[i];
        if (c == ' ') { start = true; continue; }
        s[i] = (char)(start ? std::toupper(c) : std::tolower(c));
        start = false;
    }
}

// 1.13c Fog+0x1F8A0 (fastcall str, len): case-fold in place through the Fog table, stop at NUL or len.
extern "C" void fog_sub_1F8A0(char* s, int32_t len) {  // 1.13c Fog+0x1F8A0
    if (!fog_fold_byte_ext) return;  // table lives in fog_batch_11fe0.cpp
    char* end = s + len;
    for (; *s != 0 && s < end; ++s) *s = (char)fog_fold_byte_ext((uint8_t)*s);
}

// 1.13c Fog+0x1F710 (fastcall dst, src, cap, quote): escape src into dst[cap], optionally quoted.
extern "C" uint8_t* fog_sub_1F710(uint8_t* dst, const uint8_t* src, int32_t cap, uint8_t quote) {  // 1.13c Fog+0x1F710
    if (!dst) fatal(0xc0);
    if (!src) { std::snprintf(reinterpret_cast<char*>(dst), (size_t)cap, "<NULL>"); return dst; }
    if (cap == 0) return dst;
    int32_t n = cap - 1;
    uint8_t* p = dst;
    uint8_t* e = dst;  // write cursor at loop exit
    int32_t r = n;
    bool done = false;
    if (quote) {
        if (n < 1) { done = true; }
        else { *p++ = quote; n = cap - 2; }
    }
    while (!done) {
        e = p; r = n;
        if (n < 1 || *src == 0) break;
        uint8_t c = *src++;
        r = n - 1;
        if ((int8_t)c >= ' ') { *p++ = c; n = r; continue; }
        *p = 0x5c; e = p + 1;
        if (r == 0) break;
        r = n - 2;
        if (c == 0xd) { p[1] = 'r'; p += 2; n = r; continue; }
        if (c == 10) { p[1] = 'n'; p += 2; n = r; continue; }
        if (c == 9) { p[1] = 't'; p += 2; n = r; continue; }
        if (r < 2) break;
        p[1] = 'x';
        p[2] = (uint8_t)((c & 0xf0) < 0xa0 ? (c >> 4) + 0x30 : (c >> 4) + 0x37);
        p[3] = (uint8_t)((c & 0xf) < 10 ? (c & 0xf) + 0x30 : (c & 0xf) + 0x37);
        p += 4; n -= 4;
    }
    if (quote && r > 0) { *e++ = quote; --r; }
    *e = 0;
    if (r < 0) fatal(0x100);
    if ((int32_t)(e + 1 - dst) > cap) fatal(0x101);
    return dst;
}
