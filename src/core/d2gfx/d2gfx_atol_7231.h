#pragma once

#include <cstdint>

// The thread-local CRT operations are explicit host seams, matching the retail
// callees at D2Gfx+0x28FE (__getptd), +0x535E (___updatetlocinfo), and
// +0x7623 (___isctype_mt). The context owns the runtime/TLS implementation.
struct D2GfxAtolHost {
    void* context;
    void* (*getptd)(void* context);
    uint32_t (*default_locale)(void* context);
    // Retail stores these addresses in 32-bit fields. Resolve the tokens in
    // the host address space before dereferencing them on wasm64.
    const void* (*resolve_locale)(void* context, uint32_t retail_token);
    const uint16_t* (*resolve_ctype)(void* context, uint32_t retail_token);
    uint32_t (*updatetlocinfo)(void* context);
    uint32_t (*isctype_mt)(void* context, void* self, const void* locale,
                           int32_t c, uint32_t mask);
};

extern "C" int32_t d2gfx_atol_7231(const char* str, const D2GfxAtolHost* host);

// The generic function probe has no live CRT/TLS environment. Its adapter pins the
// retail startup C locale; production callers pass their thread/runtime state.
extern "C" const D2GfxAtolHost* d2gfx_atol_fn_host();
