// BNClient.dll alpha-lane batch (row 0x0391E): the MSVC 2003 Release CRT's __crtLCMapStringA, the
// ANSI entry point behind _LCMapStringA that Bnclient's ___toupper_mt (0x019FE) and ___tolower_mt
// (0x01C76) call. Transcribed from the cached Ghidra decompile of retail 1.13c Bnclient.dll (base
// 0x6FF20000). The Win32 locale/code-page services it drives are host seams on the env argument; the
// two unrecovered internal callees (0x06F60 locale -> ANSI code page, 0x06FA7 multi-byte -> multi-byte
// conversion) are bound function pointers for the same reason. "Unicode API available" state
// (DAT_6FF3E4EC) and the two default globals (DAT_6FF3E698 locale, DAT_6FF3E6A8 code page) are zero in
// the shipped image's .data, which is how they start here.
#include <emscripten.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

// Win32 call shapes as the retail image calls them. Wide strings are UTF-16 (uint16_t) regardless of
// the host's wchar_t width.
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
};

static uint32_t g_bn_lcmap_api;          // DAT_6FF3E4EC: 0 undetected, 1 LCMapStringW works, 2 absent
static uint32_t g_bn_default_locale;     // DAT_6FF3E698
static uint32_t g_bn_default_codepage;   // DAT_6FF3E6A8

// The decompile's leading length fold: cch_src clipped at the first NUL (NUL not counted). A zero
// cch_src is left alone.
static uint32_t bn_lcmap_cch(const char* src, uint32_t cch_src) {
    if (static_cast<int32_t>(cch_src) <= 0) return cch_src;
    uint32_t rem = cch_src;
    do {
        rem--;
        if (*src++ == '\0') return cch_src - 1u - rem;   // NUL at index cch_src-1-rem
    } while (rem != 0);
    return cch_src;                                       // no NUL within cch_src bytes
}

// 1.13c BNClient+0x0391E
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t bn_lcmap_string_a(uint32_t locale, uint32_t flags, const char* src,
                                                uint32_t cch_src, char* dst, int32_t cch_dst,
                                                uint32_t codepage, int32_t b_error,
                                                const BnLcmapStringAEnv* env) {
    if (g_bn_lcmap_api == 0) {
        static const uint16_t kEmpty[] = {0};
        if (env->lcmap_string_w(0, 0x100, kEmpty, 1, nullptr, 0) == 0) {
            if (env->get_last_error() == 0x78) g_bn_lcmap_api = 2;   // ERROR_CALL_NOT_IMPLEMENTED
        } else {
            g_bn_lcmap_api = 1;
        }
    }

    cch_src = bn_lcmap_cch(src, cch_src);

    // The Unicode-capable path: widen with the caller's code page, map wide, narrow back.
    if (g_bn_lcmap_api != 2 && g_bn_lcmap_api != 0) {
        if (g_bn_lcmap_api == 1) {
            uint32_t result = 0;
            if (codepage == 0) codepage = g_bn_default_codepage;
            int32_t wlen = env->multi_byte_to_wide_char(
                codepage, static_cast<uint32_t>(b_error != 0) * 8u + 1u, src,
                static_cast<int32_t>(cch_src), nullptr, 0);
            if (wlen != 0) {
                uint16_t* wide = static_cast<uint16_t*>(std::malloc(static_cast<size_t>(wlen) * 2));
                if (wide == nullptr) return 0;
                if (env->multi_byte_to_wide_char(codepage, 1, src, static_cast<int32_t>(cch_src), wide,
                                                 wlen) != 0) {
                    result = static_cast<uint32_t>(env->lcmap_string_w(locale, flags, wide, wlen, nullptr, 0));
                    if (result != 0) {
                        if ((flags & 0x400) == 0) {   // not LCMAP_SORTKEY: narrow the mapped text
                            uint16_t* mapped = static_cast<uint16_t*>(
                                std::malloc(static_cast<size_t>(result) * 2));
                            if (mapped != nullptr) {
                                if (env->lcmap_string_w(locale, flags, wide, wlen, mapped,
                                                        static_cast<int32_t>(result)) != 0) {
                                    char* out = cch_dst == 0 ? nullptr : dst;
                                    result = static_cast<uint32_t>(env->wide_char_to_multi_byte(
                                        codepage, 0, mapped, static_cast<int32_t>(result), out, cch_dst,
                                        nullptr, nullptr));
                                }
                                std::free(mapped);
                            }
                        } else if (cch_dst != 0 && static_cast<int32_t>(result) <= cch_dst) {
                            env->lcmap_string_w(locale, flags, wide, wlen,
                                                reinterpret_cast<uint16_t*>(dst), cch_dst);
                        }
                    }
                }
                std::free(wide);
            }
            return result;
        }
        return 0;
    }

    // The ANSI path: map through the locale's ANSI code page when it differs from the requested one.
    char* converted = nullptr;
    char* mapped = nullptr;
    char* back = nullptr;
    uint32_t result = 0;
    if (locale == 0) locale = g_bn_default_locale;
    if (codepage == 0) codepage = g_bn_default_codepage;
    int32_t ansi_cp = env->locale_ansi_codepage(locale);
    if (ansi_cp == -1) return 0;
    if (static_cast<uint32_t>(ansi_cp) == codepage) {
        return static_cast<uint32_t>(env->lcmap_string_a(locale, flags, src,
                                                         static_cast<int32_t>(cch_src), dst, cch_dst));
    }

    uint32_t cb = cch_src;
    converted = env->multibyte_to_multibyte(codepage, static_cast<uint32_t>(ansi_cp), src, &cb, nullptr, 0);
    cch_src = cb;
    if (converted == nullptr) return 0;

    uint32_t n = static_cast<uint32_t>(env->lcmap_string_a(locale, flags, converted,
                                                           static_cast<int32_t>(cch_src), nullptr, 0));
    if (n != 0) {
        mapped = static_cast<char*>(std::malloc(n));
        if (mapped != nullptr) {
            std::memset(mapped, 0, n);
            uint32_t m = static_cast<uint32_t>(env->lcmap_string_a(
                locale, flags, converted, static_cast<int32_t>(cch_src), mapped, static_cast<int32_t>(n)));
            if (m != 0) {
                uint32_t cb2 = m;
                back = env->multibyte_to_multibyte(static_cast<uint32_t>(ansi_cp), codepage, mapped, &cb2,
                                                   dst, cch_dst);
                result = back != nullptr ? cb2 : 0;
            }
        }
    }

    std::free(converted);
    if (mapped != nullptr) std::free(mapped);
    if (back != nullptr && back != dst) std::free(back);
    return result;
}
