// 1.13c D2Common+0x2DF80: add a delta to a context counter and to each level of its eight-id link (ESI = context, stack
// = delta; void). The context counter clamps at zero and is marked (+0x1D4 = 10). The ids come from 0x44CE0, called with the
// DRLG (+0x1B4) in EAX and the context's level id (+0x1D0); each nonzero id is found on the DRLG list or made by 0x2D9B0
// (ECX = DRLG, EDX = id) and gets the same update. A null ids seam (the production state until installed) updates the
// context only.
#include <cstdint>

using D2Common2DF80Ids = int32_t* (*)(uint32_t pDrlg, int32_t nLevelId);        // 0x44CE0
using D2Common2DF80Node = uint32_t (*)(uint32_t pDrlg, int32_t nLevelId);       // 0x2D9B0

namespace {
uint32_t& W2DF80(uint32_t a) { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); }
void Bump2DF80(uint32_t p, int32_t delta)
{
    const uint32_t v = W2DF80(p + 0x0C) + static_cast<uint32_t>(delta);
    W2DF80(p + 0x1D4) = 10;
    W2DF80(p + 0x0C) = static_cast<int32_t>(v) < 1 ? 0u : v;
}
}

extern "C" void D2Common_2DF80_AdjustRefs(uint32_t pContext, int32_t nDelta, D2Common2DF80Ids ids44, D2Common2DF80Node node2D9B0)
{
    Bump2DF80(pContext, nDelta);
    const uint32_t pDrlg = W2DF80(pContext + 0x1B4);
    if (!ids44) return;
    const int32_t* ids = ids44(pDrlg, static_cast<int32_t>(W2DF80(pContext + 0x1D0)));
    for (int i = 0; i < 8; ++i) {
        if (!ids[i]) continue;
        uint32_t p = W2DF80(pDrlg + 0x47C);
        while (p && static_cast<int32_t>(W2DF80(p + 0x1D0)) != ids[i]) p = W2DF80(p + 0x1AC);
        if (!p) p = node2D9B0(pDrlg, ids[i]);
        Bump2DF80(p, nDelta);
    }
}
