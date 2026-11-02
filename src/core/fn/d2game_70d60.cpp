// d2game+0x70d60 sub_70D60: probe. thiscall this = record blob, [esp+4] = game. Only the guard paths are vectored (the
// 0xE03A0 lookup needs a live game), so the game argument is an opaque value the port never dereferences.
#include <emscripten.h>
#include <cstdint>
uint32_t D2GAME_70D60(uint8_t* self, void* p);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_70d60(uint32_t* r) {
    r[4] = D2GAME_70D60((uint8_t*)(uintptr_t)r[0], (void*)(uintptr_t)r[1]);
}
