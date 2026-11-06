// 1.13c D2Multi+0x1A65 rand: _holdrand = _holdrand * 0x343FD + 0x269EC3; return (_holdrand >> 16) & 0x7FFF.
#include <cstdint>
struct D2MultiRandEnv { uint8_t* (*getptd)(); };
extern "C" int d2multi_rand_env(const D2MultiRandEnv* env) {
    uint32_t* h = (uint32_t*)(env->getptd() + 0x14);
    *h = *h * 0x343fdu + 0x269ec3u;
    return (int)((*h >> 16) & 0x7fff);
}
