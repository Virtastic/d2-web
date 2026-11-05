// 1.13c D2Client+0x2D3C0 (257 B, stdcall 7: text1, text2, left, top, right, colour1, colour2): font 6; on first use (byte
// 0x1234A8) the line offset [0xF9F70] := -6 for language < 6, else -7, or -6 when a second language read is above 9.
// text1 at y = [0xDBC4C] + [0x11B9A4] + [0xF9F70] + top - 0x1E0, text2 at [0x11B9A4] + [0xF9F6C] + [0xDBC4C] + top - 0x1E0;
// each centred in right - left + 1 from x = [0x11B9A0] + left when narrower.
#include "d2client_textdraw_env.h"
static void centred(const uint16_t* t, int32_t left, int32_t right, int32_t y, uint32_t colour, const D2ClientTextDrawEnv& e) {
    int32_t x = (int32_t)((uint32_t)e.x0 + (uint32_t)left);
    int32_t span = (int32_t)((uint32_t)e.x0 - (uint32_t)x + (uint32_t)right + 1u);
    int32_t w = e.width(t);
    if (w < span) x = (int32_t)((uint32_t)x + (uint32_t)((int32_t)((uint32_t)span - (uint32_t)w) >> 1));
    e.draw(t, x, y, colour, 0);
}
// off[0] = [0xF9F6C], off[1] = [0xF9F70]; latch = byte 0x1234A8
extern "C" void d2c_port_2d3c0(const uint16_t* t1, const uint16_t* t2, int32_t left, int32_t top, int32_t right, uint32_t c1, uint32_t c2,
                               int32_t* off, uint8_t* latch, const D2ClientTextDrawEnv& e) {
    e.font(6);
    if (!*latch) {
        if ((int32_t)e.lang() < 6) off[1] = -6;
        else { int32_t l = (int32_t)e.lang(); off[1] = l > 9 ? -6 : -7; }
        *latch = 1;
    }
    centred(t1, left, right, (int32_t)((uint32_t)e.y1 + (uint32_t)e.y0 + (uint32_t)off[1] + (uint32_t)top - 0x1E0u), c1, e);
    centred(t2, left, right, (int32_t)((uint32_t)e.y0 + (uint32_t)off[0] + (uint32_t)e.y1 + (uint32_t)top - 0x1E0u), c2, e);
}
