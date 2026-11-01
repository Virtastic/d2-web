// src/core/d2game/d2game_47950.cpp -- 1.13c D2Game.dll sub_47950 (region spawn tick), transcribed from the 0x47950 decompile and its disassembly.
// Register convention: ESI = region state, one stack word (popped by ret 4) that is also left in EDI for 0x47820. Callees on the Env:
// 0xA556 (stdcall, 1 word: esi) answers the class id, 0xA74E (stdcall, 1 word) answers its record, 0x47820 (ECX = esi, EDI = the stack word,
// 2 stack words: value, record; ret 8).
#include <cstdint>
#include <cstring>

struct D2Game47950Env {
    uint32_t (*ord10826)(void* esi);
    uint8_t* (*ord10014)(uint32_t id);
    void     (*sub47820)(void* esi, uint32_t edi, uint32_t value, uint8_t* rec);
};

// 1.13c D2Game+0x47950
extern "C" void D2Game_47950(void* esi, uint32_t stackWord, const D2Game47950Env* e) {
    uint8_t* st = (uint8_t*)esi;
    uint32_t lo, hi;
    std::memcpy(&lo, st + 0x6c, 4);
    std::memcpy(&hi, st + 0x70, 4);
    const uint64_t v = (uint64_t)lo * 0x6ac690c5ull + hi;
    std::memcpy(st + 0x6c, &v, 8);
    uint32_t gate;
    std::memcpy(&gate, st + 0x78, 4);
    if ((uint32_t)v & 0x7fff || gate) return;
    const uint32_t id = e->ord10826(esi);
    uint8_t* rec = e->ord10014(id);
    if (rec[0x2e] != 0 && rec + 0x1c != nullptr) e->sub47820(esi, stackWord, id, rec);   // 1.13c D2Game+0x47950
}
