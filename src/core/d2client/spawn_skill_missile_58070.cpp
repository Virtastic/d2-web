// D2Client sub_58070: spawn a type-3 missile request for a skill, env-seam style (no direct DLL calls).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct SkillMslEnv {
    const uint8_t* dataTables;                            // *sgptDataTables
    int (*unitX)(const void*);                            // D2Client+0x1630
    int (*unitY)(const void*);                            // D2Client+0x1660
    uint32_t (*c316)(const void* u, uint32_t row138, int skillId, uint32_t arg); // thunk +0xC316
    void* (*curTarget)();                                 // D2Client+0xA5F70
    int (*c478)(uint32_t room, int x, int y, int z);      // thunk +0xC478
    int (*c31c)(uint32_t room);                           // thunk +0xC31C
    int (*c340)(uint32_t room);                           // thunk +0xC340
    void* (*create)(uint32_t* req);                       // D2Client+0xB30F0
    void (*c448)(void* msl, uint32_t arg);                // thunk +0xC448
};
static inline uint32_t rd32(const void* p, int o) { uint32_t v; memcpy(&v, (const uint8_t*)p + o, 4); return v; }
extern "C" {
// 1.13c D2Client+0x58070  __fastcall(unit, skillId, arg). Request (0x5C bytes, zeroed): +0 flags=3,
// +4 creator, +8 anchor, +0x10 missile id, +0x14/+0x18 x/y, +0x1C/+0x20 dx/dy, +0x2C skillId,
// +0x30 arg, +0x34 c316(...). `arg` (stack param) is also passed to c448 with the new missile.
EMSCRIPTEN_KEEPALIVE int d2c_58070(const SkillMslEnv* e, uint8_t* unit, int skillId, uint32_t arg) {
    const uint8_t* dt = e->dataTables;
    if (skillId < 0 || (int)rd32(dt, 0xba0) <= skillId) return 0;
    const uint8_t* row = (const uint8_t*)(uintptr_t)rd32(dt, 0xb98) + (uint32_t)skillId * 0x23c;
    if (!row) return 0;
    int id = (int16_t)(rd32(row, 0xea) & 0xffff);
    if (id < 0 || (int)rd32(dt, 0xb6c) <= id) return 0;
    if (!(rd32(dt, 0xb64) + (uint32_t)id * 0x1a4)) return 0;
    if (unit) { uint32_t f = rd32(unit, 0xc4) | 0x40; memcpy(unit + 0xc4, &f, 4); }
    uint32_t r[0x17] = {0};
    r[0] = 3; r[1] = r[2] = (uint32_t)(uintptr_t)unit; r[4] = id;
    r[5] = e->unitX(unit); r[6] = e->unitY(unit);
    r[0xb] = skillId; r[0xc] = arg;
    r[0xd] = e->c316(unit, rd32(row, 0x138), skillId, arg);
    uint32_t room = rd32(unit, 0x2c);
    int tgtY;
    uint8_t* t = (uint8_t*)e->curTarget();
    if (!t) {
        r[7] = e->c31c(room) - e->unitX(unit);
        tgtY = e->c340(room);
    } else {
        e->c478(room, e->unitX(t), e->unitY(t), 0);
        r[7] = e->unitX(t) - e->unitX(unit);
        tgtY = e->unitY(t);
    }
    r[8] = tgtY - e->unitY(unit);
    if (void* m = e->create(r)) e->c448(m, arg);
    return 1;
}
}
