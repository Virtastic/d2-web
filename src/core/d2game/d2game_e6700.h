// 1.13c D2Game+0xE6700: quest-record init callback (table entry); the shape is d2game_qinit.h, the words are
// this quest's handlers and message table.
#pragma once
#include "d2game_qinit.h"

namespace ge6700 {
template <class E> uint32_t Init(E& e, uint32_t q)
{
    return gqinit::Init(e, q, { 0xE55F0, 0xE56E0, 0xFFBE8, 0xE3040, 0xE55C0 });
}
}
