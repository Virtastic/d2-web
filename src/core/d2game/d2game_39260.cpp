// D2Game damage accumulation by element, in the retail 32-bit layout.
#include <cstdint>
#include <cstring>

static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

struct D2Game39260Env {
    uint32_t (*elemType)(uint32_t v);   // D2Common #10557 (thunk 0xB25E), stdcall 1 arg; the low byte is used
};

// 1.13c D2Game+0x39260
// EAX = source (null: nothing), ESI = damage block, EDI = amount. The low byte of #10557([EAX+4]) picks
// the field: 0 +8, 1 +0x10, 2 +0x1C, 3 +0x20, 4 +0x24, 5 +0x28, 6 +0x38, 7 +0x3C, 8 +0x40, 9 +0x44,
// 0xB +0x14 get the amount added; 0xC stores it at +0x24; 0xA and anything above 0xC do nothing.
void DAMAGE_AddByElement_39260(uint32_t src, uint32_t dmg, uint32_t amount, const D2Game39260Env& env)
{
    if (src == 0) return;
    static const int8_t kOff[13] = { 0x08, 0x10, 0x1C, 0x20, 0x24, 0x28, 0x38, 0x3C, 0x40, 0x44, -1, 0x14, -2 };
    const uint32_t t = env.elemType(rd32(src + 4)) & 0xFF;
    if (t > 0xC || kOff[t] == -1) return;
    if (kOff[t] == -2) { wr32(dmg + 0x24, amount); return; }
    wr32(dmg + kOff[t], rd32(dmg + kOff[t]) + amount);
}
