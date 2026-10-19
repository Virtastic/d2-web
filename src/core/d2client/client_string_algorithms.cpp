#include "client_string_algorithms.h"
#include <cstddef>

namespace d2::client {
namespace {
// D2Lang UCS-2 helpers, cross-checked with D2MOO D2LangTests and our
// d2lang_shim.cpp: empty substrings do not match; strncpy does not add a
// terminator when it copies exactly count units. Locals start zero-filled.
std::size_t length(const char16_t* s) {
    if (!s) return 0;
    std::size_t n = 0;
    while (s[n]) ++n;
    return n;
}
void copy(char16_t* dest, const char16_t* src) {
    do { *dest++ = *src; } while (*src++);
}
const char16_t* find(const char16_t* text, const char16_t* needle) {
    if (!text || !needle || !*needle) return nullptr;
    for (; *text; ++text) {
        const char16_t* a = text;
        const char16_t* b = needle;
        while (*b && *a == *b) { ++a; ++b; }
        if (!*b) return text;
    }
    return nullptr;
}
}

// 1.13c D2Client+0x8E600
void sub_8E600(char16_t* text, std::uint32_t token, const char16_t* replacement) {
    char16_t assembled[400]{};
    const auto count = length(text);
    if (count < 2) return;
    for (std::size_t i = 0; i < count - 1; ++i) {
        if (text[i] != u'%' || std::uint32_t(text[i + 1]) != token + 0x30u) {
            assembled[i] = text[i];
            continue;
        }
        copy(assembled + i, replacement);
        const auto end = length(assembled);
        auto suffix = i + 2;
        // Retail requires a nonempty assembled prefix here (it reads end-1).
        if (assembled[end - 1] == u' ' && text[suffix] == u' ') ++suffix;
        copy(assembled + end, text + suffix);
        copy(text, assembled);
        return;
    }
}

// 1.13c D2Client+0x8E6D0
void sub_8E6D0(char16_t* text, const char16_t* marker) {
    const auto match = find(text, marker);
    if (!match) return;
    const auto start = match + 4; // fixed eight-byte skip, independent of marker length
    const char16_t bracket[] = u"[";
    const auto stop = find(start, bracket);
    const auto count = length(start) - length(stop);
    char16_t extracted[100]{};
    for (std::size_t i = 0; i < count; ++i) extracted[i] = start[i];
    copy(text, extracted);
}
}
