// 1.13c D2Game+0x13230: register-arg leaf, EAX = value, ESI = floor, [esp+4] = percent (ret 4).
#include <cstdint>
int32_t GAME_PercentOfWithFloor(int32_t nValue, int32_t nFloor, int32_t nPercent) {   // 1.13c D2Game+0x13230
    int32_t nProduct = (int32_t)((uint32_t)nValue * (uint32_t)nPercent);   // imul r32: low 32 bits
    int32_t nQuot = (int32_t)((int64_t)nProduct / 100);                    // magic-number idiv, rounds toward zero
    return nQuot < nFloor ? nFloor : nQuot;
}
