// The unit-sound resolver the play path asks: packet 0x2C {unitType, classId, event} -> a Sounds.txt row.
// 1.13c D2Client+0x23F20 (the event switch: fixed rows, the rogue-hireling pairs, the NPC greeting) and its default case
// 1.13c D2Client+0x23200 (a player unit: fixed rows, the class voice table behind D2Client+0xF7694[class], +0x8..+0x24).
// Returns the row, or -1 when retail plays nothing or the row needs state not passed (events 0x11, 0x5C).
#include <cstdint>
#include <emscripten.h>

namespace {
// D2Client+0xF7694[class] +0x08,+0x0C,+0x10,+0x14,+0x18,+0x1C,+0x20,+0x24, per class 0..6.
const int32_t kVoice[7][8] = {
    { 2953, 2957, 2955, 2934, 2936, 2959, 2937, 2961 }, { 3265, 3269, 3267, 3246, 3248, 3271, 3249, 3273 },
    { 3109, 3113, 3111, 3090, 3092, 3115, 3093, 3117 }, { 3187, 3191, 3189, 3168, 3170, 3193, 3171, 3195 },
    { 3031, 3035, 3033, 3012, 3014, 3037, 3015, 3039 }, { 3421, 3425, 3423, 3402, 3404, 3427, 3405, 3429 },
    { 3343, 3347, 3345, 3324, 3326, 3349, 3327, 3351 } };
}

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_unit_sound_event_row(int32_t unitType, int32_t classId, int32_t event, int32_t isLocalPlayer,
                                                                       int32_t monsterTaunt, int32_t skillStartRow) {
    const int32_t ev = event & 0xFFFF;
    switch (ev) {   // 0x23F20: any unit type
    case 0x0A: return 2673;
    case 0x0D: return 2634;
    case 0x0E: return 2635;
    case 0x5A: return 4379;
    case 0x5D: return 2553;
    case 0x54: return classId == 271 ? 4615 : 4625;
    case 0x55: return classId == 271 ? 4612 : 4616;
    case 0x56: return classId == 271 ? 4613 : 4619;
    case 0x57: return classId == 271 ? 4614 : 4622;
    case 0x0F: return classId == 271 ? 4290 : -1;
    case 0x10: return unitType == 1 && monsterTaunt > 0 ? monsterTaunt : -1;
    case 0x0C: return skillStartRow > 0 ? skillStartRow : -1;
    case 0x5B: return isLocalPlayer ? 8 : -1;
    case 0x12: return classId == 271 ? 4615 : 4625;
    case 0x11: case 0x5C: return -1;
    default: break;
    }
    // 0x23200: only a player unit of class 0..6 reaches the player table
    if (unitType != 0 || classId < 0 || classId > 6) return -1;
    const int32_t* v = kVoice[classId];
    switch (ev) {
    case 2: return 7; case 3: return 10; case 4: return 11; case 5: return 12; case 6: return 13;
    case 7: return 2230; case 8: return 2231; case 9: return 9; case 11: return 228;
    case 0x13: return v[0]; case 0x14: return v[4]; case 0x15: return v[1]; case 0x16: return v[2];
    case 0x17: return v[3]; case 0x18: return v[5];
    default: break;
    }
    if (ev >= 0x19 && ev <= 0x20) return v[6] + ev * 2 - 0x32;
    if (ev >= 0x21 && ev <= 0x53) return v[7] + ev - 0x21;
    return -1;
}
