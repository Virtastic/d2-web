// 1.13c D2Client+0x160A0: unit teardown tail (clears two fields; type 2 also calls D2Gfx#10029(0)).
#include <cstdint>
#include <cstddef>
void d2client_sub_160A0(uint8_t* p, void (*gfx10029)(uint32_t)) {                                  // 1.13c D2Client+0x160A0
    if (*(int32_t*)(p + 8) == 2) gfx10029(0);
    *(uint32_t*)(p + 0x18) = 0; *(uint32_t*)(p + 0x1C) = 0;
}
