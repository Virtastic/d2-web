// 1.13c D2Client+0xAC090: ECX = packet. Copies packet bytes 1..37 into the 37-byte module record and raises its "received" flag.
// EAX is not written: the caller's value is what the function leaves.
#include <cstdint>
#include <cstring>
extern "C" uint32_t d2c_ac090(uint8_t* record, uint32_t* flag, const uint8_t* pkt, uint32_t eax) {
    std::memcpy(record, pkt + 1, 37);
    *flag = 1;
    return eax;
}
