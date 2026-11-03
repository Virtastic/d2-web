// 1.13c D2Game+0x5EED0 -- ACT5Q5 (Rite of Passage) monster-killed callback.
// ECX = quest data, EDX = quest argument {game, ?, target, player}, plain ret. Rule: while the statue sequence is armed (ex+0x11, ex+0x10,
// ex+0xC == 0) the first kill registers the 0x5E660 timer (0x33AF0, EBX = 2); a kill of one of the three ancients (0x21C/0x21D/0x21E) marks its
// slot, fires its statue (0x5D960, EBX = the statue GUID), sets the game's FX byte to 0x12 and walks the clients (0xDFCC0 over 0x359C0). The
// third kill (ex+0x44 reaching 0) closes the sequence: the killer gets quest flags 39/0 and 39/13 and the reward (0x5BF70) when he has not
// yet completed it and his level is >= (difficulty+1)*20; the clients are walked twice more (0x5C100, 0x58F40), 0x35EB0(5) runs, and the
// object 561 (the Worldstone portal) is allocated in place (inlined SUNIT alloc: type 2, act, seed, GUID counter game+0x98, ObjectData via
// the pool) and spawned (0x2F300, 0xE0F40) at the target's room and position. The sequence quest (0x33F30) runs its own handler at +0xF0
// once IsBadCodePtr has passed the quest data's own +0xF0 word; the quest moves to state 0xD and the clients are walked (0x58F90).
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct D2Game5EED0Env {
    void (*timer)(uint32_t qd, uint32_t fn, uint32_t ebx);                                  // 0x33AF0 (stdcall 2, EBX)
    void (*fire)(uint32_t qd, uint32_t arg, uint32_t ebx);                                  // 0x5D960 (stdcall 2, EBX = statue GUID)
    void (*iterate)(uint32_t edi, uint32_t ebx, uint32_t a);                                // 0xDFCC0 (EAX = 0, EDI = game, EBX = fn, stdcall 1)
    uint32_t (*playerFlags)(uint32_t player, uint32_t game);                                // 0x1020 (EAX = player, stdcall 1)
    uint32_t (*getFlag)(uint32_t flags, uint32_t q, uint32_t b);                            // 0xA568 (stdcall 3)
    uint32_t (*getStat)(uint32_t unit, uint32_t stat, uint32_t layer);                      // 0xA3E2 (stdcall 3)
    void (*setFlag)(uint32_t flags, uint32_t q, uint32_t b);                                // 0xA57A (stdcall 3)
    void (*reward)(uint32_t player, uint32_t game);                                         // 0x5BF70 (EAX = player, stdcall 1)
    void (*sub35EB0)(uint32_t eax);                                                         // 0x35EB0 (EAX)
    uint32_t (*room)(uint32_t unit);                                                        // 0xA478 (stdcall 1)
    uint32_t (*c1130)(uint32_t unit);                                                       // 0x1130 (ECX)
    uint32_t (*c1100)(uint32_t unit);                                                       // 0x1100 (ECX)
    uint32_t (*alloc)(uint32_t pool, uint32_t n);                                           // 0xA6B8 (stdcall 2)
    uint32_t (*roomLevel)(uint32_t room);                                                   // 0xA556 (stdcall 1)
    uint32_t (*levelAct)(uint32_t level);                                                   // 0xA56E (stdcall 1), the act in AL
    void (*seed)(uint32_t edi, uint32_t esi);                                               // 0xE0110 (EDI = unit, ESI = game+0xD0)
    uint32_t (*poolAlloc)(uint32_t pool, uint32_t size, uint32_t file, uint32_t line, uint32_t z);  // 0xA2E6 (ECX, EDX, stdcall 3)
    uint32_t (*objTxt)(uint32_t id);                                                        // 0xA520 (stdcall 1)
    void (*a724)(uint32_t unit);                                                            // 0xA724 (stdcall 1)
    void (*place)(uint32_t esi, uint32_t guid, uint32_t room, uint32_t y, uint32_t x);      // 0x2F300 (ESI = unit, stdcall 4)
    void (*spawn)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2);  // 0xE0F40 (EAX room, ECX x, EDX unit, stdcall 3)
    uint32_t (*questGet)(uint32_t game, uint32_t id);                                       // 0x33F30 (EAX = game, ECX = id)
    uint32_t (*badCode)(uint32_t p);                                                        // IsBadCodePtr (IAT 0xF8D14)
    void (*handler)(uint32_t fn, uint32_t q);                                               // call [q+0xF0], ECX = q
    uint32_t mod;                                                                           // module base as the port sees module addresses (0xB2000000 in the probe)
};

