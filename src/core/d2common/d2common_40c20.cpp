// 1.13c D2Common+0x40c20
// Offset of a value inside its band: band table rows of 5 dwords (type, min, max, offset, offset-for-class-0x75); 0x30/0x31 only.
#include <cstdint>
#include <cstring>

extern "C" int32_t D2Common_40C20(const uint8_t* pUnit, int32_t nType, int32_t nValue, const int32_t* pTable, int32_t nEntries) {
    // 1.13c: a type other than 0x30/0x31 or a value outside every band is a fatal exit (Ordinal_10024 + _exit): ours traps.
    if (nType != 0x30 && nType != 0x31) __builtin_trap();
    const int32_t* e = nullptr;
    for (int32_t i = 0; i < nEntries && !e; ++i) {
        const int32_t* row = pTable + i * 5;
        if (nType == row[0] && nValue >= row[1] && nValue <= row[2]) e = row;
    }
    if (!e) __builtin_trap();
    int32_t unitClass;
    std::memcpy(&unitClass, pUnit + 0x1D0, 4);
    return (unitClass == 0x75 ? e[4] : e[3]) - e[1] + nValue;
}
