// 1.13c D2Game+0xF0D50: ECX = game, EDX = A (acceptor), stack B (inviter) and two unused words (ret 0xC). id = 0x9BEB0(EAX = B, EDI = [B+0x80]) for a placed
// player B (B != 0, [B+0x80] != 0, [B] == 0), else 0xFFFF. Both 0xF02F0 orders; 0xF01D0 (ESI = B, EDI = A): a low word other than 2 sends 0xF0130 (A, B) and (B, A)
// and stops. Otherwise a party id 0xFFFF becomes 0x9BF70(EBX = game) with 0x9C4F0 (EAX = game, ECX = id; B) and (A) and 0xF03E0(game, B), else only 0x9C4F0(id; A); then
// 0xF03E0(game, A); 0xF0720(EDX = B's id or -1; A's client (#10920(A)+0x9C for a type-0 A, else 0), 8); last 0xDFC10 (EBX = callback 0xF0B90, EDI = game; &{B, A, id word}).
#pragma once
#include <cstdint>
namespace gf0d50 {
template <class E> void Run(E& e, uint32_t game, uint32_t A, uint32_t B)
{
    // 1.13c D2Game+0xF0D50
    uint32_t id = 0xFFFFu;
    if (B && e.rd32(B + 0x80) && e.rd32(B) == 0) { id = e.h(0x9BEB0, B); e.j(0x9BEB0, e.rd32(B + 0x80)); }
    e.h(0xF02F0, B, A, 0, 0, 0, game);
    e.h(0xF02F0, A, B, 0, 0, 0, game);
    const uint32_t st = e.h(0xF01D0, 0, 0, 0, 0, B); e.j(0xF01D0, A);
    if ((uint16_t)st != 2) {
        e.h(0xF0130, A); e.j(0xF0130, B);
        e.h(0xF0130, B); e.j(0xF0130, A);
        return;
    }
    if ((uint16_t)id == 0xFFFFu) {
        id = e.h(0x9BF70, 0, 0, 0, game);
        e.h(0x9C4F0, game, id, 0, 0, 0, B);
        e.h(0x9C4F0, game, id, 0, 0, 0, A);
        e.h(0xF03E0, 0, 0, 0, 0, 0, game, B);
    } else {
        e.h(0x9C4F0, game, id, 0, 0, 0, A);
    }
    e.h(0xF03E0, 0, 0, 0, 0, 0, game, A);
    const uint32_t pid = B ? e.rd32(B + 0xC) : 0xFFFFFFFFu;
    uint32_t client = 0;
    if (A && e.rd32(A) == 0) client = e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, A) + 0x9C);
    e.h(0xF0720, 0, 0, pid, 0, 0, client, 8);
    const uint32_t blk[3] = { B, A, (uint32_t)(uint16_t)id };
    e.h(0xDFC10, 0, 0, 0, 0xB2000000u | 0xF0B90u, 0, e.stk(blk));
    e.edi(0xDFC10, game);
    e.blk(0xDFC10, blk, 3);
}
}
