// Store a value in slot [index] (dword array at +0x7c) of a type-4 control; fatal on null/other type/negative index.
#include <cstdint>
#include <emscripten.h>

struct D2WinSlotSetImports {
    const char* (*error_text)(uint32_t id); // Fog ordinal 10265
    void (*display_error)(const char* caption, const char* message); // Fog 10024
};
extern "C" void d2win_exit(uint32_t code);

// 1.13c D2Win+0x08070
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_slot_set_8070(
    uint32_t* control, int32_t index, uint32_t value, const D2WinSlotSetImports& imports) {
    uint32_t id = 0x2fc;
    if (control) {
        if (control[0] == 4) {
            if (index >= 0) {
                control[index + 0x1f] = value;
                return;
            }
            id = 0x2ff;
        } else {
            id = 0x2fd;
        }
    }
    imports.display_error("", imports.error_text(id));
    d2win_exit(UINT32_MAX);
    __builtin_unreachable();
}
