// Local UTF-16 helpers for the D2Multi glue ports (retail calls D2Lang's Unicode:: members through its IAT; the ports must not link to those).
#pragma once
#include <cstdint>
#include <cstring>

static inline int32_t d2m_ulen(const uint16_t* s) { int32_t n = 0; while (s[n]) n++; return n; }
static inline void d2m_ucpy(uint16_t* d, const uint16_t* s) { while ((*d++ = *s++) != 0) {} }
static inline void d2m_ucat(uint16_t* d, const uint16_t* s) { d2m_ucpy(d + d2m_ulen(d), s); }
static inline uint16_t* d2m_ustr(uint16_t* h, const uint16_t* n) {
    int32_t nl = d2m_ulen(n);
    if (!nl) return h;
    for (; *h; h++) if (!memcmp(h, n, (size_t)nl * 2)) return h;
    return nullptr;
}
