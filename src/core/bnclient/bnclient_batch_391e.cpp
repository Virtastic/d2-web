// BNClient.dll row 0x0391E: the MSVC 2003 Release CRT's __crtLCMapStringA, the ANSI entry point behind
// _LCMapStringA that Bnclient's ___toupper_mt (0x019FE) and ___tolower_mt (0x01C76) and the case-table
// builder (0x06348) call. Transcribed from the 1.13c decompile and disassembly (base 0x6FF20000). The
// Win32 locale/code-page services are seams; the two internal callees (0x06F60 locale -> ANSI code page,
// 0x06FA7 multi-byte -> multi-byte conversion) and _free (0x03CF4) are seams too. The scratch buffers are
// _alloca'd as retail does (its _malloc fallback runs only when _alloca faults, so it is never taken here).
// The three globals (DAT_6FF3E4EC "Unicode API" state, DAT_6FF3E698 default locale, DAT_6FF3E6A8 default
// code page) are passed in.
#include <emscripten.h>
#include <cstddef>
#include <cstdint>
#include <cstring>

struct BnLcmapStringAEnv {
    int32_t (*lcmap_string_w)(uint32_t locale, uint32_t flags, const uint16_t* src, int32_t cch_src,
                              uint16_t* dst, int32_t cch_dst);                        // KERNEL32 LCMapStringW
    int32_t (*lcmap_string_a)(uint32_t locale, uint32_t flags, const char* src, int32_t cch_src,
                              char* dst, int32_t cch_dst);                            // KERNEL32 LCMapStringA
    int32_t (*multi_byte_to_wide_char)(uint32_t cp, uint32_t flags, const char* src, int32_t cb_src,
                                       uint16_t* dst, int32_t cch_dst);               // KERNEL32 MultiByteToWideChar
    int32_t (*wide_char_to_multi_byte)(uint32_t cp, uint32_t flags, const uint16_t* src, int32_t cch_src,
                                       char* dst, int32_t cb_dst, const char* default_char,
                                       int32_t* used_default);                        // KERNEL32 WideCharToMultiByte
    uint32_t (*get_last_error)();                                                     // KERNEL32 GetLastError
    int32_t (*locale_ansi_codepage)(uint32_t locale);                                 // BNClient+0x06F60
    char* (*multibyte_to_multibyte)(uint32_t cp_from, uint32_t cp_to, const char* src, uint32_t* pcb_src,
                                    char* dst, int32_t cch_dst);                      // BNClient+0x06FA7
    void (*free_)(void* p);                                                           // BNClient+0x03CF4 _free
};
struct BnLcmapGlobals {
    uint32_t* api;              // DAT_6FF3E4EC: 0 undetected, 1 LCMapStringW works, 2 absent
    const uint32_t* locale;     // DAT_6FF3E698
    const uint32_t* codepage;   // DAT_6FF3E6A8
};

// The leading length fold: cch_src clipped at the first NUL (NUL not counted). A cch_src <= 0 is left alone.
static uint32_t bn_lcmap_cch(const char* src, uint32_t cch_src) {
    if (static_cast<int32_t>(cch_src) <= 0) return cch_src;
    uint32_t rem = cch_src;
    do {
        rem--;
        if (*src++ == '\0') return cch_src - 1u - rem;
    } while (rem != 0);
    return cch_src;
}

// 1.13c BNClient+0x0391E (912 B, SEH frame 0x19B18). The ANSI path's "LCMapStringA sized nothing" exit returns
// an uninitialised frame dword in retail (EBP-0x4C is never written); here it returns 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t bn_lcmap_string_a(uint32_t locale, uint32_t flags, const char* src,
                                                uint32_t cch_src, char* dst, int32_t cch_dst,
                                                uint32_t codepage, int32_t b_error,
                                                const BnLcmapGlobals* g, const BnLcmapStringAEnv* env) {
    if (*g->api == 0) {
        static const uint16_t kEmpty[] = {0};
        if (env->lcmap_string_w(0, 0x100, kEmpty, 1, nullptr, 0) == 0) {
            if (env->get_last_error() == 0x78) *g->api = 2;   // ERROR_CALL_NOT_IMPLEMENTED
        } else {
            *g->api = 1;
        }
    }

    cch_src = bn_lcmap_cch(src, cch_src);

    if (*g->api != 2 && *g->api != 0) {
        if (*g->api != 1) return 0;
        uint32_t result = 0;
        if (codepage == 0) codepage = *g->codepage;
        const int32_t wlen = env->multi_byte_to_wide_char(codepage, static_cast<uint32_t>(b_error != 0) * 8u + 1u, src,
                                                          static_cast<int32_t>(cch_src), nullptr, 0);
        if (wlen == 0) return 0;
        uint16_t* wide = static_cast<uint16_t*>(__builtin_alloca((static_cast<uint32_t>(wlen) * 2 + 3) & ~3u));
        if (env->multi_byte_to_wide_char(codepage, 1, src, static_cast<int32_t>(cch_src), wide, wlen) == 0) return 0;
        result = static_cast<uint32_t>(env->lcmap_string_w(locale, flags, wide, wlen, nullptr, 0));
        if (result == 0) return 0;
        if ((flags & 0x400) == 0) {   // not LCMAP_SORTKEY: narrow the mapped text
            uint16_t* mapped = static_cast<uint16_t*>(__builtin_alloca((result * 2 + 3) & ~3u));
            if (env->lcmap_string_w(locale, flags, wide, wlen, mapped, static_cast<int32_t>(result)) != 0)
                result = static_cast<uint32_t>(env->wide_char_to_multi_byte(
                    codepage, 0, mapped, static_cast<int32_t>(result), cch_dst == 0 ? nullptr : dst,
                    cch_dst == 0 ? 0 : cch_dst, nullptr, nullptr));
        } else if (cch_dst != 0 && static_cast<int32_t>(result) <= cch_dst) {
            env->lcmap_string_w(locale, flags, wide, wlen, reinterpret_cast<uint16_t*>(dst), cch_dst);
        }
        return result;
    }

    // The ANSI path: map through the locale's ANSI code page when it differs from the requested one.
    if (locale == 0) locale = *g->locale;
    if (codepage == 0) codepage = *g->codepage;
    const int32_t ansi_cp = env->locale_ansi_codepage(locale);
    if (ansi_cp == -1) return 0;
    if (static_cast<uint32_t>(ansi_cp) == codepage)
        return static_cast<uint32_t>(env->lcmap_string_a(locale, flags, src, static_cast<int32_t>(cch_src), dst, cch_dst));

    char* converted = env->multibyte_to_multibyte(codepage, static_cast<uint32_t>(ansi_cp), src, &cch_src, nullptr, 0);
    if (converted == nullptr) return 0;
    uint32_t result = 0;
    char* back = nullptr;
    uint32_t n = static_cast<uint32_t>(env->lcmap_string_a(locale, flags, converted, static_cast<int32_t>(cch_src), nullptr, 0));
    if (n != 0) {
        char* mapped = static_cast<char*>(__builtin_alloca((n + 3) & ~3u));
        std::memset(mapped, 0, n);
        n = static_cast<uint32_t>(env->lcmap_string_a(locale, flags, converted, static_cast<int32_t>(cch_src), mapped,
                                                      static_cast<int32_t>(n)));
        if (n != 0) {
            back = env->multibyte_to_multibyte(static_cast<uint32_t>(ansi_cp), codepage, mapped, &n, dst, cch_dst);
            result = back != nullptr ? n : 0;
        }
    }
    env->free_(converted);
    if (back != nullptr && back != dst) env->free_(back);
    return result;
}
