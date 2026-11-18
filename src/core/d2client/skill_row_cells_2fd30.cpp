// 1.13c D2Client+0x2FD30: which cells a character-screen skill row draws. The damage cell needs a non-null entry of the
// 0xF5210 calc table (wDescDam < 0x90), the attack-rating cell a non-null entry of the 0xF52A0 table (wDescAtt < 0x48).
// The null entries are those the retail tables hold (the same sets the panel's JavaScript carried).
#include <cstdint>
#include <emscripten.h>

namespace {
bool damNull(uint32_t d) {
    if (d == 0 || d == 0x5a || d == 0x69 || d == 0x89) return true;
    return (d >= 0x19 && d <= 0x24) || (d >= 0x2a && d <= 0x35);
}
bool attNull(uint32_t a) { return a == 0 || (a >= 6 && a <= 0xe); }
}

extern "C" {
// bit 0: the damage cell is drawn, bit 1: the attack-rating cell is drawn
EMSCRIPTEN_KEEPALIVE int32_t d2_client_skill_row_cells(int32_t descdam, int32_t descatt) {
    int32_t out = 0;
    if ((uint32_t)descdam < 0x90 && !damNull((uint32_t)descdam)) out |= 1;
    if ((uint32_t)descatt < 0x48 && !attNull((uint32_t)descatt)) out |= 2;
    return out;
}
}
