// 0x606E0 (ECX = quest; plain ret, AL result): true at once when state byte +0xC != 5 and flag +9 != 0. Otherwise asks 0x33F30 (EAX = quest +4, ECX = quest +0x10)
// for a handler object; none gives 0. IsBadCodePtr(quest +0xF0) failing is the fatal path; else the handler's method at object +0xF0 is called with ECX = the object.
#include "d2game_q32.h"
using namespace q32;
constexpr uint32_t kIsBadCodePtr = 0xF8D14;
// 1.13c D2Game+0x606E0
uint32_t d2game_606E0_q32(const Call& X, uint32_t quest) {
    if (rd8(quest, 0xc) != 5 && rd8(quest, 9) != 0) return 1;
    uint32_t o = X(0x33F30, rd32(quest, 4), rd32(quest, 0x10));
    if (!o) return 0;
    if (X(kIsBadCodePtr, 0, 0, 0, 0, 0, rd32(quest, 0xf0))) { X(0xFFFF0003u); return 0; }
    return X(rd32(o, 0xf0), 0, o);
}
