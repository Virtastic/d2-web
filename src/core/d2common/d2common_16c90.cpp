// 1.13c D2Common+0x16C90 -- EAX = level, EDX = level id (ret). Find the level with that id in the DRLG's level
// list (pDrlg = level+0x1B4; head +0x47C, next +0x1AC, id +0x1D0), allocating it through 0x2D9B0 (ECX = pDrlg,
// EDX = id) when absent; then hang it on the caller's outdoor link list (level+0x14 -> +0x264) through 0x44F20
// (EAX = found; stack: list head, direction 0x2D470(level+0x1C, found+0x1C), found's type +0 == 2).
#include <cstdint>
#include <emscripten.h>
#include "d2common_16c90.h"

struct D2DrlgCoordStrc;
int DRLG_GetDirectionFromCoordinates(D2DrlgCoordStrc* pDrlgCoord1, D2DrlgCoordStrc* pDrlgCoord2);   // 1.13c D2Common+0x2D470

namespace {
inline uint32_t W(uint32_t a, uint32_t o) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a + o)); }
}

extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_16C90_LinkLevelById(uint32_t pLevel, int32_t nLevelId,
                                                                  const D2Common16C90Env* env) {   // 1.13c D2Common+0x16C90
    const uint32_t pDrlg = W(pLevel, 0x1B4);
    const uint32_t od = W(pLevel, 0x14);
    uint32_t found = W(pDrlg, 0x47C);
    while (found && int32_t(W(found, 0x1D0)) != nLevelId) found = W(found, 0x1AC);
    if (!found) found = env->allocLevel(pDrlg, nLevelId);
    const int32_t bPreset = W(found, 0) == 2;
    const int32_t nDir = DRLG_GetDirectionFromCoordinates(reinterpret_cast<D2DrlgCoordStrc*>(uintptr_t(pLevel + 0x1C)),
                                                          reinterpret_cast<D2DrlgCoordStrc*>(uintptr_t(found + 0x1C)));
    env->addOrth(found, od + 0x264, nDir, bPreset);
}
