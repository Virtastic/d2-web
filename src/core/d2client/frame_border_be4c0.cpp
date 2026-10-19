// 1.13c D2Client+0xBE4C0: draw a tiled 12-px border around rect {l,t,r,b}; one
// D2Gfx#10041 call (via thunk +0xD096) per tile, args (ctx{frame at +0, cel at +0x34}, x, y, -1, 5, 0).
#include "cell_draw.h"

uint32_t d2client_sub_BE4C0(const int32_t* rc, uint32_t cel, D2ClientCellDraw callback, void* user) {
    D2ClientCellContext ctx{};
    ctx.cel = cel;
    uint32_t result = 0;
    auto draw = [&](uint32_t frame, int32_t x, int32_t y) {
        ctx.frame = frame;
        result = callback(user, &ctx, x, y, -1, 5, 0);
    };
    const int32_t l = rc[0], t = rc[1], r1 = rc[2] + 1, b1 = rc[3] + 1;   // r1,b1: one past the edge
    const int32_t xEnd = r1 - 13, yEnd = b1 - 1;
    int32_t k = 0;
    for (int32_t x = l + 12; x < xEnd; x += 12) draw(++k % 6 + 2, x, t + 12);
    for (int32_t x = l + 13; x < xEnd; x += 12) draw(++k % 6 + 0x10, x, b1 + 10);
    for (int32_t y = t + 22; y < yEnd; y += 12) draw(++k % 3 + 10, l - 4, y);
    for (int32_t y = t + 24; y < yEnd; y += 12) draw(++k % 3 + 13, r1 - 7, y);
    draw(0, l, t + 12);
    draw(1, r1 - 12, t + 12);
    draw(8, l, b1 + 1);
    draw(9, r1 - 12, b1 + 1);
    return result;
}
