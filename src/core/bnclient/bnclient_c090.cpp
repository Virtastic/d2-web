// 1.13c BNClient+0xC090 (ECX = first string, EDX = second string): compose and send frame 0x22.
// The payload is the product tag ('D2DV', or 'D2XP' when the expansion runs), the dword 0xD and the two strings, each
// copied with a 0x100 cap and its terminator. The length is 8 + both strings with their terminators. Returns 1.
#include "bnclient_send.h"
#include <cstring>

// 1.13c BNClient+0xC090
extern "C" uint32_t bnclient_c090(const BnSendEnv* e, const char* first, const char* second) {
    alignas(4) uint8_t pl[8 + 0x200];
    const uint32_t product = 0x44324456u + (e->expansion() ? 0x13FAu : 0u), kind = 0xD;
    memcpy(pl, &product, 4);
    memcpy(pl + 4, &kind, 4);
    const uint32_t n1 = e->strcopy((char*)pl + 8, first, 0x100);
    const uint32_t n2 = e->strcopy((char*)pl + 8 + n1 + 1, second, 0x100);
    e->send(n1 + n2 + 10, 0x22, pl);
    return 1;
}
