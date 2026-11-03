// 1.13c D2Game+0x67A90 -- quest-record callback that finishes a placement.
// ECX = game (0 is fatal, line 0x2D3), EDX = quest record (q = [rec+0x18], key = [q+0x40]); returns AL.
// Looks the key up in game bucket [+0x1520 + (key & 0x7F) * 4] (chain link +0xE4, id at +0xC); no entry clears q[0x44] and returns 1.
// Otherwise it reads the entry's two extents (types 2, 4, 5: payload +0x10 / +0xC; the others D2Common #10750 / #10867 on the payload),
// takes the entry's room, checks #10459(0x26) == #10459(#10826(room)), finds the room holding (x + 4, y + 4) with 0x1280, creates the
// object through 0xE1D90 and places it with 0xA22E0, sets its flags and marks q[0x45] = 1, q[0x44] = 0. Seams are the callees.
#include <cstdint>
#include <cstring>

struct D2Game67A90Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*extentA)(uint32_t payload);                                   // D2Common #10750
    uint32_t (*extentB)(uint32_t payload);                                   // D2Common #10867
    uint32_t (*roomOf)(uint32_t entry);                                      // D2Common #10331
    uint32_t (*kind)(uint32_t v);                                            // D2Common #10459
    uint32_t (*roomKey)(uint32_t room);                                      // D2Common #10826
    uint32_t (*roomAt)(uint32_t room, uint32_t x, uint32_t y);               // D2Game 0x1280 (EAX room, EBX x, EDI y)
    uint32_t (*create)(uint32_t x, uint32_t y, uint32_t game, uint32_t room);// D2Game 0xE1D90 (ECX 2, EDX 0x3C, +3 constant words)
    void     (*setMode)(uint32_t unit, uint32_t mode);                       // D2Common #11090
    uint32_t (*place)(uint32_t game, uint32_t unit, uint32_t what, uint32_t v); // D2Game 0xA22E0
    uint32_t (*flags)(uint32_t unit);                                        // D2Common #10258
    void     (*setFlags)(uint32_t unit, uint32_t v);                         // D2Common #10111
    void     (*link)(uint32_t a, uint32_t b);                                // D2Common #10346
};

static inline uint32_t r32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void w8(uint32_t a, uint8_t v) { *(uint8_t*)(uintptr_t)a = v; }

// 1.13c D2Game+0x67A90
uint32_t D2Game_67A90(uint32_t game, uint32_t rec, const D2Game67A90Env& e)
{
    const uint32_t q = r32(rec + 0x18), key = r32(q + 0x40);
    if (!game) { e.fatal(0x2d3); return 0; }
    uint32_t it = r32(game + 0x1520 + (key & 0x7f) * 4);
    while (it && r32(it + 0xc) != key) it = r32(it + 0xe4);
    if (!it) { w8(q + 0x44, 0); return 1; }
    const uint32_t t = r32(it), p = r32(it + 0x2c);
    const bool direct = t == 2 || (t > 3 && t <= 5);
    const uint32_t a = direct ? r32(p + 0x10) : p ? e.extentA(p) : 0;
    const uint32_t b = direct ? r32(p + 0xc) : p ? e.extentB(p) : 0;
    const uint32_t room = e.roomOf(it);
    if (!room) { e.fatal(0xe42); return 0; }
    const uint32_t k0 = e.kind(0x26) & 0xff, v = e.roomKey(room);
    if (k0 != (e.kind(v) & 0xff)) { e.fatal(0xe5c); return 0; }
    const uint32_t x = b + 4, y = a + 4;
    const uint32_t r = e.roomAt(room, x, y);
    if (!r) return 0;
    const uint32_t nu = e.create(x, y, game, r);
    e.setMode(nu, 1);
    const uint32_t u2 = e.place(game, nu, 0x26, v);
    if (!u2) return 0;
    e.setFlags(u2, (v & ~0xffu) | ((e.flags(u2) | 3) & 0xff));
    e.link(r, 0);
    e.link(e.roomOf(u2), 0);
    w8(q + 0x45, 1); w8(q + 0x44, 0);
    return 1;
}
