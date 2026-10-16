// 1.13c D2Client+0x15FC0 and +0x36170: D2Gfx#10041 cell draws
// with a zeroed 72-byte context {frame at +0, cel at +0x34}; args after the context are (x, y, -1, 5, 0).
#include <cstdint>

typedef void (*CellDraw)(uint32_t frame, int32_t x, int32_t y);

// 1.13c D2Client+0x15FC0: frame = [+0x11B858], cel = [+0x11B850] (caller reads it), x = [+0x11B82C]+[+0x11B828], y = [+0x11B824]+0x21.
void d2client_sub_15FC0(const int32_t* g /* {cel, frame, x1, x2, y} */, CellDraw draw) { draw((uint32_t)g[1], g[2] + g[3], g[4] + 0x21); }

// 1.13c D2Client+0x36170: frame 0, x = 0x68, y = 0x49 + 0x14*i for i in 0..7; cel = [+0x11C440].
void d2client_sub_36170(CellDraw draw) { for (int32_t i = 0, y = 0x49; i < 8; i++, y += 0x14) draw(0, 0x68, y); }
