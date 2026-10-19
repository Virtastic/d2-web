#include "cell_draw.h"

// 1.13c D2Client+0x15FC0: globals {cel, frame, x1, x2, y}.
uint32_t d2client_sub_15FC0(const int32_t* g, D2ClientCellDraw draw, void* user) {
    D2ClientCellContext ctx{};
    ctx.cel = static_cast<uint32_t>(g[0]);
    ctx.frame = static_cast<uint32_t>(g[1]);
    return draw(user, &ctx, g[2] + g[3], g[4] + 0x21, -1, 5, 0);
}

// 1.13c D2Client+0x36170: cel is global +0x11C440.
uint32_t d2client_sub_36170(uint32_t cel, D2ClientCellDraw draw, void* user) {
    D2ClientCellContext ctx{};
    ctx.cel = cel;
    uint32_t result = 0;
    for (int32_t i = 0, y = 0x49; i < 8; ++i, y += 0x14)
        result = draw(user, &ctx, 0x68, y, -1, 5, 0);
    return result;
}
