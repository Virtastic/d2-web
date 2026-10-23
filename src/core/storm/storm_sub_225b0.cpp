// 1.13c Storm+0x225B0 -- scoped-decoder wrapper: construct a 0x3A80-byte decoder state on the stack,
// decode up to *size bytes into dst, store the produced count, destruct.
#include <cstdint>
struct Storm225b0Env {
    void (*ctor)(void* state);                                  // Storm+0x22550
    int32_t (*decode)(void* state, uint8_t* dst, int32_t cap);  // Storm+0x22420
    void (*dtor)(void* state);                                  // Storm+0x4910
};
void storm_sub_225b0(const Storm225b0Env& e, uint8_t* dst, int32_t* size) {
    alignas(8) uint8_t state[0x3A80];
    e.ctor(state);
    *size = e.decode(state, dst, *size);
    e.dtor(state);
}
