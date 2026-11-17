// Browser exports of the control-panel bar rules (hud_bar_rules.h): the experience bar of 1.13c D2Client+0x26970, the life and mana
// orb fills of +0x27BD0 / +0x27A90 and the stamina bar of +0x278B0. The same header the ported draw functions run.
#include <cstdint>
#include <emscripten.h>
#include "hud_bar_rules.h"
uint32_t d2cmp_sub_9d30(const uint8_t* pal, uint32_t count, int32_t p3, int32_t p4, int32_t p5);   // src/core/d2cmp/d2cmp_sub_9d30.cpp

extern "C" {
// pixels of the 0x77-wide experience bar for exp `have` into a level span `total` (both already relative to the level start)
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_exp_bar_pixels(uint32_t have, uint32_t total) { return d2cl_hud::expBarPixels(have, total); }
// rows (of 80) of an orb's fluid: kind 0 life (floor 2 rows when `alive`), 1 mana (cur clamped to max)
EMSCRIPTEN_KEEPALIVE int32_t d2_client_orb_fill(int32_t cur, int32_t mx, int32_t kind, int32_t alive) {
    return d2cl_hud::orbFill(cur, mx, kind == 1, kind == 0 && alive != 0);
}
// stamina bar: (colourSlot << 16) | width, width of 0x66
EMSCRIPTEN_KEEPALIVE int32_t d2_client_stamina_bar(int32_t cur, int32_t mx, int32_t over) {
    const d2cl_hud::Stamina s = d2cl_hud::staminaBar(cur, mx, over != 0);
    return (int32_t)((s.slot << 16) | (uint32_t)s.width);
}
// the stamina bar's three slot colours as 278B0 latches them on its first draw: D2Win #10190 (D2Win+0xED70 forwards to
// D2CMP+0x9D30 over the module's 256 four-byte entries) of red (0xFF,0,0), normal (0xF4,0xC0,0x4C) and blue (0,0,0xFF);
// the low byte of each answer is kept. Returns slot0 | slot1 << 8 | slot2 << 16 for the 256-entry palette `pal`.
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_stamina_colours(const uint8_t* pal) {
    return (d2cmp_sub_9d30(pal, 0x100, 0xff, 0, 0) & 0xff) | (d2cmp_sub_9d30(pal, 0x100, 0xf4, 0xc0, 0x4c) & 0xff) << 8
        | (d2cmp_sub_9d30(pal, 0x100, 0, 0, 0xff) & 0xff) << 16;
}
}
