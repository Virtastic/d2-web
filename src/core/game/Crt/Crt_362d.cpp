// 1.13c D2Common+0x362D __freeptd: free the calling thread's block (or the given one) and clear the slot. the 1.10f source has no twin.
#include <cstdint>

struct D2CommonFreePtdEnv {
    int32_t flsIndex;
    uint32_t (*flsGet)(int32_t);
    void (*flsSet)(int32_t, uint32_t);
    void (*freeFls)(uint32_t);
};

// 1.13c D2Common+0x362D
void d2common_freeptd_362d(const D2CommonFreePtdEnv& e, uint32_t ptd)
{
    if (e.flsIndex == -1) return;
    if (!ptd) ptd = e.flsGet(e.flsIndex);
    e.freeFls(ptd);
    e.flsSet(e.flsIndex, 0);
}
