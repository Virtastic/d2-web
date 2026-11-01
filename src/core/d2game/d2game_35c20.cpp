// 1.13c D2Game+0x35C20: per-act NPC-intro stage text. CL = stage; EAX = unit and EDX = pGame pass straight through to 0x34D00
// (set the intro bits of one NPC id list). Stages 0, 1, 2, 4 pick a fixed id list; 3 and anything else do nothing.
// Port seam as in d2game_batch_34e0.cpp: r[0]=unit, r[1]=stage, r[2]=pGame, r[5]=D2GameBatch20Env.
#include <cstdint>
#include "d2game_batch20env.h"

extern "C" void D2Game_34d00(uint32_t* r);

namespace {
// the four id arrays at 1.13c 0x6FD2DF90 / DFA8 / DFD4 / DFF0
const uint32_t kStage0[6]  = { 148, 154, 150, 147, 155, 265 };
const uint32_t kStage1[11] = { 175, 176, 177, 178, 202, 200, 210, 201, 198, 199, 244 };
const uint32_t kStage2[7]  = { 245, 252, 253, 254, 255, 264, 297 };
const uint32_t kStage4[7]  = { 520, 521, 511, 512, 513, 514, 515 };
}

extern "C" void D2Game_35c20(uint32_t* r) {
    const uint32_t* ids; uint32_t n;
    switch ((uint8_t)r[1]) {
    case 0: ids = kStage0; n = 6; break;
    case 1: ids = kStage1; n = 11; break;
    case 2: ids = kStage2; n = 7; break;
    case 4: ids = kStage4; n = 7; break;
    default: r[4] = 0; return;
    }
    uint32_t p[8] = { r[0], r[2], (uint32_t)(uintptr_t)ids, n, 0, r[5] };
    D2Game_34d00(p);
    r[4] = 0;
}
