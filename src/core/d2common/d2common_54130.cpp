// 1.13c D2Common+0x54130 -- field compiler (ecx = text, edx = record, stack word 0 = slot 0..7): clears the kind byte at +0x180[slot] and sets the index word at +0x188[slot] to 0xFFFF; when the word at +0x170[slot] is not negative and the text is not null, a text of up to three characters is padded with spaces to a four-character code and looked up through Fog 10213 in the table at image offset 0x9FF04, whose answer becomes the kind byte; a longer text, a negative answer or kind 0xE stores kind 0xE and the Fog 10217 index of the text in the table at 0xA0A54.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2common_9ff04;
__attribute__((weak)) uint32_t g_d2common_a0a54;
namespace d2common {
static uint32_t rd16(GE& e, uint32_t a) { return e.rd8(a) | (e.rd8(a + 1) << 8); }
static void wr16(GE& e, uint32_t a, uint32_t v) { e.wr8(a, v & 0xff); e.wr8(a + 1, (v >> 8) & 0xff); }
uint32_t f54130(GE& e, uint32_t ea, uint32_t s, uint32_t rec, uint32_t slot)
{
    if (!rec || (int32_t)slot < 0 || (int32_t)slot >= 8) return ea;
    e.wr8(rec + 0x180 + slot, 0);
    wr16(e, rec + 0x188 + slot * 2, 0xffff);
    if (rd16(e, rec + 0x170 + slot * 2) & 0x8000) return ea;
    if (!s) return ea;
    uint32_t n = 0;
    while (e.rd8(s + n)) n++;
    uint32_t v;
    if (n <= 3) {
        uint32_t code = 0x20202020u;
        for (uint32_t k = 0; k < n; k++) code = (code & ~(0xffu << (8 * k))) | (e.rd8(s + k) << (8 * k));
        v = e.h(0x9240, 0, 0, 0, 0, 0, g_d2common_9ff04, code, 1);
        e.wr8(rec + 0x180 + slot, v & 0xff);
        if (!(v & 0x80) && (v & 0xff) != 0xe) return v;
    }
    e.wr8(rec + 0x180 + slot, 0xe);
    v = e.h(0x923a, 0, 0, 0, 0, 0, g_d2common_a0a54, s, 1);
    wr16(e, rec + 0x188 + slot * 2, v);
    return v;
}
}
