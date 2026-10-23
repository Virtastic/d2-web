// 1.13c D2Win+0x06E86: CRT __convertcp. Converts a string between two code pages via UTF-16.
// Returns 0 when cp_from == cp_to or on failure; else the dest (caller's buf, or a malloc'd copy
// whose length is written back to *len when it was not -1).
// ponytail: wasm has no Win32 code pages; both pages are treated as the same single-byte (Latin-1)
// set, so the UTF-16 round trip is the identity on bytes.
#include <cstdint>
#include <cstdlib>
#include <cstring>

extern "C" char* d2win_convertcp_6e86(uint32_t cp_from, uint32_t cp_to, const char* src,
                                      uint32_t* len, char* out, int32_t out_cap) {
    if (cp_from == cp_to) return nullptr;
    uint32_t n = *len;
    if (n == 0xFFFFFFFFu) n = (uint32_t)strlen(src) + 1;
    if (out) {
        if (!n || (int32_t)n > out_cap) return nullptr;
        memcpy(out, src, n);
        return out;
    }
    if (!n) return nullptr;
    char* r = (char*)malloc(n);
    if (!r) return nullptr;
    memcpy(r, src, n);
    if (*len != 0xFFFFFFFFu) *len = n;
    return r;
}
