// Browser exports of the stats-panel cell rules of 1.13c D2Client+0xBCEA0 (the same header the port uses).
#include <cstdint>
#include <emscripten.h>
#include "stats_cell_bcea0.h"
#include "party_row_76790.h"
#include "quest_rules_1ef10.h"

extern "C" {
// colour of a cell drawn against its base (stats 7, 9, 11 and the attribute cells)
EMSCRIPTEN_KEEPALIVE int32_t d2_client_cell_colour(int32_t value, int32_t base, int32_t col) {
    return (int32_t)d2cl_cell::cmpColour(value, base, (uint32_t)col);
}
// party-panel row colour mode (D2Client+0x76790): 2 my party, 8 another party, 1 hostile to me, 0 none
EMSCRIPTEN_KEEPALIVE int32_t d2_client_party_row_mode(int32_t share, int32_t pt, int32_t mine, int32_t hostile) {
    return (int32_t)d2cl_party::rowMode((uint32_t)share, (uint16_t)pt, (uint16_t)mine, hostile != 0);
}
// quest log (D2Client+0x1EF10): bit n set when act n+1's tab draws. `stateMask` bit i = the state-0 flag of quest kActQuestion[i]
EMSCRIPTEN_KEEPALIVE int32_t d2_client_quest_tabs(int32_t selAct, int32_t expansion, int32_t stateMask) {
    int32_t out = 0;
    for (uint32_t a = 0; a < 5; ++a)
        if (d2cl_quest::tabShown(a, (uint32_t)selAct, expansion != 0, ((uint32_t)stateMask >> a) & 1)) out |= 1 << a;
    return out;
}
// quest log icon cel: (sheet << 16) | frame; sheet 1 = questdone.dc6
EMSCRIPTEN_KEEPALIVE int32_t d2_client_quest_icon(int32_t mode, int32_t prev, int32_t anim, int32_t iconIndex) {
    const d2cl_quest::Icon i = d2cl_quest::iconFor((uint32_t)mode, prev != 0, (uint32_t)anim, (uint32_t)iconIndex);
    return (int32_t)((i.sheet << 16) | i.frame);
}
// resist cell: returns the colour; *out receives the drawn value
EMSCRIPTEN_KEEPALIVE int32_t d2_client_resist_cell(int32_t value, int32_t maxStat, int32_t col, int32_t adj, int32_t* out) {
    return (int32_t)d2cl_cell::resistCell(value, maxStat, (uint32_t)col, adj, out);
}
}
