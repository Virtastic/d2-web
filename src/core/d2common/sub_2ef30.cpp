// 1.13c D2Common+0x2EF30 (#10426)
#include <cstdint>
// Animation-clock "finished" predicate over resolved fields: with a sequence (pSeq, +0x30) it is frameCountPrecise (+0x48) < 1;
// without, frameCountPrecise <= (int16)speed (+0x4c) + framePrecise (+0x44). Null unit is fatal in retail (_exit).
bool D2Common_2EF30_AnimFinished(uint32_t pSeq, int32_t frameCount, int32_t frame, int16_t speed)
{
    if (pSeq) return frameCount < 1;
    return frameCount <= (int32_t)speed + frame;
}
