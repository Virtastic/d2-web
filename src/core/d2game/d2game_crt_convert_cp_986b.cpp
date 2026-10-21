// D2Game.dll 1.13c row 0x0986B, CRT __crtGetStringTypeA's codepage converter (451 B). Stage A from
// build-wasm/ghidra-d2game/decomp/0x986B.c. Win32 NLS calls and the heap are explicit seams.
#include <cstdint>
#include <cstring>

struct D2GameConvertCpEnv {
    int32_t (*is_single_byte_cp)(uint32_t cp); // GetCPInfo(cp).MaxCharSize == 1, false on failure
    int32_t (*mb_to_wide)(uint32_t cp, uint32_t flags, const char* s, int32_t cb, uint16_t* o, int32_t n);
    int32_t (*wide_to_mb)(uint32_t cp, uint32_t flags, const uint16_t* s, int32_t cw, char* o, int32_t n);
    void* (*calloc1)(uint32_t n, uint32_t size); // D2Game+0x6018
    void (*free)(void* p);                       // D2Game+0x5A8D
};

// 1.13c D2Game+0x0986B
extern "C" char* d2game_crt_convert_cp(uint32_t from, uint32_t to, const char* src, uint32_t* pcb,
                                       char* out, int32_t outlen, const D2GameConvertCpEnv* env) {
    char* result = nullptr;
    const uint32_t cb = *pcb;
    if (from == to) return nullptr;
    const bool sb = env->is_single_byte_cp(from) && env->is_single_byte_cp(to);
    uint32_t n;
    if (sb) n = cb == 0xffffffffu ? static_cast<uint32_t>(std::strlen(src)) + 1 : cb;
    else {
        n = static_cast<uint32_t>(env->mb_to_wide(from, 1, src, static_cast<int32_t>(cb), nullptr, 0));
        if (n == 0) return nullptr;
    }
    // Retail: __chkstk alloca, calloc only after SEH stack overflow (not reproducible natively).
    auto* wide = static_cast<uint16_t*>(__builtin_alloca(n * 2u));
    std::memset(wide, 0, n * 2u);
    if (env->mb_to_wide(from, 1, src, static_cast<int32_t>(cb), wide, static_cast<int32_t>(n)) == 0)
        return nullptr;
    if (out == nullptr) {
        if (!sb) {
            n = static_cast<uint32_t>(env->wide_to_mb(to, 0, wide, static_cast<int32_t>(n), nullptr, 0));
            if (n == 0) return nullptr;
        }
        result = static_cast<char*>(env->calloc1(1, n));
        if (result) {
            const uint32_t w = static_cast<uint32_t>(
                env->wide_to_mb(to, 0, wide, static_cast<int32_t>(n), result, static_cast<int32_t>(n)));
            if (w == 0) { env->free(result); result = nullptr; }
            else if (cb != 0xffffffffu) *pcb = w;
        }
    } else if (env->wide_to_mb(to, 0, wide, static_cast<int32_t>(n), out, outlen) != 0) {
        result = out;
    }
    return result;
}
