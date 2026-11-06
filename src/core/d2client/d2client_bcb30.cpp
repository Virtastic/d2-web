// 1.13c D2Client+0xBCB30: ESI = event record (u16 +0xC x, u16 +0xE y, dword +0x18 set to 1 on a hit). gx = dword at D2Client+0xDBC48, gy = +0xDBC4C, g8 = +0x11B864.
// 0xD0AE() == 2 and 0x26320(EAX = rec) == 0, x <= (gx - (gx >> 31)) >> 1 (signed), y < gy - 0x2F (signed): when g8 == 6 call 0xBEB90, set rec+0x18 = 1,
// call 0xBE84(rec) and return 1; any other case returns 0.
#include <cstdint>
extern "C" uint32_t d2c_port_bcb30(uint8_t* rec, int32_t gx, int32_t gy, int32_t g8, uint32_t (*cd0ae)(), uint32_t (*f26320)(uint32_t),
                                   void (*beb90)(), void (*cbe84)(uint32_t)) {
    if (cd0ae() != 2) return 0;
    if (f26320((uint32_t)(uintptr_t)rec) != 0) return 0;
    int32_t half = (gx - (gx >> 31)) >> 1;
    if ((int32_t)*(const uint16_t*)(rec + 0xC) > half) return 0;
    if ((int32_t)*(const uint16_t*)(rec + 0xE) >= gy - 0x2F) return 0;
    if (g8 == 6) beb90();
    *(uint32_t*)(rec + 0x18) = 1;
    cbe84((uint32_t)(uintptr_t)rec);
    return 1;
}
