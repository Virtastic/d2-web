// 1.13c D2Common+0x2E790: map a path to its 14-pair table entry (ESI = path; plain ret).
// Reads x at +0, y at +4, the level at +0x40 when x is 0 or 1 and +0x30 is set (else +0x10), the
// aux token at +0x60 (saved before normalize), normalizes when bit 3 of +0xC8 is set, then resolves
// through the injected resolver and scans the 14 (value, key) pairs of the table for the key.
// The resolver and normalizer are callees (0x43860, 0x501E0); the caller injects them.
#include <cstdint>
#include <cstring>

namespace {
uint32_t Read32(const void* p, uint32_t off) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + off, 4); return v; }
int32_t Signed32(const void* p, uint32_t off) { return static_cast<int32_t>(Read32(p, off)); }
}

struct D2Common2E790Env {
    void* (*resolve)(uint32_t token);
};

// 1.13c D2Common+0x2E790. mapping points at the 14 (value, key) dword pairs of 0x9CFA0.
extern "C" int32_t D2Common_2E790_PathMapping(void* path, const D2Common2E790Env* env,
    const int32_t (*mapping)[2],
    void (*normalize)(void*, int32_t*, int32_t*, int32_t*),
    void (*resolve)(void*, int32_t, int32_t, int32_t, void*, int32_t*)) // 1.13c D2Common+0x2E790
{
    int32_t x = Signed32(path, 0);
    int32_t y = Signed32(path, 4);
    const uint32_t auxToken = Read32(path, 0x60);
    void* auxiliary = auxToken ? env->resolve(auxToken) : nullptr;
    int32_t level = Signed32(path, ((x == 0 || x == 1) && Read32(path, 0x30)) ? 0x40 : 0x10);
    if ((Read32(path, 0xC8) >> 3) & 1)
        normalize(path, &x, &y, &level);
    resolve(path, x, y, level, auxiliary, &x);
    for (int32_t i = 0; i < 14; ++i)
        if (x == mapping[i][1])
            return mapping[i][0];
    return -1;
}
