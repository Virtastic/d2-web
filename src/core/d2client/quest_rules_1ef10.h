// 1.13c D2Client+0x1EF10: the quest log's two small decisions, shared by the panel port and the browser export.
#pragma once
#include <cstdint>
namespace d2cl_quest {
// The quest the non-selected act tabs ask about (act tab 0 always draws): act N's tab shows once quest kActQuestion[N]'s state 0 is set.
static const uint32_t kActQuestion[5] = {0, 7, 0xF, 0x17, 0x1A};
// act = 0..4; `done` = QUESTRECORD_GetQuestState(rec, kActQuestion[act], 0) != 0; the base game has four acts and shows
// act 4's tab only with the expansion.
static inline bool tabShown(uint32_t act, uint32_t selAct, bool expansion, bool done) {
    if (!expansion && act > 3) return false;
    if (act == selAct || act == 0) return true;
    return done;
}
// The cel the icon row draws for a record mode (0 completing, 1 done, 2 locked, 3 in progress). `prev` = this row is the previously
// clicked one. Mode 1 swaps to the questdone sheet (sheet 1, frame = icon index) while pressed. Mode 0 clamps the animation at 0x18.
struct Icon { uint32_t sheet, frame; };
static inline Icon iconFor(uint32_t mode, bool prev, uint32_t anim, uint32_t iconIndex) {
    switch (mode) {
    case 0: return {0, anim >= 0x19 ? 0x18u : anim};
    case 1: return prev ? Icon{1, iconIndex} : Icon{0, 0x18};
    case 2: return {0, 0x1A};
    default: return {0, prev ? 0x19u : 0u};
    }
}
}
