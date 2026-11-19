// 1.13c Fog+0x2d40 _strrchr: last occurrence of (char)ch, the terminator included.
#include <cstdint>
extern "C" char* fog_strrchr_2d40(const char* s, int ch) {
    const char* r = nullptr;
    for (const char* p = s;; ++p) { if (*p == (char)ch) r = p; if (!*p) break; }
    return (char*)r;
}
