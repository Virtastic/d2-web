// 1.13c D2Client+0x75D30 (790 B): hover tooltips for the party panel: the local party badge (box
// panel_x+0xe3..+0x118, y 0x18..0x23 minus panel_y) and each list entry button (+4/+8/+0x14/+0x18, next at +0xcd).
// Globals: [11BFD8] button list, [11BC14] party list (+0x22 id word, +0x80 next), [11B9A0]/[11B9A4] panel x/y,
// [11B828]/[11B824] mouse x/y, [11BFF8] the colour BYTE (Ordinal 10190 result, AL only).
// Each tooltip line: 0x75370(id, w) gives a width W; Ordinal 10003 (0xD1EC, ECX = id, no stack) gives the text;
// Ordinal 10150 (0xD37E, ECX = text, EDX = panel_x + W, stack y, 0, 0) draws it.
#include <cstdint>
#include <emscripten.h>

struct D2c75d30Host {
    uint32_t (*ord10190)(int32_t, int32_t, int32_t);                      // 0xD390, 3 stack words
    void (*sub_75220)(uint32_t, uint32_t, uint32_t, uint32_t);              // stdcall, 4 words
    int32_t (*sub_75370)(uint32_t, uint32_t);                               // stdcall, 2 words
    uint32_t (*ord10003)(uint32_t id);                                      // 0xD1EC: ECX = string id
    void (*ord10150)(uint32_t text, int32_t x, int32_t y, uint32_t, uint32_t);  // 0xD37E: ECX, EDX, stack y, 0, 0
};

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static inline int16_t rd16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }

static void line(const D2c75d30Host* h, uint32_t id, uint32_t w, int32_t px, int32_t y) {
    int32_t W = h->sub_75370(id, w);
    h->ord10150(h->ord10003(id), px + W, y, 0, 0);
}

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_75d30(uint32_t btn_bfd8, uint32_t pl_bc14, int32_t mx_b828, int32_t my_b824, int32_t px_b9a0,
                     int32_t py_b9a4, uint8_t* col_bff8, const D2c75d30Host* h) {
    *col_bff8 = (uint8_t)h->ord10190(0, 0, 0);
    if (pl_bc14) {
        int n = 0;
        for (uint32_t q = pl_bc14; q; q = rd32(q + 0x80)) if (rd16(q + 0x22) == rd16(pl_bc14 + 0x22)) n++;
        if (n > 1 && rd16(pl_bc14 + 0x22) != -1 && px_b9a0 + 0xe3 <= mx_b828 && mx_b828 <= px_b9a0 + 0x118 &&
            0x18 - py_b9a4 <= my_b824 && my_b824 <= 0x23 - py_b9a4) {
            h->sub_75220(0x1018, 0, 0xd5, 0);
            line(h, 0x1018, 0xd5, px_b9a0, 0x37 - py_b9a4);
        }
    }
    for (uint32_t b = btn_bfd8; b; b = rd32(b + 0xcd)) {
        int32_t x = (int32_t)rd32(b + 4) + px_b9a0, y = (int32_t)rd32(b + 8) - py_b9a4;
        if (x <= mx_b828 && mx_b828 <= x + 0x35 && y - 0x14 <= my_b824 && my_b824 <= y && rd32(b + 0x14) == 0) {
            uint32_t k = rd32(b + 0x18), s;
            if (k == 0) s = 0x1004; else if (k == 2) s = 0x1006; else if (k == 4) s = 0x1008; else continue;
            h->sub_75220(s, s + 1, 0xda, 1);
            line(h, s, 0xda, px_b9a0, 0x32 - py_b9a4);
            line(h, s + 1, 0xda, px_b9a0, 0x41 - py_b9a4);
        }
    }
}