namespace {
inline uint32_t r32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
inline uint32_t r8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
inline void w32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
inline void w8(uint32_t a, uint32_t o, uint32_t v) { *(uint8_t*)(uintptr_t)(a + o) = (uint8_t)v; }
}

// 1.13c D2Game+0x5EED0
void D2Game_5EED0(uint32_t qd, uint32_t arg, const D2Game5EED0Env& e)
{
    const uint32_t ex = r32(qd, 0x18);
    if (!r8(ex, 0x11) || !r8(ex, 0x10) || r32(ex, 0xC) != 0) return;
    if (!r8(ex, 4)) { w8(ex, 4, 1); e.timer(qd, e.mod + 0x5E660, 2); }
    const uint32_t tgt = r32(arg, 8);
    const int32_t cls = tgt ? (int32_t)r32(tgt, 4) : -1;
    uint32_t guid;
    if (cls == 0x21C) { guid = r32(ex, 0x1C); w8(ex, 0x3E, 1); }
    else if (cls == 0x21D) { guid = r32(ex, 0x14); w8(ex, 0x3C, 1); }
    else if (cls == 0x21E) { guid = r32(ex, 0x18); w8(ex, 0x3D, 1); }
    else return;
    e.fire(qd, arg, guid);
    {
        const uint32_t game = r32(qd, 4);
        w8(r32(game, 0x10F4), 0x20, 0x12);
        e.iterate(game, e.mod + 0x359C0, 0);
    }
    const uint32_t left = r32(ex, 0x44) - 1;
    w32(ex, 0x44, left);
    if (left != 0) return;
    w8(ex, 0, 1);
    if (!r8(qd, 9)) return;
    if (const uint32_t player = r32(arg, 0xC); player) {
        const uint32_t flags = e.playerFlags(player, r32(qd, 4));
        if (e.getFlag(flags, 0x27, 0) == 0) {
            const int32_t lvl = (int32_t)e.getStat(r32(arg, 0xC), 0xC, 0);
            if (lvl >= (int32_t)((r8(r32(qd, 4), 0x6D) + 1) * 20)) {
                e.setFlag(flags, 0x27, 0);
                e.setFlag(flags, 0x27, 0xD);
                e.reward(r32(arg, 0xC), r32(arg, 0));
            }
        }
    }
    e.iterate(r32(arg, 0), e.mod + 0x5C100, r32(arg, 8));
    e.iterate(r32(arg, 0), e.mod + 0x58F40, r32(arg, 8));
    e.sub35EB0(5);
    const uint32_t room = e.room(r32(arg, 8));
    const uint32_t game = r32(arg, 0);
    const uint32_t x = e.c1130(r32(arg, 8));
    const uint32_t y = e.c1100(r32(arg, 8));
    if (const uint32_t u = e.alloc(r32(game, 0x1C), 2); u) {
        w32(u, 0, 2);
        w32(u, 4, 0x231);
        w32(u, 8, r32(game, 0x1C));
        w32(u, 0x80, game);
        uint32_t f = r32(u, 0xC8);
        f = r32(game, 0x70) ? (f | 0x2000000u) : (f & 0xFDFFFFFFu);
        w32(u, 0xC8, f);
        w32(u, 0xC8, f | 0x4000000u);
        const uint32_t act = e.levelAct(e.roomLevel(room)) & 0xFF;
        w8(u, 0x18, act);
        w32(u, 0x1C, r32(game, 0xBC + act * 4));
        w32(u, 0x74, 0);
        e.seed(u, game + 0xD0);
        w32(u, 0xC4, r32(u, 0xC4) | 0x10);
        w32(u, 0xD0, 0xB);
        uint32_t id = r32(game, 0x98) + 1;
        if (id == 0xFFFFFFFFu) id = 1;
        w32(game, 0x98, id);
        const uint32_t od = e.poolAlloc(r32(game, 0x1C), 0x38, e.mod + 0xFB02C, 0x265, 0);
        w32(u, 0x14, od);
        for (uint32_t i = 0; i < 14; i++) w32(od, i * 4, 0);
        const uint32_t txt = e.objTxt(0x231);
        w32(r32(u, 0x14), 0, txt);
        w32(u, 0x10, 0);
        e.a724(u);
        e.place(u, id, room, y, x);
        e.spawn(room, x, u, y, game, 1);
    }
    if (const uint32_t q = e.questGet(r32(qd, 4), r32(qd, 0x10)); q) {
        if (e.badCode(r32(qd, 0xF0))) abort();   // retail: assert line 0x357, exit(-1)
        e.handler(r32(q, 0xF0), q);
    }
    if (r8(qd, 0xB) != 0xD) {
        w8(qd, 0x14, 0);
        w8(qd, 0xB, 0xD);
        e.iterate(r32(qd, 4), e.mod + 0x58F90, 0);
    }
}
