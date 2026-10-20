// 1.13c D2Game+0x0C110 (sub_C110, 94 B, caller 0xC5D0), RE-DONE alpha lane, pinned against retail disassembly.
// pUnit arrives in ESI. True only for a monster (type 1) that is not dead (0xDFA90), whose class id
// (0x18A0, -1 when outside the class table) is neither 0x222 nor 0x227, that is not in state 0x6E
// (Ordinal 10494 via 0xA68E) and whose Ordinal 10830 (via 0xA514) is not 2.
#include <cstdint>

struct D2GameC110Env {
    int32_t (*isDead)(void* unit);                 // 0xDFA90
    int32_t (*classId)(void* unit);                // 0x18A0
    int32_t classCount;                            // *(*sgptDataTables + 0xA80)
    int32_t (*testState)(void* unit, int32_t st);  // 0xA68E -> Ordinal 10494
    int32_t (*ordinal10830)(void* unit);           // 0xA514
};

bool d2_fn_d2game_c110(void* unit, const D2GameC110Env* env)
{
    if (!unit || *(const int32_t*)unit != 1) return false;
    if (env->isDead(unit)) return false;
    int32_t id = env->classId(unit);
    if (id < 0 || id >= env->classCount) id = -1;
    if (id == 0x222 || id == 0x227) return false;
    if (env->testState(unit, 0x6E)) return false;
    return env->ordinal10830(unit) != 2;
}
