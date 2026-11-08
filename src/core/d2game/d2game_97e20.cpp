// 1.13c D2Game+0x97E20 -- add a pet to an owner's list slot (EAX = owner unit; stack = a game word handed on in EBX / EDI, the pet unit or 0, the slot index, a pointer to
// two words; ret 16). Written from the 1.13c disassembly as a rule over E.
// rec = 0xA4C0(owner) (null is fatal); rec+0x44 (the pet table, zero: return) holds the array base of 0xC-byte slots, the index must be 0 <= idx < sgptDataTables +0xBF0.
// 0x97A20 (ESI = the slot, EBX = the game word; owner, pet, &{w0, w1, w2}) appends the entry; when it succeeds a 7-word block {pet +0xC or -1, owner +0xC or -1,
// pet +4 as a word (0xFFFF none), idx, w0, w1, 0} goes to 0xDFC10 (EBX = callback 0x6FCB66E0, EDI = the game word, &block). No result is defined.
#ifndef D2GAME_RULE_97E20
#define D2GAME_RULE_97E20
#include <cstdint>

namespace g97e20 {
template <class E> void Add(E& e, uint32_t owner, uint32_t game, uint32_t pet, uint32_t idx, uint32_t a3, uint32_t g)
{
    const uint32_t rec = e.h(0xA4C0, 0, 0, 0, 0, 0, owner);
    const uint32_t p44 = e.rd32(rec + 0x44);
    if (!p44 || (int32_t)idx < 0 || (int32_t)idx >= (int32_t)e.rd32(e.rd32(g) + 0xBF0)) return;
    const uint32_t slot = e.rd32(p44) + idx * 0xCu;
    if (!slot) return;
    uint32_t w[3] = { e.rd32(a3), e.rd32(a3 + 4), e.rd32(a3 + 8) };
    const uint32_t ok = e.h(0x97A20, 0, 0, 0, game, slot, owner, pet, e.stk(w));
    e.blk(0x97A20, w, 3);
    if (!ok) return;
    uint32_t b[7] = { pet ? e.rd32(pet + 0xC) : 0xFFFFFFFFu, owner ? e.rd32(owner + 0xC) : 0xFFFFFFFFu, (pet ? e.rd32(pet + 4) : 0xFFFFFFFFu) & 0xFFFFu, idx, w[0], w[1], 0 };
    e.h(0xDFC10, 0, 0, 0, 0xB2000000u | 0x966E0, 0, e.stk(b));
    e.edi(0xDFC10, game);
    e.blk(0xDFC10, b, 7);
}
}
#endif
