#include <cstdint>

// 1.13c D2Game+0x38BB0 (#10029): clears one DLL byte global (DAT 0x6FD2DBE8); no arguments, no callers.
uint8_t gnGameFlag38BB0;

void __stdcall GAME_ClearFlag38BB0()
{
    gnGameFlag38BB0 = 0;
}

// Replay hook for s3215: preset the global, call, read it back.
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE int d2_game_clear_flag(int v) { gnGameFlag38BB0 = (uint8_t)v; GAME_ClearFlag38BB0(); return gnGameFlag38BB0; }
