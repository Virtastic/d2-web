// 1.13c D2Client+0x75690 (216 B): the party-row status label. EAX = mode (0..4), ECX = row y, stack (colour flag, 16-bit id), ret 8.
// Modes pick the string id (0 0x1036, 1 0x1034/0x1035 by the flag, 2 0x1037, 3 0x1038, 4 0x13EF) and the column offset (0xBE; mode 1:
// 0x9B with flag, 0xAF without); mode 1 with id 0xFFFF draws nothing; any other mode asserts (D2Lang 10184 path, ECX 0).
// The text is centred in a 0x36-wide column at gB9A0 + offset: x += (0x36 - width) >> 1 (arithmetic) when narrower; y = row - gB9A4 - 2.
// Returns retail's EAX: the draw call's (main path), the assert call's (bad mode), 1 (mode 1, id 0xFFFF).
#include <cstdint>
#include <emscripten.h>

struct D2c75690Host {
    uint32_t (*str)(uint32_t id);                                              // 0xD1EC: ECX = string id, EAX = string
    int32_t (*width)(uint32_t s);                                              // 0xD35A: ECX = string, EAX = pixel width
    uint32_t (*draw)(uint32_t s, int32_t x, int32_t y, uint32_t col, uint32_t z); // 0xD37E: ECX = string, EDX = x, stack y, col, 0
    uint32_t (*bad)(uint32_t ecx);                                             // 0xD3A8: ECX = 0
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_75690(uint32_t mode, int32_t row_y, uint32_t flag, uint16_t id, int32_t g_b9a0, int32_t g_b9a4,
                         const D2c75690Host* h) {
    uint32_t col = 0, sid; int32_t off = 0xbe;
    switch (mode) {
    case 0: sid = 0x1036; break;
    case 1:
        if (id == 0xffff) return 1;
        if (flag) { sid = 0x1034; col = 2; off = 0x9b; } else { sid = 0x1035; col = 8; off = 0xaf; }
        break;
    case 2: sid = 0x1037; break;
    case 3: sid = 0x1038; break;
    case 4: sid = 0x13ef; break;
    default: return h->bad(0);
    }
    const uint32_t s = h->str(sid);
    const int32_t y = (int32_t)((uint32_t)row_y - (uint32_t)g_b9a4 - 2u);
    int32_t x = (int32_t)((uint32_t)g_b9a0 + (uint32_t)off);
    const int32_t w = h->width(s);
    if (w < 0x36) x += (0x36 - w) >> 1;
    return h->draw(s, x, y, col, 0);
}
