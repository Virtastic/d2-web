// 1.13c BNClient+0xC100 (ECX = game name filter, EDX = second string; two stack words, not read): ask for the game list.
// The payload is four dwords {0, 0, 0, 0x19} and three strings: the filter, the second string and an empty one, each
// with its terminator (a null pointer counts as the empty string; cap 0x100). The two answer slots of message 9 are
// cleared, the frame goes out with id 9, the answer is awaited and the list result word is returned.
#include "bnclient_send.h"
#include <cstring>

struct BnC100Globals { uint32_t* g3ed7c; uint32_t* g3eefc; const uint32_t* g3f888; };

// 1.13c BNClient+0xC100
extern "C" uint32_t bnclient_c100(const BnSendEnv* e, const BnC100Globals* g, const char* name, const char* second) {
    alignas(4) uint8_t pl[0x10 + 0x300];
    const uint32_t head[4] = { 0, 0, 0, 0x19 };
    memcpy(pl, head, 0x10);
    char* p = (char*)pl + 0x10;
    p += e->strcopy(p, name ? name : "", 0x100) + 1;
    p += e->strcopy(p, second ? second : "", 0x100) + 1;
    const uint32_t n3 = e->strcopy(p, "", 0x100);
    *g->g3ed7c = 0;
    *g->g3eefc = 0;
    e->send((uint32_t)(p - (char*)pl) + n3 + 1, 9, pl);
    e->wait(9);
    return *g->g3f888;
}
