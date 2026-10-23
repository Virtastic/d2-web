#include <cstdint>
#include <stddef.h>
#include <emscripten.h>

// A semantic view of the retail multibyte state. Retail reads these values at
// +0x04 (code page), +0x14 (MBCS active), +0x28 (maximum byte count), and
// +0x48 (the 2-byte-per-character classification table).
struct D2ClientMbtowcState {
    uint32_t code_page;
    uint32_t mbcs_active;
    uint32_t max_bytes;
    const uint8_t* ctype;
};

struct D2ClientMbtowcEnv {
    int32_t (*multi_byte_to_wide_char)(uint32_t code_page, uint32_t flags,
                                      const char* src, int32_t src_bytes,
                                      uint16_t* dest, int32_t dest_chars);
    int32_t* (*errno_slot)();
};

// 1.13c D2Client+0x0aa50
// VC7.1 ___mbtowc_mt: converts one multibyte character using the locale state.
// Win32 conversion and CRT errno access are explicit host seams; flags=9 is the
// retail MB_PRECOMPOSED | MB_ERR_INVALID_CHARS combination.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t
d2_client_aa50(const D2ClientMbtowcState* state, uint16_t* out,
               const uint8_t* src, uint32_t src_bytes,
               const D2ClientMbtowcEnv* env) {
    if (src == nullptr || src_bytes == 0) return 0;

    const uint8_t first = src[0];
    if (first == 0) {
        if (out != nullptr) *out = 0;
        return 0;
    }

    if (state->mbcs_active == 0) {
        if (out != nullptr) *out = first;
        return 1;
    }

    const bool lead = state->ctype != nullptr &&
        (state->ctype[(static_cast<uint32_t>(first) << 1) + 1] & 0x80u) != 0;
    if (!lead) {
        const int32_t converted = env->multi_byte_to_wide_char(
            state->code_page, 9, reinterpret_cast<const char*>(src), 1,
            out, out != nullptr ? 1 : 0);
        if (converted != 0) return 1;
    } else {
        const uint32_t count = state->max_bytes;
        if (count > 1 && count <= src_bytes) {
            const int32_t converted = env->multi_byte_to_wide_char(
                state->code_page, 9, reinterpret_cast<const char*>(src),
                static_cast<int32_t>(count), out, out != nullptr ? 1 : 0);
            if (converted != 0) return static_cast<int32_t>(count);
        }
        if (count <= src_bytes && src[1] != 0) {
            return static_cast<int32_t>(count);
        }
    }

    *env->errno_slot() = 0x2a;
    return -1;
}
