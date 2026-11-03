// 1.13c D2Game+0x6C580: the 0x6C200 body (see d2game_6c200.h) with class id 0x206D706F ('opm '), the fatal 0x8B2 and ring missile code 0x2D (0x6C200: 0x206D7067, 0x8F4, 0x30).
#pragma once
#include "d2game_6c200.h"

namespace r6c580 {
template <class E> void Run(E& e, uint32_t ctx, uint32_t range) { r6c200::Run(e, ctx, range, 0x206D706Fu, 0x2D); }
}
