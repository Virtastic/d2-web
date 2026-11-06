// 1.13c D2Client+0x5AC70 (113 B, ESI = wide text buffer, stack arg0 / arg1 / arg2, ret 0xC): font := 6 (0xD3A8, answers the previous font),
// while the text is wider than 0x42 (signed) zero its wide chars from +0x3C downwards, re-measuring after each; x = arg0 + (0x16 - width/2),
// 0 when that is negative; draw 0xD37E(text, x, arg1, arg2, 0); font := the previous one.
#include <emscripten.h>
#include <cstdint>
struct D2c5ac70Env {
    uint32_t (*font)(uint32_t id);
    int32_t (*width)(uint16_t* text);
    void (*draw)(uint16_t* text, int32_t x, uint32_t a1, uint32_t a2, uint32_t z);
};
extern "C" void d2c_port_5ac70(uint16_t* text, int32_t a0, uint32_t a1, uint32_t a2, const D2c5ac70Env* e) {
    uint32_t prev = e->font(6);
    uint8_t* edi = (uint8_t*)text + 0x3e;
    if (e->width(text) > 0x42) {
        do { edi -= 2; edi[0] = 0; edi[1] = 0; } while (e->width(text) > 0x42);
    }
    int32_t w = e->width(text);
    int32_t half = w / 2;
    int32_t x = (int32_t)((uint32_t)a0 + (uint32_t)(0x16 - half));
    if (x < 0) x = 0;
    e->draw(text, x, a1, a2, 0);
    e->font(prev);
}
