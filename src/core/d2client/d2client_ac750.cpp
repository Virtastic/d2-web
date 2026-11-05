// Packet forwarder: copies a 26-byte packet into a local and hands the copy to the packet handler with a zero first argument.
#include <cstdint>
#include <cstring>
struct D2cAC750Env {
    void (*handle)(void* ctx, uint32_t zero, const uint8_t* copy);
    void* ctx;
};
extern "C" void d2c_ac750(const uint8_t* pkt, const D2cAC750Env* e) {
    uint8_t copy[28];
    std::memcpy(copy, pkt, 26);
    e->handle(e->ctx, 0, copy);
}
