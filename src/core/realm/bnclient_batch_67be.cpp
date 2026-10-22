// 1.13c BNClient+0x067BE (9 B): the __unlock(0xD) thunk -- tail-call the lock release at 0x30AE with lock 0xD.
#include <cstdint>
#include <emscripten.h>
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_unlock_d(void (*unlock)(int32_t num)) { unlock(0xd); }
