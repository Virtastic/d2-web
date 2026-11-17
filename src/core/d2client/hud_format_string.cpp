// The browser's bar hover texts (string.tbl 4163 / 4164: "%u / %u" style lines) are formatted by the retail
// Unicode::sprintf (D2Lang+0xB250) over the string table's entry, not by a JavaScript replace.
#include <cstdint>
#include <cstring>
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
void D2Lang_Unicode_sprintf(int cap, char16_t* out, const char16_t* fmt, ...);
extern "C" const char* d2_lang_string_by_index(int index);

extern "C" {
// Formats string.tbl entry `id` with two machine-word arguments (the first %u/%d takes a, the second b) into out as UTF-8.
// cap: size of out in bytes. Returns the byte length written (0 when the entry is absent or out is too small).
EMSCRIPTEN_KEEPALIVE int32_t d2_client_format_string(int32_t id, uint32_t a, uint32_t b, char* out, int32_t cap) {
    char16_t fmt[256], buf[200];   // retail's own hover buffer is 100 wide characters
    const uint8_t* s = reinterpret_cast<const uint8_t*>(d2_lang_string_by_index(id & 0xffff));
    if (!s || cap < 2) return 0;
    uint32_t n = 0;
    while (*s && n < 254) {
        uint32_t c = *s++;
        if (c >= 0xf0) { c = (c & 7) << 18; for (int i = 0; i < 3 && *s; ++i) c |= (uint32_t(*s++) & 0x3f) << (12 - 6 * i); }
        else if (c >= 0xe0) { c = (c & 0xf) << 12; for (int i = 0; i < 2 && *s; ++i) c |= (uint32_t(*s++) & 0x3f) << (6 - 6 * i); }
        else if (c >= 0xc0) { c = (c & 0x1f) << 6; if (*s) c |= uint32_t(*s++) & 0x3f; }
        fmt[n++] = c > 0xffff ? char16_t(0xfffd) : char16_t(c);
    }
    fmt[n] = 0;
    std::memset(buf, 0, sizeof buf);
    D2Lang_Unicode_sprintf(100, buf, fmt, (uintptr_t)a, (uintptr_t)b);
    int32_t o = 0;
    for (const char16_t* p = buf; *p; ++p) {
        const uint32_t c = *p;
        const int32_t need = c < 0x80 ? 1 : c < 0x800 ? 2 : 3;
        if (o + need + 1 > cap) { out[0] = 0; return 0; }
        if (need == 1) out[o++] = char(c);
        else if (need == 2) { out[o++] = char(0xc0 | (c >> 6)); out[o++] = char(0x80 | (c & 0x3f)); }
        else { out[o++] = char(0xe0 | (c >> 12)); out[o++] = char(0x80 | ((c >> 6) & 0x3f)); out[o++] = char(0x80 | (c & 0x3f)); }
    }
    out[o] = 0;
    return o;
}
}
