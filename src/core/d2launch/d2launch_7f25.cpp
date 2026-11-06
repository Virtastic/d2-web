// src/core/d2launch/d2launch_7f25.cpp -- 1.13c D2Launch ___initmbctable.
#include <cstdint>

// 1.13c D2Launch+0x07F25 -- ___initmbctable: the latch DAT_6fa680d8 clear runs __setmbcp(-3) (0x7DD5, the ANSI code page)
// and sets the latch to 1; set, nothing. Always returns 0.
extern "C" int32_t d2launch_initmbctable(int32_t* flag, int32_t (*setmbcp)(int32_t)) {
    if (*flag == 0) {
        setmbcp(-3);
        *flag = 1;
    }
    return 0;
}
