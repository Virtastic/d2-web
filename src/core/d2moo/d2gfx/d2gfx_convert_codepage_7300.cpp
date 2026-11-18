#include <cstdint>
#include <cstring>
#include <emscripten.h>

// Host operations stand in for the Win32 NLS imports and this DLL's CRT leaves.
struct D2GfxConvertCpEnv {
    struct CpInfo { uint32_t max_char_size; uint8_t rest[16]; };
    int32_t (*get_cp_info)(uint32_t cp, CpInfo* info);            // GetCPInfo
    uint32_t (*multi_byte_to_wide_char)(uint32_t cp, uint32_t flags, const char* src,
                                        int32_t cb, uint16_t* out, int32_t cch);
    uint32_t (*wide_char_to_multi_byte)(uint32_t cp, uint32_t flags, const uint16_t* src,
                                        int32_t cch, char* out, int32_t cb,
                                        const char* default_char, void* used);
    uint32_t (*string_length)(const char* s);                     // D2Gfx+0x4150 _strlen
    void* (*calloc)(uint32_t count, uint32_t size);               // D2Gfx+0x2C45 (1, n)
    void (*free)(void* p);                                        // D2Gfx+0x2BD4 _free
};

// 1.13c D2Gfx+0x07300 (451 B, callers 0x6988/0x6BD0): __convertcp, byte string from codepage `from`
// to `to`. Same codepage -> null. 
extern "C" EMSCRIPTEN_KEEPALIVE char* d2gfx_convert_codepage(uint32_t from, uint32_t to,
        const char* src, uint32_t* pcb, char* outbuf, int32_t outlen, const D2GfxConvertCpEnv* env) {
    char* result = nullptr;
    const uint32_t cb = *pcb;
    if (from == to) return nullptr;
    D2GfxConvertCpEnv::CpInfo info;
    const bool both_single = env->get_cp_info(from, &info) != 0 && info.max_char_size == 1 &&
                             env->get_cp_info(to, &info) != 0 && info.max_char_size == 1;
    uint32_t wide_len = cb;
    if (both_single && cb == 0xffffffffu) wide_len = env->string_length(src) + 1;
    if (!both_single) {
        wide_len = env->multi_byte_to_wide_char(from, 1, src, (int32_t)cb, nullptr, 0);
        if (wide_len == 0) return nullptr;
    }
    uint16_t* wide = (uint16_t*)__builtin_alloca(wide_len * 2);  // retail _alloca; its heap branch is dead
    std::memset(wide, 0, wide_len * 2);
    if (env->multi_byte_to_wide_char(from, 1, src, (int32_t)cb, wide, (int32_t)wide_len) != 0) {
        if (outbuf == nullptr) {
            uint32_t bytes = wide_len;
            if (both_single ||
                (bytes = env->wide_char_to_multi_byte(to, 0, wide, (int32_t)wide_len, nullptr, 0,
                                                      nullptr, nullptr)) != 0) {
                result = (char*)env->calloc(1, bytes);
                if (result != nullptr) {
                    const uint32_t written = env->wide_char_to_multi_byte(to, 0, wide, (int32_t)bytes,
                                                                          result, (int32_t)bytes, nullptr, nullptr);
                    if (written == 0) { env->free(result); result = nullptr; }
                    else if (cb != 0xffffffffu) *pcb = written;
                }
            }
        } else if (env->wide_char_to_multi_byte(to, 0, wide, (int32_t)wide_len, outbuf, outlen,
                                                nullptr, nullptr) != 0) {
            result = outbuf;
        }
    }
    return result;
}
