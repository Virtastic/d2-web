// Packet forwarder: copies a 40-byte packet into a local and hands the copy to its handler as the only stack argument.
#include <cstdint>
#include <cstring>
struct D2cAD6B0Env {
    void (*handle)(void* ctx, const uint8_t* copy);
    void* ctx;
};
extern "C" void d2c_ad6b0(const uint8_t* pkt, const D2cAD6B0Env* e) {
    uint8_t copy[40];
    std::memcpy(copy, pkt, 40);
    e->handle(e->ctx, copy);
}
