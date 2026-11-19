// src/core/d2client/d2client_batch_595c0.cpp -- D2Client.dll 0x595C0, the first of the exit/disconnect-confirm state machine
// (0x595C0..0x59BC0). The other eleven rows of the range have their one body in d2client_<rva>.cpp, which their probes drive.
// Module state is one struct (retail RVA beside each field); unrecovered callees are host seams after the
// retail arguments. Callback arguments are retail RVAs.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

#include "d2client_595c0.h"

static void sendO(const D2cExitEnv* e, uint8_t op) {
    uint8_t p[8] = {'O', op, 0, 0, 0, 0, 0, 0};
    e->send(p);
}

// 1.13c D2Client+0x595C0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_595c0(D2cExitState* s, const D2cExitEnv* e) {
    sendO(e, 7);
    s->state = 3;
}
