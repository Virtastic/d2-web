// 1.13c D2Game+0x74200: the trade/command packet handler, stdcall (game, unit, cmd, arg), ret 0x10.
// The command is the low word of the third argument; the partner is found by unit+0x64 (0xE03A0) and both player-data blocks
// (thunk 0xA4C0) carry the trade state at +0x50, the trade time at +0x58 and the gold block at +0x5C. The callees are seams.
#ifndef D2GAME_74200_CPP
#define D2GAME_74200_CPP
#include "d2g_mem.h"
namespace d2game_74200 {
using namespace d2g;
struct Env {
    U (*pdata)(U unit);                                        // thunk 0xA4C0 (stdcall 1): player data
    U (*partner)(U game, U id);                                // 0xE03A0 (ECX game, EDX id)
    I (*busy)(U unit, U game);                                 // 0x9B570 (ECX unit, EBX game)
    void (*end)(U game, U unit, U partner, U pdU, U pdP, U code);   // 0x719A0 (EAX game, ESI unit, 4 stack)
    U (*tick)();                                               // GetTickCount
    void (*f717e0)(U id, U unit, U game, U cmd, U arg);        // EAX id, ECX unit, EDX game, 2 stack
    void (*f734e0)(U unit, U cmd, U game);                     // EAX unit, ECX cmd, 1 stack
    void (*send)(U client, uint8_t* buf, U n);                 // 0x8A3E0 (EAX client, 2 stack)
    void (*f70b70)(U unit, U pdU, U pdP);                      // EAX unit, 2 stack
    uint8_t (*check)(U unit, U game);                          // 0x70FB0 (EBX unit, EDI game)
    void (*f700c0)(U unit, U game);                            // EAX unit, EBX game
    void (*msg)(U client, uint8_t code);                       // 0x8B340 (AL code over EAX = client, 1 stack)
    P (*name)(U client);                                       // 0x49A20 (EAX client): 16-byte name
    void (*pkt)(U client, uint8_t* p);                         // 0x8B2F0 (EAX packet, 1 stack)
    void (*gold)(U client, U amount, uint8_t side);            // 0x8B1F0 (EAX amount, CL side, 1 stack)
    void (*f73d90)(U pdU, U partner, U game, U unit, U pdP);   // EAX pdU, ECX partner, 3 stack
    U (*stat)(U unit, U id, U layer);                          // thunk 0xA42A (stdcall 3)
    U (*maxgold)(U unit);                                      // thunk 0xA45A (stdcall 1)
    U (*a6a6)(U v);                                            // thunk 0xA6A6 (stdcall 1)
};
// 1.13c D2Game+0xB88C0 (EAX game): 0 when game+0x70 is set or the game's 0x17 list entry has its +0x14 flag clear, else 1
inline U Fn_b88c0_113c(U game) {
    if (rd((P)(uintptr_t)game, 0x70)) return 0;
    U p = rd((P)(uintptr_t)game, 0x10F4);
    if (!p || !(p = rd((P)(uintptr_t)p, 0))) return 1;
    for (; p; p = rd((P)(uintptr_t)p, 0xF4))
        if (rd((P)(uintptr_t)p, 0) == 0x17) return rd8((P)(uintptr_t)rd((P)(uintptr_t)p, 0x18), 0x14) ? 1 : 0;
    return 1;
}
inline U client(const Env& e, U unit) { return rd((P)(uintptr_t)unit, 0) == 0 ? rd((P)(uintptr_t)e.pdata(unit), 0x9C) : 0; }
// 1.13c D2Game+0x74200
inline U Fn74200_113c(const Env& e, U game, U unit, U cmd, U arg) {
    if (!unit) return 1;
    P u = (P)(uintptr_t)unit;
    const U st = rd(u, 0x68);
    const uint16_t w = (uint16_t)cmd;
    uint8_t m[4];
    if (!rd8(u, 0x6C)) { const U c = client(e, unit); m[0] = 0x77; m[1] = 0x0C; e.send(c, m, 2); return 0; }
    if (w >= 0x12 && w <= 0x14) { if (st != 2) return 1; e.f717e0(rd(u, 0x64), unit, game, cmd, arg); return 0; }
    if (w >= 0x17 && w <= 0x18) { if (st != 4) return 3; e.f734e0(unit, cmd, game); return 0; }
    if (st) { const U c = client(e, unit); m[0] = 0x77; m[1] = 0x0D; e.send(c, m, 2); return 3; }
    const U pt = e.partner(game, rd(u, 0x64));
    if (!pt) {
        if (e.busy(unit, game)) return 0;
        e.msg(client(e, unit), 0xC);
        return 0;
    }
    P p = (P)(uintptr_t)pt;
    const U pdU = e.pdata(unit), pdP = e.pdata(pt);
    P du = (P)(uintptr_t)pdU, dp = (P)(uintptr_t)pdP;
    auto stamp = [&] { const U t = e.tick(); wr(dp, 0x58, t); wr(du, 0x58, t); };
    auto both = [&](uint8_t code) { e.msg(client(e, unit), code); e.msg(client(e, pt), code); };
    switch (w) {
    case 2: {
        const U a = rd(du, 0x50), b = rd(dp, 0x50);
        if (((a == 1 || a == 2) && (b == 1 || b == 2)) || (!e.busy(unit, game) && !e.busy(pt, game))) {
            e.end(game, unit, pt, pdU, pdP, 9);
            stamp();
        }
        return 0;
    }
    case 3:
        if (e.busy(unit, game) || e.busy(pt, game)) {
            const U a = rd(du, 0x50);
            if (a == 7) wr(du, 0x50, 0xE);
            else if (a == 8) wr(du, 0x50, 0xF);
            else e.end(game, unit, pt, pdU, pdP, 0xC);
            return 0;
        }
        if (Fn_b88c0_113c(game)) { e.end(game, unit, pt, pdU, pdP, 9); return 0; }
        if (rd(du, 0x50) != 2 || rd(dp, 0x50) != 1) { e.end(game, unit, pt, pdU, pdP, 0xC); return 0; }
        e.f70b70(unit, pdU, pdP);
        if (e.check(unit, game)) {
            if (e.check(pt, game)) {
                uint8_t k[24] = {};
                k[0] = 0x78;
                std::memcpy(k + 1, e.name(client(e, pt)), 16);
                k[16] = 0;
                wr(k, 17, rd(p, 0xC));
                e.pkt(client(e, unit), k);
                std::memcpy(k + 1, e.name(client(e, unit)), 16);
                k[16] = 0;
                wr(k, 17, rd(u, 0xC));
                e.pkt(client(e, pt), k);
                return 0;
            }
            e.f700c0(unit, game);
        }
        both(0xC);
        return 0;
    case 4:
        if (!e.busy(unit, game) && !e.busy(pt, game)) {
            if (Fn_b88c0_113c(game)) { e.end(game, unit, pt, pdU, pdP, 9); return 0; }
            if (rd(du, 0x50) == 3 && !e.a6a6(rd(u, 0x60))) {
                e.msg(client(e, pt), 5);
                if (rd(du, 0x50) != 3) return 0;
                wr(du, 0x50, 4);
                if (rd(dp, 0x50) != 4) return 0;
                e.f73d90(pdU, pt, game, unit, pdP);
                stamp();
                return 0;
            }
        }
        e.msg(client(e, unit), 6);
        return 0;
    case 7:
        if (rd(du, 0x50) == 4) wr(du, 0x50, 3);
        if (rd(dp, 0x50) == 4) wr(dp, 0x50, 3);
        both(6);
        return 0;
    case 8: {
        U amt = arg;
        if (amt > e.stat(unit, 0xE, 0)) {
            if (!e.busy(unit, game) && !e.busy(pt, game)) e.end(game, unit, pt, pdU, pdP, 9);
            return 0;
        }
        const U mx = e.maxgold(pt), g = e.stat(pt, 0xE, 0);
        if (g + amt > mx) amt = mx - g;
        const U gb = rd(du, 0x5C);
        if (!gb) return 0;
        wr((P)(uintptr_t)gb, 0xC, amt);
        e.gold(client(e, pt), amt, 0);
        e.gold(client(e, unit), amt, 1);
        return 0;
    }
    default:
        return 0;
    }
}
}
#endif
