// 1.13c Storm+0x225B0 -- scoped-decoder wrapper: construct a 0x3A80-byte decoder state on the stack, build the bit
// reader over the compressed stream (first dword is the initial bit buffer, 32 bits valid, the u16 words follow),
// decode up to *size bytes into dst, store the produced count, destruct.
#include <cstdint>
struct Storm225b0Bits { const uint16_t* p; uint32_t bits; uint32_t cnt; };   // ESI of Storm+0x22420
struct Storm225b0Env {
    void (*ctor)(void* state);                                                              // Storm+0x22550
    int32_t (*decode)(void* state, Storm225b0Bits* reader, uint8_t* dst, int32_t cap);      // Storm+0x22420
    void (*dtor)(void* state);                                                              // Storm+0x4910
};
void storm_sub_225b0(const Storm225b0Env& e, uint8_t* dst, int32_t* size, const uint32_t* stream) {
    alignas(8) uint8_t state[0x3A80];
    Storm225b0Bits rd = { reinterpret_cast<const uint16_t*>(stream + 1), stream[0], 0x20 };
    e.ctor(state);
    *size = e.decode(state, &rd, dst, *size);
    e.dtor(state);
}
