// Stage A implementation from the cached 1.13c decompile. Win32 locale/code-page APIs and the
// sibling D2Gfx helpers at 0x72B9 (locale -> ANSI code page) and 0x7300 (multibyte conversion) are
// explicit host seams; the browser port does not have the retail OS locale subsystem.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

// One instance per emulated CRT module. The CRT locale setters and this wrapper share it.
struct D2GfxLcMapState {
    uint32_t unicode_state; // DAT_6FA91170: 0 unknown, 1 supported, 2 absent
    uint32_t default_locale; // DAT_6FA91130
    uint32_t default_codepage; // DAT_6FA91140
};
struct D2GfxLcMapStringAEnv {
    D2GfxLcMapState* state;
    int32_t (*lcmap_string_w)(uint32_t locale, uint32_t flags, const uint16_t* src, int32_t cch_src,
                              uint16_t* dst, int32_t cch_dst);
    int32_t (*lcmap_string_a)(uint32_t locale, uint32_t flags, const char* src, int32_t cch_src,
                              char* dst, int32_t cch_dst);
    int32_t (*multi_byte_to_wide_char)(uint32_t codepage, uint32_t flags, const char* src,
                                       int32_t cb_src, uint16_t* dst, int32_t cch_dst);
    int32_t (*wide_char_to_multi_byte)(uint32_t codepage, uint32_t flags, const uint16_t* src,
                                       int32_t cch_src, char* dst, int32_t cb_dst,
                                       const char* default_char, int32_t* used_default);
    uint32_t (*get_last_error)(void);
    int32_t (*locale_codepage)(uint32_t locale); // D2Gfx+0x072B9
    char* (*multibyte_convert)(uint32_t from, uint32_t to, const char* src, uint32_t* length,
                               char* dst, int32_t dst_length); // D2Gfx+0x07300
};

// 1.13c D2Gfx+0x06BD0. Retail's 912-byte body is the VC7.1 __crtLCMapStringA wrapper. Its stack buffers are represented
// by owned allocations here; only API calls, returned count, and destination bytes are observable.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2gfx_lcmap_string_a_6bd0(
    uint32_t locale, uint32_t flags, const char* src, uint32_t src_length, char* dst,
    int32_t dst_length, uint32_t codepage, int32_t strict, const D2GfxLcMapStringAEnv* env) {
    if (env->state->unicode_state == 0) {
        static const uint16_t empty[] = {0};
        if (env->lcmap_string_w(0, 0x100, empty, 1, nullptr, 0) == 0) {
            if (env->get_last_error() == 0x78) env->state->unicode_state = 2;
        } else {
            env->state->unicode_state = 1;
        }
    }

    if (static_cast<int32_t>(src_length) > 0) {
        uint32_t remaining = src_length;
        const char* scan = src;
        do {
            --remaining;
            if (*scan++ == '\0') {
                src_length = src_length - 1u - remaining;
                break;
            }
        } while (remaining != 0);
    }
    if (env->state->unicode_state != 2 && env->state->unicode_state != 0) {
        if (env->state->unicode_state != 1) return 0;
        uint32_t result = 0;
        if (codepage == 0) codepage = env->state->default_codepage;
        const int32_t wide_length = env->multi_byte_to_wide_char(
            codepage, static_cast<uint32_t>(strict != 0) * 8u + 1u, src,
            static_cast<int32_t>(src_length), nullptr, 0);
        if (wide_length == 0) return 0;
        auto* wide = static_cast<uint16_t*>(std::malloc(static_cast<uint32_t>(wide_length) * 2u));
        if (wide == nullptr) return 0;
        if (env->multi_byte_to_wide_char(codepage, 1, src, static_cast<int32_t>(src_length), wide,
                                         wide_length) != 0) {
            result = static_cast<uint32_t>(env->lcmap_string_w(locale, flags, wide, wide_length,
                                                               nullptr, 0));
            if (result != 0) {
                if ((flags & 0x400u) == 0) {
                    auto* mapped = static_cast<uint16_t*>(std::malloc(result * 2u));
                    if (mapped != nullptr) {
                        if (env->lcmap_string_w(locale, flags, wide, wide_length, mapped,
                                                static_cast<int32_t>(result)) != 0) {
                            char* out = dst_length == 0 ? nullptr : dst;
                            result = static_cast<uint32_t>(env->wide_char_to_multi_byte(
                                codepage, 0, mapped, static_cast<int32_t>(result), out, dst_length,
                                nullptr, nullptr));
                        }
                        std::free(mapped);
                    }
                } else if (dst_length != 0 && static_cast<int32_t>(result) <= dst_length) {
                    env->lcmap_string_w(locale, flags, wide, wide_length,
                                        reinterpret_cast<uint16_t*>(dst), dst_length);
                }
            }
        }
        std::free(wide);
        return result;
    }

    if (locale == 0) locale = env->state->default_locale;
    if (codepage == 0) codepage = env->state->default_codepage;
    const int32_t locale_codepage = env->locale_codepage(locale);
    if (locale_codepage == -1) return 0;
    if (static_cast<uint32_t>(locale_codepage) == codepage) {
        return static_cast<uint32_t>(env->lcmap_string_a(locale, flags, src,
                                                         static_cast<int32_t>(src_length), dst,
                                                         dst_length));
    }

    char* converted = nullptr;
    char* mapped = nullptr;
    char* converted_back = nullptr;
    uint32_t result = 0;
    uint32_t converted_length = src_length;
    converted = env->multibyte_convert(codepage, static_cast<uint32_t>(locale_codepage), src,
                                       &converted_length, nullptr, 0);
    src_length = converted_length;
    if (converted == nullptr) return 0;

    const uint32_t mapped_length = static_cast<uint32_t>(env->lcmap_string_a(
        locale, flags, converted, static_cast<int32_t>(src_length), nullptr, 0));
    if (mapped_length != 0) {
        mapped = static_cast<char*>(std::malloc(mapped_length));
        if (mapped != nullptr) {
            std::memset(mapped, 0, mapped_length);
            const uint32_t actual_length = static_cast<uint32_t>(env->lcmap_string_a(
                locale, flags, converted, static_cast<int32_t>(src_length), mapped,
                static_cast<int32_t>(mapped_length)));
            if (actual_length != 0) {
                uint32_t output_length = actual_length;
                converted_back = env->multibyte_convert(static_cast<uint32_t>(locale_codepage),
                    codepage, mapped, &output_length, dst, dst_length);
                if (converted_back != nullptr) result = output_length;
            }
        }
    }
    std::free(converted);
    if (mapped != nullptr) std::free(mapped);
    if (converted_back != nullptr && converted_back != dst) std::free(converted_back);
    return result;
}
