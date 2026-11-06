// 1.13c D2Net+0x381E ___initmbctable: the one-shot latch at 0xB7B4. When it is
// clear, __setmbcp(-3) (the ANSI codepage) runs and the latch is set to 1; the result is always 0.
#include <cstdint>
namespace d2net_381e {
// 1.13c D2Net+0x381E
int initmbctable(uint32_t& latch, int (*setmbcp)(int)) {
    if (latch == 0) {
        setmbcp(-3);
        latch = 1;
    }
    return 0;
}
}
