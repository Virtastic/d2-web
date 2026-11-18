// 1.13c D2Client+0x50020 (170 B, no arguments, plain ret): only when Ordinal 10031 (thunk 0xD0AE) returns 2. Builds the 18-word
// zero block {2, ..., [13] = DAT 0x11C300, ...}; when 0x4FE70 reports the cursor over the panel and DAT 0xFADA4 is zero, makes
// the label (thunk 0xD1EC: ECX = 0xF93, stack {H-0x32, 0, 1}, returns the handle) and places it (thunk 0xD384: ECX = handle, EDX =
// W/2 + 0xB2). Always ends with thunk 0xD096 (stack {block, W/2 + 0xA3, H - 8, -1, 5, 0}, ret 24). W = DAT 0xDBC48, H = DAT 0xDBC4C
// (signed; /2 truncates toward zero as the retail cdq/sub/sar does).
#include <emscripten.h>
#include <cstdint>
struct D2c50020Env {
    uint32_t (*cd0ae)();
    uint32_t (*c4fe70)();
    uint32_t (*cd1ec)(uint32_t ecx, uint32_t s0, uint32_t s1, uint32_t s2);
    void (*cd384)(uint32_t ecx, uint32_t edx);
    void (*cd096)(const uint32_t* block, uint32_t x, uint32_t y, uint32_t s3, uint32_t s4, uint32_t s5);
};
extern "C" void d2c_port_50020(const uint32_t* gc300, const uint32_t* gada4, const int32_t* gw, const int32_t* gh, const D2c50020Env* e) {
    if (e->cd0ae() != 2) return;
    uint32_t block[18] = {};
    block[13] = *gc300;
    block[0] = 2;
    if (e->c4fe70() != 0 && *gada4 == 0) {
        int32_t w = *gw, h = *gh;
        int32_t x = (w - (w >> 31)) >> 1;
        uint32_t lbl = e->cd1ec(0xF93, (uint32_t)(h - 0x32), 0, 1);
        e->cd384(lbl, (uint32_t)(x + 0xB2));
    }
    int32_t w = *gw, h = *gh;
    int32_t x = (w - (w >> 31)) >> 1;
    e->cd096(block, (uint32_t)(x + 0xA3), (uint32_t)(h - 8), 0xFFFFFFFFu, 5, 0);
}
