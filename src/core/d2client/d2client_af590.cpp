// D2GS 0x0D receiver: fastcall ECX = the dispatch unit, EDX = the 13-byte record.
// Builds {x, y, hit class} from the record, runs the type dispatcher 0x2A750 (ECX = action byte, EDX = unit,
// stack &record, 1; ret 8), then for a player record (byte +1 == 0) looks the id up in the client-unit list
// (0x9D630, EDX = id) and stores the life byte at +0x14 of the node it returns.
#include <cstdint>
typedef uint32_t u32;
struct D2cAf590Env {
    void (*dispatch)(u32 action, u32 unit, const u32* rec, u32 one);   // 0x2A750
    u32 (*lookup)(u32 id);                                             // 0x9D630
};
static inline u32 rd8(u32 a) { return *(const uint8_t*)(uintptr_t)a; }
static inline u32 rd16(u32 a) { return *(const uint16_t*)(uintptr_t)a; }
static inline u32 rd32(u32 a) { return *(const uint32_t*)(uintptr_t)a; }
// 1.13c D2Client+0xAF590
void d2c_af590(u32 unit, u32 pk, const D2cAf590Env* e) {
    u32 id = rd32(pk + 2);
    u32 rec[3] = { rd16(pk + 7), rd16(pk + 9), rd8(pk + 0xB) };
    e->dispatch(rd8(pk + 6), unit, rec, 1);
    if (rd8(pk + 1) != 0) return;
    u32 node = e->lookup(id);
    if (node) *(uint32_t*)(uintptr_t)(node + 0x14) = rd8(pk + 0xC);
}
