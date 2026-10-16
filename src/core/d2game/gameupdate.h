// 1.13c D2Game+0x2E050 GAME_UpdateProgress (EAX = game; ret plain). Written from the 1.13c decompile ONE definition, included by the capture (the retail capture tool/game-update.inc, i686)
// as the model retail must agree with, and by the wasm replay (game_gameupdate.cpp). Every callee is a logged seam call; the sequencer has no scripted results.
#pragma once
#include <cstdint>
enum { UP_LOG = 20, UP_IN = 8, UP_OUT = 4 + 5 * UP_LOG };
struct UpRec {
    uint32_t frame, trig, act[5], pad;                  // game +0xA8 before the call; +0x1DC8 debug trigger; +0xBC..+0xCC act pointers (0 or nonzero); pad
    uint32_t frameOut, exited, nCalls, over, log[UP_LOG][5];   // game +0xA8 after; _exit taken; seam-call log: id, args
};
static_assert(sizeof(UpRec) == 4 * (UP_IN + UP_OUT), "UpRec layout (shared with D2VecGameUpdate)");
enum { UP_GAME = 0xA1, UP_ACT = 0xB1 };   // symbolic handles: game, act i = UP_ACT + i

struct UpCtx {
    UpRec* r;
    void call(uint32_t id, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0) {
        if (r->nCalls >= UP_LOG) { ++r->over; r->nCalls++; return; }
        uint32_t* q = r->log[r->nCalls++]; q[0] = id; q[1] = a; q[2] = b; q[3] = c; q[4] = d;
    }
};

static inline void UP_RUN(UpCtx& x)
{
    UpRec& r = *x.r; r.frameOut = r.frame + 1; r.exited = r.nCalls = r.over = 0;
    for (auto& q : r.log) for (auto& w : q) w = 0;
    // trigger 1 / 2 are a null / bad-pointer write (access violation, never taken in play); 3 asserts and exits; 4 an oversized allocation, then on
    if (r.trig == 3) { x.call(1, 0xDC7); x.call(2, 0x6FD19FF9u, 0); x.call(3, 0xFFFFFFFFu); r.exited = 1; return; }
    if (r.trig == 4) x.call(4, 0x6FD1CD98u, 0x70000000u, 0xDCB, 0);
    const int32_t f = (int32_t)r.frameOut;
    x.call(5, UP_GAME); x.call(6, UP_GAME); x.call(7, UP_GAME); x.call(8, UP_GAME); x.call(9, UP_GAME); x.call(10, UP_GAME); x.call(11, UP_GAME);
    if (f % 20 == 0) x.call(12, UP_GAME);
    if (f % 12 == 0) x.call(13, UP_GAME);
    if (f % 11 == 0) for (uint32_t i = 0; i < 5; i++) if (r.act[i]) x.call(14, UP_ACT + i);
    x.call(15, UP_GAME);
}
