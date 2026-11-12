// 1.13c D2Game+0xC5730: quest-record init callback (table entry); the shape is d2game_qinit.h, the words are
// this quest's handlers and message table.
#pragma once
#include "d2game_qinit.h"

namespace gc5730 {
template <class E> uint32_t Init(E& e, uint32_t q)
{
    return gqinit::Init(e, q, { 0xC4690, 0xC4780, 0x102DE8, 0xC25E0, 0xC25D0 });
}
}
