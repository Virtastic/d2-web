// 1.13c D2Multi+0x1A58 srand: ptd->_holdrand (+0x14) = seed.
#include <cstdint>
struct D2MultiSrandEnv { uint8_t* (*getptd)(); };
// Returns the ptd pointer: retail leaves it in EAX and the recorded return word carries it.
extern "C" uint8_t* d2multi_srand_env(uint32_t seed, const D2MultiSrandEnv* env) {
    uint8_t* p = env->getptd(); *(uint32_t*)(p + 0x14) = seed; return p;
}
