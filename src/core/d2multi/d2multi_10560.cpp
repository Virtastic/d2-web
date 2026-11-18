// 1.13c D2Multi+0x10560 (300 B): entry of the chat callback table.
// Splits the name (first stack word) with 0xEEA0 into a prefix and an account part. With an account part the shown name
// is prefix + account (the two-string format 0x17708 through the module's sprintf 0x1000 when the prefix is not empty,
// else a plain copy); without one it is the text after the first '*' of the name when that is not empty, else the name.
// The whisper line (format 0x175D8: shown name, the second stack word) goes to 0xFC90 with colour 2. Returns 1.
#include "d2multi_env.h"
#include <cstring>

uint32_t d2multi_10560(const D2mEnv& e, uint32_t name, uint32_t text) {
    char prefix[256], account[256], shown[256], line[500];
    d2m_call(e, 0xeea0, name, 0, 0, 0, 0, d2m_ptr(prefix), d2m_ptr(account));
    if (account[0] != 0) {
        if (prefix[0] != 0) d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(shown), D2M_MOD(0x17708), d2m_ptr(prefix), d2m_ptr(account));
        else strcpy(shown, account);
        d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(line), D2M_MOD(0x175d8), d2m_ptr(shown), text);
    } else {
        const char* star = strchr((const char*)(uintptr_t)name, '*');
        uint32_t key = (star != nullptr && star[1] != 0) ? d2m_ptr(star + 1) : name;
        d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(line), D2M_MOD(0x175d8), key, text);
    }
    d2m_call(e, 0xfc90, 0, 0, 0, 0, 0, d2m_ptr(line), 2);
    return 1;
}
