// 1.13c D2Game+0x3FB40 -- missile server-do for the rabies / plague carrier (MISSMODE_SrvDo30_RabiesPlague).
// fastcall ECX = game, EDX = missile; returns 1 (kept) or 2 (removed). Written from the 1.13c disassembly; every callee is a seam.
// 1.10f to 1.13c: the owner path update is the D2Game helper 0x3A1D0 (ECX = game, EBX = missile; it resolves the owner itself),
// not an inline D2Common_10229 on SUNIT_GetOwner; the skill id is read a second time for the spawn request.
#include <cstdint>
#include <cstring>

struct D2G3FB40Seams {
    int32_t (*skill)(uint32_t m);                                 // 0xB26A
    uint32_t (*targetType)(uint32_t m);                           // 0xB288
    uint32_t (*targetGuid)(uint32_t m);                           // 0xB24C
    uint32_t (*serverUnit)(uint32_t game, uint32_t type, uint32_t guid);   // 0xE03A0 (ecx, edx, eax)
    uint32_t (*owner)(uint32_t m, uint32_t game);                 // 0xE0A20 (eax, stack)
    int32_t (*isDead)(uint32_t u);                                // 0xDFA90 (eax)
    int32_t (*curFrame)(uint32_t m);                              // 0xB102
    void (*pathUpdate)(uint32_t game, uint32_t m);                // 0x3A1D0 (ecx, ebx)
    int32_t (*remaining)(uint32_t m);                             // 0xB28E
    int32_t (*level)(uint32_t m);                                 // 0xB0FC
    int32_t (*roll)(uint32_t seed, int32_t range);                // 0x1180 (ecx, eax)
    uint32_t (*create)(uint32_t game, uint32_t* req);             // 0x6F930 (ecx, edx = 23-dword request)
    uint32_t (*stateList)(uint32_t u, int32_t state);             // 0xA5D4
    uint32_t (*listValue)(uint32_t list);                         // 0xAB74
    void (*setTargetX)(uint32_t m, uint32_t v);                   // 0xA50E
    void (*collide)(uint32_t game, uint32_t m);                   // 0x3F320 (stack, ebx)
    void (*hit)(uint32_t game, uint32_t m, uint32_t z, uint32_t one);   // 0x3EDE0 (stack x3, edi)
};

namespace {
inline uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
inline const uint8_t* P(uint32_t a) { return (const uint8_t*)(uintptr_t)a; }
}

// dt = *sgptDataTables (missiles.txt base +0xB64 stride 0x1A4 count +0xB6C; skills.txt base +0xB98 stride 0x23C count +0xBA0)
extern "C" uint32_t d2game_3fb40_port(uint32_t game, uint32_t m, const uint8_t* dt, const D2G3FB40Seams* s)
{
    if (!m) return 2;
    const int32_t cls = (int32_t)rd32(P(m), 4);
    if (cls < 0 || cls >= (int32_t)rd32(dt, 0xB6C)) return 2;
    const uint32_t mrec = rd32(dt, 0xB64) + (uint32_t)cls * 0x1A4;
    if (!mrec) return 2;
    int16_t sub; memcpy(&sub, P(mrec) + 0x18, 2);
    if (sub < 0) return 2;
    const int32_t sk = s->skill(m);
    if (sk < 0 || sk >= (int32_t)rd32(dt, 0xBA0)) return 2;
    const uint32_t srec = rd32(dt, 0xB98) + (uint32_t)sk * 0x23C;
    if (!srec) return 2;
    const uint32_t ty = s->targetType(m);
    const uint32_t gu = s->targetGuid(m);
    const uint32_t unit = s->serverUnit(game, ty, gu);
    const uint32_t own = s->owner(m, game);
    if (!unit || !own || s->isDead(own) || s->curFrame(m) < 0) {
        s->hit(game, m, 0, 1);
        return 2;
    }
    s->pathUpdate(game, m);
    int32_t prm = (int32_t)rd32(P(mrec), 0x38);
    if (prm < 1) prm = 1;
    if (s->remaining(m) % prm == 0) {
        uint32_t req[23] = {};
        req[0] = 2; req[4] = (uint32_t)(int32_t)sub; req[1] = unit; req[2] = m;
        req[11] = (uint32_t)s->skill(m);
        req[12] = (uint32_t)s->level(m);
        const int32_t p1 = (int32_t)rd32(P(mrec), 0x3C);
        const int32_t range = 2 * p1 + 1;
        req[7] = (uint32_t)(s->roll(unit + 0x20, range) - p1);
        req[8] = (uint32_t)(s->roll(unit + 0x20, range) - p1);
        const uint32_t sm = s->create(game, req);
        int16_t st; memcpy(&st, P(srec) + 0x82, 2);
        if (st > 0) {
            const uint32_t lst = s->stateList(own, st);
            if (sm && lst) s->setTargetX(sm, s->listValue(lst));
        }
    }
    s->collide(game, m);
    return 1;
}
