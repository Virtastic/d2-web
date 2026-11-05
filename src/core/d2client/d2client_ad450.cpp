// Session packet switch: nothing when the connection id is -1; otherwise by the packet's first byte, 0xAF sets the
// session flag, 0xB0 clears it, 0xB3 goes to the fragment reassembler and 0xB4 to the typed dispatcher (others ignored).
#include <cstdint>
struct D2cAD450Env {
    uint32_t* flag;
    void (*fragment)(void* ctx, const uint8_t* pkt);
    void (*typed)(void* ctx, const uint8_t* pkt);
    void* ctx;
};
extern "C" void d2c_ad450(int32_t conn, const uint8_t* pkt, const D2cAD450Env* e) {
    if (conn == -1) return;
    switch (pkt[0]) {
        case 0xAF: *e->flag = 1; break;
        case 0xB0: *e->flag = 0; break;
        case 0xB3: e->fragment(e->ctx, pkt); break;
        case 0xB4: e->typed(e->ctx, pkt); break;
        default: break;
    }
}
