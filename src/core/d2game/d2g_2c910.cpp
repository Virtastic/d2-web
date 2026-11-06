// 1.13c D2Game+0x2C910 GAME_TriggerClientSave, stdcall (client, game), ret 8.
// Saves the client's player, then walks the game's client list (+0x88, next +0x4A8): each client's unit is +0x174 when
// flag +0x3D4 bit 0 is set, else (when +0x174 is set) 0xE03A0(ECX +0x1A8, EDX +0x170, EAX +0x16C), clearing +0x174 on a
// miss. Each other client's unit is saved (0x54320, name +0x0D, 0); then, with the ladder callback table (DAT 0x111C3C,
// slot +0x28) and game+0x74 set, the callback gets ECX = name, EDX = unit class (+4) and the stack words
// (level, experience, 0, client word +0x0A, client+0x190): the two D2Common #10587 calls (stdcall 3) leave the last
// two words of the callback's frame pushed under their own.
#ifndef D2G_2C910_CPP
#define D2G_2C910_CPP
#include "d2g_mem.h"
namespace d2g_2c910 {
using namespace d2g;
struct Env {
    P (*getPlayer)(P client, I ignoreDeath);              // 0x4AD10: ESI = client, stack flag, ret 4
    I (*save)(P game, P unit, P name, I flag);            // 0x54320: EBX = game, ESI = unit, stack (name, flag), ret 8
    P (*serverUnit)(U ecx, U edx, U eax);                 // 0xE03A0: ECX = +0x1A8, EDX = +0x170, EAX = +0x16C
    I (*stat)(P unit, I stat, I layer, U under);          // thunk 0xA3E2 (#10587, ret 0xC); `under` = the stack word below its frame
    U (*table)();                                         // DAT 0x111C3C
    void (*ladder)(U table, P name, U cls, I lvl, I exp, I zero, U word); // [table+0x28], stdcall 5 (the fifth word: client+0x190)
};
// 1.13c D2Game+0x2C910
inline void GAME_TriggerClientSave_113c(const Env& e, P client, P game) {
    if (!client) return;
    P player = e.getPlayer(client, 0);
    if (!player) return;
    if (!e.save(game, player, at(client, 0xD), 1)) return;
    for (P c = pf(game, 0x88); c; ) {
        P next = pf(c, 0x4A8);
        P unit;
        if (rd8(c, 0x3D4) & 1) unit = pf(c, 0x174);
        else {
            if (!rd(c, 0x174)) { c = next; continue; }
            unit = e.serverUnit(rd(c, 0x1A8), rd(c, 0x170), rd(c, 0x16C));
            if (!unit) wr(c, 0x174, 0);
        }
        if (unit) {
            if (c != client) e.save(game, unit, at(c, 0xD), 0);
            U t = e.table();
            if (t && rd((P)(uintptr_t)t, 0x28) && rd(game, 0x74)) {
                U word = rd16(c, 0xA), cls = rd(unit, 4);
                I exp = e.stat(unit, 0xD, 0, word);
                I lvl = e.stat(unit, 0xC, 0, (U)exp);
                e.ladder(t, at(c, 0xD), cls, lvl, exp, 0, word);
            }
        }
        c = next;
    }
}
}
#endif
