// 1.13c D2Game+0x78BD (__NLG_Notify1). Records the register args in three
// module globals (RVA 0xFF9A4 = ecx, 0xFF9A0 = eax, 0xFF9A8 = ebp).
#include <cstdint>

struct D2GameNlgGlobals {
    uint32_t at_ff9a0; // eax
    uint32_t at_ff9a4; // param_1 (ecx)
    uint32_t at_ff9a8; // ebp
};

void D2Game_NLG_Notify1_78BD(D2GameNlgGlobals* g, uint32_t eax, uint32_t ecx, uint32_t ebp) {
    g->at_ff9a4 = ecx;
    g->at_ff9a0 = eax;
    g->at_ff9a8 = ebp;
}
