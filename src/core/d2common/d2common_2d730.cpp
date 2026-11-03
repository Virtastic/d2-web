// 1.13c D2Common+0x2D730: get or create the DRLG warp record of a level id (regs: EDI = DRLG, EBX = id; returns the record). The list
// head is DRLG+0x90 (next +0x44, id word 0). A missing id gets a new 0x48-byte record, filled from its levels.txt row
// (words 1..8 from row+0x48..+0x64, words 9..16 from row+0x68..+0x84) and pushed on the head. The alloc is Fog 10045
// (file, line, flags). An id outside the table reads a zero row here; retail reads from address 0x48 on that path (not captured).
//
#include <cstdint>

struct D2Common2D730Env {
    uint32_t levelsBase;                                                        // levels.txt rows (0x9C each)
    uint32_t levelCount;                                                        // the count in the data tables (+0xC5C)
    uint32_t (*alloc)(const char* file, uint32_t line, uint32_t flags);         // Fog 10045, returns the record
};

extern "C" uint32_t D2Common_2D730_GetOrCreateWarp(uint32_t drlg, int32_t id, const D2Common2D730Env* e)
{
    auto W = [](uint32_t a) -> uint32_t& { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); };
    for (uint32_t node = W(drlg + 0x90); node; node = W(node + 0x44))
        if (static_cast<int32_t>(W(node)) == id) return node;
    const bool inRange = id >= 0 && static_cast<uint32_t>(id) < e->levelCount;
    const uint32_t row = inRange ? e->levelsBase + static_cast<uint32_t>(id) * 0x9C : 0;
    const uint32_t rec = e->alloc("..\\Source\\D2Common\\DRLG\\Drlg.cpp", 0x3E0, 0);
    W(rec) = static_cast<uint32_t>(id);
    for (uint32_t k = 0; k < 8; ++k) {
        W(rec + 4 * (1 + k)) = row ? W(row + 0x48 + 4 * k) : 0;
        W(rec + 4 * (9 + k)) = row ? W(row + 0x68 + 4 * k) : 0;
    }
    W(rec + 0x44) = W(drlg + 0x90);
    W(drlg + 0x90) = rec;
    return rec;
}
