#pragma once
#include <cstdint>
#include "mcp_stringtype.h"

// Native view of the MCP CRT locale. The classification pointer addresses character
// zero; classification[-1] represents EOF. Windows byte flags are preserved verbatim.
struct McpNumericLocale {
    int32_t multibyte_max;
    const uint16_t* classification;
    uint32_t codepage;
    uint32_t locale;
    McpStringTypeHost* string_type;
};

struct McpNumericLocaleRuntime {
    void* context;
    // Obtains the calling thread's current locale, refreshing from the module
    // default when necessary (retail __getptd / ___updatetlocinfo).
    const McpNumericLocale* (*current_locale)(void*);
    int32_t (*locale_info)(void*, uint32_t locale, uint32_t type, char*, int32_t capacity);
};

extern "C" int32_t d2_mcp_atol_5881(const char*, McpNumericLocaleRuntime*);
extern "C" int32_t d2_mcp_locale_codepage_5909(uint32_t, McpNumericLocaleRuntime*);
