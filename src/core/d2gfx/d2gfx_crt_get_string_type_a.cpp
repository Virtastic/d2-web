#include <cstdint>
#include <cstring>
#include <emscripten.h>

// Host operations stand in for Win32 NLS and this DLL's internal codepage helpers.
// They are explicit so the wasm shim never substitutes the host locale or ASCII rules.
struct D2GfxStringTypeHost {
    uint32_t wide_state;       // D2Gfx data +0x91148: 0 unknown, 1 wide API, 2 ANSI API
    uint32_t default_locale;   // D2Gfx data +0x91130
    uint16_t* default_ctype;   // D2Gfx data +0x91140
    int32_t (*wide_type)(uint32_t, const uint16_t*, int32_t, uint16_t*);
    uint32_t (*last_error)();
    int32_t (*to_wide)(uint32_t, uint32_t, const char*, int32_t, uint16_t*, int32_t);
    int32_t (*ansi_codepage)(uint32_t); // D2Gfx+0x72B9
    char* (*convert)(uint32_t, uint32_t, const char*, uint32_t*, char*, int32_t); // +0x7300
    int32_t (*ansi_type)(uint32_t, uint32_t, const char*, int32_t, uint16_t*);
    void* (*temporary)(uint32_t);
    void (*release)(void*);
    void (*free_converted)(void*); // D2Gfx+0x2BD4
};

// 1.13c D2Gfx+0x06988 (421 B), ___crtGetStringTypeA.
// The MSVC CRT control flow is the same as the corresponding 421-byte helper in
// D2MCPClient. This shim exposes the Win32 NLS and internal codepage calls as host ops.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2gfx_crt_get_string_type_a(
        uint32_t type, const char* source, int32_t count, uint16_t* output,
        uint32_t codepage, uint32_t locale, int32_t error, D2GfxStringTypeHost* host) {
    if (host->wide_state == 0) {
        const uint16_t empty[1] = {0};
        uint16_t classification[2] = {};
        if (host->wide_type(1, empty, 1, classification)) host->wide_state = 1;
        else if (host->last_error() == 0x78) host->wide_state = 2;
    }
    if (host->wide_state == 1) {
        const int32_t size = host->to_wide(codepage, error ? 9 : 1, source, count, nullptr, 0);
        if (size == 0) return 0;
        const uint32_t bytes = uint32_t(size) * 2;
        auto* wide = static_cast<uint16_t*>(host->temporary((bytes + 3u) & ~3u));
        if (!wide) return 0;
        std::memset(wide, 0, bytes);
        const int32_t converted = host->to_wide(codepage, 1, source, count, wide, size);
        uint16_t* dest = output != nullptr ? output : host->default_ctype;
        const int32_t result = converted ? host->wide_type(type, wide, converted, dest) : 0;
        host->release(wide);
        return result;
    }
    if (host->wide_state != 0 && host->wide_state != 2) return 0;
    if (locale == 0) locale = host->default_locale;
    const int32_t ansi_cp = host->ansi_codepage(locale);
    if (ansi_cp == -1) return 0;
    char* converted = nullptr;
    if (uint32_t(ansi_cp) != codepage) {
        uint32_t length = uint32_t(count);
        converted = host->convert(codepage, uint32_t(ansi_cp), source, &length, nullptr, 0);
        if (!converted) return 0;
        source = converted;
        count = int32_t(length);
    }
    uint16_t* dest = output != nullptr ? output : host->default_ctype;
    const int32_t result = host->ansi_type(locale, type, source, count, dest);
    if (converted) host->free_converted(converted);
    return result;
}
