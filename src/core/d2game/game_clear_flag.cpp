#include <cstdint>
#include <D2Game/GAME/Game.h>

// Replay hook for s3215 (1.13c D2Game+0x38BB0, body in the game tree: GAME_ClearFlag38BB0): preset the global, call, read it back.
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE int d2_game_clear_flag(int v) { gnGameFlag38BB0 = (uint8_t)v; GAME_ClearFlag38BB0(); return gnGameFlag38BB0; }
