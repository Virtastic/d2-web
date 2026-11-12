// 1.13c D2Multi+0x107A0: entry of the chat callback table.
// Splits the name (first stack word) with 0xEEA0 into a prefix and an account part. The shown name is prefix + account
// when there is an account part (format 0x17708 through sprintf 0x1000 when the prefix is not empty, else a copy),
// else the name itself. The line (format 0x17618: shown name, the second stack word) goes to 0xFC90 with colour 5.
// Returns 1.
#include "d2multi_env.h"
#include <cstring>

uint32_t d2multi_107a0(const D2mEnv& e, uint32_t name, uint32_t text) {
    char prefix[256], account[256], full[256], line[500];
    d2m_call(e, 0xeea0, name, 0, 0, 0, 0, d2m_ptr(prefix), d2m_ptr(account));
    uint32_t shown = name;
    if (account[0] != 0) {
        if (prefix[0] != 0) d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(full), D2M_MOD(0x17708), d2m_ptr(prefix), d2m_ptr(account));
        else strcpy(full, account);
        shown = d2m_ptr(full);
    }
    d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(line), D2M_MOD(0x17618), shown, text);
    d2m_call(e, 0xfc90, 0, 0, 0, 0, 0, d2m_ptr(line), 5);
    return 1;
}
