// D2Client FUN_6fada6c0: real function; the D2cA6C0Env form in clear_flag_2a6c0.cpp stays for old callers.
#include <cstdint>
#define W __attribute__((weak))
extern "C" {
W int32_t Ordinal_11090() __asm__("Ordinal_11090_6fada6c0");
W void FUN_6fb1a220();
W void FUN_6fada590(uint8_t* edi, int32_t* esi);   // D2Client+0x2A590
W void FUN_6fad4490(uint8_t* edi, int32_t zero);   // D2Client+0x24490

// 1.13c D2Client+0x2A6C0
// unaff_ESI = unit dword array, unaff_EDI = hidden register argument passed to the callees.
void FUN_6fada6c0(int32_t* unaff_ESI, uint8_t* unaff_EDI) {
    if (unaff_ESI) unaff_ESI[0x31] &= (int32_t)0xffffffbf;
    if (Ordinal_11090()) { FUN_6fb1a220(); FUN_6fada590(unaff_EDI, unaff_ESI); FUN_6fad4490(unaff_EDI, 0); }
}
}
