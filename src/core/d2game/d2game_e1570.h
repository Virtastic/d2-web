// 1.13c D2Game+0xE1570: the unit recorded at unit[0x94]/[0x98], looked up after refreshing a game reference. Stack: game, unit (ret 8).
// Callees are seams: a56e = stdcall(byte) returning a byte; a526 = stdcall(rec, p, q); a5a4 = stdcall(rec, p, q); cb80 = +0x2CB80 (EAX = a5a4 result,
// EDI = game); findUnit = +0xE03A0 (ECX game, EAX type, EDX guid). fatal never returns.
#pragma once
#include <cstdint>
struct D2GameE1570Env {
    uint32_t (*a56e)(uint32_t b);
    uint32_t (*a526)(uint32_t rec, uint32_t p, uint32_t q);
    uint32_t (*a5a4)(uint32_t rec, uint32_t p, uint32_t q);
    void (*cb80)(uint32_t eax, uint32_t game);
    uint32_t (*findUnit)(uint32_t game, uint32_t type, uint32_t guid);
    void (*fatal)(uint32_t code);
};
uint32_t d2game_e1570_port(uint32_t game, uint32_t unit, const D2GameE1570Env* e);
