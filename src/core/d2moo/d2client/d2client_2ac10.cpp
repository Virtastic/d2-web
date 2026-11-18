// 1.13c D2Client+0x2AC10: unit interaction request (ECX kind, low word; EDX unit id). 0xA4E20 finds the unit in the
// per-kind hash at 0x10A608 + kind * 0x200 (EAX = id & 0x7F, ECX = id, stack kind); none leaves. Kind 0: #C72A(player,
// x, y) at the unit's position. Kind 1: at most once per 200 ticks (unit+0xD4 keeps the last). Kind 2: #C364(player+0x2C,
// 0); a unit with +0xC4 bit 2 also gets #C72A and a seven-word request {#C42A(#C838(player, 1), UnitMode.cpp, 0x20D),
// -1, 2, id} for 0xA30C0(player, &req); class 0x194 mode 0 needs a #CA0C(player+0x60) item #C820 0x20326671 (line 0x218)
// and class 0x178 mode 2 one with 0x20686668 (line 0x22E) -- both then name #C4EA(player, 0, -1) -- else 0x23200(player,
// 0x13) ends it; class 0x178 mode 0 hands off to 0x14910(ECX 2, EDX id). Kind 4 goes to 0x148E0(ECX 4, EDX id, byte
// global 0xFAD84). The others send the 9-byte packet {0x13, kind, id} through 0x143E0.
#include "d2client_callee.h"
// 1.13c D2Client+0x2AC10
extern "C" void d2c_2ac10(const D2cSvc& v, uint32_t ecx, uint32_t id, uint32_t* player /*[0x11BBFC]*/, uint32_t g84 /*byte [0xFAD84]*/,
                          uint32_t tick) {
    const uint32_t kind = ecx & 0xffff, p = (uint32_t)(uintptr_t)player;
    const uint32_t src = 0xB2000000u | 0xd6ad4;   // "..\Source\D2Client\UNIT\UnitMode.cpp"
    uint32_t* u = (uint32_t*)(uintptr_t)d2c_call(v, 0xa4e20, id & 0x7f, id, 0x10a608 + kind * 0x200, 0, 0, kind);
    if (!u) return;
    const uint32_t uu = (uint32_t)(uintptr_t)u;
    if (kind == 0) {
        const uint32_t y = d2c_call(v, 0x1660, 0, uu, 0, 0, 0);
        const uint32_t x = d2c_call(v, 0x1630, 0, uu, 0, 0, 0);
        d2c_call(v, 0xc72a, 0, 0, 0, 0, 0, p, x, y);
    } else if (kind == 1) {
        if (tick - u[0xd4 / 4] < 200) return;
        u[0xd4 / 4] = tick;
    } else if (kind == 2) {
        d2c_call(v, 0xc364, 0, 0, 0, 0, 0, player[0x2c / 4], 0);
        if (u[0xc4 / 4] >> 2 & 1) {
            const uint32_t y = d2c_call(v, 0x1660, 0, uu, 0, 0, 0);
            const uint32_t x = d2c_call(v, 0x1630, 0, uu, 0, 0, 0);
            d2c_call(v, 0xc72a, 0, 0, 0, 0, 0, p, x, y);
            uint32_t req[7] = {};
            req[0] = d2c_call(v, 0xc42a, 0, 0, 0, 0, 0, d2c_call(v, 0xc838, 0, p, 1, 0, 0), src, 0x20d);
            uint32_t line = 0;
            if (u[1] == 0x194) {
                if (u[4] == 0) line = 0x218;
            } else if (u[1] == 0x178) {
                if (u[4] == 0) { d2c_call(v, 0x14910, 0x113, 2, id, 0, 0); return; }
                if (u[4] == 2) line = 0x22e;
            }
            if (line) {
                const uint32_t q = d2c_call(v, 0xca0c, 0, 0, 0, 0, 0, player[0x60 / 4]);
                if (!q || d2c_call(v, 0xc820, 0, 0, 0, 0, 0, q) != (line == 0x218 ? 0x20326671u : 0x20686668u)) {
                    d2c_call(v, 0x23200, 0, 0, 0, 0, 0, p, 0x13);
                    return;
                }
                req[0] = d2c_call(v, 0xc42a, 0, 0, 0, 0, 0, d2c_call(v, 0xc4ea, 0, p, 0, 0, 0, 0xffffffffu), src, line);
            }
            req[1] = 0xffffffffu; req[2] = 2; req[3] = id;
            d2c_call(v, 0xa30c0, 0, p, (uint32_t)(uintptr_t)req, 0, 0);
        }
    } else if (kind == 4) {
        d2c_call(v, 0x148e0, 0, 4, id, 0, 0, g84 & 0xff);
        return;
    }
    uint8_t pkt[12] = { 0x13, (uint8_t)kind, (uint8_t)(kind >> 8), 0, 0, (uint8_t)id, (uint8_t)(id >> 8), (uint8_t)(id >> 16), (uint8_t)(id >> 24) };
    d2c_call(v, 0x143e0, 0, 0, 0, 9, 0, (uint32_t)(uintptr_t)pkt);
}
