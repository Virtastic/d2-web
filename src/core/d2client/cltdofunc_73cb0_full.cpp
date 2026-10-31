// cltdofunc 25 (Poison Nova), the full function with its callees through D2cHost (client_host.h) in retail order.
// nova_73cb0.cpp keeps the vector-proven presentation (sound stream); this is the 1:1 control flow.
#include "client_host.h"
#include <emscripten.h>

// 1.13c D2Client+0x73CB0: ECX = unit, EDX = skillId, stack (arg) ret 4. dt = sgptDataTables words reached through the slot
// 0x6FB7E474: skillCount (+0xBA0), skillBase (+0xB98), ownerCount (+0xB6C). unit may be 0. unitFlags = &unit[0xC4/4] (null for a
// null unit). Returns 0 on a gate failure, else 1.
struct D2c73cb0Dt { int32_t skillCount; uint32_t skillBase; int32_t ownerCount; };
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_73cb0(D2cHost h, uint32_t unitTok, uint32_t* unitFlags, int32_t skillId, uint32_t arg, const D2c73cb0Dt* dt, uint32_t formulaWord /* *(row+0x114) */) {
    if (skillId < 0 || skillId >= dt->skillCount) return 0;
    uint32_t row = (uint32_t)skillId * 0x23Cu + dt->skillBase;
    if (row == 0) return 0;
    D2cCall o = { 0x40550, (uint32_t)skillId, 0, 0, 0, 0, { unitTok, 0, 0, 0 } };
    int32_t owner = (int32_t)h(&o);
    if (owner < 0 || owner >= dt->ownerCount) return 0;
    if (unitFlags) *unitFlags |= 0x40;
    D2cCall a = { 0xCB5C, 0, 0, 0, 0, 0, { (uint32_t)owner, arg, 0, 0 } };
    uint32_t r1 = h(&a);
    D2cCall b = { 0xC316, 0, 0, 0, 0, 0, { unitTok, formulaWord, (uint32_t)skillId, arg } };
    uint32_t r2 = h(&b);
    D2cCall c = { 0xA1020, unitTok, 0, 0, r1 + r2, 0, { unitTok, (uint32_t)owner, 0xFFFFFFFFu, (uint32_t)skillId } };
    h(&c);   // the fifth stack word is arg
    return 1;
}
