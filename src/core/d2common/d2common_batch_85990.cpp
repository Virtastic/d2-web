// 1.13c D2Common+0x85990 (sub_85990): walk the unit's data's state list (+0x264) for the entry whose
// state-def (+0x00) has id 0x28 at +0x1D0; if found, call the +0x42A70 helper with a direction
// pair: entry kind (+0x04) == 3 gives (0, h-1, 0x16b), else (w-1, 0, 0x16a). Caller: +0x85E20.
// The unit is a retail-layout image addressed by its 32-bit address: unit +0x14 data, data +0x5C width,
// +0x60 height, +0x264 first node; node +0 def, +4 kind, +0x14 next. The callee is a seam (null: 0x42A70).
#include <cstdint>
#include <cstring>

typedef void (*B85990_Call)(void* pUnit, int32_t a, int32_t b, int32_t c, int32_t d, int32_t e);
extern "C" void D2Common_42A70(void* pUnit, int32_t a, int32_t b, int32_t c, int32_t d, int32_t e);

namespace {
    inline uint32_t ld(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a) + o), 4); return v; }
}

extern "C" void D2Common_85990(uint32_t pUnit, B85990_Call call)
{
    if (!call) call = D2Common_42A70;
    void* ctx = reinterpret_cast<void*>(static_cast<uintptr_t>(pUnit));
    const uint32_t d = ld(pUnit, 0x14);
    for (uint32_t n = ld(d, 0x264); n; n = ld(n, 0x14)) {
        if (static_cast<int32_t>(ld(ld(n, 0), 0x1D0)) != 0x28) continue;
        const int32_t w = static_cast<int32_t>(ld(d, 0x5C)), h = static_cast<int32_t>(ld(d, 0x60));
        if (ld(n, 4) == 3) call(ctx, 0, h - 1, 0x16b, -1, 0);
        else call(ctx, w - 1, 0, 0x16a, -1, 0);
        return;
    }
}
