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
Unicode* Unicode::strcpy(Unicode* dest, const Unicode* src) { Unicode* d = dest; while ((*d++ = *src++).ch) {} return dest; }
Unicode* Unicode::strcat(Unicode* dest, const Unicode* src) { Unicode* d = dest; while (d->ch) d++; strcpy(d, src); return dest; }
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
