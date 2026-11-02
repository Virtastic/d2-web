// probe for D2Game 0x3BED0 (src/core/d2game/d2game_batch_3abd0.cpp D2Game_3BED0): r[0]=ecx pGame, r[1]=edx missile, r[3]=dt blob, r[5]=hook log, r[6]=hook returns (rva, value pairs).
#include <emscripten.h>
#include <cstdint>
uint32_t D2Game_3BED0_probe(void*, void*, const uint32_t*, uint32_t*, const uint32_t*);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3bed0(uint32_t* r) {
    r[4] = D2Game_3BED0_probe((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], r, (uint32_t*)(uintptr_t)r[5], (const uint32_t*)(uintptr_t)r[6]);
}
