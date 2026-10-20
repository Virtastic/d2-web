// src/core/d2gfx/d2gfx_sub_5399.cpp -- 1.13c D2Gfx codepage->locale-id leaf, reimplemented from the
// cached decompile (ghidra-d2gfx/decomp/0x5399.c). Alpha lane (OBJECTIVE v8.3): implementation + marker.
#include <cstdint>

// 1.13c D2Gfx+0x05399 (47 B): map a Windows codepage to its CRT locale id.
// 932 (Shift-JIS) -> 0x411 (Japanese), 936 (GBK) -> 0x804 (Chinese PRC), 949 (Korean) -> 0x412,
// 950 (Big5) -> 0x404 (Chinese Taiwan); every other codepage maps to 0 (the ANSI/C locale).
// Called only by __setmbcp (0x558C); the codepage arrives in EAX in retail, so it is the sole
// host argument here (the batch-b45 convention for register-only inputs).
extern "C"
uint32_t d2gfx_sub_5399(uint32_t codepage) {
    switch (codepage) {
    case 0x3A4: return 0x411;
    case 0x3A8: return 0x804;
    case 0x3B5: return 0x412;
    case 0x3B6: return 0x404;
    default:    return 0;
    }
}
