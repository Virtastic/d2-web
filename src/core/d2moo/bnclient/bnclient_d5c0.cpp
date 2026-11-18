// 1.13c BNClient+0xD5C0 (ECX = file name, EDX = a request id): ask for a file's time, frame 0x33.
// The payload is {the request id, 0, the name with its terminator (cap 0x104)}; the length is the name's + 9. Returns 1.
#include "bnclient_send.h"
#include <cstring>

// 1.13c BNClient+0xD5C0
extern "C" uint32_t bnclient_d5c0(const BnSendEnv* e, const char* name, uint32_t request) {
    alignas(4) uint8_t pl[8 + 0x104];
    const uint32_t zero = 0;
    memcpy(pl, &request, 4);
    memcpy(pl + 4, &zero, 4);
    const uint32_t n = e->strcopy((char*)pl + 8, name, 0x104);
    e->send(n + 9, 0x33, pl);
    return 1;
}
