// 1.13c D2Common+0x2D5B0: room id at a point (regs: EAX = DRLG, EDI = x, ESI = y). Walks the DRLG level list (+0x47C,
// next +0x1AC) for the first level whose box (+0x1C/+0x20 origin, +0x24/+0x28 size) holds the point and returns its id
// (+0x1D0), or 0 when none does.
#include <cstdint>

extern "C" uint32_t D2Common_2D5B0_LookupRoomIdAt(uint32_t drlg, int32_t x, int32_t y)
{
    auto W = [](uint32_t a) -> int32_t& { return *reinterpret_cast<int32_t*>(static_cast<uintptr_t>(a)); };
    uint32_t node = static_cast<uint32_t>(W(drlg + 0x47C));
    while (node) {
        const int32_t ox = W(node + 0x1C), oy = W(node + 0x20);
        if (ox <= x && oy <= y && x < W(node + 0x24) + ox && y < W(node + 0x28) + oy)
            return static_cast<uint32_t>(W(node + 0x1D0));
        node = static_cast<uint32_t>(W(node + 0x1AC));
    }
    return 0;
}
