// 1.13c D2Client+0x8c520 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "../d2client/ui_record_bounds.hpp"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_8c520(uint32_t* r) {
    const d2::client::ui_record_bounds::Bounds* b = (const d2::client::ui_record_bounds::Bounds*)(uintptr_t)r[2];
    r[4] = (uint32_t)d2::client::ui_record_bounds::contains_8c520(*b, (int32_t)r[0], (int32_t)r[1]);
}
