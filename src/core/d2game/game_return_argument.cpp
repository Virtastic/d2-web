#include <D2Game/GAME/Game.h>

int32_t __stdcall GAME_ReturnArgument(int32_t value)
{
    // 1.13c D2Game+0x29AD0
    return value;
}

// Replay hook for s3206.
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE int d2_game_return_argument(int v) { return GAME_ReturnArgument(v); }
