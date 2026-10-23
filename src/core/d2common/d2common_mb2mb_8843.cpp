// D2Common 1.13c row 0x08843: MSVC CRT multibyte code-page converter (callers 0x04EF4, 0x064D3).
// Adapted from src/core/d2client/crt_mb2mb_b83d.cpp; same logic as the cached decompile. Win32 calls are host seams;
// retail _alloca/_calloc(0x52CA) wide buffer is calloc here (not observable).
#include <emscripten.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct D2commonMb2MbEnv {
    int32_t (*get_cp_max_char_size)(uint32_t cp);   // KERNEL32 GetCPInfo: MaxCharSize, 0 if the call fails
    int32_t (*multi_byte_to_wide_char)(uint32_t cp, uint32_t flags, const char* src, int32_t cb_src,
                                       uint16_t* dst, int32_t cch_dst);
    int32_t (*wide_char_to_multi_byte)(uint32_t cp, uint32_t flags, const uint16_t* src, int32_t cch_src,
                                       char* dst, int32_t cb_dst, const char* def, int32_t* used_def);
};

// 1.13c D2Common+0x08843
extern "C"
EMSCRIPTEN_KEEPALIVE char* d2common_mb2mb_8843(uint32_t cp_from, uint32_t cp_to, const char* src, uint32_t* pcb,
                                     char* dst, int32_t cb_dst, const D2commonMb2MbEnv* env) {
    uint32_t cb = *pcb;
    char* out = nullptr;
    if (cp_from == cp_to) return nullptr;
    bool single = env->get_cp_max_char_size(cp_from) == 1 && env->get_cp_max_char_size(cp_to) == 1;
    uint32_t n = cb;
    if (single) {
        if (cb == 0xffffffffu) n = static_cast<uint32_t>(std::strlen(src)) + 1;
    } else {
        n = static_cast<uint32_t>(env->multi_byte_to_wide_char(cp_from, 1, src, static_cast<int32_t>(cb), nullptr, 0));
        if (n == 0) return nullptr;
    }
    uint16_t* wide = static_cast<uint16_t*>(std::calloc(n, 2));
    if (wide == nullptr) return nullptr;
    if (env->multi_byte_to_wide_char(cp_from, 1, src, static_cast<int32_t>(cb), wide, static_cast<int32_t>(n)) != 0) {
        if (dst == nullptr) {
            if (!single) n = static_cast<uint32_t>(env->wide_char_to_multi_byte(cp_to, 0, wide, static_cast<int32_t>(n), nullptr, 0, nullptr, nullptr));
            if (single || n != 0) {
                out = static_cast<char*>(std::calloc(n, 1));
                if (out != nullptr) {
                    uint32_t r = static_cast<uint32_t>(env->wide_char_to_multi_byte(cp_to, 0, wide, static_cast<int32_t>(n), out, static_cast<int32_t>(n), nullptr, nullptr));
                    if (r == 0) { std::free(out); out = nullptr; }
                    else if (cb != 0xffffffffu) *pcb = r;
                }
            }
        } else if (env->wide_char_to_multi_byte(cp_to, 0, wide, static_cast<int32_t>(n), dst, cb_dst, nullptr, nullptr) != 0) {
            out = dst;
        }
    }
    std::free(wide);
    return out;
}
