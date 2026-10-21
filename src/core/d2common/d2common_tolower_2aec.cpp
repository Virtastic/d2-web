#include <cstdint>

// 1.13c D2Common+0x02AEC (34 B): _tolower. Fetch the thread data block (+0x2A24 callee path), refresh
// locale info when it is not the process locale, then fold through ___tolower_mt. Seams are the
// retail callees; locale refresh is the caller's job, so we receive the resolved locinfo.
extern "C" uint32_t D2Common_Tolower_mt_2a24(const uint8_t* loc, uint32_t c,
                                             int32_t (*)(const uint8_t*, uint32_t, int32_t),
                                             int32_t (*)(uint32_t, uint32_t, const char*, int32_t,
                                                         uint8_t*, int32_t, uint32_t, int32_t));
typedef int32_t (*IsctypeFn)(const uint8_t*, uint32_t, int32_t);
typedef int32_t (*LcMapFn)(uint32_t, uint32_t, const char*, int32_t, uint8_t*, int32_t, uint32_t, int32_t);

extern "C" int D2Common_Tolower_2aec(const uint8_t* loc, int c, IsctypeFn isctype, LcMapFn lcmap)
{
    return (int)D2Common_Tolower_mt_2a24(loc, (uint32_t)c, isctype, lcmap);
}
