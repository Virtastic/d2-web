// 1.13c D2Multi+0x106A0 (235 B): entry of the chat callback table.
// Splits the name (first stack word) with 0xEEA0 into a prefix and an account part. With an account part the full name
// (prefix + account, format 0x17708 through sprintf 0x1000 when the prefix is not empty, else a copy) is the stack word
// of the 0x10000 call, else 0. EDI of that call is the text after the first '*' of the name when that is not empty,
// else the name; EBX is the second stack word. Returns 1.
#include "d2multi_env.h"
#include <cstring>

uint32_t d2multi_106a0(const D2mEnv& e, uint32_t name, uint32_t b) {
    char prefix[256], account[256], full[256];
    d2m_call(e, 0xeea0, name, 0, 0, 0, 0, d2m_ptr(prefix), d2m_ptr(account));
    uint32_t arg = 0;
    if (account[0] != 0) {
        if (prefix[0] != 0) d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(full), D2M_MOD(0x17708), d2m_ptr(prefix), d2m_ptr(account));
        else strcpy(full, account);
        arg = d2m_ptr(full);
    }
    const char* star = strchr((const char*)(uintptr_t)name, '*');
    uint32_t key = (star != nullptr && star[1] != 0) ? d2m_ptr(star + 1) : name;
    d2m_call(e, 0x10000, 0, 0, 0, b, 0, arg);
    d2m_edi(e, 0x10000, key);
    return 1;
}
