// 1.13c D2CMP+0x1B10 (ESI = array {cap, count, data}): destroy each 12-byte list (0x1490), free the
// data block under "__AV__TSExplicitList_UCELLIST__", zero cap/count/data.
#include "d2cmp_guest.h"
using namespace d2cmpg;
// 1.13c D2CMP+0x1B10
extern "C" void d2cmp_1b10(uint8_t* m, uint32_t arr, const D2CmpGuestEnv* e) {
    for (uint32_t i = 0; i < rd(m, arr + 4); ++i) e->f1490(m, rd(m, arr + 8) + i * 0xc);
    uint32_t data = rd(m, arr + 8);
    if (data) e->free(m, data, kTagTsList, -2, 0);
    wr(m, arr, 0); wr(m, arr + 4, 0); wr(m, arr + 8, 0);
}
