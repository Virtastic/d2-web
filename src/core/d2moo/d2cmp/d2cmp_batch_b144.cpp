// D2CMP batch b144: F8A0 / FAA0 / FD70 RLE blits+encoder, 1A06B CRT digit accumulator.
// Retail register args are named by role; no clip except FAA0.
#include <cstdint>
#include <cstring>

// 1.13c D2CMP+0x0f8a0 -- remap-then-blend: dst = blend[dst*256 + remap[src]] over each literal run.
// run byte b>=0 : b literals; b&0x80 : skip (b&0x7f), 0 ends the row.
extern "C" void d2cmp_f8a0(const int8_t* src, uint8_t* dst, int rows, int stride,
                           const uint8_t* remap, const uint8_t* blend) {
    do {
        uint8_t* next = dst + stride;
        for (;;) {
            uint8_t b = (uint8_t)*src++;
            if (b & 0x80) {
                b &= 0x7f;
                if (!b) break;
                dst += (int8_t)b;
                continue;
            }
            for (int i = 0; i < b; i++) dst[i] = blend[dst[i] * 256 + remap[(uint8_t)src[i]]];
            dst += b; src += b;
        }
        dst = next;
    } while (--rows);
}

// 1.13c D2CMP+0x0faa0 -- clipped plain copy; x0 = start x (EAX), visible [clipL, clipR).
extern "C" void d2cmp_faa0(int x0, const int8_t* src, uint8_t* dst, int rows, int stride,
                           int clipL, int clipR) {
    uint8_t* row = dst;
    int x = x0;
    for (;;) {
        uint8_t b = (uint8_t)*src++;
        if (b & 0x80) {
            b &= 0x7f;
            if (b) { x += (int8_t)b; dst += (int8_t)b; continue; }
            if (--rows == 0) return;
            row += stride; dst = row; x = x0;
            continue;
        }
        int s = clipL - x; if (s < 0) s = 0;
        int e = clipR - x; if (b < e) e = b;
        if (e > s) std::memcpy(dst + s, src + s, e - s);
        src += b; x += b; dst += b;
    }
}

// 1.13c D2CMP+0x0fd70 -- RLE encode: per row, zero run -> 0x80|n, literal run -> n,bytes; row ends 0x80.
// Trailing zeros of a row are dropped. flags&1: top-down, else bottom-up. *out is alloc'd (2w+1)*h.
extern "C" void d2cmp_fd70(uint32_t width, const uint8_t* buf, uint32_t height, uint32_t flags,
                           uint8_t** out, int* outSize, void* (*alloc)(uint32_t n)) {
    uint8_t* base = (uint8_t*)alloc((2 * width + 1) * height);
    uint8_t* o = base;
    for (uint32_t r = 0; r < height; r++) {
        const uint8_t* p = buf + (size_t)((flags & 1) ? r : height - 1 - r) * width;
        const uint8_t* e = p + width;
        while (p < e) {
            const uint8_t* q = p;
            if (!*p) {
                while (q < e && !*q && q - p < 0x7f) q++;
                const uint8_t* z = q;
                while (z < e && !*z) z++;
                if (z == e) break;  // only zeros remain
                *o++ = 0x80 | (uint8_t)(q - p);
            } else {
                while (q < e && *q && q - p < 0x7f) q++;
                *o++ = (uint8_t)(q - p);
                std::memcpy(o, p, q - p); o += q - p;
            }
            p = q;
        }
        *o++ = 0x80;
    }
    *out = base;
    *outSize = (int)(o - base);
}

// 1.13c D2CMP+0x1a06b -- decimal digits (signed chars, as retail) -> 96-bit v=v*10+c, then x87 extended
// layout: mantissa normalised to bit 15 of word2, exponent (start 0x404E) as int16 at byte 10.
extern "C" void d2cmp_1a06b(const char* s, int n, uint32_t* o) {
    auto shl = [](uint32_t* p) {
        p[2] = p[2] << 1 | p[1] >> 31; p[1] = p[1] << 1 | p[0] >> 31; p[0] <<= 1;
    };
    int16_t exp = 0x404e;
    o[0] = o[1] = o[2] = 0;
    for (; n; n--, s++) {
        uint32_t t[3] = {o[0], o[1], o[2]};
        shl(o); shl(o);
        uint64_t c = (uint64_t)o[0] + t[0]; o[0] = (uint32_t)c;
        c = (uint64_t)o[1] + t[1] + (c >> 32); o[1] = (uint32_t)c;
        o[2] = o[2] + t[2] + (uint32_t)(c >> 32);
        shl(o);
        c = (uint64_t)o[0] + (uint32_t)(int32_t)*s; o[0] = (uint32_t)c;
        c = (uint64_t)o[1] + (c >> 32); o[1] = (uint32_t)c;
        o[2] += (uint32_t)(c >> 32);
    }
    if (o[2] == 0) {
        uint32_t hi;
        do {
            exp -= 0x10;
            hi = o[1] >> 16;
            o[1] = o[0] >> 16 | o[1] << 16;
            o[0] <<= 16;
        } while (hi == 0);
        o[2] = hi;
    }
    while (!(o[2] & 0x8000)) { shl(o); exp--; }
    std::memcpy((char*)o + 10, &exp, 2);
}
