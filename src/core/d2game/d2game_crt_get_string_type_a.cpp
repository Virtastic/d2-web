// D2Game.dll 1.13c row 0x075E8, ___crtGetStringTypeA. Stage A implementation from the cached
// decompile at build-wasm/ghidra-d2game/decomp/0x75E8.c. The 421-byte VC7.1 CRT wrapper probes the
// wide NLS API once, then classifies through either the wide API or an ANSI conversion path. Its
// internal D2Game calls at 0x9824 (locale ANSI codepage) and 0x986B (codepage conversion), and the
// imported NLS/heap calls, are supplied as seams until those rows have their own mappings.
#include <cstdint>
#include <cstring>

struct D2GameGetStringTypeAEnv {
    uint32_t default_locale;                       // DAT_6fd3003c
    uint32_t default_code_page;                    // DAT_6fd3004c
    uint32_t (*get_last_error)(void);              // GetLastError
    int32_t (*get_string_type_w)(uint32_t info_type, const uint16_t* src, int32_t cch,
                                 uint16_t* out);   // GetStringTypeW
    int32_t (*get_string_type_a)(uint32_t locale, uint32_t info_type, const char* src,
                                 int32_t cch, uint16_t* out); // GetStringTypeA
    uint32_t (*multi_byte_to_wide_char)(uint32_t cp, uint32_t flags, const char* src,
                                        int32_t cb, uint16_t* out, int32_t cch);
    int32_t (*locale_ansi_codepage)(uint32_t locale, const void* env); // D2Game+0x09824
    char* (*convert)(uint32_t from, uint32_t to, const char* src, uint32_t* pcb,
                     char* out, int32_t outlen, const void* env);      // D2Game+0x0986B
    void* (*malloc)(uint32_t bytes);
    void (*free)(void* p);
};

namespace {
// DAT_6fd30054: 0 unknown, 1 wide API available, 2 wide API unavailable.
uint32_t g_d2game_string_type_wide_state;
}

// 1.13c D2Game+0x075E8 (421 B), ___crtGetStringTypeA. The retail wrapper's stack-variable names
// after its injected SEH prolog are unreliable in the cached decompile; argument slots were checked
// against this DLL's instructions: +0x14 output, +0x18 code page, +0x1c LCID. The locale, codepage
// lookup/conversion and Win32 APIs remain explicit inputs so FUNCTION capture can pin them later.
extern "C" int32_t d2game_crt_get_string_type_a(
    uint32_t info_type, const char* src, int32_t cch, uint16_t* out, uint32_t code_page,
    uint32_t locale, int32_t b_error, const D2GameGetStringTypeAEnv* env) {
    if (g_d2game_string_type_wide_state == 0) {
        const uint16_t empty[] = {0};
        uint16_t probe[2] = {0, 0};
        if (env->get_string_type_w(1, empty, 1, probe) != 0)
            g_d2game_string_type_wide_state = 1;
        else if (env->get_last_error() == 0x78)
            g_d2game_string_type_wide_state = 2;
    }

    if (code_page == 0) code_page = env->default_code_page;
    if (g_d2game_string_type_wide_state == 1) {
        const int32_t wide_count = static_cast<int32_t>(env->multi_byte_to_wide_char(
            code_page, static_cast<uint32_t>(b_error != 0) * 8u + 1u, src, cch, nullptr, 0));
        if (wide_count == 0) return 0;
        const uint32_t allocation = (static_cast<uint32_t>(wide_count) * 2u + 3u) & ~3u;
        // Retail uses __chkstk/alloca, with calloc only after an SEH stack-overflow handler.
        // Native/Wasm stack exhaustion traps rather than exposing Windows SEH; ordinary calls
        // must still use stack storage and must not observe a heap allocation/free.
        auto* wide = static_cast<uint16_t*>(__builtin_alloca(allocation));
        if (wide == nullptr) return 0;
        std::memset(wide, 0, static_cast<size_t>(wide_count) * sizeof(uint16_t));
        int32_t result = 0;
        const int32_t converted = static_cast<int32_t>(env->multi_byte_to_wide_char(
            code_page, 1, src, cch, wide, wide_count));
        if (converted != 0)
            result = env->get_string_type_w(info_type, wide, converted, out);
        return result;
    }
    if (g_d2game_string_type_wide_state != 0 && g_d2game_string_type_wide_state != 2) return 0;

    const uint32_t ansi_locale = locale != 0 ? locale : env->default_locale;
    const int32_t ansi_codepage = env->locale_ansi_codepage(ansi_locale, env);
    if (ansi_codepage == -1) return 0;
    char* converted = nullptr;
    if (static_cast<uint32_t>(ansi_codepage) != code_page) {
        uint32_t length = static_cast<uint32_t>(cch);
        converted = env->convert(code_page, static_cast<uint32_t>(ansi_codepage), src, &length,
                                 nullptr, 0, env);
        if (converted == nullptr) return 0;
        src = converted;
        cch = static_cast<int32_t>(length);
    }
    const int32_t result = env->get_string_type_a(ansi_locale, info_type, src, cch, out);
    if (converted != nullptr) env->free(converted);
    return result;
}
