// 1.13c D2CMP+0x116A0 (45 bytes): null-checked read of the dword at +0x10. Null is fatal
// (retail logs string 0x5A5 via Fog #10024, then _exit(-1)); the log is a seam, omitted here.
#include <cstdint>
#include <cstdlib>
#include <cstring>

// 1.13c D2CMP+0x116A0
extern "C" uint32_t d2cmp_sub_116a0(const uint8_t* p) {
    if (!p) _Exit(-1);
    uint32_t v; std::memcpy(&v, p + 0x10, 4);
    return v;
}
