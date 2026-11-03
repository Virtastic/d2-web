// 1.13c Fog+0xEAF0 (ordinal 10165): stdcall(srv, key, a2, a3), ret 0x10. Loads ESI = srv (stack word 0), EAX = key (stack word 1) and calls Fog+0xE550.
// The last two words are popped and unused.
#include "fog_env.h"
extern "C" void fog_e550(const FogEnv* e, uint32_t key, uint32_t esi);
extern "C" void fog_sub_EAF0(const FogEnv* e, uint32_t srv, uint32_t key, uint32_t, uint32_t) {  // 1.13c Fog+0xEAF0
    fog_e550(e, key, srv);
}
