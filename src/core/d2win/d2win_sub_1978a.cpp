// D2Win 0x1978A (1.13c) -- decimal digit string -> 96-bit x87-extended-style value (CRT strtod helper)
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
// Returns the EAX retail leaves: AX is the exponent word it stores at out+10. The high half is what the last writer of EAX left:
// ___shl_12 (mov eax,[esp+4]) leaves the out pointer, so its high word; the 16-bit shift loop leaves out[0] << 16 (taken before
// that pass); ___add_12 leaves its carry (0); with no digits, EAX still holds n.
extern "C" uint32_t d2win_sub_1978a_eax(const char* digits, int n, uint32_t* out) {  // 1.13c D2Win+0x1978A
    int16_t e = 0x404e;
    uint32_t hi = (uint32_t)n >> 16;
    out[0] = out[1] = out[2] = 0;
    if (n) hi = 0;
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
            hi = out[0] & 0xFFFF;
            out[0] <<= 16;
        } while (u == 0);
        out[2] = u;
    }
    while (!(out[2] & 0x8000)) { shl96(out); e--; hi = (uint32_t)(uintptr_t)out >> 16; }
    *(int16_t*)((char*)out + 10) = e;
    return (hi << 16) | (uint16_t)e;
}
extern "C" void d2win_sub_1978a(const char* digits, int n, uint32_t* out) { d2win_sub_1978a_eax(digits, n, out); }
