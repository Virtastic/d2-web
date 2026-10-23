// D2Client+0xA8230: sample the 48x48 light grid at pixel (x,y); env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA8230Env {
    int32_t originX, originY;   // image VA 6fbb4df0 / 6fbb4df4
    int32_t colourEnabled;      // DAT 6fb8e830
    const uint8_t* grid;        // 48*48 cells of 8 bytes at 6fbb520c (byte0 intensity, 1..3 r,g,b)
};
extern "C" {
// 1.13c D2Client+0xA8230 (ecx = x, eax = y; thiscall with eax implicit)
EMSCRIPTEN_KEEPALIVE void d2c_a8230(const D2cA8230Env* e, int32_t x, int32_t y, uint8_t out[4]) {
    int32_t cx = (x >> 3) - e->originX, cy = (y >> 3) - e->originY;
    cx = cx < 0 ? 0 : cx > 0x2f ? 0x2f : cx;
    cy = cy < 0 ? 0 : cy > 0x2f ? 0x2f : cy;
    const uint8_t* c = e->grid + (cy * 0x30 + cx) * 8;
    out[0] = c[0];
    if (e->colourEnabled) { out[1] = c[1]; out[2] = c[2]; out[3] = c[3]; }
}
}
