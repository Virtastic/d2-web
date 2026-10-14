#include <D2Game/GAME/Game.h>

// 1.13c D2Game+0x2A210 (#10017): one store to the DLL's init-seed global (DAT 0x6FD2E354).
int32_t gnGameInitSeed;

void __stdcall GAME_SetInitSeed(int32_t nInitSeed)
{
    gnGameInitSeed = nInitSeed;
}

// Replay hook for s3208: set, then read the global back.
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE int d2_game_set_init_seed(int v) { GAME_SetInitSeed(v); return gnGameInitSeed; }
