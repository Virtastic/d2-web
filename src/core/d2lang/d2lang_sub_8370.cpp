// 1.13c D2Lang+0x08370: Unicode::utf8ToUnicode(dst, utf8, cap).
// Decodes strlen+1 bytes into a 0x3ff-wchar scratch via the D2Lang+0x7AD0 decoder (seam), then copies
// up to cap wchars stopping at NUL; always NUL-terminates dst (overwriting dst[cap-1] when truncated).
#include <cstdint>

struct D2LangU8Env {
    // D2Lang+0x7AD0: decode src[0..srclen) into out[0..outcap); returns wchar count written, or <0 on failure.
    int (*decode)(uint16_t* out, int outcap, const char* src, int srclen);
};

extern "C" uint16_t* d2lang_sub_8370(uint16_t* dst, const char* utf8, int cap, const D2LangU8Env* e) {
    uint16_t buf[0x400];
    int n = 0;
    while (utf8[n++]) {}                       // length incl. NUL
    int cnt = e->decode(buf, 0x3ff, utf8, n);
    if (cnt < 0) cnt = 0;                      // failure: empty string
    buf[cnt] = 0;
    int i = 0;
    while (i < cap && buf[i]) { dst[i] = buf[i]; i++; }
    if (buf[i] == 0) dst[i] = 0; else dst[i - 1] = 0;
    return dst;
}
