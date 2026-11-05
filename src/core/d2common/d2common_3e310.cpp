// 1.13c D2Common+0x3E310: the target point of a following path. Reads the retail 32-bit record layout (path+0x58 target unit, unit+0x2c its dynamic path,
// +0x64 direction, path+0x68 speed byte). The cos/sin LUTs are Fog #10083/#10084 (injected). The null path / null target arms end in the retail _exit and trap here.
#include <cstdint>
#include <cstring>
namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
inline uint16_t rd16(uint32_t a) { uint16_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 2); return v; }
inline uint8_t rd8(uint32_t a) { return *reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }
inline uint16_t coord(uint32_t unit, uint32_t wordOff, uint32_t shortOff)
{
    const uint32_t type = rd32(unit), dp = rd32(unit + 0x2C);
    if (type == 2 || (type > 3 && type < 6)) return static_cast<uint16_t>(rd32(dp + wordOff));
    return dp ? rd16(dp + shortOff) : 0;
}
}
extern "C" void D2Common_3E310_TargetPointSeam(uint32_t path, uint32_t out, float (*cosLut)(int16_t), float (*sinLut)(int16_t))
{
    if (!path) __builtin_trap();
    const uint32_t unit = rd32(path + 0x58);
    if (!unit) __builtin_trap();
    uint16_t x = coord(unit, 0xC, 2), y = coord(unit, 0x10, 6);
    const uint32_t speed = rd8(path + 0x68);
    if (speed) {
        const uint32_t dp = rd32(unit + 0x2C);
        const uint32_t dir = dp ? rd8(dp + 0x64) : 0;
        const int16_t angle = static_cast<int16_t>(((dir - speed * 32u) & 0x3Fu) << 3);
        const double s = static_cast<double>(speed);
        x = static_cast<uint16_t>(x + static_cast<uint16_t>(static_cast<int64_t>(static_cast<double>(cosLut(angle)) * s)));
        y = static_cast<uint16_t>(y + static_cast<uint16_t>(static_cast<int64_t>(static_cast<double>(sinLut(angle)) * s)));
    }
    memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(out)), &x, 2);
    memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(out) + 2), &y, 2);
}
