// D2sound batch b70. Most rows skipped (x87/SEH/CRT/Win32); see hand-back.
#include <initializer_list>
#include <cstdint>

extern "C" {
extern __attribute__((weak)) int32_t DAT_6f9c6054;
extern __attribute__((weak)) int32_t DAT_6f9c6058;
extern __attribute__((weak)) void (*DAT_6f9c62cc)(void);
__attribute__((weak)) void FUN_6f9b260c(int32_t msg);
}

// 1.13c D2sound+0x2784  (__FF_MSGBANNER)
extern "C" void D2sound_FF_MSGBANNER_2784(void)
{
    if (DAT_6f9c6054 == 1 || (DAT_6f9c6054 == 0 && DAT_6f9c6058 == 1)) {
        FUN_6f9b260c(0xfc);
        if (DAT_6f9c62cc) DAT_6f9c62cc();
        FUN_6f9b260c(0xff);
    }
}
