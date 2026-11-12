// 1.13c D2Game+0xEA5F0: object operate callback "relic chest, heart" (table entry); the shape is
// d2game_relicchest.h: drop code "qhr ", player-visit callback 0xE47D0, data count +0x18 and byte +0x26.
#pragma once
#include "d2game_relicchest.h"

namespace gea5f0 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    return grelicchest::Operate(e, op, { 0x20726871, 0xE47D0, 0x18, 0x26 });
}
}
