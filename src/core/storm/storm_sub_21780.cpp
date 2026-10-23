#include <cstdint>
#include <cstring>
// 1.13c Storm+0x21780: ADPCM encoder. Returns bytes written (or required size on overflow).
static const int32_t kAdj[32] = {-1,0,-1,4,-1,2,-1,6,-1,1,-1,5,-1,3,-1,7,-1,1,-1,5,-1,3,-1,7,-1,2,-1,4,-1,6,-1,8};
static const int32_t kStep[89] = {
 7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,73,80,88,97,107,118,130,143,
 157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,724,796,876,963,1060,1166,1282,1411,
 1552,1707,1878,2066,2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,
 10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,27086,29794,32767};
extern "C" uint32_t Storm_Sub21780(int16_t* out, uint32_t outSize, const int16_t* in, uint32_t ch, uint8_t shift, uint32_t inBytes) {
    if (outSize < 2) return 2;
    uint8_t* o = (uint8_t*)out;
    uint16_t hdr = (uint16_t)((uint8_t)(shift - 1) << 8);
    memcpy(o, &hdr, 2);
    uint32_t pos = 2 + ch * 2;
    if ((int32_t)pos > (int32_t)outSize) return pos;
    int32_t idx[2] = {0x2c, 0x2c}, last[2] = {0, 0};
    for (uint32_t i = 0; i < ch; i++) { memcpy(o + 2 + i * 2, in, 2); last[i & 1] = *in++; }
    int32_t budget = (int32_t)(inBytes / 2) - (int32_t)pos; if (budget < 0) budget = 0;
    uint32_t c = ch - 1, n = ch;
    while (n < inBytes >> 1) {
        if ((int32_t)outSize < (int32_t)(pos + 1)) return pos + 1;
        if (ch == 2) c = (c == 0);
        int32_t s = *in++;
        uint32_t d = s - last[c]; int32_t m = (int32_t)d >> 31; uint32_t ad = (d ^ m) - m;
        uint32_t step = kStep[idx[c]];
        uint8_t sign = (last[c] <= s) ? 0 : 0x40;
        if (ad < (step >> (shift & 31))) {
            if (idx[c]) idx[c]--;
            o[pos++] = 0x80;
        } else {
            while (step * 2 < ad && idx[c] < 0x58 && budget) {
                idx[c] += 8; if (idx[c] > 0x58) idx[c] = 0x58;
                step = kStep[idx[c]]; o[pos++] = 0x81; budget--;
            }
            uint32_t acc = step >> ((shift - 1) & 31), q = 0, bit = 1, lim = 1u << ((shift - 2) & 31), sum = 0;
            if (lim > 0x20) lim = 0x20;
            for (;;) {
                if (sum + step <= ad) { q |= bit; sum += step; }
                if (bit == lim) break;
                step >>= 1; bit <<= 1;
            }
            int32_t v = (int32_t)(sum + acc);
            if (!sign) { v = last[c] + v; if (v > 0x7fff) v = 0x7fff; }
            else { v = last[c] - v; if (v < -0x8000) v = -0x8000; }
            last[c] = v;
            o[pos++] = sign | (uint8_t)q;
            idx[c] += kAdj[q & 0x1f];
            if (idx[c] < 0) idx[c] = 0; else if (idx[c] > 0x58) idx[c] = 0x58;
        }
        n++;
    }
    return pos;
}
