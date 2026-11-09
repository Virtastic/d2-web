// 1.13c D2Client+0xA8880 (196 B, plain ret, no inputs): marks the cells of the 0x30 x 0x30 light window around the origin (0x104DF0, 0x104DF4)
// that the player's unit can reach. The player id 0x11BBFC (0 ends it) resolves through Ordinal_10331 (thunk 0xC1F0) to a unit; every cell then
// asks 0x1400 (EAX = the running unit, EBX = x, EDI = y; a zero answer falls back to Ordinal_10331 of the id and the answer becomes the running
// unit) and Ordinal_10482 (thunk 0xC4A8, stdcall unit, x, y, 0x22; the low word of the answer counts) and a hit stores 1 in the first dword of the
// 8-byte cell at 0x105208 + (row * 0x30 + col) * 8.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0xA8880
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a8880(uint32_t playerId, int32_t orgX, int32_t orgY, uint32_t* cells, uint32_t (*c1f0)(uint32_t),
                                                     uint32_t (*r1400)(uint32_t eax, uint32_t ebx, uint32_t edi),
                                                     uint32_t (*c4a8)(uint32_t unit, int32_t x, int32_t y, uint32_t flags)) {
    if (playerId == 0) return;
    uint32_t u = c1f0(playerId);
    if (u == 0) return;
    for (int32_t row = 0, base = 0; base < 0x900; ++row, base += 0x30) {
        for (int32_t col = 0; col < 0x30; ++col) {
            int32_t x = (int32_t)((uint32_t)orgX + (uint32_t)col), y = (int32_t)((uint32_t)orgY + (uint32_t)row);
            u = r1400(u, (uint32_t)x, (uint32_t)y);
            if (u == 0) u = c1f0(playerId);
            if ((uint16_t)c4a8(u, x, y, 0x22) != 0) cells[(base + col) * 2] = 1;
        }
    }
}
