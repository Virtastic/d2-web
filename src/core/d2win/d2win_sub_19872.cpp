// src/core/d2win/d2win_sub_19872.cpp -- 1.13c D2Win.dll MSVC CRT decimal-string -> 80-bit extended
// (12-byte) parser, from the cached Ghidra decompile (build-wasm/ghidra-d2win/decomp/0x19872.c).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" void d2win_sub_1978A(const char* digits, int32_t n, uint32_t* out12);  // D2Win+0x1978A
extern "C" void d2win_sub_1A184(uint32_t* ext12, int32_t exp10, int32_t flag);    // D2Win+0x1A184

static inline bool dig(uint8_t c) { return c >= '0' && c <= '9'; }

// 1.13c D2Win+0x19872
// out12: 12-byte result (mantissa 8 bytes, exponent|sign u16 at +10). *endp: first unparsed char.
// p5/p6: exponent bias added when no exponent / no fraction was seen; allowExp: param_7 (accept e+-).
// Returns bit mask: 0 ok, 1 underflow, 2 overflow, 4 no digits. DAT_6f8ff984 (decimal point) taken as '.'.
// ponytail: endp is a native pointer (retail int*).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_19872(uint8_t* out12, const uint8_t** endp, const uint8_t* s,
                                              int32_t p4, int32_t p5, int32_t p6, int32_t allowExp) {
    const uint8_t DP = '.';
    int st = 0, sign = 1, expv = 0, nd = 0, dexp = 0;
    bool b1 = false, b2 = false, b3 = false;
    uint16_t sgn = 0;
    uint32_t ret = 0;
    char digits[40] = {0};
    char* w = digits;
    const uint8_t* save = s;
    uint8_t c;
    const uint8_t* n;
    uint8_t buf[12] = {0};

    while ((c = *s) == ' ' || c == 9 || c == 10 || c == 13) s++;
top:
    c = *s; n = s + 1;
    switch (st) {
    case 0:
        if (c > '0' && c <= '9') goto to3;
        if (c == DP) { st = 5; s = n; goto top; }
        st = 2;
        if (c == '+') { sgn = 0; s = n; goto top; }
        if (c == '-') { sgn = 0x8000; s = n; goto top; }
        break;
    case 1:
        st = 1; b1 = true;
        if (c > '0' && c <= '9') goto to3;
        if (c == DP) goto dot;
        if (c == '+' || c == '-') goto sg;
        s = n;
        if (c != '0') goto ex;
        goto top;
    case 2:
        if (c > '0' && c <= '9') goto to3;
        s = save;
        if (c == DP) { st = 5; s = n; goto top; }
        break;
    case 3:
        while (dig(c)) {
            if ((uint32_t)nd < 0x19) { nd++; *w++ = c - '0'; } else dexp++;
            c = *n++;
        }
        if (c != DP) goto after;
    dot:
        b1 = true; st = 4; s = n; goto top;
    case 4:
        b3 = true;
        if (nd == 0) while (c == '0') { dexp--; c = *n++; }
        while (dig(c)) {
            if ((uint32_t)nd < 0x19) { nd++; *w++ = c - '0'; dexp--; }
            c = *n++;
        }
    after:
        if (c == '+' || c == '-') {
    sg:
            b1 = true; st = 0xb; s = n - 1;
        } else {
    ex:
            b1 = true;
            if (c < 'D' || (c > 'E' && (c < 'd' || c > 'e'))) { s = n - 1; goto fin; }
            st = 6; s = n;
        }
        goto top;
    case 5:
        b3 = true;
        s = save;
        if (dig(c)) { st = 4; goto to3b; }
        goto fin;
    case 6:
        save = s - 1;
        if (c < '1' || c > '9') {
            if (c == '+') goto plus;
            if (c == '-') goto minus;
            s = save;
            if (c == '0') goto z8;
            goto fin;
        }
    to9:
        st = 9;
    to3b:
        s = n - 1; goto top;
    case 7:
        if (c > '0' && c <= '9') goto to9;
        s = save;
        if (c != '0') goto fin;
    z8:
        st = 8; s = n; goto top;
    case 8:
        b2 = true;
        while (c == '0') c = *n++;
        if (c > '0' && c <= '9') goto to9;
        s = n - 1; goto fin;
    case 9:
        b2 = true; expv = 0; goto accum;
    default:
        goto cd;
    case 0xb:
        if (allowExp) {
            save = s;
            if (c == '+') {
    plus:   st = 7; s = n;
            } else {
                if (c != '-') goto fin;
    minus:      sign = -1; st = 7; s = n;
            }
            goto top;
        }
        st = 10; n = s;
    cd:
        s = n;
        if (st != 10) goto top;
        goto fin;
    }
    if (c != '0') goto fin;
    st = 1; s = n; goto top;
to3:
    st = 3; s = n - 1; goto top;

accum:
    while (dig(c)) {
        expv = (int8_t)c - '0' + expv * 10;
        if (expv > 0x1450) { expv = 0x1451; break; }
        c = *n++;
    }
    while (dig(c)) c = *n++;
    s = n - 1;
fin:
    *endp = s;
    if (b1) {
        if ((uint32_t)nd > 0x18) {
            if (digits[23] > 4) digits[23]++;
            w--; dexp++; nd = 0x18;
        }
        if (nd != 0) {
            while (*--w == 0) { nd--; dexp++; }
            d2win_sub_1978A(digits, nd, (uint32_t*)buf);
            if (sign < 0) expv = -expv;
            int32_t e = expv + dexp;
            if (!b2) e += p5;
            if (!b3) e -= p6;
            if (e > 0x1450) {
                memset(buf, 0, 12);
                buf[6 + 3] = 0x80; buf[10] = 0xff; buf[11] = 0x7f;
                ret = 2;
                goto done;
            }
            if (e > -0x1451) { d2win_sub_1A184((uint32_t*)buf, e, p4); goto done; }
            ret = 1;
        }
    } else {
        ret = 4;
    }
    memset(buf, 0, 12);
done:
    uint16_t se; memcpy(&se, buf + 10, 2); se |= sgn; memcpy(buf + 10, &se, 2);
    memcpy(out12, buf, 12);
    return ret;
}
