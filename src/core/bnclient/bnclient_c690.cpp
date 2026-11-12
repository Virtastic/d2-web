// 1.13c BNClient+0xC690 (ECX = a flag): ask for the channel list, frame 0x0B.
// The payload is one dword: 1 when the expansion runs and the flag is zero, else 0. The two answer slots of message
// 0x0B are cleared, the frame goes out, the answer is awaited and the channel list result word is returned.
#include "bnclient_send.h"
#include <cstring>

struct BnC690Globals { uint32_t* g3ed84; uint32_t* g3ef04; const uint32_t* g3f880; };

// 1.13c BNClient+0xC690
extern "C" uint32_t bnclient_c690(const BnSendEnv* e, const BnC690Globals* g, uint32_t flag) {
    uint32_t word = 0;
    if (e->expansion() != 0 && flag == 0) word |= 1;
    *g->g3ed84 = 0;
    *g->g3ef04 = 0;
    e->send(4, 0x0B, (const uint8_t*)&word);
    e->wait(0x0B);
    return *g->g3f880;
}
