// 1.13c D2Win+0x1978A -- decimal digit string -> 96-bit x87-extended-style value (CRT strtod helper)
#include <cstdint>
static void shl96(uint32_t* p) {  // ___shl_12
    p[2] = (p[2] << 1) | (p[1] >> 31);
    p[1] = (p[1] << 1) | (p[0] >> 31);
    p[0] <<= 1;
}
static void add96(uint32_t* d, const uint32_t* s) {  // ___add_12
    uint64_t c = 0;
    for (int i = 0; i < 3; i++) { c += (uint64_t)d[i] + s[i]; d[i] = (uint32_t)c; c >>= 32; }
}
void d2win_sub_1978a(const char* digits, int n, uint32_t* out) {
    int16_t e = 0x404e;
    out[0] = out[1] = out[2] = 0;
    for (; n; n--) {
        uint32_t t[3] = {out[0], out[1], out[2]};
        shl96(out); shl96(out); add96(out, t); shl96(out);  // *10
        uint32_t d[3] = {(uint32_t)(int32_t)(int8_t)*digits++, 0, 0};  // signed char, as retail
        add96(out, d);
    }
    if (out[2] == 0) {  // retail loops forever on a zero value; mirrored
        uint32_t u;
        do {
            e -= 0x10;
            u = out[1] >> 16;
            out[1] = (out[0] >> 16) | (out[1] << 16);
            out[0] <<= 16;
        } while (u == 0);
        out[2] = u;
    }
    while (!(out[2] & 0x8000)) { shl96(out); e--; }
    *(int16_t*)((char*)out + 10) = e;
}
