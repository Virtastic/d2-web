// 1.13c D2Client+0x2D300 (183 B, EAX = value, stack colour, left, top, right, ret 0x10): "%d" of value, font 1 below 1000
// else 0, drawn at y = [0x11B9A4] + [0xDBC4C] + top - 0x1E0, x = [0x11B9A0] + left, moved right by half the slack when
// the text is narrower than the span right - left + 1.
#include "d2client_textdraw_env.h"
#include "d2client_wfmt.h"
extern "C" void d2c_port_2d300(int32_t v, uint32_t colour, int32_t left, int32_t top, int32_t right, const D2ClientTextDrawEnv& e) {
    uint16_t t[32] = {};
    d2client_wfmt::Buf o; d2client_wfmt::putInt(o, v); d2client_wfmt::widen(o, t);
    e.font(v < 1000 ? 1 : 0);
    int32_t y = (int32_t)((uint32_t)e.y0 + (uint32_t)e.y1 + (uint32_t)top - 0x1E0u);
    int32_t x = (int32_t)((uint32_t)e.x0 + (uint32_t)left);
    int32_t span = (int32_t)((uint32_t)e.x0 - (uint32_t)x + (uint32_t)right + 1u);
    int32_t w = e.width(t);
    if (w < span) x = (int32_t)((uint32_t)x + (uint32_t)((int32_t)((uint32_t)span - (uint32_t)w) >> 1));
    e.draw(t, x, y, colour, 0);
}
