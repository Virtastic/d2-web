// 1.13c Storm+0x22550 -- decoder-state constructor: run the base init (Storm+0x22240), then zero the
// first dword of each of 128 12-byte entries at +0x3474. Returns state.
#include <cstdint>
void* storm_sub_22550(void* state, void (*init)(void* state)) {
    init(state);
    uint32_t* p = (uint32_t*)((uint8_t*)state + 0x3474);
    for (int i = 0; i < 0x80; i++, p += 3) *p = 0;
    return state;
}
