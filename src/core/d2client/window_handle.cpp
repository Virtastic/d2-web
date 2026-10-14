#include <cstdint>
#include <emscripten.h>

namespace { uint32_t g_windowHandle = 0; }

// Imported platform state: an opaque retail DWORD, never a wasm pointer.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_window_handle_pin(uint32_t handle) {
    g_windowHandle = handle;
}

static uint32_t importedWindowHandle() { return g_windowHandle; }

// 1.13c D2Client+0x0D0B4
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_window_handle() {
    return importedWindowHandle();
}
