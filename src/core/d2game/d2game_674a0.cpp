// 1.13c D2Game+0x674A0 -- place the A1 quest object near the player.
// stdcall(game, unit), ret 8. plr = game[+0x10F4] (plr[+8] == 0 is fatal 0x82C), quest record = plr[+0xC]. Leaves through the fail tail
// (unit +0x6E = 0x14, +0x70 = unit, +0xC4 |= 0x400, returns 0; unit == 0 is fatal 0x558) unless: flag(rec, 4, 0xB) and flag(player quest [game+0x6D], 4, 0xA)
// are clear, the A1 flag gate passes (game[+0x70] == 0 needs flag 0x1A, otherwise flag 0x28), the unit's room has key 1, the unit position finds a room
// (0x34130), the room-key kinds agree (fatal 0xE5C), the position moves (D2Common #10654) into a room 0x1280 finds, the object is built (0xE1D90), placed
// (0xA22E0, type 0x27) and flagged; then 0x33920(game, 4, 0xB) and return 1. The AEC8 value keeps the upper 24 bits of the unit word (retail reuses the
// argument slot for the flag byte). Seams are the callees.
#include <cstdint>
#include <cstring>

struct D2Game674A0Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*flag)(uint32_t rec, uint32_t a, uint32_t b);                 // D2Common #10174
    uint32_t (*playerData)(uint32_t unit);                                  // D2Common #10920
    uint32_t (*roomOf)(uint32_t unit);                                      // D2Common #10331
    uint32_t (*roomKey)(uint32_t room);                                     // D2Common #10826
    void     (*getPos)(uint32_t unit, uint32_t* pos);                       // D2Common #10141
    void     (*findRoom)(uint32_t room, uint32_t* pos, uint32_t* out);      // D2Game 0x34130 (room, pos, 3, 0x400, out, 0x64)
    void     (*release)(uint32_t room);                                     // D2Common #10057
    uint32_t (*kind)(uint32_t v);                                           // D2Common #10459
    uint32_t (*move)(uint32_t room, uint32_t* pos);                         // D2Common #10654 (room, pos, pos, 3, 0x3e01, 0xc01, 0)
    uint32_t (*roomAt)(uint32_t room, uint32_t x, uint32_t y);              // D2Game 0x1280 (EAX room, EBX x, EDI y)
    uint32_t (*create)(uint32_t x, uint32_t y, uint32_t game, uint32_t room); // D2Game 0xE1D90
    void     (*setMode)(uint32_t unit, uint32_t mode);                      // D2Common #11090
    uint32_t (*place)(uint32_t game, uint32_t unit, uint32_t what, uint32_t v); // D2Game 0xA22E0
    uint32_t (*flags)(uint32_t unit);                                       // D2Common #10258
    void     (*setFlags)(uint32_t unit, uint32_t v);                        // D2Common #10111
    void     (*link)(uint32_t a, uint32_t b);                               // D2Common #10346
    void     (*mark)(uint32_t game, uint32_t a, uint32_t b);                // D2Game 0x33920
    void     (*unitDone)(uint32_t unit);                                    // D2Common #10158
};

static inline uint32_t r32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void w32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

// 1.13c D2Game+0x674A0
uint32_t D2Game_674A0(uint32_t game, uint32_t unit, const D2Game674A0Env& e)
{
    const uint32_t plr = r32(game + 0x10f4);
    if (!r32(plr + 8)) { e.fatal(0x82c); return 0; }
    const uint32_t diff = *(const uint8_t*)(uintptr_t)(game + 0x6d);
    auto pq = [&]() { return r32(e.playerData(unit) + diff * 4 + 0x10); };
    do {
        if (e.flag(r32(plr + 0xc), 4, 0xb)) break;
        if (e.flag(pq(), 4, 0xa)) break;
        if (!r32(game + 0x70)) {
            if (!e.flag(pq(), 0x1a, 0)) break;
        } else if (!e.flag(pq(), 0x28, 0)) break;
        const uint32_t room = e.roomOf(unit);
        if (!room || e.roomKey(room) != 1) break;
        uint32_t pos[2] = { 0, 0 }, f = 0;
        e.getPos(unit, pos);
        e.findRoom(room, pos, &f);
        if (!f) break;
        uint32_t cx = pos[0], cy = pos[1];
        if (unit) e.release(f);
        const uint32_t k0 = e.kind(0x27) & 0xff, v = e.roomKey(f);
        if (k0 != (e.kind(v) & 0xff)) { e.fatal(0xe5c); return 0; }
        uint32_t p2[2] = { cx, cy };
        if (!e.move(f, p2)) break;
        const uint32_t x = p2[0], y = p2[1];
        const uint32_t r = e.roomAt(f, x, y);
        if (!r) break;
        const uint32_t nu = e.create(x, y, game, r);
        e.setMode(nu, 1);
        const uint32_t u2 = e.place(game, nu, 0x27, v);
        if (!u2) break;
        e.setFlags(u2, (unit & ~0xffu) | ((e.flags(u2) | 3) & 0xff));
        e.link(r, 0);
        e.link(e.roomOf(u2), 0);
        e.mark(game, 4, 0xb);
        return 1;
    } while (0);
    if (!unit) { e.fatal(0x558); return 0; }
    uint16_t w = 0x14; std::memcpy((void*)(uintptr_t)(unit + 0x6e), &w, 2);
    w32(unit + 0x70, unit);
    e.unitDone(unit);
    w32(unit + 0xc4, r32(unit + 0xc4) | 0x400);
    return 0;
}
