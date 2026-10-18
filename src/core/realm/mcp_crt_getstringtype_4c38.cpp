#include <cstdint>
#include <cstring>
#include <emscripten.h>

// Host operations are explicit: classification and conversion are Windows NLS answers,
// not an ASCII approximation. State belongs to this DLL, not to the BNClient CRT copy.
struct McpStringTypeHost {
    uint32_t wide_state;       // module global +0xAD80: 0 unknown, 1 wide, 2 ANSI
    uint32_t default_locale;   // +0xAD68
    uint32_t default_codepage; // +0xAD78
    int32_t (*wide_type)(uint32_t, const uint16_t*, int32_t, uint16_t*);
    uint32_t (*last_error)();
    int32_t (*to_wide)(uint32_t, uint32_t, const char*, int32_t, uint16_t*, int32_t);
    int32_t (*ansi_codepage)(uint32_t); // +0x5909
    char* (*convert)(uint32_t, uint32_t, const char*, uint32_t*, char*, int32_t); // +0x5950
    int32_t (*ansi_type)(uint32_t, uint32_t, const char*, int32_t, uint16_t*);
    void* (*temporary)(uint32_t); // replaces x86 stack allocation / calloc fallback
    void (*release)(void*);
    void (*free_converted)(void*); // +0x1BB4
};

// 1.13c D2MCPClient+0x04C38
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_crt_getstringtype_4c38(
        uint32_t type, const char* source, int32_t count, uint16_t* output,
        uint32_t codepage, uint32_t locale, int32_t error, McpStringTypeHost* host) {
    if (host->wide_state == 0) {
        const uint16_t empty[1] = {0};
        uint16_t classification[2];
        if (host->wide_type(1, empty, 1, classification)) host->wide_state = 1;
        else if (host->last_error() == 0x78) host->wide_state = 2;
    }
    if (codepage == 0) codepage = host->default_codepage;
    if (host->wide_state == 1) {
        const int32_t size = host->to_wide(codepage, error ? 9 : 1, source, count, nullptr, 0);
        if (size == 0) return 0;
        const uint32_t bytes = uint32_t(size) * 2;
        auto* wide = static_cast<uint16_t*>(host->temporary((bytes + 3u) & ~3u));
        if (!wide) return 0;
        std::memset(wide, 0, bytes);
        // Retail classifies the count returned by the SECOND conversion, not its capacity.
        const int32_t converted = host->to_wide(codepage, 1, source, count, wide, size);
        const int32_t result = converted ? host->wide_type(type, wide, converted, output) : 0;
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
    const int32_t result = host->ansi_type(locale, type, source, count, output);
    if (converted) host->free_converted(converted);
    return result;
}
