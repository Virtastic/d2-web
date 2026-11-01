// 1.13c D2Common+0x382F0: copy the entries of a stat list whose high
// word is nType, starting at the first entry of the (nType << 16) key, at most nLimit of them, 8 bytes
// each. The array is the list's +0x24 array when the flag dword at +0x10 is >= 0, else its +0x48 array.
// Returns the number of entries copied (the 1.13c register result; the decompile shows void).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" int32_t d2_port_d2common_38220(intptr_t pTable, int32_t key);

namespace {
inline uint32_t Rd32(const uint8_t* p, uint32_t off)
{
    uint32_t v;
    std::memcpy(&v, p + off, 4);
    return v;
}
}  // namespace

extern "C" {
EMSCRIPTEN_KEEPALIVE int32_t d2_port_d2common_382f0(uint32_t pList, uint32_t pOut, int32_t nType, int32_t nLimit)
{
    if (!pList)
        return 0;
    uint8_t* list = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(pList));
    uint8_t* arr = list + ((static_cast<int32_t>(Rd32(list, 0x10)) < 0) ? 0x48 : 0x24);

    const int32_t idx = d2_port_d2common_38220(reinterpret_cast<intptr_t>(arr),
                                               static_cast<int32_t>(static_cast<uint32_t>(nType) << 16));
    if (idx < 0)
        return 0;

    const uint8_t* entries = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(Rd32(arr, 0)));
    int16_t count16;
    std::memcpy(&count16, arr + 4, 2);
    const int32_t n = count16;
    uint8_t* out = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(pOut));

    int32_t copied = 0;
    for (int32_t i = idx; i < n; ++i) {
        const uint8_t* e = entries + static_cast<uint32_t>(i) * 8;
        if (((Rd32(e, 0) >> 16) & 0xffffu) != static_cast<uint32_t>(nType))
            break;
        if (copied >= nLimit)
            break;
        std::memcpy(out + static_cast<uint32_t>(copied) * 8, e, 8);
        ++copied;
    }
    return copied;
}
}
