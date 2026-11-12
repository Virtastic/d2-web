// 1.13c D2Game+0xEA4C0: object operate callback "relic chest, eye" (table entry); the shape is
// d2game_relicchest.h: drop code "qey ", player-visit callback 0xE4890, data count +0x10 and byte +0x24.
#pragma once
#include "d2game_relicchest.h"

namespace gea4c0 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    return grelicchest::Operate(e, op, { 0x20796571, 0xE4890, 0x10, 0x24 });
}
}
