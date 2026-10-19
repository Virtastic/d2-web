// src/core/d2client/crt_get_environment_strings_a_8b43.cpp -- one UNMAPPED 1.13c D2Client.dll row
// reimplemented from build-wasm/ghidra-d2client/decomp/0x8B43.c (alpha lane). Base 0x6FAB0000; every
// RVA below is retail's. MSVC 2003 Release CRT body. Prior art:
// src/core/gfx/d2ddraw_batch_22ed.cpp (0x24FB) and src/core/realm/mcp_crt_batch_b56.cpp (0x221B) are
// the same VC7.1 body in sibling DLLs; this file follows their seam shape.
//
// The module global is the probe word at +0x0AB80 (DAT_6fbaab80) and the kernel32 environment/probe
// calls are host seams (the wasm module has no PE import table). _malloc/_memcpy/_free are replaced by
// the process allocator/copy: d2_client_7d5f, d2_client_5d20 and d2_client_4891 all document the same
// std allocation and byte copy.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

// --- retail module state ------------------------------------------------------------------------
// 0 = untried, 1 = wide block available, 2 = ANSI block (GetEnvironmentStringsW reported
// ERROR_CALL_NOT_IMPLEMENTED, 0x78).
static uint32_t g_env_strings_kind;     // +0x0AB80  DAT_6fbaab80

// --- host seams ---------------------------------------------------------------------------------
// Defaults are the wasm process's own stand-ins: no environment block, so the row follows its rule
// against empty host state.
struct D2ClientEnvStringsEnv {
    void*  (*get_environment_strings_w)(void);                               // GetEnvironmentStringsW
    const char* (*get_environment_strings_a)(void);                          // GetEnvironmentStrings
    int32_t (*wide_char_to_multi_byte)(uint32_t cp, const void* wide, int32_t wide_len,
                                       char* out, int32_t out_size);         // WideCharToMultiByte
    uint32_t (*get_last_error)(void);                                        // GetLastError
    void   (*free_environment_strings_w)(const void* block);                 // FreeEnvironmentStringsW
    void   (*free_environment_strings_a)(const char* block);                 // FreeEnvironmentStringsA
};

static void* env_default_environment_w(void) { return nullptr; }
static const char* env_default_environment_a(void) { return nullptr; }
// CP_ACP stand-in: one byte per wide unit, '?' outside ASCII; embedded NULs are preserved.
static int32_t env_default_wide_to_mb(uint32_t, const void* wide, int32_t wide_len, char* out,
                                      int32_t out_size) {
    const wchar_t* in = (const wchar_t*)wide;
    if (out == nullptr) return wide_len;
    if (wide_len > out_size) return 0;
    for (int32_t i = 0; i < wide_len; ++i)
        out[i] = in[i] >= 0 && in[i] < 0x80 ? (char)in[i] : '?';
    return wide_len;
}
static uint32_t env_default_last_error(void) { return 0; }
static void env_default_free_w(const void*) {}
static void env_default_free_a(const char*) {}

static D2ClientEnvStringsEnv g_env = {
    env_default_environment_w,
    env_default_environment_a,
    env_default_wide_to_mb,
    env_default_last_error,
    env_default_free_w,
    env_default_free_a,
};

// 1.13c D2Client+0x08B43: `___crtGetEnvironmentStringsA`. Probe GetEnvironmentStringsW once if the
// kind word is 0; a wide block sets kind 1, ERROR_CALL_NOT_IMPLEMENTED (0x78) sets kind 2, any other
// error leaves it untried (which still takes the ANSI path). Kind 1 walks the wide block to its final
// NUL, sizes it with WideCharToMultiByte(cp 0, no flags), allocates, converts, and on a zero second
// conversion frees the buffer and returns null; the source block is always released. Any other kind
// (0 or 2) fetches the ANSI block, walks it to its final NUL, allocates length + 1 and copies
// including the final NUL, then releases the source. A missing block or allocation failure returns
// null. Every path returns the offset of the final NUL plus one from the block start, so the copied
// length includes the terminating NUL.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_8b43(const D2ClientEnvStringsEnv* env) {
    const wchar_t* wide = nullptr;
    if (g_env_strings_kind == 0) {
        wide = (const wchar_t*)env->get_environment_strings_w();
        if (wide != nullptr) {
            g_env_strings_kind = 1;
        } else if (env->get_last_error() == 0x78) {
            g_env_strings_kind = 2;
        }
    }
    if (g_env_strings_kind != 1) {
        if (g_env_strings_kind != 2 && g_env_strings_kind != 0) return nullptr;
        const char* ansi = env->get_environment_strings_a();
        if (ansi == nullptr) return nullptr;
        const char* p = ansi;
        while (*p != 0) { while (*p != 0) ++p; ++p; }   // p stops on the final NUL
        const size_t bytes = (size_t)(p - ansi) + 1;    // plus that NUL
        void* dst = std::malloc(bytes);
        if (dst != nullptr) std::memcpy(dst, ansi, bytes);
        env->free_environment_strings_a(ansi);
        return dst;
    }
    if (wide == nullptr) {
        wide = (const wchar_t*)env->get_environment_strings_w();
        if (wide == nullptr) return nullptr;
    }
    const wchar_t* p = wide;
    while (*p != 0) { while (*p != 0) ++p; ++p; }
    const int32_t chars = (int32_t)(p - wide) + 1;      // includes the final NUL
    const int32_t bytes = env->wide_char_to_multi_byte(0, wide, chars, nullptr, 0);
    char* dst = bytes != 0 ? (char*)std::malloc((size_t)bytes) : nullptr;
    if (dst != nullptr &&
        env->wide_char_to_multi_byte(0, wide, chars, dst, bytes) == 0) {
        std::free(dst);
        dst = nullptr;
    }
    env->free_environment_strings_w(wide);
    return dst;
}

// Probe record ABI: no arguments; r[4] = the copied block, or 0.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_8b43(uint32_t* r) {
    r[4] = (uint32_t)(uintptr_t)d2_client_8b43(&g_env);
}
