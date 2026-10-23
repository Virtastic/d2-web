// 1.13c D2Client+0xAA090: clamp act-progress level (EAX in) against the max (DAT 0x6FB8E6D0): >= max -> 0. Then step it down while the
// boss quest-state flag 0 is clear (Ordinal_10174 = QUESTRECORD_GetQuestState, quest 0x1C/0x17/0xF for levels 4/3/2, quest 7 for any of 1..4).
// Result stored to DAT 0x6FBACDD6.
#include <stdint.h>
#include <emscripten.h>
typedef uint32_t (*D2cQuest)(uint32_t rec, uint32_t quest, uint32_t flag);  // Ordinal_10174
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_aa090(uint32_t lvl, uint32_t max, uint32_t rec, D2cQuest q) {
    if (lvl >= max) lvl = 0;
    switch (lvl) {
    case 4: if (!q(rec, 0x1C, 0)) lvl = 3; [[fallthrough]];
    case 3: if (!q(rec, 0x17, 0) && lvl >= 3) lvl = 2; [[fallthrough]];
    case 2: if (!q(rec, 0xF, 0) && lvl >= 2) lvl = 1; [[fallthrough]];
    case 1: if (!q(rec, 7, 0)) lvl = 0; return lvl;
    default: return lvl;
    }
}
