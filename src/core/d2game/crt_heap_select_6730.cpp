// 1.13c D2Game+0x6730 (26 B, caller 0x674A __heap_init): ___heap_select. OS platform word
// DAT_6fd2fd24 == 2 (VER_PLATFORM_WIN32_NT) and major version DAT_6fd2fd30 > 4 -> OS heap (1); else
// small-block heap (3). The two OS words are arguments (same convention as bn_heap_select).
#include <cstdint>
#include <emscripten.h>

extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2game_heap_select(uint32_t os_platform, uint32_t win_major) {
    return (os_platform == 2 && win_major > 4) ? 1 : 3;
}
