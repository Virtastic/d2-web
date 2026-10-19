// Prior art: D2MOO source/D2Win/src/D2WinMain.cpp control headers have a
// 32-bit type at offset zero. Its 1.10f ordinal 10023 reads the focused edit
// box instead; the cached 1.13c decompile establishes this export's contract.
#include <cstdint>
#include <emscripten.h>

// Imported Fog functions are supplied by the caller. Keep both the returned
// diagnostic pointer and the callback pointers native-width on wasm64.
struct D2WinControlTypeImports {
    const char* (*error_text)(uint32_t id); // Fog ordinal 10265
    void (*display_error)(const char* caption, const char* message); // Fog 10024
};
extern "C" void d2win_exit(uint32_t code);

// 1.13c D2Win+0x0B370
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_control_type_b370(
    const uint32_t* control, const D2WinControlTypeImports& imports) {
    if (control == nullptr) {
        const char* message = imports.error_text(0xD8A);
        imports.display_error("", message);
        d2win_exit(UINT32_MAX);
        __builtin_unreachable();
    }
    return *control;
}
