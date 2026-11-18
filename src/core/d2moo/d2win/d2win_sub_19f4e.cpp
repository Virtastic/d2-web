// src/core/d2win/d2win_sub_19f4e.cpp -- 1.13c D2Win.dll CRT 80-bit extended multiply (a *= b),
// from the cached Ghidra decompile (build-wasm/ghidra-d2win/decomp/0x19F4E.c) and the disassembly.
#include <cstdint>
#include <cstring>

// 1.13c D2Win+0x19F4E
// a, b: 12-byte values (10-byte mantissa at +0, sign:15 exponent at +10). Result in a.
// Returns the EAX retail leaves: the exponent is kept as a 32-bit word (its adds carry into the high half),
// so EAX is that word (or'ed with the sign on the normal exit), 0 on the zero exits, a's exponent on the early overflow.
extern "C" uint32_t d2win_sub_19f4e_eax(uint8_t* a, const uint8_t* b) {
    auto rd = [](const uint8_t* p, int o) { uint16_t v; memcpy(&v, p + o, 2); return v; };
    auto zero = [&](uint8_t* p) { memset(p, 0, 8); memset(p + 8, 0, 4); };
    uint16_t eb = rd(b, 10) & 0x7fff, ea = rd(a, 10) & 0x7fff;
    uint16_t sign = (rd(b, 10) ^ rd(a, 10)) & 0x8000;
    uint32_t d = (uint32_t)eb + ea;   // [ebp-0x14]
    auto inf = [&]() {
        memset(a, 0, 8);
        uint32_t t = (sign ? 0x80000000u : 0) + 0x7fff8000u;
        memcpy(a + 8, &t, 4);
    };
    if (!(ea < 0x7fff && eb < 0x7fff && (uint16_t)d < 0xbffe)) { inf(); return ea; }
    if ((uint16_t)d < 0x3fc0) { zero(a); return 0; }
    uint32_t a2; memcpy(&a2, a + 8, 4);
    uint32_t b2; memcpy(&b2, b + 8, 4);
    uint64_t a0; memcpy(&a0, a, 8);
    uint64_t b0; memcpy(&b0, b, 8);
    if (ea == 0) {
        d++;
        if (!(a2 & 0x7fffffffu) && !a0) { memset(a + 10, 0, 2); return 0; }
    }
    if (eb == 0) {
        d++;
        if (!(b2 & 0x7fffffffu) && !b0) { zero(a); return 0; }
    }
    uint16_t w[6] = {0, 0, 0, 0, 0, 0}; // 96-bit accumulator, w[0] lowest
    uint16_t pa[6], pb[6];
    memcpy(pa, a, 12); memcpy(pb, b, 12);
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5 - i; j++) {
            uint32_t cur = w[i] | ((uint32_t)w[i + 1] << 16);
            uint32_t add = (uint32_t)pb[4 - j] * pa[i + j];
            uint32_t sum = cur + add;
            w[i] = (uint16_t)sum; w[i + 1] = (uint16_t)(sum >> 16);
            if (sum < cur) w[i + 2]++;
        }
    d += 0xc002;
    int sticky = 0;
    if ((int16_t)d >= 1) {
        do {
            if (w[5] & 0x8000) break;
            for (int k = 5; k > 0; k--) w[k] = (uint16_t)((w[k] << 1) | (w[k - 1] >> 15));
            w[0] <<= 1;
            d += 0xffff;
        } while ((int16_t)d > 0);
    }
    if ((int16_t)d < 1) {
        d += 0xffff;
        if ((int16_t)d < 0) {
            uint32_t n = (uint16_t)(0u - d);
            d += n;
            do {
                if (w[0] & 1) sticky++;
                for (int k = 0; k < 5; k++) w[k] = (uint16_t)((w[k] >> 1) | (w[k + 1] << 15));
                w[5] >>= 1;
            } while (--n);
            if (sticky) w[0] |= 1;
        }
    }
    uint32_t lo17 = (w[0] | ((uint32_t)w[1] << 16)) & 0x1ffff;
    if (w[0] > 0x8000 || lo17 == 0x18000) {
        // increment the 80-bit mantissa w[1..5]
        int k = 1;
        while (k < 5 && ++w[k] == 0) k++;
        if (k == 5) {
            if (w[5] == 0xffff) { d++; w[5] = 0x8000; }
            else w[5]++;
        }
    }
    if ((uint16_t)d > 0x7ffe) { inf(); return d; }
    memcpy(a, &w[1], 10);
    d |= sign;
    uint16_t ex = (uint16_t)d;
    memcpy(a + 10, &ex, 2);
    return d;
}

extern "C" void d2win_sub_19f4e(uint8_t* a, const uint8_t* b) { d2win_sub_19f4e_eax(a, b); }
