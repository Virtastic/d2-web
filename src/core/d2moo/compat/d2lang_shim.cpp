// Compat: the D2Lang surface the vendored loaders use, over our string tables (src/core/lang).
// Unicode is D2Lang's UCS-2 char class; only the static string helpers the loaders call are here.
// The returned Unicode* strings live as long as the tables do (they point into LangTables).
#include <D2Common.h>
#include <D2Unicode.h>
#include <D2Lang.h>
#include <D2BitManip.h>
#include <unordered_map>
#include <charconv>
#include <cstdlib>
#include <cstring>
#include <Fog.h>
#include "../../lang/strtable.h"
#include "../../tables/shared.h"

static_assert(sizeof(Unicode) == 2, "Unicode is one UCS-2 unit");

Unicode::Unicode(unsigned short c) : ch(c) {}
// 1.13c D2Lang+0x11C0 Unicode default constructor closure: *(u16*)this = 0 (this = Unicode(0))
static inline void Unicode_default_ctor_closure(Unicode* t) { *t = Unicode(0); }
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
// 1.13c D2Lang+0xB0B0 (#10040): length in units; 0 for NULL.
int __fastcall Unicode::strlen(const Unicode* s) { int n = 0; if (s) while (s[n].ch) n++; return n; }
// 1.13c D2Lang+0xB170 (#10035): first unit equal to ch; NULL if absent or if the match is the terminator.
Unicode* __fastcall Unicode::strchr(const Unicode* s, Unicode c) {
    for (; s->ch; s++) if (s->ch == c.ch) return const_cast<Unicode*>(s);
    return nullptr;
}
// 1.13c D2Lang+0xB200 (#10036): unsigned unit compare, -1/0/1.
int __fastcall Unicode::strcmp(const Unicode* a, const Unicode* b) {
    while (a->ch && b->ch && a->ch == b->ch) { a++; b++; }
    return a->ch < b->ch ? -1 : a->ch > b->ch;
}
// 1.13c D2Lang+0xB1B0 (#10042): as strcmp over at most count units; 0 when count runs out.
int __fastcall Unicode::strncmp(const Unicode* a, const Unicode* b, size_t n) {
    while (a->ch && b->ch && n) { if (a->ch != b->ch) return a->ch < b->ch ? -1 : 1; a++; b++; n--; }
    return 0;
}
// 1.13c D2Lang+0xB610 (#10039): strcmp through the toUpper table; a shorter prefix sorts first.
int __fastcall Unicode::stricmp(const Unicode* a, const Unicode* b) {
    for (;; a++, b++) {
        if (!a->ch) return b->ch ? -1 : 0;
        if (!b->ch) return 1;
        unsigned short x = a->toUpper().ch, y = b->toUpper().ch;
        if (x != y) return x < y ? -1 : 1;
    }
}
// 1.13c D2Lang+0xB560 (#10045): stricmp over at most count units; 0 when count runs out.
int __fastcall Unicode::strnicmp(const Unicode* a, const Unicode* b, size_t n) {
    for (; n && a->ch && b->ch; a++, b++, n--) {
        unsigned short x = a->toUpper().ch, y = b->toUpper().ch;
        if (x != y) return x < y ? -1 : 1;
    }
    return 0;
}
// 1.13c D2Lang+0xB120 (#10041): append at most count units of src to dest, always NUL-terminate.
Unicode* __fastcall Unicode::strncat(Unicode* dest, const Unicode* src, int count) {
    Unicode* d = dest; while (d->ch) d++;
    for (; count && src->ch; count--) *d++ = *src++;
    d->ch = 0;
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
// 1.13c D2Lang+0xA8A0 (wrapper over 0xA7B0: returns its index result, string via out slot)
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

// 1.13c D2Lang+0x9450 (#10003): the string a table index names (the inverse of D2LANG_GetTblIndex); "" when absent.
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

// Prior art used: D2MOO D2StrTable.cpp STRTABLE_GetLanguage (archive-backed,
// once-only locale); the 1.13c GetLocale body and immutable locale descriptors.
#include <D2StrTable.h>
#include <Archive.h>

// 1.13c D2Lang+0x09290
D2C_Language STRTABLE_GetLanguage() {
    static bool initialized = false;
    static D2C_Language language = LANGUAGE_ENGLISH;
    if (!initialized) {
        // Retail publishes this flag before reading, including on a failed read.
        initialized = true;
        auto* bytes = static_cast<unsigned char*>(
            ARCHIVE_ALLOC_BUFFER_AND_READ_FILE_TO_IT(nullptr, "data\\local\\use", nullptr));
        // Retail dereferences this result unconditionally. Missing locale data
        // cannot silently turn into an English game.
        if (!bytes) std::abort();
        const unsigned value = *bytes;
        std::free(bytes);
        language = static_cast<D2C_Language>(value <= 12 ? value : 0);
    }
    return language;
}

// 1.13c D2Lang+0x09D00
void __stdcall STRTABLE_GetLocale(char* destination, int field) {
    struct Locale { const char* strings[3]; const char* code; };
    static const Locale locales[13] = {
        {{"ENG", "ENG", "USA"}, "SUne"},
        {{"ESP", "ESP", "ESP"}, "SEse"},
        {{"DEU", "DEU", "DEU"}, "EDed"},
        {{"FRA", "FRA", "FRA"}, "RFrf"},
        {{"POR", "POR", "BRA"}, "RBtp"},
        {{"ITA", "ITA", "ITA"}, "TIti"},
        {{"JPN", "ENG", "JPN"}, "AJaj"},
        {{"KOR", "ENG", "HAN"}, "RKok"},
        {{"SIN", "ENG", "SIN"}, "NChz"},
        {{"CHI", "ENG", "CHI"}, "WThz"},
        {{"POL", "POL", "POL"}, "LPlp"},
        {{"RUS", "RUS", "RUS"}, "URur"},
        {{"ENG", "ENG", "HAN"}, "RKok"},
    };
    const auto& locale = locales[static_cast<unsigned>(STRTABLE_GetLanguage())];
    // Retail's table scan matches the identity language ids 0..12. The fatal
    // first-row-id guard is unreachable: the immutable English row has id 0.
    if (field == 3) std::memcpy(destination, locale.code, 4);
    else std::strcpy(destination, locale.strings[field == 1 ? 1 : field == 2 ? 2 : 0]);
}

// Prior art: D2MOO D2StrTable.cpp STR_GroupIntDigits/GroupCStrDigits. Checked
// against the cached 1.13c body; the strict capacity comparison is intentional.
// 1.13c D2Lang+0x9BC0
void __stdcall STR_GroupIntDigits(Unicode* destination, int value, int capacity) {
    if (capacity < 3) {
        FOG_DisplayAssert("nMaxLength > 2", "D2Lang\\D2StrTable.cpp", 0x4c7);
        std::exit(-1);
    }
    if (value < 0) {
        *destination++ = Unicode('-');
        --capacity;
        // x86 neg wraps at INT_MIN. The following signed decimal conversion
        // consequently retains a second minus sign in that one retail case.
        const uint32_t magnitude = 0u - static_cast<uint32_t>(value);
        std::memcpy(&value, &magnitude, sizeof(value));
    }
    char digits[64];
    const auto result = std::to_chars(digits, digits + sizeof(digits), value);
    const int count = static_cast<int>(result.ptr - digits);
    int output = count + (count - 1) / 3;
    if (output >= capacity - 1) {
        destination[0] = Unicode('*');
        destination[1] = Unicode(0);
        return;
    }
    destination[output] = Unicode(0);
    int group = 0;
    for (int input = count; input > 0;) {
        destination[--output] = Unicode(static_cast<unsigned char>(digits[--input]));
        if (++group == 3 && count > 3 && output > 0) {
            group = 0;
            destination[--output] = Unicode(',');
        }
    }
}


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

// 1.13c D2Lang+0x09E20 (#10005): release one init reference; the last one frees every
// table/list (Fog free, ordinal 10043) and unloads the system map. Our tables live in
// static storage, so only the reference count is modelled. Stateful: NAMED, no vector.
static int g_d2langInitRefs = 0;
void __stdcall D2LANG_Release() {
    if (g_d2langInitRefs != 0 && --g_d2langInitRefs == 0) {
        // no heap tables to free here
    }
}
// 1.13c D2Lang+0x8320 Unicode::win2Unicode: widen bytes, NUL-terminate; truncation overwrites the last unit
Unicode* __fastcall Unicode::win2Unicode(Unicode* dst, const char* src, int n) {
    uint16_t* d = (uint16_t*)dst;
    int i = 0;
    for (; i < n && src[i]; i++) d[i] = (uint8_t)src[i];
    if (src[i]) d[i - 1] = 0; else d[i] = 0;
    return dst;
}
// 1.13c D2Lang+0x8520 (ESI=wide str, stack=narrow set; caller Personalize): true when the wide string's last unit
// equals any of the first 19 narrow chars. Custom register convention: NAMED, no vector.
static bool d2lang_lastCharIn(const uint16_t* w, const char* set) {
    int n = 0;
    while (n < 19 && set[n]) n++;
    int wl = 0;
    if (w) while (w[wl]) wl++;
    if (wl == 0) return false;  // retail reads w[-1] here
    for (int i = 0; i < n; i++) if (w[wl - 1] == (uint8_t)set[i]) return true;
    return false;
}

// 1.13c D2Lang+0x0AC40 Unicode::unloadSysMap: delete[] the three sysmap tables (0x6fc10a58, 0a50, 0a5c), null each.
// Stateful, no-op here (the shim holds no sysmap tables).
