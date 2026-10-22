// Same null-guard shape as d2win_control_type_b370.cpp (Fog 10265/10024 + _exit(-1)).
#include <cstdint>
#include <emscripten.h>

struct D2WinClampImports {
    const char* (*error_text)(uint32_t id); // Fog ordinal 10265
    void (*display_error)(const char* caption, const char* message); // Fog 10024
};
extern "C" void d2win_exit(uint32_t code);

// 1.13c D2Win+0x0C770: control[4] = min(value, control[5]); NULL control reports string 0xD4C and exits.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_clamp_byte_c770(
    uint8_t* control, uint8_t value, const D2WinClampImports& imports) {
    if (control == nullptr) {
        imports.display_error("", imports.error_text(0xD4C));
        d2win_exit(UINT32_MAX);
        __builtin_unreachable();
    }
    control[4] = value < control[5] ? value : control[5];
}
