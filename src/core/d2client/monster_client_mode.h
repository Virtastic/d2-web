#pragma once
// The client's own end of a monster animation, without a sequence: D2Client's per-unit update (1.13c
// D2Client+0x4F4A0) picks a mode row (+0x4BFF0), runs the row's clock rule (+0x4EE40) and, when the
// row's end rule fires, hands the unit to its end-of-animation hook (+0x4DA60), which changes the
// mode on the client's own unit (+0x2A6C0, then D2Common #10595). The server is not asked: a
// monster whose skill animation ends turns neutral on the client the tick the end is seen.
// Pure over resolved fields so the engine and a native test share it.
#include <cstdint>

namespace d2c_monster_mode {

// 1.13c D2Client+0xF21A8 (modes 0-15) and +0xF2268 (the modes MonStats2 marks as moving), int32
// triples: { path-step kind, clock rule, end rule }. +0xF22B0 and +0xF22BC are rows 6 and 7 of the
// second table, read whole for two class/mode pairs.
inline constexpr int32_t kRows[2][16][3] = {
    { {0,2,0},{0,0,0},{1,0,1},{0,1,2},{2,1,3},{2,1,3},{0,1,2},{0,1,2},
      {0,1,2},{0,1,2},{0,2,0},{0,0,0},{0,2,0},{1,2,2},{2,1,2},{1,0,1} },
    { {1,2,0},{0,1,2},{0,1,2},{0,1,2},{1,0,4},{1,0,4},{2,1,3},{1,0,4},
      {1,0,4},{1,0,4},{0,2,0},{1,2,0},{0,1,2},{0,1,2},{0,1,2},{0,1,2} },
};

// 1.13c D2Client+0x4BFF0: `movingModes` is MonStats2 +0x104 (two bytes, a bit per mode);
// `deathRowAlt` is D2Common #10707's answer, read only for mode 0.
inline const int32_t* row(int32_t classId, uint32_t mode, const uint8_t movingModes[2], bool deathRowAlt) {
    if (mode >= 16) return nullptr;
    if (mode == 11 && (classId == 0xF3 || classId == 0x14D)) return kRows[1][6];
    if (mode == 10 && (classId == 0x21F || classId == 0x220 || classId == 0x23A)) return kRows[1][7];
    if (movingModes[mode >> 3] & (1u << (mode & 7))) return kRows[1][mode];
    if (mode == 0 && deathRowAlt) return kRows[1][0];
    return kRows[0][mode];
}

// The animation fields of a unit without a sequence (+0x44 frame, +0x48 count, +0x4C speed).
struct Clock { int32_t frame; int32_t count; int16_t speed; };

// 1.13c D2Common+0x2EF30 (#10426), no sequence.
inline bool atEnd(const Clock& c) { return c.count <= (int32_t)c.speed + c.frame; }

// 1.13c D2Common+0x32460 (#10853) for a monster without a sequence: the start frame +0x32220 gives
// is 0 for every type but a player.
inline void advance(Clock& c) {
    c.frame += c.speed;
    while (c.count > 0 && c.frame >= c.count) c.frame -= c.count;
}

// 1.13c D2Client+0x4EE40 (the clock rules) and 0x4F4A0's end rules 2 and 3, which read its result.
// Returns 1 when the end fires, 0 when it does not, -1 for the rules this does not model (end rules 1
// and 4 read the path step, rule 0 has no clock-side end; mode 0's DT callback is left to its caller).
inline int step(Clock& c, const int32_t* r, uint32_t mode, uint32_t& flags) {
    if (!r || (r[2] != 2 && r[2] != 3)) return -1;
    int32_t ret = 0;
    switch (r[1]) {
    case 0: advance(c); break;
    case 1: if (atEnd(c)) ret = 1; else advance(c); break;
    case 2:
        if (atEnd(c)) { c.frame = c.count - 0x100; c.speed = 0; if (mode == 0) flags |= 0x20; }
        else advance(c);
        break;
    case 3: c.frame = 0x100; c.speed = 0; break;
    default: break;
    }
    return ret != 0;
}

// 1.13c D2Client+0x4DA60 for a monster: the mode the client's unit changes to at the end, or -1 when
// it keeps its mode. `baseId` is MonStats +0x02 of the class (+0x1700), `splClientEnd` MonStats
// +0x1A7 of the base row (+0x1230); `async` is unit flag 0x200000 (+0xC4 bit 21), `pathFlag` the
// extended flag 0x2000 (+0xC8 bit 13, D2Common #10212's).
// Mode 13 takes another branch (back to mode 3) that never reaches here: its rows' end rule never fires.
inline int32_t endMode(uint32_t mode, int32_t baseId, bool splClientEnd, bool async, bool pathFlag) {
    if (splClientEnd) {
        switch (mode) {
        case 2: if (baseId == 0xE7 || baseId == 0x76) return -1; break;
        case 4: if (baseId == 0x193 || baseId == 0x194) return 8; break;
        case 8: if (baseId == 0x6E) return -1; [[fallthrough]];
        case 5: case 9: if (baseId == 0x193 || baseId == 0x194) return 8; break;
        case 10: if (baseId == 0x88) return 0xB; break;
        case 11: if (baseId == 0x88) return -1; break;
        case 12: if (baseId == 0x11C) return -1; break;
        case 14: if (baseId == 0xF7) return -1; break;
        default: break;
        }
    }
    if (async || !pathFlag) return 1;
    return -1;
}

}  // namespace d2c_monster_mode
