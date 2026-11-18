// 1.13c D2Game+0x711D0 (layout from the disassembly), restored unchanged from the stand-alone batch the game-tree migration removed.
// EAX = player (EDI); ECX is copied to EBX and never used; the count starts at 0 (the decompile's unaff_EBX is a local initialised
// to 0); one stack word `arg` (ret 4). Every callee is a seam in retail's argument order.
#include <cstdint>
typedef int32_t i32;
#define W(a, o) (*(i32*)((intptr_t)(a) + (o)))

struct D2Game711D0Env {
    intptr_t (*firstNode)(intptr_t list);                // #10460, 0xA3DC (1 stack word)
    intptr_t (*nodeUnit)(intptr_t node);                 // #11147, 0xA46C (1)
    intptr_t (*nextNode)(intptr_t node);                 // #10464, 0xA4CC (1)
    int32_t  (*unitType)(intptr_t unit);                 // #10020, 0xA6C4 (1; al is compared)
    void     (*clearFlag)(i32* flag, i32 mask, i32 on);  // #10758, 0xAA5A (3; writes *flag)
    void     (*setMode)(intptr_t unit, i32 mode);        // #11030, 0xA958 (2)
    void     (*send)(intptr_t item, i32 flag);           // 0x8C810: EAX = item, stack word = flag (ret 4)
    void     (*update)(intptr_t unit, i32 arg, i32 zero);// 0xD2D90: EAX = unit, stack words arg, 0 (ret 8)
};
extern "C" i32 D2GAME_711D0(i32 in_eax, i32 arg, const D2Game711D0Env* e)
{
    if (W(in_eax, 0x60) == 0) return 0;
    i32 count = 0;
    i32 flag = 0;
    intptr_t it = e->firstNode(W(in_eax, 0x60));
    while (it) {
        intptr_t u = e->nodeUnit(it);
        it = e->nextNode(it);
        if (u && (char)e->unitType(u) == 3) {
            flag = 0;
            e->clearFlag(&flag, 0x20, 1);
            e->setMode(u, 3);
            e->send(u, flag);
            e->update(in_eax, arg, 0);
            count++;
        }
    }
    return count;
}
