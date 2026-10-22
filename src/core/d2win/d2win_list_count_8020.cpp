// Count of nodes chained from a type-4 control (+0x48 head, node +0x1c next); fatal on null/other type.
#include <cstdint>
#include <emscripten.h>

struct D2WinListCountImports {
    const char* (*error_text)(uint32_t id); // Fog ordinal 10265
    void (*display_error)(const char* caption, const char* message); // Fog 10024
};
extern "C" void d2win_exit(uint32_t code);

// 1.13c D2Win+0x08020
// ponytail: retail 32-bit layout offsets applied to native-width pointers (+0x48 -> head, node +0x1c -> next).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_list_count_8020(
    const uint32_t* control, const D2WinListCountImports& imports) {
    uint32_t id = 0x309;
    if (control) {
        if (control[0] == 4) {
            int32_t n = 0;
            for (const uint8_t* p = *(const uint8_t* const*)((const uint8_t*)control + 0x48);
                 p && (p = *(const uint8_t* const*)(p + 0x1c)); ++n) {}
            return n;
        }
        id = 0x30a;
    }
    imports.display_error("", imports.error_text(id));
    d2win_exit(UINT32_MAX);
    __builtin_unreachable();
}
