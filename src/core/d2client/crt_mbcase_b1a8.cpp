// D2Client's VC7.1 multibyte case-table builder. Win32 NLS calls are explicit
// seams; callers supply the code-page result and the two 256-byte mappings.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClientCpInfo {
    uint32_t max_char_size;
    uint8_t default_char[2];
    uint8_t lead_byte[12];
};
static_assert(offsetof(D2ClientCpInfo, lead_byte) == 6);
static_assert(sizeof(D2ClientCpInfo) == 20);

struct D2ClientMbCaseState {
    uint8_t mbctype[257];      // retail 0x123740, indexed by byte + 1
    uint8_t mbcasemap[256];    // retail 0x123860, indexed by byte
    uint32_t code_page;        // retail 0x123844
    uint32_t locale_id;        // retail 0x12372c
};

struct D2ClientMbCaseEnv {
    int32_t (*get_cp_info)(uint32_t, D2ClientCpInfo*);
    int32_t (*get_string_type_a)(uint32_t, const char*, int32_t, uint16_t*,
                                  uint32_t, uint32_t, int32_t);
    int32_t (*lcmap_string_a)(uint32_t, uint32_t, const char*, int32_t,
                               char*, int32_t, uint32_t, int32_t);
};

// 1.13c D2Client+0x0B1A8
extern "C" EMSCRIPTEN_KEEPALIVE void d2client_build_mbcase_b1a8(
    D2ClientMbCaseState* state, const D2ClientMbCaseEnv* env) {
    D2ClientCpInfo cp{};
    if (env->get_cp_info(state->code_page, &cp) == 1) {
        char chars[256];
        uint16_t types[256] = {};
        char lower[256] = {};
        char upper[256] = {};
        for (uint32_t i = 0; i < 256; ++i) chars[i] = static_cast<char>(i);
        chars[0] = ' ';
        // The source edits the first lead-byte pair while walking the rest.
        for (uint32_t pair = 0; pair < 6 && cp.lead_byte[pair * 2] != 0; ++pair) {
            uint8_t lo = cp.lead_byte[pair * 2];
            uint8_t hi = cp.lead_byte[pair * 2 + 1];
            if (lo <= hi) std::memset(chars + lo, ' ', static_cast<size_t>(hi - lo) + 1);
        }
        env->get_string_type_a(1, chars, 256, types, state->code_page, state->locale_id, 0);
        env->lcmap_string_a(state->locale_id, 0x100, chars, 256, lower, 256, state->code_page, 0);
        env->lcmap_string_a(state->locale_id, 0x200, chars, 256, upper, 256, state->code_page, 0);
        for (uint32_t i = 0; i < 256; ++i) {
            if (types[i] & 1) {
                state->mbctype[i + 1] |= 0x10;
                state->mbcasemap[i] = static_cast<uint8_t>(lower[i]);
            } else if (types[i] & 2) {
                state->mbctype[i + 1] |= 0x20;
                state->mbcasemap[i] = static_cast<uint8_t>(upper[i]);
            } else {
                state->mbcasemap[i] = 0;
            }
        }
    } else {
        for (uint32_t i = 0; i < 256; ++i) {
            if (i >= 'A' && i <= 'Z') {
                state->mbctype[i + 1] |= 0x10;
                state->mbcasemap[i] = static_cast<uint8_t>(i + ('a' - 'A'));
            } else if (i >= 'a' && i <= 'z') {
                state->mbctype[i + 1] |= 0x20;
                state->mbcasemap[i] = static_cast<uint8_t>(i - ('a' - 'A'));
            } else {
                state->mbcasemap[i] = 0;
            }
        }
    }
}
