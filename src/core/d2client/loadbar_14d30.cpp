// The join loading screen's progress bar: what feeds D2Client 0xC1760's bar-fill word (D2Client+0x11C4F8) and how often the frame word moves.
// 1.13c D2Client+0x14D30 sweeps the fill across one chunk of the character-file upload, 1.13c D2Client+0x14D90 is the upload
// that calls it (255 bytes per 0x6C packet), and 1.13c D2Client+0x6D010 / 0x4447E / 0x43F90 are the only callers of the frame routine with 1.
// Everything here is plain integers; the platform layer draws the three rectangles it returns.
#include <cstdint>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

namespace {
// 1.13c D2Client+0x14D30 for one sweep step k (0..7): width = ((a << 3 << 8) + 0x100 * k) / (total << 3), signed idiv, 0 when total is 0.
int32_t sweepWidth(int32_t a, int32_t total, int32_t k) {
    const int32_t den = (int32_t)((uint32_t)total << 3);
    if (den == 0) return 0;
    const int32_t num = (int32_t)(((uint32_t)a << 3 << 8) + 0x100u * (uint32_t)k);
    return num / den;   // the fill spans 0x100 pixels: a/total of it plus k/8 of one chunk
}
}  // namespace

// Fill width at sweep step k of chunk a of total (a = 0, total = 0 is the 2 s opening sweep: width 0 for all eight steps).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_loadbar_width(int32_t a, int32_t total, int32_t k) { return sweepWidth(a, total, k); }

// 1.13c D2Client+0x14D90: the number of full 255-byte packets of a size-byte file (the sweeps run for these, the remainder packet has none),
// and, after the upload, the fill word: 0x100 when there was a full packet (edi << 8) / edi, else 0.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_loadbar_chunks(int32_t size) { return size > 0 ? size / 0xff : 0; }
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_loadbar_final_width(int32_t chunks) { return chunks == 0 ? 0 : (int32_t)((uint32_t)chunks << 8) / chunks; }

// 1.13c D2Client+0x6D010's count of frame-routine calls with 1 in one walk of `rooms` list entries when its gate is set: the first of every 32
// entries (counter & 0x1f == 0) is skipped.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_loadbar_walk_calls(int32_t rooms) { return rooms > 0 ? rooms - (rooms + 31) / 32 : 0; }

// 1.13c D2Client+0xC1760's rectangles. mode = game mode word (D2Client+0x11C394), suppress = D2Client+0x11AAD78, width = D2Client+0x11C4F8.
// out: 6 ints per rectangle (x0, y0, x1, y1, colour, draw mode); colour 0 = the border colour (palette index of 0xFF,0xFF,0: word D2Client+0x11BBB9),
// 1 = black index 0, 2 = the fill colour (index of 0x60,0,0: word D2Client+0x11BBB8). Returns the rectangle count (0 outside modes 6-9 or when suppressed).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_loadbar_rects(int32_t mode, int32_t W, int32_t H, int32_t suppress, int32_t width, int32_t* out) {
    if (!(mode == 6 || mode == 7 || mode == 8 || mode == 9) || suppress) return 0;
    const int32_t cx = W / 2, cy = H / 2;
    const int32_t r[3][6] = {{cx - 0x82, cy + 0x9e, cx + 0x82, cy + 0xb2, 0, 1}, {cx - 0x80, cy + 0xa0, cx + 0x80, cy + 0xb0, 1, 5},
                             {cx - 0x80, cy + 0xa0, width + (cx - 0x80), cy + 0xb0, 2, 5}};
    const int n = width != 0 ? 3 : 2;
    for (int i = 0; i < n; ++i) for (int j = 0; j < 6; ++j) out[i * 6 + j] = r[i][j];
    return n;
}
