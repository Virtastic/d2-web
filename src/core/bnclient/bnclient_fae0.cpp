// 1.13c BNClient+0xFAE0 (no arguments): open the connection and start the receive thread.
// Logs the connecting line. When a connection is already up, or the socket setup (0xEE20) fails, returns 0. Otherwise
// it clears the closing flag and the pending-ad word, sleeps 10 ms, resets the two queues (0x13490, 0x10AE0), builds
// the connection object (0xA4E0: ESI = its slot, four stack words), stamps it with the tick count (0xABE0: EAX = the
// tick, ESI = the object), clears the e-mail flag and starts the receive thread; the thread handle is returned.
#include "bnclient_send.h"

struct BnFae0Host {
    void (*sleep)(uint32_t ms);
    uint32_t (*tick)();
    uint32_t (*thread)(uint32_t entry);   // starts the receive loop, returns the handle
};
struct BnFae0Globals {
    const uint32_t* g3f654; uint32_t* g3f660; uint32_t* g3f1f8; uint32_t* g3f668;
    const uint32_t* g3e0f8; const uint32_t* g3e11c; const uint32_t* g3f8e4;
};

// 1.13c BNClient+0xFAE0
extern "C" uint32_t bnclient_fae0(const BnCallEnv& e, const BnFae0Host* h, const BnFae0Globals* g) {
    bn_call(e, 0x17420, 0, 0, 0, 0, 0, BN_MOD(0x1ad14));
    if (*g->g3f654 != 0) return 0;
    if (bn_call(e, 0xee20) == 0) return 0;
    *g->g3f660 = 0;
    *g->g3f1f8 = 0;
    h->sleep(10);
    bn_call(e, 0x13490);
    bn_call(e, 0x10ae0);
    bn_call(e, 0xa4e0, 0, 0, 0, 0, BN_MOD(0x1f8e4), *g->g3e0f8, BN_MOD(0x1e0f4), *g->g3e11c, BN_MOD(0x1e0fc));
    const uint32_t now = h->tick();
    bn_call(e, 0xabe0, now, 0, 0, 0, *g->g3f8e4);
    *g->g3f668 = 0;
    return h->thread(BN_MOD(0xf9b0));
}
