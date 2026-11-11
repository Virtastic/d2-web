// 1.13c D2Game+0xCE520: the skill packet handler twin of 0xCE5A0 (thunk 0xA3B2 for 0xA3AC). Disassembled convention: ECX = game,
// EDX = unit, stack word0 = packet pointer, word1 = length; ret 8. 0xCBF50 (EAX = packet, ESI = unit; stack game, length, &word0,
// &word1) rewrites both argument slots; a non-zero result returns. A zero 0xA3B2(unit) returns 3; otherwise stat 0x148 is bumped
// (0xA3E2 unit, 0x148, 0 then 0xA472 unit, 0x148, value + 1, 0), and 0xCDF30 takes EAX = a second 0xA3B2(unit), ECX = unit and
// stack game, word0 & 0xFFFF, word1 & 0xFFFF (the 0xA3B2 call's leftover pushes); returns 0.
#include <cstdint>
#include "d2game_ce520.h"

uint32_t d2game_ce520_port(uint32_t game, uint32_t unit, uint32_t pkt, uint32_t len, const D2GameCE520Env* e) {
    uint32_t w0 = pkt, w1 = len;
    const uint32_t r = e->f_cbf50(w0, unit, game, w1, &w0, &w1);
    if (r) return r;
    if (!e->f_a3b2(unit)) return 3;
    e->f_a472(unit, 0x148, e->f_a3e2(unit, 0x148, 0) + 1, 0);
    const uint32_t sk = e->f_a3b2(unit);
    e->f_cdf30(sk, unit, game, w0 & 0xFFFF, w1 & 0xFFFF);
    return 0;
}
