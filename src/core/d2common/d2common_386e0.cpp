// 1.13c D2Common+0x386E0: stdcall(unit, nType, out, nLimit). When the
// unit's stat list (unit+0x5C) exists and its flag dword at +0x10 is negative, the entries come from
// 0x382F0 (list in EAX, output in ECX, nType and nLimit on the stack); otherwise the result is 0.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" int32_t d2_port_d2common_382f0(uint32_t pList, uint32_t pOut, int32_t nType, int32_t nLimit);

extern "C" {
EMSCRIPTEN_KEEPALIVE int32_t d2_port_d2common_386e0(uint32_t pUnit, int32_t nType, uint32_t pOut, int32_t nLimit)
{
    if (!pUnit)
        return 0;
    const uint8_t* unit = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(pUnit));
    uint32_t list;
    std::memcpy(&list, unit + 0x5c, 4);
    if (!list)
        return 0;
    uint32_t flags;
    std::memcpy(&flags, reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(list)) + 0x10, 4);
    if (static_cast<int32_t>(flags) >= 0)
        return 0;
    return d2_port_d2common_382f0(list, pOut, nType, nLimit);
}
}
