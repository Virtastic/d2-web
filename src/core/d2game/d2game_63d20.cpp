// 1.13c D2Game+0x63D20 -- quest callback: EDX = quest node. A node still in state 0 (byte +0xC) goes to state 1 (trace "changing state to 1"). Returns 1 (AL; the rest of EAX is stale).
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f63d20(NE& e, uint32_t node)
{
    if (!e.rd8(node + 0xc)) { e.wr8(node + 0xc, 1); e.say(e.rd32(node + 4), 1); }
    return 1;
}
}
