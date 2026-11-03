// D2Client+0x601A0: centered label (decompile-derived). EBX = narrow string; stack p1 (unused), p2 = y + 10, p3 = colour (>12 becomes 0), p4 = centre x
// (the fourth stack word: ret 0xC pops three, the caller leaves its x there). Interface-shaped: the D2Win/D2Lang calls are passed in.
#include <cstdint>
struct D2c601a0Env {
    uint32_t (*setFont)(uint32_t font);                                            // 0xD3A8, ECX = font; returns the previous one
    uint32_t (*textWidth)(const uint16_t* text, uint32_t y, uint32_t colour, uint32_t zero);   // 0xD35A, ECX = text
    void (*drawText)(const uint16_t* text, uint32_t x);                            // 0xD37E, ECX = text, EDX = x
};
// 1.13c D2Client+0x601A0
void d2c_601a0(const char* s, uint32_t p1, int32_t p2, uint8_t p3, int32_t p4, const D2c601a0Env* e) {
    (void)p1;
    uint32_t len = 0;
    while (s[len]) len++;
    if (len == 0) return;
    if (p3 > 0xc) p3 = 0;
    uint32_t prev = e->setFont(6);
    uint16_t buf[23];
    for (int i = 0; i < 23; i++) buf[i] = 0;
    for (uint32_t i = 0; i < 22 && i < len; i++) buf[i] = (uint8_t)s[i];
    int32_t w = (int32_t)e->textWidth(buf, (uint32_t)(p2 - 10), p3, 0);
    e->drawText(buf, (uint32_t)(p4 - ((w + (w < 0 ? 1 : 0)) >> 1)));
    e->setFont(prev);
}
