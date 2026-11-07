// 1.13c D2CMP+0x620B (_wctomb, VS2003 CRT): cdecl (mbch, wch). Retail: ptd = __getptd(); locale = ptd->_tfpecode
// (+0x64); if locale differs from the module ptlocinfo global (0x3276c) it calls ___updatetlocinfo (0x67FE), a seam
// here outside the vector set; then ___wctomb_mt(locale, mbch, wch).
#include <cstdint>

extern "C" int32_t d2cmp_61ab(uint32_t locale, char* mbch, uint16_t wch);

// 1.13c D2CMP+0x620B
extern "C" int32_t d2cmp_620b(char* mbch, uint16_t wch, uint32_t tfpecode, uint32_t global) {
    if (tfpecode != global) return -1; // ___updatetlocinfo seam: not reproduced, not vectored
    return d2cmp_61ab(tfpecode, mbch, wch);
}
