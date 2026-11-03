// 1.13c D2Game+0x715E0 -- the inventory pass of a unit save: pages 1..12 selected, type-9 chain entries refreshed, items saved.
// EAX = unit, ECX = game (plain ret). 0xA4C0(unit) (stdcall 1) must return the data block (fatal 0x87D); 0xA742(unit+0x60, page) (stdcall 2) for
// pages 1..12; block+0x5C must be non-null (fatal 0x885); every node on the chain at unit+0xDC (next at +0x24, read before the call) whose
// first byte is 9 goes through 0xAE6F0 (EAX = the NODE, not the unit); then 0x74810 (EAX = rec+4, ECX = game; stack unit, 0x60, 0x2000, 0, &out; ret 0x14).
// 0xA42A(unit, 0xE, 0) is the stat result; when it is positive (signed) and unit+0x10 is neither 0 nor 0x11, 0x21670 (EBX = 0xE, stdcall 1)
// takes -result, then rec+0x10. The vendored port passed the unit to AE6F0 and left EBX off; this follows the 1.13c disassembly.
#include <cstdint>
#include <cstring>

struct D2Game715E0Env {
    uint32_t (*unitData)(uint32_t unit);                                              // 0xA4C0
    void (*setPage)(uint32_t inv, uint32_t page);                                     // 0xA742
    void (*chainItem)(uint32_t node);                                                 // 0xAE6F0 (EAX = node)
    void (*saveItems)(uint32_t eax, uint32_t game, uint32_t unit, uint32_t a, uint32_t b, uint32_t c, uint32_t* out);  // 0x74810
    int32_t (*statResult)(uint32_t unit, uint32_t a, uint32_t b);                     // 0xA42A
    void (*adjust)(uint32_t ebx, uint32_t v);                                         // 0x21670 (EBX = 0xE)
    void (*fatal)(uint32_t code);
};

static inline uint32_t r32(uint32_t a, uint32_t o)
{
    uint32_t v;
    std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a) + o), 4);
    return v;
}

// 1.13c D2Game+0x715E0
void D2Game_715E0(uint32_t unit, uint32_t game, const D2Game715E0Env& e)
{
    const uint32_t data = e.unitData(unit);
    if (data == 0) { e.fatal(0x87D); return; }
    for (uint32_t page = 1; page < 0xD; ++page) e.setPage(r32(unit, 0x60), page);
    const uint32_t rec = r32(data, 0x5C);
    if (rec == 0) { e.fatal(0x885); return; }
    for (uint32_t node = r32(unit, 0xDC); node != 0;) {
        const uint32_t next = r32(node, 0x24);
        if (*reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(node)) == 9) e.chainItem(node);
        node = next;
    }
    uint32_t out = 0;
    e.saveItems(r32(rec, 4), game, unit, 0x60, 0x2000, 0, &out);
    const int32_t result = e.statResult(unit, 0xE, 0);
    const uint32_t kind = r32(unit, 0x10);
    if (result > 0 && kind != 0 && kind != 0x11) {
        e.adjust(0xE, (uint32_t)-result);
        e.adjust(0xE, r32(rec, 0x10));
    }
}
