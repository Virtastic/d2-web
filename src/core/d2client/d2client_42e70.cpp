// 1.13c D2Client+0x42E70 -- button-down handler of the centre-bottom button (96 wide around the screen's middle,
// 32 high ending 0x50 above the bottom edge): clears the release flag; inside the box it raises the pressed word,
// outside it drops the four button latches. The message is finished.
#include "cl_pa4.h"
namespace pa4 {
bool inCentreButton(const cl::Env& e) {
    const int32_t lo = (int32_t)G(e, 0xdbc48) / 2 - 0x30;
    const int32_t bottom = (int32_t)(G(e, 0xdbc4c) - 0x50);
    const int32_t x = (int32_t)G(e, 0x11b828);
    if (x < lo || x > lo + 0x60) return false;
    const int32_t y = (int32_t)G(e, 0x11b824);
    return y >= bottom - 0x20 && y <= bottom;
}
}  // namespace pa4
extern "C" uint32_t d2c_42e70(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    S(e, 0x11c410, 0);
    if (inCentreButton(e)) S(e, 0x11c38c, 1);
    else clearButtons(e);
    return fin(e, msg);
}
