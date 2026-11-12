// 1.13c D2Game+0x5A7C0: quest-record init callback (table entry); the shape is d2game_qinit.h, the words are
// this quest's handlers and message table.
#pragma once
#include "d2game_qinit.h"

namespace g5a7c0 {
template <class E> uint32_t Init(E& e, uint32_t q)
{
    return gqinit::Init(e, q, { 0x596B0, 0x597B0, 0x10A9A8, 0x56EA0, 0x59680 });
}
}
