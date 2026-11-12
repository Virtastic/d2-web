// 1.13c D2Game+0xEA390: object operate callback "relic chest, brain" (table entry); the shape is
// d2game_relicchest.h: drop code "qbr ", player-visit callback 0xE4830, data count +0x14 and byte +0x25.
#pragma once
#include "d2game_relicchest.h"

namespace gea390 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    return grelicchest::Operate(e, op, { 0x20726271, 0xE4830, 0x14, 0x25 });
}
}
