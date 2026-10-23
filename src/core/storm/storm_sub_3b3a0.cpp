#include <cstdint>
#include <cstring>
void Storm_SetLastError(uint32_t); // seam: DAT_6fc45388 + SetLastError
// 1.13c Storm+0x3AFD0 (#506 SStrLen): strlen; null -> error 0x57, returns 0.
uint32_t Storm_Sub3AFD0(const char* s) {
    if (!s) { Storm_SetLastError(0x57); return 0; }
    return static_cast<uint32_t>(strlen(s));
}
// 1.13c Storm+0x3B3A0 (#504 SStrTokenize): quote-aware tokenizer. *str advances past the token and
// one delimiter; out receives up to size-1 chars, NUL-terminated; *quoted = 1 if a quote was seen.
void Storm_Sub3B3A0(char** str, char* out, char* size, const char* delims, uint32_t* quoted) {
    uint32_t sz = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(size));
    if (!(str && *str && delims && !(out == nullptr && size != nullptr))) { Storm_SetLastError(0x57); return; }
    bool q = strchr(delims, '"') != nullptr, inq = false;
    uint32_t sawq = 0;
    char* p = *str;
    for (char c = *p; c; c = *p) {                     // skip leading delimiters
        if (!strchr(delims, c)) break;
        if (q && c == '"') { inq = true; sawq = 1; ++p; break; }
        ++p;
    }
    uint32_t n = 0;
    char* w = out;
    for (char c = *p; c; ) {
        if (q && c == '"') {
            if (n && !inq) break;                      // quote ends token, left unconsumed
            inq = !inq; sawq = 1; ++p;
            if (!inq) break;
        } else {
            if (!inq && strchr(delims, c)) { ++p; break; }
            if (n + 1 < sz) { *w++ = c; ++n; }
            ++p;
        }
        c = *p;
    }
    if (n < sz) out[n] = 0;
    *str = p;
    if (quoted) *quoted = sawq;
}
