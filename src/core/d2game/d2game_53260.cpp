// 1.13c D2Game+0x53260: unit census for a non-null game (EAX). The four callees are D2Common/Fog
// ordinal thunks (0x2A6A6 #11017, 0x2A3DC #10460, 0x2A46C #11147, 0x2A4CC #10464), each taking ONE stack argument.
#include <cstdint>
struct D2Game53260Env {
    int32_t (*first)(uint32_t arg);      // 0x2A6A6, arg = the game
    int32_t (*head)(uint32_t arg);       // 0x2A3DC, arg = the game
    int32_t (*probe)(uint32_t node);     // 0x2A46C
    int32_t (*next)(uint32_t node);      // 0x2A4CC
};
// Returns one when first(game) is non-zero, plus one per list node whose probe is non-zero; the walk is head(game), next(node)...
// 1.13c D2Game+0x53260
extern "C" uint16_t d2game_53260(uint32_t game, const D2Game53260Env* env)
{
    if (game == 0) return 0;
    uint32_t n = env->first(game) != 0;
    for (uint32_t node = (uint32_t)env->head(game); node != 0;) {
        const int32_t hit = env->probe(node);
        node = (uint32_t)env->next(node);
        if (hit != 0) n++;
    }
    return (uint16_t)n;
}
