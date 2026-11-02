// d2game+0xac940 sub_AC940: probe. fastcall ecx = int a[5] (blob), edx = n; the port takes the table by address.
#include <emscripten.h>
#include <cstdint>
int32_t D2GAME_AC940(const int32_t* a, int32_t n);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ac940(uint32_t* r) {
    r[4] = (uint32_t)D2GAME_AC940((const int32_t*)(uintptr_t)r[0], (int32_t)r[1]);
}
