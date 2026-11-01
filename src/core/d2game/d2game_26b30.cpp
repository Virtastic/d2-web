// D2Game+0x26B30: leap landing test. EAX = skill id, EDI = target y (passed through to 0x1280),
// stack: unit, target x (ret 8). The skill must index a Skills.txt row; the unit must have a room
// (D2Common 10331); the room under (x, y) must exist (D2Game 0x1280: EAX room, EBX x, EDI y). A skill
// whose row byte +5 has bit 0 of gdwBitMasks clear may not land in town (D2Common 10057 on that room).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game26B30Env {
    uint32_t dataTables;                                        // *sgptDataTables
    uint32_t bitMasks;                                          // &gdwBitMasks[0]
    uint32_t (*getRoom)(uint32_t unit);                         // thunk 0xA478 (D2Common 10331)
    uint32_t (*findRoom)(uint32_t room, int32_t x, int32_t y);  // D2Game 0x1280
    uint32_t (*isTown)(uint32_t room);                          // thunk 0xA808 (D2Common 10057)
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline uint8_t rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }

// 1.13c D2Game+0x26B30
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_26b30(int32_t skillId, int32_t y, uint32_t unit, int32_t x,
                                                      const D2Game26B30Env* env)
{
    const uint32_t dt = env->dataTables;
    if (skillId < 0 || skillId >= (int32_t)rd32(dt, 0xba0)) return 0;
    const uint32_t rec = rd32(dt, 0xb98) + (uint32_t)skillId * 0x23c;
    if (!rec) return 0;
    const uint32_t room = env->getRoom(unit);
    if (!room) return 0;
    const uint32_t dest = env->findRoom(room, x, y);
    if (!dest) return 0;
    if ((rd8(env->bitMasks, 0) & rd8(rec, 5)) == 0 && env->isTown(dest)) return 0;
    return 1;
}
