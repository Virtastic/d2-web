// 1.13c D2Game+0xE4500: ECX = record (byte +0xC), EDX = id, stack (p3, list, unused), ret 0xC.
// EAX of the 0xE660 calls is the FIRST stack word (p3), the stat unit of the 0xA568 calls the SECOND (list).
#include "d2game_h16.h"
namespace h16 {
uint32_t e4500(const Env& e, uint32_t a, uint32_t id, uint32_t p3, uint32_t list)
{
    if (e.o10174(list, 0x13, 0)) return 0;
    if (id == 0xfd) return (e.findQuest(p3, rd32(a, 4), 0x20333367) == 0 && rd8(a, 0xc) == 1) ? 1 : 0;
    if (id == 0xff) {
        if (e.findQuest(p3, rd32(a, 4), 0x20333367) != 0) return 1;
        if (e.o10174(list, 0x13, 6) == 0) return 0;
        return e.o10174(list, 0x13, 8) == 0 ? 1 : 0;
    }
    if (id == 0xfc) {
        if (e.o10174(list, 0x13, 6) == 0) return 0;
        return e.o10174(list, 0x13, 7) == 0 ? 1 : 0;
    }
    return 0;
}
}
