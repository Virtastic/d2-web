#include "mcp_numeric_locale.h"
#include <bit>
#include <emscripten.h>

// Prior art: the VC7.1 CRT arithmetic also recovered in the BNClient CRT
// batches; Windows locale semantics follow MCP's own 1.13c cached decompile.
// 1.13c D2MCPClient+0x05881
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_atol_5881(
        const char* text, McpNumericLocaleRuntime* runtime) {
    const McpNumericLocale* locale = runtime->current_locale(runtime->context);
    auto* cursor = reinterpret_cast<const uint8_t*>(text);
    // The multibyte branch calls the real shared classifier. Since _atol passes
    // an unsigned byte, its 0..255 fast path reads this same locale table.
    while (locale->multibyte_max < 2
            ? (locale->classification[*cursor] & 8u) != 0
            : d2_mcp_isctype_5b2a(locale->classification, locale->codepage,
                  locale->locale, *cursor, 8u, locale->string_type) != 0) {
        ++cursor;
    }
    const uint8_t sign = *cursor;
    if (sign == '-' || sign == '+') ++cursor;
    // Retail's x86 LONG arithmetic wraps at 32 bits even on overflow.
    uint32_t magnitude = 0;
    while (*cursor >= '0' && *cursor <= '9') {
        magnitude = magnitude * 10u + uint32_t(*cursor - '0');
        ++cursor;
    }
    if (sign == '-') magnitude = 0u - magnitude;
    return std::bit_cast<int32_t>(magnitude);
}

// 1.13c D2MCPClient+0x05909
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_locale_codepage_5909(
        uint32_t locale, McpNumericLocaleRuntime* runtime) {
    char codepage[7];
    codepage[6] = '\0';
    if (!runtime->locale_info(runtime->context, locale, 0x1004, codepage, 6)) return -1;
    return d2_mcp_atol_5881(codepage, runtime);
}
