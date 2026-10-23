// D2Client sub_A34B0: row hit-test. Pure; panel/pt are 32-bit retail layouts as byte blobs.
#include <cstdint>
#include <cstring>

static int32_t rd32(const uint8_t* p, uint32_t o) { int32_t v; memcpy(&v, p + o, 4); return v; }
static int32_t rd16(const uint8_t* p, uint32_t o) { uint16_t v; memcpy(&v, p + o, 2); return v; }

// 1.13c D2Client+0xA34B0. EAX = pt (+0xC x u16, +0xE y u16), ESI = panel (+0x2C x, +0x30 y0, +0x34 w, +0x50 count,
// rows from +0x158 stride 0x110: +0 height, +0x1C active==1). Returns first hit row index or -1.
extern "C" int32_t d2_client_a34b0(const uint8_t* panel, const uint8_t* pt) {
    if (!panel) return -1;
    int32_t acc = 0, n = rd32(panel, 0x50);
    int32_t px = rd16(pt, 0xC), py = rd16(pt, 0xE);
    int32_t x0 = rd32(panel, 0x2C), w = rd32(panel, 0x34), y0 = rd32(panel, 0x30);
    for (int32_t i = 0; i < n; i++) {
        const uint32_t r = 0x158u + (uint32_t)i * 0x110u;
        int32_t y = y0 + rd32(panel, r) + acc;
        if (rd32(panel, r + 0x1C) == 1 && x0 + 0xF < px && px < x0 - 0xF + w && y - 0xB < py && py < y + 4) return i;
        acc += rd32(panel, r);
    }
    return -1;
}
