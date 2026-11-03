// 1.13c D2Game+0x41DE0 -- missile server-do for Tiger Strike's fury carrier (MISSMODE_SrvDo32_TigerFury), on retail's 32-bit
// layout (the vendored typed form stays in d2moo/src/D2Game/MISSILES/MissMode.cpp). fastcall ECX = game, EDX = missile.
// A null missile, a class outside missiles.txt or a record whose first sub-missile (+0x18) is negative answers 2. When the
// path query on the missile's path (+0x2C) is set, a zeroed 23-dword request {+0 1, +4 owner, +8 missile, +0x10 sub-missile,
// +0x2C skill, +0x30 skill level} goes to the missile creator; the answer is the guided-missile do 0x41710(game, missile).
// 1.10f to 1.13c: the path query is D2Common #10203 (was #10234); the owner is the D2Game helper 0xE0A20 (EAX missile, stack game).
#include <cstdint>
#include <cstring>

struct D2G41DE0Seams {
    uint32_t (*pathQuery)(uint32_t path);                  // D2Common #10203 (thunk 0xACC4)
    uint32_t (*owner)(uint32_t m, uint32_t game);          // 0xE0A20 (eax, stack)
    uint32_t (*skill)(uint32_t m);                         // D2Common #10240 (thunk 0xB26A)
    uint32_t (*level)(uint32_t m);                         // D2Common #11029 (thunk 0xB0FC)
    void (*create)(uint32_t game, uint32_t* req);          // 0x6F930 (ecx, edx = 23-dword request)
    uint32_t (*guided)(uint32_t game, uint32_t m);         // 0x41710 (ecx, edx)
};

namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
}

// dt = *sgptDataTables (missiles.txt base +0xB64, stride 0x1A4, count +0xB6C)
extern "C" uint32_t d2game_41de0_port(uint32_t game, uint32_t m, uint32_t dt, const D2G41DE0Seams* s)
{
    if (!m) return 2;
    const int32_t cls = (int32_t)rd32(m + 4);
    if (cls < 0 || cls >= (int32_t)rd32(dt + 0xB6C)) return 2;
    const uint32_t rec = rd32(dt + 0xB64) + (uint32_t)cls * 0x1A4;
    if (!rec) return 2;
    int16_t sub; memcpy(&sub, (const void*)(uintptr_t)(rec + 0x18), 2);
    if (sub < 0) return 2;
    if (s->pathQuery(rd32(m + 0x2C))) {
        uint32_t req[23] = {};
        req[0] = 1;
        req[1] = s->owner(m, game);
        req[2] = m;
        req[11] = s->skill(m);
        req[12] = s->level(m);
        req[4] = (uint32_t)(int32_t)sub;
        s->create(game, req);
    }
    return s->guided(game, m);
}
