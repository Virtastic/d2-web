// 1.13c BNClient+0xF370 (ECX = account name, EDX = password): start a logon with the scheme in use.
// Scheme 0 clears the two answer slots of message 0x3A and calls the hashed-password logon (0xC9E0, both strings on
// the stack); returns 1. Scheme 1 clears the two slots of message 0x54 and calls the proof logon (0x11E60, the name in
// ESI, the password on the stack); returns what it returns. Any other scheme returns 0.
#include "bnclient_send.h"

struct BnF370Globals { const uint32_t* g3f878; uint32_t* g3ee40; uint32_t* g3efc0; uint32_t* g3eea8; uint32_t* g3f028; };

// 1.13c BNClient+0xF370
extern "C" uint32_t bnclient_f370(const BnCallEnv& e, const BnF370Globals* g, uint32_t name, uint32_t password) {
    if (*g->g3f878 == 0) {
        *g->g3ee40 = 0;
        *g->g3efc0 = 0;
        bn_call(e, 0xc9e0, 0, 0, 0, 0, 0, name, password);
        return 1;
    }
    if (*g->g3f878 == 1) {
        *g->g3eea8 = 0;
        *g->g3f028 = 0;
        return bn_call(e, 0x11e60, 0, 0, 0, 0, name, password);
    }
    return 0;
}
