// 1.13c D2Game+0x760F0: end an effect command of a unit. EAX = unit, stack = game (ret 4). The unit needs flag 0x400 at +0xC8; its command comes
// from 0xE03A0 (+0x94 type, +0x98 id) and must index the data-table rows (count +0xA80, rows +0xA78, stride 0x1A8). Two values come from the
// command's object (+0x2C) by its type; with the unit stat 0x3C01 and a roll they pick whether 0xF2380 starts the row's skill (+0x170, mode +0x180):
// then the command's type-2 events are cleared through 0x8E6F0, otherwise 0xDEEE0 drops the command.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_760f0.h"

namespace {
inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 4); return v; }
inline bool direct(int32_t t) { return t == 2 || (t > 3 && t <= 5); }
}

// 1.13c D2Game+0x760F0
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_760f0_port(const G760f0Env* e, uint32_t unit, uint32_t game) {
    if (unit == 0 || ((rd32(unit, 0xc8) >> 10) & 1) == 0) return;
    const uint32_t cmd = e->fE03A0(rd32(unit, 0x94), rd32(unit, 0x98), game);
    if (cmd == 0) return;
    const int32_t cls = (int32_t)rd32(cmd, 4);
    if (cls < 0) return;
    const uint32_t dt = e->dataTables();
    if (cls >= (int32_t)rd32(dt, 0xa80)) return;
    const uint32_t row = rd32(dt, 0xa78) + (uint32_t)cls * 0x1a8;
    if (row == 0) return;
    const int32_t type = (int32_t)rd32(cmd, 0);
    const uint32_t obj = rd32(cmd, 0x2c);
    const uint32_t v1 = direct(type) ? rd32(rd32(cmd, 0x2c), 0xc) : (obj != 0 ? e->t2A454(obj) : 0);
    const uint32_t obj2 = rd32(cmd, 0x2c);
    const uint32_t v2 = direct(type) ? rd32(rd32(cmd, 0x2c), 0x10) : (obj2 != 0 ? e->t2A3B8(obj2) : 0);
    uint32_t out[8] = {};
    const uint32_t roll = e->t2A71E(cmd, 0x3c01, 0);
    const bool ok = e->t2A694(e->t2A478(cmd, out, roll)) != 0;
    if (e->t2A514(unit) == 0 && ok) {
        int16_t sk; std::memcpy(&sk, (const uint8_t*)(uintptr_t)(row + 0x170), 2);
        if (e->fF2380(*(const uint8_t*)(uintptr_t)(row + 0x180), cmd, game, (uint32_t)(int32_t)sk, 0, v1, v2) != 0) {
            for (uint32_t n = rd32(cmd, 0xdc); n != 0;) {
                const uint32_t next = rd32(n, 0x24);
                if (*(const uint8_t*)(uintptr_t)n == 2) e->f8E6F0(n, game);
                n = next;
            }
            return;
        }
    }
    e->fDEEE0(game, cmd, 0, 1);
}
