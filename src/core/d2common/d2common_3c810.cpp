// 1.13c D2Common+0x3C810 (#10043): stdcall(a, b), ret 8. Each unit's list is *(*(unit + 0x10) + 0x58) (null at any step is 0). When the two
// lists differ: a non-null first list gets 0x2DF80(list, -1) and a non-null second list gets 0x2DF80(list, +1) (ESI = list, one stack word, ret 4),
// a seam. The first call runs before the second; the second list is never compared again.
#include <cstdint>
#include <cstring>

struct D2Common3C810Env { void (*adjust)(uint32_t list, int32_t delta); };

namespace {
inline uint32_t Rd(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
inline uint32_t ListOf(uint32_t unit) {
    if (unit == 0) return 0;
    const uint32_t d = Rd(unit + 0x10);
    return d == 0 ? 0 : Rd(d + 0x58);
}
}

extern "C" void D2Common_3C810_Raw(uint32_t a, uint32_t b, const D2Common3C810Env* env)
{
    const uint32_t la = ListOf(a), lb = ListOf(b);
    if (la == lb) return;
    if (la != 0) env->adjust(la, -1);
    if (lb != 0) env->adjust(lb, 1);
}
