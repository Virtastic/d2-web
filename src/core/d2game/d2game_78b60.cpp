// 1.13c D2Game+0x78B60 -- release a unit after handing its +0x60 item to the node found for the unit's owner id.
// Retail's entry: EAX low byte = a flag, stack = game, unit (ret 8). The flag byte is stored into the unit's stack slot (the stack word the decompile
// calls param_2), which 0xD2610 receives by address as an in/out word; its value afterwards is what 0xA7AE and 0xD57F0 get.
// id = 0xB042(unit); 0 returns without the release. game null is a Fog assert (0x2D3), as is 0xA7AE != 0 (0x4E9); neither returns.
// Node = bucket (id & 0x7F) of game+0x1720, chain at +0xE4, id at +0xC. With a node and a nonzero unit+0x60 handle h, and all of
// 0xA6C4(node) low byte == 0, 0xAB26(h, node) != 0, 0xD2610(EAX = node; unit, &slot, 0) != 0 true: 0xA7AE(h, slot) must return 0,
// then 0xD57F0(EAX = unit; game, id, slot) (ret 0xC). 0xA9BE(unit, 0) always closes a non-zero id. The vendored 1.10f port had
// the slot wrong (param_1 fed in and the slot never passed on); this follows the 1.13c disassembly.
#include <cstdint>
#include <cstring>

struct D2Game78B60Env {
    uint32_t (*ownerId)(uint32_t unit);                                    // 0xB042 (stdcall 1)
    uint32_t (*nodeState)(uint32_t node);                                  // 0xA6C4 (stdcall 1; low byte tested)
    uint32_t (*accept)(uint32_t h, uint32_t node);                         // 0xAB26 (stdcall 2)
    uint32_t (*resolve)(uint32_t node, uint32_t unit, uint32_t* slot, uint32_t zero);  // 0xD2610 (EAX = node; stdcall 3)
    uint32_t (*takeItem)(uint32_t h, uint32_t v);                          // 0xA7AE (stdcall 2)
    void (*apply)(uint32_t unit, uint32_t game, uint32_t id, uint32_t v);  // 0xD57F0 (EAX = unit; stdcall 3)
    void (*release)(uint32_t unit, uint32_t zero);                         // 0xA9BE (stdcall 2)
    void (*fatal)(uint32_t code);
};

static inline uint32_t r32(uint32_t a, uint32_t o)
{
    uint32_t v;
    std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a) + o), 4);
    return v;
}

// 1.13c D2Game+0x78B60
void D2Game_78B60(uint32_t eax, uint32_t game, uint32_t unit, const D2Game78B60Env& e)
{
    uint32_t slot = eax & 0xFF;
    const uint32_t id = e.ownerId(unit);
    if (id == 0) return;
    if (game == 0) { e.fatal(0x2D3); return; }
    uint32_t node = r32(game, 0x1720 + (id & 0x7F) * 4);
    while (node != 0 && r32(node, 0xC) != id) node = r32(node, 0xE4);
    const uint32_t h = r32(unit, 0x60);
    if (node != 0 && h != 0 && (uint8_t)e.nodeState(node) == 0 && e.accept(h, node) != 0 && e.resolve(node, unit, &slot, 0) != 0) {
        if (e.takeItem(h, slot) != 0) { e.fatal(0x4E9); return; }
        e.apply(unit, game, id, slot);
    }
    e.release(unit, 0);
}
