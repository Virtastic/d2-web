// Level orth node: EAX = level, three stack words (head pointer, direction, preset; callee pops them). A 0x18-byte node is allocated from the DRLG's pool (level +0x1B4 ->
// drlg, drlg +0x478 -> pool; file DrlgRoom.cpp line 0x15F), zeroed and filled (+0 level, +4 direction, +8 preset, +0xC 0, +0x10 = level + 0x1C), then handed to the sorted
// insert 0x44B20 (EBX = node, stack = head); its EAX is the function's. D2Common_44F20 is the retail layout (32-bit tokens, `resolve`, `alloc` and `insert` seams);
// D2Common_44F20_Engine is the engine form on D2MOO structs.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include "Drlg/D2DrlgDrlg.h"

extern "C" void D2Common_44B20(D2DrlgOrthStrc** head, D2DrlgOrthStrc* node, bool (*before)(const D2DrlgOrthStrc*, const D2DrlgOrthStrc*));
extern "C" bool D2Common_44B20_Before(const D2DrlgOrthStrc* a, const D2DrlgOrthStrc* b);

// 1.13c D2Common+0x44F20
extern "C" uint32_t D2Common_44F20(uint32_t level, uint32_t head, int32_t direction, int32_t preset, uint8_t* (*resolve)(uint32_t),
                                   uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line), uint32_t (*insert)(uint32_t node, uint32_t head))
{
    auto rd = [](const uint8_t* b, uint32_t o) { uint32_t v; std::memcpy(&v, b + o, 4); return v; };
    auto wr = [](uint8_t* b, uint32_t o, uint32_t v) { std::memcpy(b + o, &v, 4); };
    const uint32_t pool = rd(resolve(rd(resolve(level), 0x1B4)), 0x478);
    const uint32_t node = alloc(pool, 0x18, 0x15F);
    uint8_t* n = resolve(node);
    std::memset(n, 0, 0x18);
    wr(n, 4, static_cast<uint32_t>(direction));
    wr(n, 0, level);
    wr(n, 8, static_cast<uint32_t>(preset));
    wr(n, 0xC, 0);
    wr(n, 0x10, level + 0x1C);
    return insert(node, head);
}

// 1.13c D2Common+0x44F20
extern "C" void D2Common_44F20_Engine(D2DrlgOrthStrc** head, D2DrlgLevelStrc* level,
    int32_t direction, int32_t preset)
{
    auto* node = D2_CALLOC_STRC_POOL(level->pDrlg->pMempool, D2DrlgOrthStrc);
    node->pLevel = level; std::memcpy(&node->nDirection, &direction, sizeof(direction));
    node->bPreset = preset; node->pBox = &level->pLevelCoords;
    D2Common_44B20(head, node, D2Common_44B20_Before);
}

