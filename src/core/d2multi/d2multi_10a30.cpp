// 1.13c D2Multi+0x10A30 (112 B): entry of the chat callback table.
// Walks the channel user list from the head in DAT_6fa09ef4 (next at +0xB0) for the row whose name (+4) equals the string in the first stack word, byte for byte. Found: calls 0x10410 with ECX = EDX = the incoming ECX, ESI = the row, one stack word = the name (EAX holds the second stack word at the call), and returns 1. Not found, or an empty list: returns 0. ret 8.
#include "d2multi_env.h"
#include <cstring>

uint32_t d2multi_10a30(const D2mEnv& e, uint32_t ecx, uint32_t name, uint32_t b, uint32_t head) {
    for (uint32_t n = head; n != 0; memcpy(&n, (const void*)(uintptr_t)(n + 0xb0), 4)) {
        if (strcmp((const char*)(uintptr_t)(n + 4), (const char*)(uintptr_t)name) != 0) continue;
        d2m_call(e, 0x10410, b, ecx, ecx, 0, n, name);   // EAX still holds the second stack word
        return 1;
    }
    return 0;
}
