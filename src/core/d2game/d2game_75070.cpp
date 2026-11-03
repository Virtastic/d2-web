// 1.13c D2Game+0x75070 -- PLRSTATE_InitRecord: initialise a 0x20-byte state record.
// ESI = record, EDI = unit-like handle, stack = index (ret 4). A signed index of 0x10 or more is a Fog assert (0x7B8). 0xA496 (stdcall 2: EDI, 0; the push ebx before it is only a save)
// runs first; the record is zeroed, +0 = index, +4 = EDI, and byte +0x15 is 0x65 when 0x74F20 (EAX = index, ECX = EDI) is nonzero, else 100.
#include <cstdint>
#include <cstring>

struct D2Game75070Env {
    void (*ord11103)(uint32_t edi, uint32_t zero);         // 0xA496
    uint32_t (*classState)(int32_t index, uint32_t edi);  // 0x74F20 (EAX = index, ECX = EDI)
    void (*fatal)(uint32_t code);
};

// 1.13c D2Game+0x75070
void D2Game_75070(uint8_t* rec, uint32_t edi, int32_t index, const D2Game75070Env& e)
{
    if (index >= 0x10) { e.fatal(0x7B8); return; }
    e.ord11103(edi, 0);
    std::memset(rec, 0, 0x20);
    std::memcpy(rec, &index, 4);
    std::memcpy(rec + 4, &edi, 4);
    rec[0x15] = e.classState(index, edi) != 0 ? 0x65 : 100;
}
