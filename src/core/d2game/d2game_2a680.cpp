// 1.13c D2Game+0x2A680 (78 B): GAME_CountGameUnits. ESI = the game's 128-entry unit-hash bucket array; every
// non-empty bucket is a chain linked through the dword at node+0xE4 (null ends it); the result is the total node
// count. The walk's own "null node" assert (string 0xDF) sits inside a loop whose condition already excludes it.
#include <cstdint>
#include <cstring>

int32_t GAME_CountGameUnits_2A680(const void* pBuckets)
{
    int32_t nCount = 0;
    for (int i = 0; i < 0x80; ++i) {
        uint32_t node;
        std::memcpy(&node, static_cast<const uint8_t*>(pBuckets) + i * 4, 4);
        while (node != 0) {
            ++nCount;
            std::memcpy(&node, reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(node)) + 0xE4, 4);
        }
    }
    return nCount;
}
