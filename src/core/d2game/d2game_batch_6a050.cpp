// Quest event dispatch by index: 1.13c D2Game+0x6A050 (124 bytes), called from the AI think and object
// code.
// Register inputs are explicit: EBX is the event index, ESI the think context (unit at ctx[1]), EDI the
// caller's EDI, which the 0xEDF20 dispatch reads through. wasm32 pointers are 4 bytes.
#include <cstdint>
#include <cstring>

namespace ordns_6a050 {
__attribute__((weak, import_module("env"), import_name("Ordinal_10331"))) uint32_t Ordinal_10331(uint32_t unit);
__attribute__((weak, import_module("env"), import_name("Ordinal_10826"))) uint32_t Ordinal_10826(uint32_t room);
__attribute__((weak, import_module("env"), import_name("Ordinal_10459"))) int8_t   Ordinal_10459(uint32_t levelId);
// 1.13c D2Game+0xEDF20: returns EAX (0 when the event is not accepted). Reads ESI and EDI.
__attribute__((weak, import_module("env"), import_name("FUN_6fd0df20"))) int32_t FUN_6fd0df20(uint32_t esi, uint32_t edi);
}

namespace {
using ordns_6a050::Ordinal_10331;
using ordns_6a050::Ordinal_10826;
using ordns_6a050::Ordinal_10459;
using ordns_6a050::FUN_6fd0df20;

inline uint32_t rd32(const void* p, uint32_t off) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + off, 4); return v; }
}

// 1.13c D2Game+0x6A050. `tables` is the value of the sgptDataTables global (the count of event rows is at
// +0xA80). An index outside [0, count) does nothing. Event 0xEA is gated on the unit's room level: when the
// unit has a room, its level id must pass 10459, or the dispatch is skipped. The dispatch itself is tried up
// to three times through 0xEDF20; the first non-zero result stops the sequence (the third result is unused).
extern "C" void D2GAME_QUEST_DispatchIndexedQuestEvent_6A050(int32_t index, const int32_t* esi, uint32_t edi,
                                                             const void* tables)
{
    if (index < 0 || static_cast<uint32_t>(index) >= rd32(tables, 0xA80)) return;
    uint32_t room = Ordinal_10331(static_cast<uint32_t>(esi[1]));
    if (index == 0xEA && room != 0) {
        uint32_t levelId = Ordinal_10826(room);
        if (static_cast<uint8_t>(Ordinal_10459(levelId)) == 0) return;
    }
    uint32_t esiU = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(esi));
    if (FUN_6fd0df20(esiU, edi) == 0 && FUN_6fd0df20(esiU, edi) == 0) FUN_6fd0df20(esiU, edi);
}
