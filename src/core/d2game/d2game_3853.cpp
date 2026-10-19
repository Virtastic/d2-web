// src/core/d2game/d2game_3853.cpp -- 1.13c D2Game.dll+0x03853 ___tolower_mt (200 B), the MSVC 2003
// locale-aware single-character lower-case fold, transcribed from the cached Ghidra decompile
// (build-wasm/ghidra-d2game/decomp/0x3853.c). Stage A (OBJECTIVE v9): implementation + marker, no
// spec, no vector.
//
// Prior art: the same CRT routine in src/core/gfx/fog_batch_1400.cpp (Fog+0x184D) and
// src/core/d2client/crt_strings.cpp (D2Client+0x7604); the callers here are 0x2813 __stricmp (which
// reaches it through its D2GameBatch17Env::tolowerMt seam) and 0x391B.
//
// Signature at retail: __thiscall(this in ECX, locinfo on the stack, c on the stack). `this` is only
// forwarded to ___isctype_mt and used as the alias of the 3-byte stack output buffer for
// __crtLCMapStringA, so it is an opaque parameter here. The D2Game callees 0x6B8D (the
// __crtLCMapStringA wrapper) and 0x6F63 (___isctype_mt) are still UNMAPPED rows and are seams.
#include <cstdint>
#include <cstddef>

namespace {

// __crt_locale_data offsets the comments reads; only these fields are modelled.
struct D2GameCrtLocale {
    uint32_t flags;            // +0x04  locale flags word, the 7th __crtLCMapStringA argument
    uint32_t lcid;             // +0x14  0 = C locale
    uint32_t mbcs;             // +0x24  nonzero when a multibyte code page is active
    uint32_t mbcurmax;         // +0x28  1 for a single-byte code page
    const uint8_t* ctype;      // +0x48  two-byte entries: [0] type bits, [1] lead-byte flag 0x80
};

constexpr uint32_t kCtypeLower = 1;   // bit 0 of a ctype entry

// 0x6F63 ___isctype_mt (__thiscall: this, locinfo, c, type) delegates extended-byte classification
// through this locale-aware CRT seam; the current C locale has no extended locale API installed.
typedef int (*D2GameGetStringTypeA)(uintptr_t locale, const char* src, int src_len, uint16_t* out,
                                    uint32_t locale_flags, uint32_t lcid, int ansi);
D2GameGetStringTypeA g_get_string_type_a = nullptr;

typedef uint32_t (*D2GameIsctypeMt)(void* self, const D2GameCrtLocale* locale, int32_t c, uint32_t type);
uint32_t d2game_isctype_mt(void* self, const D2GameCrtLocale* locale, int32_t c, uint32_t type) {
    const uint8_t* table = locale->ctype;
    uint16_t value;
    if (static_cast<uint32_t>(c + 1) < 0x101u) {
        const uint8_t* entry = table + static_cast<int64_t>(c) * 2;
        value = static_cast<uint16_t>(entry[0] | (static_cast<uint16_t>(entry[1]) << 8));
    } else {
        char src[2];
        int len;
        if ((table[1 + ((static_cast<uint32_t>(c) >> 8 & 0xffu) * 2)] & 0x80) == 0) {
            src[0] = static_cast<char>(c);
            len = 1;
        } else {
            src[0] = static_cast<char>(c >> 8);
            src[1] = static_cast<char>(c);
            len = 2;
        }
        value = 0;
        if (g_get_string_type_a == nullptr ||
            g_get_string_type_a(1, src, len, &value, locale->flags, locale->lcid, 1) == 0)
            return 0;
    }
    (void)self;
    return value & type;
}
uint32_t d2game_ascii_isctype(void*, const D2GameCrtLocale*, int32_t c, uint32_t type) {
    if (c >= 0x80) return 0;
    if (type == kCtypeLower) return c >= 'a' && c <= 'z';
    return 0;
}
D2GameIsctypeMt g_isctype_mt = d2game_isctype_mt;

// 0x6B8D, the __crtLCMapStringA wrapper. The port carries no locale tables, so with no seam
// installed the mapping fails and the input is returned unchanged.
typedef int (*D2GameLcmapStringA)(uint32_t lcid, uint32_t mapFlags, const char* src, int srcLen,
                                  char* dst, int dstLen, uint32_t localeFlags, int isUnicode);
D2GameLcmapStringA g_lcmap_string_a = nullptr;

} // namespace

// 1.13c D2Game+0x03853 (___tolower_mt): the C locale (lcid 0), or a multibyte code page with c < 0x80,
// folds 'A'..'Z' by +0x20 and returns every other byte unchanged. Otherwise a byte below 0x100 must
// carry the lower-case ctype bit (direct table when mbcurmax < 2, else ___isctype_mt) or it is
// returned as is. The character is then passed lead byte first when the high byte is flagged as a
// DBCS lead, mapped with LCMAP_LOWERCASE (0x100) through the 0x6B8D wrapper; a one-byte result is
// returned in the low byte, a two-byte result byte-swapped (CONCAT11), and a failed map returns the
// input.
uint32_t d2_fn_d2game_3853(void* self, const void* locinfo_, uint32_t c) {
    const auto* locinfo = static_cast<const D2GameCrtLocale*>(locinfo_);
    if (locinfo->lcid == 0 || (locinfo->mbcs != 0 && c < 0x80)) {
        if (c > 0x40 && c < 0x5B) return c + 0x20;
        return c;
    }
    if (c < 0x100) {
        uint32_t is;
        if (locinfo->mbcurmax < 2)
            is = locinfo->ctype[static_cast<size_t>(c) * 2] & kCtypeLower;
        else
            is = g_isctype_mt(self, locinfo, c, kCtypeLower);
        if (is == 0) return c;
    }
    char src[2];
    char dst[3] = {0, 0, 0};
    int len;
    if ((locinfo->ctype[1 + ((c >> 8) & 0xFF) * 2] & 0x80) == 0) {
        src[0] = static_cast<char>(c);
        len = 1;
    } else {
        src[0] = static_cast<char>(c >> 8);
        src[1] = static_cast<char>(c);
        len = 2;
    }
    if (g_lcmap_string_a == nullptr) return c;
    const int n = g_lcmap_string_a(locinfo->lcid, 0x100, src, len, dst, 3, locinfo->flags, 1);
    if (n == 0) return c;
    if (n != 1)
        return (static_cast<uint32_t>(static_cast<uint8_t>(dst[0])) << 8) |
               static_cast<uint8_t>(dst[1]);
    return static_cast<uint8_t>(dst[0]);
}

// 1.13c D2Game+0x06F63 (___isctype_mt): use the locale's 16-bit ctype entry for the EOF-through-byte
// range; otherwise build the one- or two-byte sequence (lead byte first) and delegate to
// ___crtGetStringTypeA, returning its output masked by the requested classification bits.
extern "C" uint32_t d2_fn_d2game_6f63(void* self, const void* locinfo_, int32_t c, uint32_t type) {
    return d2game_isctype_mt(self, static_cast<const D2GameCrtLocale*>(locinfo_), c, type);
}
