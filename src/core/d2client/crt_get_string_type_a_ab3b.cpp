// src/core/d2client/crt_get_string_type_a_ab3b.cpp -- 1.13c D2Client.dll row 0x0AB3B,
// ___crtGetStringTypeA, the MSVC 2003 Release CRT wrapper behind ___isctype_mt (0x0707B) and
// 0x0B1A8. Transcribed from the cached decompile (build-wasm/ghidra-d2client/decomp/0xAB3B.c;
// base 0x6FAB0000; the same CRT object as d2mcpclient 0x04C38 and d2game 0x064D3).
// Seven cdecl arguments -- classification type, byte source, signed count, word output, code page,
// locale, error flag; the applied library signature mislabels the first as a locale pointer and
// shifts every name by one. The Windows NLS services and the two internal callees (0x0B7F6
// locale -> code page, 0x0B83D byte conversion) are host seams on the env argument. The three
// module globals are zero in the shipped image's .data and start that way here.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

struct D2cGetStringTypeAEnv {
    int32_t (*get_string_type_w)(uint32_t info_type, const uint16_t* src, int32_t cch,
                                 uint16_t* out);                             // KERNEL32 GetStringTypeW
    int32_t (*get_string_type_a)(uint32_t locale, uint32_t info_type, const char* src, int32_t cch,
                                 uint16_t* out);                             // KERNEL32 GetStringTypeA
    int32_t (*multi_byte_to_wide_char)(uint32_t cp, uint32_t flags, const char* src, int32_t cb_src,
                                       uint16_t* dst, int32_t cch_dst);      // KERNEL32 MultiByteToWideChar
    uint32_t (*get_last_error)();                                            // KERNEL32 GetLastError
    int32_t (*locale_ansi_codepage)(uint32_t locale);                        // D2Client+0x0B7F6
    char* (*multibyte_to_multibyte)(uint32_t cp_from, uint32_t cp_to, const char* src,
                                    uint32_t* pcb_src, char* dst, int32_t cch_dst); // D2Client+0x0B83D
};

static uint32_t g_string_type_api;      // DAT_6FBAAE0, RVA 0x0FAABE0: 0 unknown, 1 wide, 2 ANSI
static uint32_t g_default_locale;       // DAT_6FBAC1C, RVA 0x0FAC1C
static uint32_t g_default_codepage;     // DAT_6FBAC2C, RVA 0x0FAC2C

// 1.13c D2Client+0x0AB3B
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2c_crt_get_string_type_a(uint32_t info_type, const char* src,
                                                       int32_t cch_src, uint16_t* char_type,
                                                       uint32_t code_page, uint32_t locale,
                                                       int32_t b_error,
                                                       const D2cGetStringTypeAEnv* env) {
    if (g_string_type_api == 0) {
        static const uint16_t kEmpty[] = {0};
        uint16_t type[2];
        if (env->get_string_type_w(1, kEmpty, 1, type) != 0) {
            g_string_type_api = 1;
        } else if (env->get_last_error() == 0x78) {   // ERROR_CALL_NOT_IMPLEMENTED
            g_string_type_api = 2;
        }
    }

    // Unicode-capable path: widen with the caller's code page, then classify the wide text.
    if (g_string_type_api == 1) {
        if (code_page == 0) code_page = g_default_codepage;
        int32_t wlen = env->multi_byte_to_wide_char(
            code_page, static_cast<uint32_t>(b_error != 0) * 8u + 1u, src, cch_src, nullptr, 0);
        if (wlen == 0) return 0;
        uint16_t* wide = static_cast<uint16_t*>(std::malloc(static_cast<size_t>(wlen) * 2));
        if (wide == nullptr) return 0;
        std::memset(wide, 0, static_cast<size_t>(wlen) * 2);
        int32_t converted = env->multi_byte_to_wide_char(code_page, 1, src, cch_src, wide, wlen);
        int32_t result =
            converted != 0 ? env->get_string_type_w(info_type, wide, converted, char_type) : 0;
        std::free(wide);
        return result;
    }

    // States 0 and 2 use the ANSI API; any other cached state returns 0.
    if (g_string_type_api != 0 && g_string_type_api != 2) return 0;
    if (locale == 0) locale = g_default_locale;
    if (code_page == 0) code_page = g_default_codepage;
    int32_t ansi_cp = env->locale_ansi_codepage(locale);
    if (ansi_cp == -1) return 0;
    char* converted = nullptr;
    if (static_cast<uint32_t>(ansi_cp) != code_page) {
        uint32_t cb = static_cast<uint32_t>(cch_src);
        converted = env->multibyte_to_multibyte(code_page, static_cast<uint32_t>(ansi_cp), src, &cb,
                                                nullptr, 0);
        if (converted == nullptr) return 0;
        src = converted;
        cch_src = static_cast<int32_t>(cb);
    }
    int32_t result = env->get_string_type_a(locale, info_type, src, cch_src, char_type);
    if (converted != nullptr) std::free(converted);
    return result;
}
