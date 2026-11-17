// 1.13c D2Client control-panel bar arithmetic, shared by the ported draw functions and the browser exports.
#pragma once
#include <cstdint>
namespace d2cl_hud {
// 1.13c D2Client+0x26970: the experience bar's width in pixels (the bar is 0x77 wide). `have` = exp - level start (0 when below),
// `total` = next level - level start. Unsigned math as retail: values >= 0x226B902 are scaled down by 7 bits first; a result over
// 0x77 (exp past the next level) draws nothing. total == 0 draws nothing (the caller returns before the divide).
static inline uint32_t expBarPixels(uint32_t have, uint32_t total) {
    if (total == 0) return 0;
    uint32_t num = have, den = total;
    if (num >= 0x226b902u) { num >>= 7; den >>= 7; }
    if (den == 0) return 0;   // retail would trap here
    uint32_t bar = (num * 0x77u) / den;
    return bar > 0x77 ? 0 : bar;
}
// 1.13c D2Client+0x27BD0 (life) / +0x27A90 (mana): rows of the orb's fluid drawn, of 80. Mana clamps cur to max first
// (`clampToMax`); a life orb of a living player (unit type 0, mode != 0x11: `lifeFloor`) shows at least 2 rows when not empty.
static inline int32_t orbFill(int32_t cur, int32_t mx, bool clampToMax, bool lifeFloor) {
    if (mx == 0) return 0;
    if (clampToMax && cur > mx) cur = mx;
    int32_t fill = (int32_t)((uint32_t)cur * 80u) / mx;
    if (lifeFloor && fill > 0 && fill <= 2) fill = 2;
    return fill;
}
// 1.13c D2Client+0x278B0: the stamina bar's width (of 0x66) and colour slot (0 red, 1 normal, 2 blue). `cur` is the smoothed
// current value, `mx` the maximum; `over` = the 0x18 state, which (like cur > max + 5) draws blue at full width.
struct Stamina { int32_t width; uint32_t slot; };
static inline Stamina staminaBar(int32_t cur, int32_t mx, bool over) {
    uint32_t col = 1;
    if (mx + 5 < cur || over) { col = 2; mx = cur; }
    if (mx > 0) { const int32_t w = (int32_t)((uint32_t)cur * 0x66u) / mx; if (w < 0x19) col = 0; return {w, col}; }
    return {0, 0};
}
}
