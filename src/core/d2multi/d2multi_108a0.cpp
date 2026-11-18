// 1.13c D2Multi+0x108A0 (384 B): entry of the chat callback table.
// Splits the name (first stack word) with 0xEEA0 into a prefix and an account part, then looks the name up in the
// channel user list (head in the module dword 0x39EF4, name at +4, next at +0xB0). The text colour is 4 and the extra
// word 0; a listed speaker with flag bit 2 (flags at +0x90) gets colour 0, else with bit 1 or bit 8 colour 3 and extra
// word 3. The speaker tag (format 0x17718) is built from prefix + account when there is an account part (format 0x17708
// when the prefix is not empty, else a copy), else from the name. 0xFBB0 shows tag, text, colour and extra. Returns 1.
#include "d2multi_env.h"
#include <cstring>

uint32_t d2multi_108a0(const D2mEnv& e, uint32_t name, uint32_t text, uint32_t head) {
    char prefix[256], account[256], full[256], tag[256];
    d2m_call(e, 0xeea0, name, 0, 0, 0, 0, d2m_ptr(prefix), d2m_ptr(account));
    uint32_t colour = 4, extra = 0;
    for (uint32_t n = head; n != 0; memcpy(&n, (const void*)(uintptr_t)(n + 0xb0), 4)) {
        if (strcmp((const char*)(uintptr_t)(n + 4), (const char*)(uintptr_t)name) != 0) continue;
        uint32_t flags;
        memcpy(&flags, (const void*)(uintptr_t)(n + 0x90), 4);
        if (flags & 2) colour = 0;
        else if ((flags & 1) || (flags & 8)) { colour = 3; extra = 3; }
        break;
    }
    if (account[0] == 0) {
        d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(tag), D2M_MOD(0x17718), name);
    } else {
        if (prefix[0] != 0) d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(full), D2M_MOD(0x17708), d2m_ptr(prefix), d2m_ptr(account));
        else strcpy(full, account);
        d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(tag), D2M_MOD(0x17718), d2m_ptr(full));
    }
    d2m_call(e, 0xfbb0, 0, 0, 0, 0, 0, d2m_ptr(tag), text, colour, extra);
    return 1;
}
