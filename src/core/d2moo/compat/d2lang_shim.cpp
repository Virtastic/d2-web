// Compat: the D2Lang surface the vendored loaders use, over our string tables (src/core/lang).
// Unicode is D2Lang's UCS-2 char class; only the static string helpers the loaders call are here.
// The returned Unicode* strings live as long as the tables do (they point into LangTables).
#include <D2Common.h>
#include <D2Unicode.h>
#include <D2Lang.h>
#include <D2BitManip.h>
#include <unordered_map>
#include "../../lang/strtable.h"
#include "../../tables/shared.h"

static_assert(sizeof(Unicode) == 2, "Unicode is one UCS-2 unit");

Unicode::Unicode(unsigned short c) : ch(c) {}
Unicode::operator unsigned short() const { return ch; }
Unicode& Unicode::operator=(const Unicode& o) { ch = o.ch; return *this; }
// 1.13c D2Lang+0x1040/0x1050/0x1070/0x10A0/0x10C0
static_assert(sizeof(Unicode) == 2, "Unicode is one 16-bit char at +0");
BOOL Unicode::isNewline() const { return ch == 10; }
BOOL Unicode::isPipe() const { return ch == 0xFF; }
BOOL Unicode::isASCII() const { return ch < 0x80; }
// 1.13c D2Lang+0x1080 isAlpha, +0x1160 isWordEnd: C-locale ASCII classes
static bool d2lang_alnum(uint16_t c) { return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
BOOL Unicode::isAlpha() const { return ch < 0x80 && ((ch | 0x20) >= 'a' && (ch | 0x20) <= 'z'); }
BOOL Unicode::isWordEnd(const Unicode* str, size_t index) { return index != 0 && d2lang_alnum(str[index].ch) && !d2lang_alnum(str[index + 1].ch); }
// retail's 256-entry tables: Windows-1252 style Latin-1 case map; toLower shares the Latin-1 upper-casing (vanilla bug)
static unsigned short latin1Upper(unsigned short c) {
    if (c == 0x9A || c == 0x9C) return c - 0x10;
    if ((c >= 0xE0 && c <= 0xF6) || (c >= 0xF8 && c <= 0xFE)) return c - 0x20;
    return c == 0xFF ? 0x9F : c;
}
Unicode Unicode::toUpper() const { return ch >= 0x100 ? *this : Unicode(ch >= 'a' && ch <= 'z' ? ch - 0x20 : latin1Upper(ch)); }
Unicode Unicode::toLower() const { return ch >= 0x100 ? *this : Unicode(ch >= 'A' && ch <= 'Z' ? ch + 0x20 : latin1Upper(ch)); }
// 1.13c D2Lang+0x10E0/0x1120/0x1020: case-folded equality via the toUpper table; isWhitespace = C-locale isspace (retail returns the CRT _SPACE mask, 8)
BOOL Unicode::compare(Unicode o) const { return toUpper().ch == o.toUpper().ch; }
BOOL __fastcall Unicode::compare(Unicode l, Unicode r) { return l.toUpper().ch == r.toUpper().ch; }
BOOL Unicode::isWhitespace() const { return ch == 32 || (ch >= 9 && ch <= 13); }
Unicode* Unicode::strcpy(Unicode* dest, const Unicode* src) { Unicode* d = dest; while ((*d++ = *src++).ch) {} return dest; }
Unicode* Unicode::strcat(Unicode* dest, const Unicode* src) { Unicode* d = dest; while (d->ch) d++; strcpy(d, src); return dest; }
// 1.13c D2Lang+0xB020 (#10046): first occurrence of substr in str; NULL for a NULL arg, an empty substr, or no match.
Unicode* Unicode::strstr(const Unicode* str, const Unicode* substr) {
    if (!str || !substr || !substr->ch) return nullptr;
    for (; str->ch; str++) {
        const Unicode *a = str, *b = substr;
        while (b->ch && a->ch == b->ch) { a++; b++; }
        if (!b->ch) return const_cast<Unicode*>(str);
    }
    return nullptr;
}
// 1.13c D2Lang+0xB0D0 (#10044): copy up to count units, NUL-pad the rest of count, return dest.
Unicode* Unicode::strncpy(Unicode* dest, const Unicode* src, int count) {
    int i = 0;
    for (; i < count && src[i].ch; i++) dest[i] = src[i];
    for (; i < count; i++) dest[i] = Unicode(0);
    return dest;
}
Unicode* Unicode::toUnicode(Unicode* dest, const char* src, int count) {
    std::u16string u = d2::utf8ToU16(src ? src : "");
    int n = (int)u.size(); if (n > count - 1) n = count - 1; if (n < 0) n = 0;
    for (int i = 0; i < n; i++) dest[i] = Unicode((unsigned short)u[i]);
    dest[n] = Unicode(0);
    return dest;
}

// D2LANG_GetTblIndex (#10013): index the string table assigns the key (0 when absent), and the
// string itself through pUnicode. D2LANG_GetStringByReferenceString (#10003): the string or NULL.
short D2LANG_GetTblIndex(char* szReference, const Unicode** pUnicode) {
    const std::u16string* v = nullptr;
    uint16_t idx = d2::sharedLang().tblIndex(szReference ? szReference : "", &v);
    if (pUnicode) *pUnicode = v ? (const Unicode*)v->c_str() : nullptr;
    return (short)idx;
}
// A missing key resolves to the key followed by D2Lang's placeholder; an empty key to "".
const Unicode* D2LANG_GetStringByReferenceString(char* szReference) {
    static std::unordered_map<std::string, std::u16string> fallbacks;
    std::string key(szReference ? szReference : "");
    const std::u16string* v = d2::sharedLang().find(key);
    if (v) return (const Unicode*)v->c_str();
    auto it = fallbacks.find(key);
    if (it == fallbacks.end()) it = fallbacks.emplace(key, key.empty() ? std::u16string() : d2::utf8ToU16(key) + u" -not xlated call ken ").first;
    return (const Unicode*)it->second.c_str();
}

// D2Lang #10004: the string a table index names (the inverse of D2LANG_GetTblIndex); "" when absent.
const Unicode* D2LANG_GetStringFromTblIndex(short index) {
    static const std::u16string empty;
    const std::u16string* v = d2::sharedLang().byTblIndex((uint16_t)index);
    return (const Unicode*)(v ? v : &empty)->c_str();
}
// 1.13c D2Lang+0x82F0 (#10055): each unit narrowed to its low byte, at most count-1 units, then NUL.
char* Unicode::unicode2Win(char* dest, const Unicode* src, int count) {
    char* p = dest;
    for (; count > 1 && src->ch; src++, count--) *p++ = (char)src->ch;
    *p = 0;
    return dest;
}

// D2Lang #10009 (1.13c): the string table language. English tables only.
#include <D2StrTable.h>
D2C_Language STRTABLE_GetLanguage() { return LANGUAGE_ENGLISH; }

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
// Replay hook for s3723: offset in units of the match, -1 for NULL.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_lang_strstr_b020(const uint16_t* h, const uint16_t* n) {
    const Unicode* r = Unicode::strstr((const Unicode*)h, (const Unicode*)n);
    return r ? (int32_t)(r - (const Unicode*)h) : -1;
}
// Replay hook for s3717: the unit read through operator unsigned short.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_lang_ushort_1010(const uint16_t* p) { return (unsigned short)*(const Unicode*)p; }
// Replay hook for s3726: returned pointer minus dst, in units.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_lang_strncpy_b0d0(uint16_t* d, const uint16_t* s, int32_t n) {
    return (int32_t)((uint16_t*)Unicode::strncpy((Unicode*)d, (const Unicode*)s, n) - d);
}
#endif
