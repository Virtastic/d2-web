// Count of nodes chained from a type-4 control (+0x48 head, node +0x1c next); fatal on null/other type.
// 1.13c D2Win+0x08020 (ordinal 10087). Retail 32-bit layout: every pointer field is a dword guest pointer.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2WinListCountImports {
    const char* (*error_text)(uint32_t id); // Fog ordinal 10265
    void (*display_error)(const char* caption, const char* message); // Fog 10024
};
extern "C" void d2win_exit(uint32_t code);

// dword at guest pointer p + off (retail 32-bit field, host bytes)
static uint32_t dword_at(uint32_t p, uint32_t off) {
    uint32_t v;
    std::memcpy(&v, (const uint8_t*)(uintptr_t)(p + off), 4);
    return v;
}

// 1.13c D2Win+0x08020
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_list_count_8020(
    const uint32_t* control, const D2WinListCountImports& imports) {
    uint32_t id = 0x309;
    if (control) {
        if (control[0] == 4) {
            int32_t n = 0;
            for (uint32_t p = control[0x48 / 4]; p && (p = dword_at(p, 0x1c)); ++n) {}
            return n;
        }
        id = 0x30a;
    }
    imports.display_error("", imports.error_text(id));
    d2win_exit(UINT32_MAX);
    __builtin_unreachable();
}
