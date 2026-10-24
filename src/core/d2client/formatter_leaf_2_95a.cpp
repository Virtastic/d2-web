// Real extern "C" forms of D2Client 0x2C7E0, 0x2E6E0, 0x2F520, 0x2F600, 0x2F6E0 (formatter 0x316C0 callees).
// Register args are explicit; the only env is the D2Lang #10003 pointer (D2ClientTextEnv), as in text_append.cpp.
#include <cstdio>
#include <cstring>
#include <emscripten.h>
#include "text_append.h"

static void cat(char16_t* d, const char16_t* s) { if (!d || !s) return; while (*d) ++d; while ((*d++ = *s++)) {} }
static void cat_num(char16_t* d, const char* a) { char16_t w[32]{}; for (int i = 0; a[i] && i < 31; ++i) w[i] = (unsigned char)a[i]; cat(d, w); }

// 1.13c D2Client+0x2C7E0: ECX=thisValue, EAX=value, stack dst (32 units). "%d" if this==0 or value<0 else "+%d". Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2c7e0(uint32_t thisValue, int32_t value, char16_t* dst) {
    char a[32]; std::snprintf(a, sizeof a, (thisValue == 0 || value < 0) ? "%d" : "+%d", value);
    int i = 0; for (; a[i] && i < 31; ++i) dst[i] = (unsigned char)a[i];
    dst[i] = 0;
    return 1;
}

// 1.13c D2Client+0x2E6E0: ECX=level, EAX=classIdx, EDX=dst. rows/rowCount = *(sgptDataTables)+0xb98/+0xba0 (stride 0x23c).
// level<=1 -> 1. v = (level-2)*rec[0x14c] + rec[0x148] (0 when classIdx out of range). 2DBF0(0x10d5,v,0); text(0x110D); text(0xF9E).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2e6e0(int32_t level, int32_t classIdx, char16_t* dst, const uint8_t* rows, int32_t rowCount, const D2ClientTextEnv* e) {
    if (level <= 1) return 1;
    int32_t v = 0;
    if (classIdx >= 0 && classIdx < rowCount && rows) {
        const uint8_t* r = rows + (uint32_t)classIdx * 0x23c;
        int32_t a, b; std::memcpy(&a, r + 0x148, 4); std::memcpy(&b, r + 0x14c, 4);
        v = (level - 2) * b + a;
    }
    d2_client_2dbf0(dst, 0x10d5, v, 0, e);
    cat(dst, e->lang(0x110D));
    cat(dst, e->lang(0xF9E));
    return 1;
}

static void range_txt(char16_t* num, int32_t lo, int32_t hi, int32_t plus) {
    char a[48]; std::snprintf(a, sizeof a, plus ? "+%d-%d" : "%d-%d", lo, hi);
    cat_num(num, a);
}

// 1.13c D2Client+0x2F520: EAX=dst. lo==hi -> 2DCA0(dst,key,lo,plus). Else text(key), [text(0xF9B) if extra], "[+]lo-hi", text(0xF9E).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2f520(char16_t* dst, uint32_t key, int32_t lo, int32_t hi, int32_t plus, int32_t extra, const D2ClientTextEnv* e) {
    if (lo == hi) return d2_client_2dca0(dst, key, lo, (uint32_t)plus, e);
    char16_t n[32]{}; range_txt(n, lo, hi, plus);
    cat(dst, e->lang(key));
    if (extra) cat(dst, e->lang(0xF9B));
    cat(dst, n);
    cat(dst, e->lang(0xF9E));
    return 1;
}

// 1.13c D2Client+0x2F600: EAX=dst. lo==hi -> 2D9B0(dst,k1,k2,lo,0). Else text(k1), text(k2), "[+]lo-hi", text(0xF9E).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2f600(char16_t* dst, uint32_t k1, uint32_t k2, int32_t lo, int32_t hi, int32_t plus, const D2ClientTextEnv* e) {
    if (lo == hi) return d2_client_2d9b0(dst, k1, k2, lo, 0, e);
    char16_t n[32]{}; range_txt(n, lo, hi, plus);
    cat(dst, e->lang(k1)); cat(dst, e->lang(k2));
    cat(dst, n);
    cat(dst, e->lang(0xF9E));
    return 1;
}

// 1.13c D2Client+0x2F6E0: as 2F600 but text(k1) first, text(k2) after the number; each skipped when its low word is 0x1506.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2f6e0(char16_t* dst, uint32_t k1, uint32_t k2, int32_t lo, int32_t hi, int32_t plus, const D2ClientTextEnv* e) {
    if (lo == hi) return d2_client_2d9b0(dst, k1, k2, lo, 0, e);
    char16_t n[32]{}; range_txt(n, lo, hi, plus);
    if ((uint16_t)k1 != 0x1506) cat(dst, e->lang(k1));
    cat(dst, n);
    if ((uint16_t)k2 != 0x1506) cat(dst, e->lang(k2));
    cat(dst, e->lang(0xF9E));
    return 1;
}
