// 1.13c D2Game+0x54320 D2GAME_SAVE_WriteFile: ESI = pUnit, EBX = pGame, stack = szName, dwArg (ret 8).
// The rule over an environment E (HookEnv in the FN probe src/core/fn/d2game_54320.cpp); each callee is one E::h call in retail's order,
// the same order as the batch port src/core/d2game/d2game_batch_2af80.cpp:D2GAME_SAVE_WriteFile.
// - null unit: 0.
// - unit +0xC8 &= ~2; bFlag = byte unit +0x6C nonzero and unit +0x68 == 0.
// - client = (#10920 thunk 0xA4C0)(unit) +0x9C when unit type is 0, else null; retail then reads client +0x60/+0x68 before branching,
//   so a non-player unit faults there (it is never passed one).
// - Storm #501 thunk 0xA280 copies 0x32 bytes of client +0x1D into a stack local (the account name).
// - game +0x6A byte 1 or 2: 0x53E50 (ECX = client; game, unit, bFlag); 1.
// - else the event-callback global D2Game+0x111C3C set: 0x54000 (ECX = bFlag, EDX = unit; game, szName, &account, client +0x60, dwArg,
//   client +0x68), its result.  Else 0x53F10 (game, unit, szName, bFlag, dwArg); 1.
#pragma once
#include <cstdint>

namespace g54320 {
template <class E> uint32_t SaveWriteFile(E& e, uint32_t unit, uint32_t game, uint32_t name, uint32_t arg, uint32_t callbacks)
{
    if (!unit) return 0;
    e.wr32(unit + 0xC8, e.rd32(unit + 0xC8) & ~2u);
    const uint32_t flag = e.rd8(unit + 0x6C) != 0 && e.rd32(unit + 0x68) == 0;
    uint32_t client = 0;
    if (e.rd32(unit) == 0) client = e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C);
    uint8_t account[0x34];
    const uint32_t acc = e.stk(account);
    e.h(0xA280, 0, 0, 0, 0, 0, acc, client + 0x1D, 0x32);
    const uint32_t token = e.rd32(client + 0x60), info = e.rd32(client + 0x68);
    const uint32_t type = e.rd8(game + 0x6A);
    if (type == 1 || type == 2) {
        e.h(0x53E50, 0, client, 0, 0, 0, game, unit, flag);
        return 1;
    }
    if (callbacks) {
        const uint32_t w[6] = { game, name, acc, token, arg, info };
        const uint32_t r = e.h(0x54000, 0, flag, unit, 0, 0, w[0], w[1], w[2], w[3]);
        e.blk(0x54000, w, 6);
        return r;
    }
    const uint32_t w[5] = { game, unit, name, flag, arg };
    e.h(0x53F10, 0, 0, 0, 0, 0, w[0], w[1], w[2], w[3]);
    e.blk(0x53F10, w, 5);
    return 1;
}
}
