// 1.13c D2Multi+0x11480 (24 B): entry of the chat callback table.
// Hands its two stack words and the incoming ECX to 0x10D10(a, b, ecx) (stdcall, three words), which adds the user row to the channel list, and returns 1; ret 8.
#include "d2multi_env.h"
#include <cstring>

uint32_t d2multi_11480(const D2mEnv& e, uint32_t ecx, uint32_t a, uint32_t b) {
    d2m_call(e, 0x10d10, 0, 0, 0, 0, 0, a, b, ecx);
    return 1;
}
