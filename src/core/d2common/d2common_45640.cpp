// 1.13c D2Common+0x45640
// Selector-keyed left shift of one dword: selectors 6..11 and 0xD8/0xD9 shift by 8, others leave it.
#include <cstdint>

extern "C" void D2Common_45640(int32_t nSelector, int32_t* pValue) {
    switch (nSelector) {
    case 6: case 7: case 8: case 9: case 10: case 11: case 0xD8: case 0xD9:
        *pValue = (int32_t)((uint32_t)*pValue << 8);
        break;
    default:
        break;
    }
}
