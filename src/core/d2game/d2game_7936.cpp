// D2Game.dll 1.13c RVA 0x7936 (411 B). Stage A implementation from the cached
// decompile at build-wasm/ghidra-d2game/decomp/0x7936.c.
// Prior art: 's RE pipeline entries; D2MOO has no implementation of this
// statically linked MSVC CRT locale-table initializer. The sibling D2Client CRT routines are
// architectural cross-checks only; this implementation follows the 1.13c D2Game body.
#include <cstdint>

// 1.13c D2Game+0x07936

struct D2GameCpInfo {
    uint32_t max_char_size;
    uint8_t default_char[2];
    uint8_t lead_byte[12];
};

struct D2GameInitCtypeEnv {
    uint32_t code_page;  // DAT_6fd3a0e4
    uint32_t locale;     // DAT_6fd39fd0
    // These are the imported NLS calls / already-mapped D2Game CRT wrappers.
    int32_t (*get_cp_info)(uint32_t code_page, D2GameCpInfo* info);
    int32_t (*get_string_type_a)(uint32_t info_type, const char* src, int32_t cch,
                                 uint16_t* out, uint32_t code_page, uint32_t locale,
                                 int32_t b_error);
    uint32_t (*lcmap_string_a)(uint32_t locale, uint32_t flags, const char* src,
                               uint32_t cch_src, char* dst, int32_t cch_dst,
                               uint32_t code_page, int32_t b_error);
};

// ctype_flags is the 257-byte region beginning at DAT_6fd39fdf; retail writes its byte i at
// DAT_6fd39fe0+i. case_map is DAT_6fd3a100. Existing ctype bits are preserved by OR operations.
extern "C" void d2_fn_d2game_7936(const D2GameInitCtypeEnv* env, uint8_t* ctype_flags,
                                  uint8_t* case_map) {
    D2GameCpInfo cp_info{};
    if (env->get_cp_info(env->code_page, &cp_info) != 0) {
        char bytes[256];
        uint16_t ctype1[256]{};
        char lower[256]{};
        char upper[256]{};
        for (uint32_t i = 0; i < 256; ++i) bytes[i] = static_cast<char>(i);
        bytes[0] = ' ';

        for (uint32_t i = 0; i < sizeof(cp_info.lead_byte) && cp_info.lead_byte[i] != 0; i += 2) {
            const uint32_t first = cp_info.lead_byte[i];
            const uint32_t last = cp_info.lead_byte[i + 1];
            if (first <= last) {
                for (uint32_t c = first; c <= last; ++c) bytes[c] = ' ';
            }
        }

        env->get_string_type_a(1, bytes, 0x100, ctype1, env->code_page, env->locale, 0);
        env->lcmap_string_a(env->locale, 0x100, bytes, 0x100, lower, 0x100,
                            env->code_page, 0);
        env->lcmap_string_a(env->locale, 0x200, bytes, 0x100, upper, 0x100,
                            env->code_page, 0);

        for (uint32_t i = 0; i < 256; ++i) {
            if ((ctype1[i] & 1) != 0) {
                ctype_flags[i + 1] |= 0x10;
                case_map[i] = static_cast<uint8_t>(lower[i]);
            } else if ((ctype1[i] & 2) != 0) {
                ctype_flags[i + 1] |= 0x20;
                case_map[i] = static_cast<uint8_t>(upper[i]);
            } else {
                case_map[i] = 0;
            }
        }
        return;
    }

    // Fallback used when GetCPInfo fails: ASCII-only case conversion and ctype flags.
    for (uint32_t i = 0; i < 256; ++i) {
        if (i >= 0x41 && i <= 0x5a) {
            ctype_flags[i + 1] |= 0x10;
            case_map[i] = static_cast<uint8_t>(i + 0x20);
        } else if (i >= 0x61 && i <= 0x7a) {
            ctype_flags[i + 1] |= 0x20;
            case_map[i] = static_cast<uint8_t>(i - 0x20);
        } else {
            case_map[i] = 0;
        }
    }
}
