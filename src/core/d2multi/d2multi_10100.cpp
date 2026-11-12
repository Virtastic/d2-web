// 1.13c D2Multi+0x10100 (94 B): entry of the chat callback table.
// Calls slot +0x1C of the handler table (ECX = the second stack word, EDX = 1, one stack word 0), runs 0xBE00, then
// hands the four control handles (the module dwords at 0x39154, 0x39150, 0x39158, 0x3915C, in that order) to the
// thunk 0x7C1A with EDX = 0 and clears the dword at 0x39F20. Returns 1.
#include "d2multi_env.h"
#include <cstring>

// table: the handler table the module dword 0x3917C points at; h: the four dwords at 0x39150; flag: the dword at 0x39F20.
uint32_t d2multi_10100(const D2mEnv& e, uint32_t a, uint32_t b, const uint32_t* table, const uint32_t* h, uint32_t* flag) {
    (void)a;
    d2m_call(e, table[7] & 0xFFFFFF, 0, b, 1, 0, 0, 0);
    d2m_call(e, 0xbe00);
    d2m_call(e, 0x7c1a, 0, h[1], 0);
    d2m_call(e, 0x7c1a, 0, h[0], 0);
    d2m_call(e, 0x7c1a, 0, h[2], 0);
    d2m_call(e, 0x7c1a, 0, h[3], 0);
    *flag = 0;
    return 1;
}
