// 1.13c D2Game+0xF0FF0: EAX = X, stack game and Y (ret 8). A placed player X (X != 0, [X+0x80] != 0, [X] == 0) whose 0x9BEB0 id (EAX = X, EDI = [X+0x80]) has a low word other than
// 0xFFFF takes #10587(Y, 0xC, 0) (0xA3E2): a value of 9 or more calls 0x9C160 (EAX = id, ESI = game, EBX = Y; callback 0xF0CD0) and returns. Otherwise Y must be a type-0 unit (else
// fatal 0x24): 0xF02F0 (EAX = X, ECX = Y; game), 0xF0640 (EAX = 8, ECX = Y, ESI = X; game, 1); X must be a type-0 unit (else fatal 0x24); 0xF02F0 and 0xF0640 swapped; then
// 0xF0720 (EDX = [Y+0xC]; #10920(X)+0x9C, 3).
#pragma once
#include <cstdint>
namespace gf0ff0 {
template <class E, class Fatal> void Run(E& e, Fatal& fatal, uint32_t X, uint32_t game, uint32_t Y)
{
    // 1.13c D2Game+0xF0FF0
    if (X && e.rd32(X + 0x80) && e.rd32(X) == 0) {
        const uint32_t id = e.h(0x9BEB0, X); e.j(0x9BEB0, e.rd32(X + 0x80));
        if ((uint16_t)id != 0xFFFFu) {
            if ((int32_t)e.h(0xA3E2, 0, 0, 0, 0, 0, Y, 0xC, 0) >= 9) e.h(0x9C160, id, 0, 0, Y, game, 0xB2000000u | 0xF0CD0u);
            return;
        }
    }
    if (!Y || e.rd32(Y) != 0) { fatal(0x24u); return; }
    e.h(0xF02F0, X, Y, 0, 0, 0, game);
    e.h(0xF0640, 8, Y, 0, 0, X, game, 1);
    if (!X || e.rd32(X) != 0) { fatal(0x24u); return; }
    e.h(0xF02F0, Y, X, 0, 0, 0, game);
    e.h(0xF0640, 8, X, 0, 0, Y, game, 1);
    const uint32_t pid = e.rd32(Y + 0xC);
    e.h(0xF0720, 0, 0, pid, 0, 0, e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, X) + 0x9C), 3);
}
}
