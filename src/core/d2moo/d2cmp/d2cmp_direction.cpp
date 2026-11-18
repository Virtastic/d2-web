// Prior art used: D2MOO's D2CMP/src/CelCmp.cpp is an empty module map.
// Rules checked against cached 1.13c decompiles; immutable direction maps
// checked against D2CMP.dll .data RVAs 0x1fcd8..0x1fd07.
#include <cstdint>
#include <unistd.h>

// 1.13c D2CMP+0x121C0
extern "C" uint8_t d2cmp_121c0(uint32_t direction, uint32_t sourceCount,
                              int32_t destinationCount, int32_t alternate) {
    if (direction >= sourceCount) _exit(-1);
    static constexpr uint8_t ordinary8[] = {1,1,3,3,5,5,7,7};
    static constexpr uint8_t alternate8[] = {3,3,5,5,7,7,1,1};
    static constexpr uint8_t ordinary16[] = {0,0,2,2,4,4,6,6,8,8,10,10,12,12,14,14};
    static constexpr uint8_t alternate16[] = {2,2,4,4,6,6,8,8,10,10,12,12,14,14,0,0};
    if (sourceCount == 8 && destinationCount == 4)
        return alternate ? alternate8[direction] : ordinary8[direction];
    if (sourceCount == 16 && destinationCount == 8)
        return alternate ? alternate16[direction] : ordinary16[direction];
    return 0;
}

// 1.13c D2CMP+0x12190
extern "C" uint32_t d2cmp_12190(int32_t sourceCount, int32_t destinationCount) {
    return sourceCount == 8 && destinationCount == 16;
}
