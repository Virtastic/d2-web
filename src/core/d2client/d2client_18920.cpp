// 1.13c D2Client+0x18920: ESI = unit; position (path +0xC/+0x10 or the 0xC3BE / 0xC2EC thunks), room (0xC1F0) and 0xCFFA(unit) go to the 0xC66A import
#include "cl17.h"
extern "C" uint32_t d2c_18920(uint32_t unit, const cl::Env& e) {
    uint32_t x = cl17::posX(e, unit), y = cl17::posY(e, unit);
    uint32_t room = e.H(0xc1f0, unit);
    uint32_t k = e.H(0xcffa, unit);
    return e.H(0xc66a, room, x, y, k);
}
