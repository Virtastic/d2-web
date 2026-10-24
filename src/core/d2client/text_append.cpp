// Real extern "C" forms of D2Client+0x2CF20 and +0x2D9B0 (string-append helpers of the skill formatter).
// D2Lang #10003 is the env pointer; the logic is the retail decompile's (EBX = destination).
#include <cstdio>
#include <emscripten.h>
#include "text_append.h"

static void cat(char16_t* d, const char16_t* s) {
    if (!d || !s) return;
    while (*d) ++d;
    while ((*d++ = *s++)) {}
}
static void cat_ascii(char16_t* d, const char* s) {   // Unicode::toUnicode of an ASCII number, 32-unit buffer
    char16_t w[32]{};
    for (int i = 0; s[i] && i < 31; ++i) w[i] = (unsigned char)s[i];
    cat(d, w);
}

// 1.13c D2Client+0x2CF20 (ret 0xc): stack lo, hi, key. (0,0) -> 0. lo==hi!=0 -> "%d", else "%d-%d".
// Append text(0x10e2), text(key), number, text(0x10be), text(0xF9E); return 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2cf20(char16_t* dst, int32_t lo, int32_t hi, uint32_t key, const D2ClientTextEnv* e) {
    char a[32];
    if (lo == 0) { if (hi == 0) return 0; std::snprintf(a, sizeof a, "%d-%d", lo, hi); }
    else if (lo == hi) std::snprintf(a, sizeof a, "%d", lo);
    else std::snprintf(a, sizeof a, "%d-%d", lo, hi);
    cat(dst, e->lang(0x10e2));
    cat(dst, e->lang(key));
    cat_ascii(dst, a);
    cat(dst, e->lang(0x10be));
    cat(dst, e->lang(0xF9E));
    return 1;
}

// 1.13c D2Client+0x2D9B0 (ret 0x10): stack key1, key2, value, plus. value 0 -> 0. "+%d" if plus && value>=0
// else "%d". Append text(key1), number, text(key2), text(0xF9E); return 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2d9b0(char16_t* dst, uint32_t key1, uint32_t key2, int32_t value, uint32_t plus, const D2ClientTextEnv* e) {
    if (!value) return 0;
    char a[32];
    std::snprintf(a, sizeof a, (plus && value >= 0) ? "+%d" : "%d", value);
    cat(dst, e->lang(key1));
    cat_ascii(dst, a);
    cat(dst, e->lang(key2));
    cat(dst, e->lang(0xF9E));
    return 1;
}

// Unicode::sprintf(0x100, buf, fmt, value), one int argument. ponytail: handles %d %i %u %% (flags/width
// skipped); other conversions are copied literally. Upgrade when a format string uses more.
static void wfmt1(char16_t* buf, const char16_t* f, int32_t v) {
    uint32_t n = 0;
    auto put = [&](char16_t c) { if (n < 255) buf[n++] = c; };
    for (; f && *f; ++f) {
        if (*f != u'%') { put(*f); continue; }
        const char16_t* g = f + 1;
        while (*g == u'-' || *g == u'+' || *g == u' ' || *g == u'#' || (*g >= u'0' && *g <= u'9')) ++g;
        if (*g == u'%') { put(u'%'); f = g; }
        else if (*g == u'd' || *g == u'i' || *g == u'u') {
            char t[16];
            std::snprintf(t, sizeof t, *g == u'u' ? "%u" : "%d", (unsigned)v);
            for (char* c = t; *c; ++c) put((unsigned char)*c);
            f = g;
        } else put(*f);
    }
    buf[n] = 0;
}

// 1.13c D2Client+0x2C9F0 (ret 8): key==0x1506 (16-bit) -> 0. buf = sprintf(text(key), value); append buf,
// text(0xF9E); return 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2c9f0(char16_t* dst, uint32_t key, int32_t value, const D2ClientTextEnv* e) {
    if ((uint16_t)key == 0x1506) return 0;
    char16_t buf[256]{};
    wfmt1(buf, e->lang(key), value);
    cat(dst, buf);
    cat(dst, e->lang(0xF9E));
    return 1;
}

// 1.13c D2Client+0x2CB60 (ret 8): either 16-bit arg == 0x1506 -> 0. Append text(a), 0xF9D, 0xF9B, text(b), 0xF9E.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2cb60(char16_t* dst, uint32_t a, uint32_t b, const D2ClientTextEnv* e) {
    if ((uint16_t)a == 0x1506 || (uint16_t)b == 0x1506) return 0;
    cat(dst, e->lang(a)); cat(dst, e->lang(0xF9D)); cat(dst, e->lang(0xF9B));
    cat(dst, e->lang(b)); cat(dst, e->lang(0xF9E));
    return 1;
}

static uint32_t signed_num(char16_t* dst, uint32_t key, int32_t value, uint32_t plus, bool numFirst, const D2ClientTextEnv* e) {
    if (!value) return 0;
    char a[32];
    std::snprintf(a, sizeof a, (plus && value >= 0) ? "+%d" : "%d", value);
    if (numFirst) cat_ascii(dst, a);
    cat(dst, e->lang(key));
    if (!numFirst) cat_ascii(dst, a);
    cat(dst, e->lang(0xF9E));
    return 1;
}
// 1.13c D2Client+0x2DBF0 (ret 0xc): value 0 -> 0. Append number, text(key), text(0xF9E); return 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2dbf0(char16_t* dst, uint32_t key, int32_t value, uint32_t plus, const D2ClientTextEnv* e) { return signed_num(dst, key, value, plus, true, e); }
// 1.13c D2Client+0x2DCA0 (ret 0xc): value 0 -> 0. Append text(key), number, text(0xF9E); return 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2dca0(char16_t* dst, uint32_t key, int32_t value, uint32_t plus, const D2ClientTextEnv* e) { return signed_num(dst, key, value, plus, false, e); }

// 1.13c D2Client+0x2DD50 (ret 8): key==0x1506 -> append text(0x1506), text(0xF9E) to dst. Else
// buf = sprintf(text(key), value) + text(0xF9E), appended to dst. Return 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2dd50(char16_t* dst, uint32_t key, int32_t value, const D2ClientTextEnv* e) {
    if ((uint16_t)key == 0x1506) {
        cat(dst, e->lang(0x1506)); cat(dst, e->lang(0xF9E));
        return 1;
    }
    char16_t buf[256]{};
    wfmt1(buf, e->lang(key), value);
    cat(buf, e->lang(0xF9E));
    cat(dst, buf);
    return 1;
}

// 1.13c D2Client+0x2DA70 (ret 0x10): stack dst, strA, strB, plus; EBX = value. value 0 -> 0. Append number,
// text(value==1 ? strA : strB), text(0xF9E); return 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2da70(char16_t* dst, int32_t value, uint32_t strA, uint32_t strB, uint32_t plus, const D2ClientTextEnv* e) {
    return signed_num(dst, value == 1 ? strA : strB, value, plus, true, e);
}
