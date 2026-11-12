// 1.13c D2Game+0x63CC0: quest-record init callback (table entry); the shape is d2game_qinit.h, the words are
// this quest's handlers and message table.
#pragma once
#include "d2game_qinit.h"

namespace g63cc0 {
template <class E> uint32_t Init(E& e, uint32_t q)
{
    return gqinit::Init(e, q, { 0x62E50, 0x62F00, 0x107F70, 0x60810, 0x62DF0 });
}
}
