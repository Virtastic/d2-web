// 1.13c D2Game+0xF0B10 -- EDX = b, one stack argument p (ret 4). Same unit: the setter 0xF0220 (ESI = b, EDI = b, stack 1) then the worker 0x8D120 (EAX = b, EBX = b). Otherwise 0xF0220 (ESI = p, EDI = b, stack 0); a nonnull b with a +0x80 word and type 0 asks 0x9BEB0 (EAX = b) and the flag is 1 unless the answer low word is 0xFFFF (else 0); 0xF0220 (ESI = b, EDI = p, stack flag); 0x8D120 (EAX = p, EBX = b) and 0x8D120 (EAX = b, EBX = p). Returns the last worker result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t ff0b10(GE& e, uint32_t b, uint32_t p)
{
    if (b == p) {
        e.h(0xF0220, 0, 0, 0, 0, b, 1); e.j(0xF0220, b);
        return e.h(0x8D120, b, 0, 0, b, 0);
    }
    e.h(0xF0220, 0, 0, 0, 0, p, 0); e.j(0xF0220, b);
    uint32_t flag = 0;
    if (b && e.rd32(b + 0x80) && e.rd32(b) == 0)
        flag = (e.h(0x9BEB0, b, 0, 0, 0, 0) & 0xffff) != 0xffff;
    e.h(0xF0220, 0, 0, 0, 0, b, flag); e.j(0xF0220, p);
    e.h(0x8D120, p, 0, 0, b, 0);
    return e.h(0x8D120, b, 0, 0, p, 0);
}
}
