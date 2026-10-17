// 1.13c D2Common+0x2EB30 (#10073)
#include <cstdint>
// stdcall(pUnit, value): for a type-0 unit, store value at pUnit->pData (+0x14) + 0x2C and return.
// A null unit asserts 0xFBF and any other unit type asserts 0xFC0, then _exit(-1) -- fatal, so there is
// no return to model and the probe supplies only type-0 units.
void D2Common_2EB30(uint32_t pUnit, uint32_t value)
{
    const uint32_t* u = (const uint32_t*)(uintptr_t)pUnit;
    if (!u || u[0] != 0) return;
    *(uint32_t*)(uintptr_t)(u[5] + 0x2C) = value;
}
