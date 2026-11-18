// 1.13c D2Client+0x68C30: releases the static 0x80C-byte cache block at +0x114CD8:
// callee 0x68810(block), total(+0x11C044) -= block[8], free block[4] (line 0xE50) when set, then zero the block.
#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_68c30(uint32_t blk, uint32_t total, const Env* e) {
    e->call(0x68810, 0, 0, 0, 0, 0, 0xB2000000u | 0x114CD8, 0);
    W(total, 0) = R(total, 0) - R(blk, 8);
    if (R(blk, 4)) e->freeMem(R(blk, 4), 0xE50);
    for (uint32_t i = 0; i < 0x203; ++i) W(blk, i * 4) = 0;
}
