// 1.13c D2Common+0x2D600: room touch walk (regs: EAX = owner, EDI = x, ESI = y; void). Walks the room chain of the owner
// (+0x10, next +0x24) for the first room whose tile box (+0x34/+0x38 origin, +0x3C/+0x40 size) holds the point. Retail
// exits through 0x2FD (null owner) and 0x2FE (no first room); the port reports those codes to onFatal and returns.
//
#include <cstdint>

extern "C" void D2Common_2D600_TouchRoomAt(uint32_t owner, int32_t x, int32_t y, void (*onFatal)(uint32_t code))
{
    auto W = [](uint32_t a) -> int32_t& { return *reinterpret_cast<int32_t*>(static_cast<uintptr_t>(a)); };
    if (!owner) { if (onFatal) onFatal(0x2FD); return; }
    uint32_t room = static_cast<uint32_t>(W(owner + 0x10));
    if (!room) { if (onFatal) onFatal(0x2FE); return; }
    for (;;) {
        const int32_t tx = W(room + 0x34), ty = W(room + 0x38);
        if (!(x < tx || y < ty || tx + W(room + 0x3C) <= x || ty + W(room + 0x40) <= y)) return;
        room = static_cast<uint32_t>(W(room + 0x24));
        if (!room) return;
    }
}
