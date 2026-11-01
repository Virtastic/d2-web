// 1.13c D2Common+0x44CE0
// Vis-id array of a level: the DRLG's override chain (drlg +0x90, node +0 level id, +4 the array, +0x44 next) is searched
// first, then the level-defs row (stride 0x9C, array at +0x48) when the id is in range; otherwise the bare offset 0x48
// (retail's null-based pointer). A chain node with level id 0 is fatal in retail. Retail links are 32-bit tokens, so the
// result is a token and `env->resolve` turns a link into an address.
#include <cstdint>
#include <cstring>

struct D2Common44CE0Env {
    const uint8_t* (*resolve)(uint32_t);
    int32_t nLevelCount;     // sgptDataTables + 0xC5C
    uint32_t pLevelRows;     // token of the level-defs rows (global 0xA0B38)
};

static uint32_t Rd32_44CE0(const uint8_t* p, uint32_t off) { uint32_t v; std::memcpy(&v, p + off, 4); return v; }

extern "C" uint32_t D2Common_44CE0(uint32_t pDrlg, int32_t nLevelId, const D2Common44CE0Env* env)
{
    for (uint32_t node = Rd32_44CE0(env->resolve(pDrlg), 0x90); node; ) {
        const uint8_t* p = env->resolve(node);
        const int32_t id = static_cast<int32_t>(Rd32_44CE0(p, 0));
        if (!id) __builtin_trap();
        if (id == nLevelId) return node + 4;
        node = Rd32_44CE0(p, 0x44);
    }
    if (nLevelId < 0 || nLevelId >= env->nLevelCount) return 0x48;
    return env->pLevelRows + static_cast<uint32_t>(nLevelId) * 0x9C + 0x48;
}
