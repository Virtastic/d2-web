// Packet forwarder: unpacks a mode byte (+1), a word (+2) and four dwords (+4, +8, +0xC, +0x10) and calls the handler with
// mode, the +8 target, a three-dword block {+0xC, +0x10, 0}, the +4 dword and the zero-extended +2 word.
#include <cstdint>
#include <cstring>
struct D2cAEBA0Env {
    void (*call)(void* ctx, uint32_t mode, uint32_t target, const uint32_t* block, uint32_t a, uint32_t b);
    void* ctx;
};
static uint32_t aeba0_rd32(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
extern "C" void d2c_aeba0(const uint8_t* pkt, const D2cAEBA0Env* e) {
    uint16_t w; std::memcpy(&w, pkt + 2, 2);
    const uint32_t block[3] = { aeba0_rd32(pkt + 0xC), aeba0_rd32(pkt + 0x10), 0 };
    e->call(e->ctx, pkt[1], aeba0_rd32(pkt + 8), block, aeba0_rd32(pkt + 4), w);
}
