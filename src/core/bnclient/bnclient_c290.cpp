// 1.13c BNClient+0xC290 (ECX = game name, EDX = password, stack: description, a dword): advertise a game, frame 0x1C.
// The payload is five dwords {1 when the password is not empty else 0, 0, the stack dword, 0, 0} and the three strings
// with their terminators (a null pointer counts as the empty string; cap 0x100 each). Returns 1.
#include "bnclient_send.h"
#include <cstring>

// 1.13c BNClient+0xC290
extern "C" uint32_t bnclient_c290(const BnSendEnv* e, const char* name, const char* password, const char* description, uint32_t word) {
    alignas(4) uint8_t pl[0x14 + 0x300];
    if (!name) name = "";
    if (!password) password = "";
    if (!description) description = "";
    const uint32_t head[5] = { *password ? 1u : 0u, 0, word, 0, 0 };
    memcpy(pl, head, 0x14);
    char* p = (char*)pl + 0x14;
    p += e->strcopy(p, name, 0x100) + 1;
    p += e->strcopy(p, password, 0x100) + 1;
    const uint32_t n3 = e->strcopy(p, description, 0x100);
    e->send((uint32_t)(p - (char*)pl) + n3 + 1, 0x1C, pl);
    return 1;
}
