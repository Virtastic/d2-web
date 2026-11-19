// 1.13c D2Client+0x143E0 seam for the platform layer: the outgoing-packet gate (duplicate window per opcode class). The rule is
// d2_client_143e0 (d2client_143e0.cpp); this file keeps the last-packet store and counters, and answers whether the packet goes out.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

struct D2Client143e0State { uint32_t lastTick; uint8_t last[0x200]; uint32_t bytes; uint32_t count; };
struct D2Client143e0Env {
    uint32_t now; D2Client143e0State* st; void* ctx;
    uint32_t (*send)(void* ctx, uint32_t len, const uint8_t* copy);
    void (*fatal)(void* ctx, uint32_t strId);
};
extern "C" void d2_client_143e0(const uint8_t* buf, int32_t len, const D2Client143e0Env* env);

static D2Client143e0State g_gate;
// A new game's input starts with an empty last-packet store (the retail globals are zero at load).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_send_gate_reset() { g_gate = D2Client143e0State{}; }
// 1 when the packet is sent (the caller transmits it), 0 when the window drops it. A length of 0x200 or more is fatal in retail.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_send_gate(const uint8_t* buf, int32_t len, uint32_t now) {
    int32_t sent = 0;
    const D2Client143e0Env e = { now, &g_gate, &sent,
        [](void* c, uint32_t, const uint8_t*) -> uint32_t { *static_cast<int32_t*>(c) = 1; return 1; },
        [](void*, uint32_t) { std::exit(-1); } };
    d2_client_143e0(buf, len, &e);
    return sent;
}
