// Browser export of 1.13c D2Client+0x7FE80's rain drops: the same drops the G trace records as D2Gfx 10010 calls, as
// {x0, y0, x1, y1, colour, alpha} int32 sextets in screen space, clipped as the native pass clips them.
#include <cstdint>
#include <emscripten.h>
#include "weather_sim.h"

extern "C" {
static int32_t g_weatherLines[6 * 512];
// Returns the count; d2_client_weather_lines_ptr() is the buffer. mode is DAT_6FBCC414 (0 whole screen, 1/2 a half).
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_weather_lines(int32_t w, int32_t h, int32_t mode) {
    const uint32_t n = d2client::weather::lines(w, h, mode, g_weatherLines, 512);
    return n > 512 ? 512 : n;
}
EMSCRIPTEN_KEEPALIVE const int32_t* d2_client_weather_lines_ptr() { return g_weatherLines; }
}
