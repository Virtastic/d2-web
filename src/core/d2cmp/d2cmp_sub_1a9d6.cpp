#include <cstdint>
#include <cstring>

static uint32_t rd32(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
static void wr32(uint8_t* p, uint32_t v) { memcpy(p, &v, 4); }
static uint16_t rd16(const uint8_t* p) { uint16_t v; memcpy(&v, p, 2); return v; }
static void wr16(uint8_t* p, uint16_t v) { memcpy(p, &v, 2); }

// 1.13c D2CMP+0x1A9D6 -- 80-bit extended multiply (CRT x87 emulation): a *= b.
void d2cmp_sub_1a9d6(uint8_t* a, const uint8_t* b)
{
    auto zero = [&]() { memset(a, 0, 12); };
    auto overflow = [&](uint16_t sg) {
        wr32(a, 0); wr32(a + 4, 0);
        wr32(a + 8, (sg ? 0x80000000u : 0) + 0x7fff8000u);
    };
    uint8_t t[12] = {0}; // 96-bit product accumulator
    int sticky = 0;
    uint16_t ea = rd16(a + 10) & 0x7fff, eb = rd16(b + 10) & 0x7fff;
    uint16_t sign = (rd16(b + 10) ^ rd16(a + 10)) & 0x8000;
    uint16_t es = (uint16_t)(ea + eb), ex;
    if (!(ea < 0x7fff && eb < 0x7fff && es < 0xbffe)) return overflow(sign);
    if (es < 0x3fc0) return zero();
    if (ea == 0) {
        es++;
        ex = 0;
        if ((rd32(a + 8) & 0x7fffffff) == 0 && rd32(a + 4) == 0 && rd32(a) == 0) { wr16(a + 10, 0); return; }
    }
    if (eb == 0) {
        es++;
        if ((rd32(b + 8) & 0x7fffffff) == 0 && rd32(b + 4) == 0 && rd32(b) == 0) return zero();
    }
    for (int i = 0, n = 5; n > 0; i++, n--) {
        for (int j = 0; j < n; j++) {
            uint32_t m = (uint32_t)rd16(b + 8 - 2 * j) * rd16(a + 2 * (i + j));
            uint32_t o = rd32(t + 2 * i), s = o + m;
            wr32(t + 2 * i, s);
            if (s < o || s < m) wr16(t + 2 * i + 4, rd16(t + 2 * i + 4) + 1);
        }
    }
    ex = (uint16_t)(es + 0xc002);
    auto shr = [&]() {
        uint32_t d0 = rd32(t), d1 = rd32(t + 4), d2 = rd32(t + 8);
        wr32(t + 4, d1 >> 1 | d2 << 31); wr32(t + 8, d2 >> 1); wr32(t, d0 >> 1 | d1 << 31);
    };
    auto shl = [&]() {
        uint32_t d0 = rd32(t), d1 = rd32(t + 4), d2 = rd32(t + 8);
        wr32(t, d0 << 1); wr32(t + 4, d1 << 1 | d0 >> 31); wr32(t + 8, d2 << 1 | d1 >> 31);
    };
    bool denorm = (int16_t)ex < 1;
    if (!denorm) {
        do {
            if (t[11] & 0x80) break;
            shl();
            ex--;
        } while ((int16_t)ex > 0);
        denorm = (int16_t)ex < 1;
    }
    if (denorm) {
        ex--;
        if ((int16_t)ex < 0) {
            uint32_t n = (uint16_t)-ex;
            ex = 0;
            do {
                if (t[0] & 1) sticky++;
                shr();
            } while (--n);
            if (sticky) t[0] |= 1;
        }
    }
    {
        uint16_t w0 = rd16(t);
        if (w0 > 0x8000 || (rd32(t) & 0x1ffff) == 0x18000) {
            uint32_t A = rd32(t + 2), B = rd32(t + 6);
            uint16_t C = rd16(t + 10);
            if (A == 0xffffffff) {
                A = 0;
                if (B == 0xffffffff) {
                    B = 0;
                    if (C == 0xffff) { ex++; C = 0x8000; } else C++;
                } else B++;
            } else A++;
            wr32(t + 2, A); wr32(t + 6, B); wr16(t + 10, C);
        }
    }
    if (ex > 0x7ffe) return overflow(sign);
    memcpy(a, t + 2, 10);
    wr16(a + 10, ex | sign);
    return;
}
