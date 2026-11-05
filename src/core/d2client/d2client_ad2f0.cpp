// Packet forwarder: copies the 7-byte packet (dword, word, byte) into a local; with no active game it is fatal, otherwise
// the copy goes to the handler.
#include <cstdint>
#include <cstring>
struct D2cAD2F0Env {
    uint32_t (*active)(void* ctx);
    void (*fatal)(void* ctx, uint32_t msg);
    void (*handle)(void* ctx, const uint8_t* copy);
    void* ctx;
};
extern "C" void d2c_ad2f0(const uint8_t* pkt, const D2cAD2F0Env* e) {
    uint8_t copy[8];
    std::memcpy(copy, pkt, 7);
    if (!e->active(e->ctx)) e->fatal(e->ctx, 0xD21);
    e->handle(e->ctx, copy);
}
