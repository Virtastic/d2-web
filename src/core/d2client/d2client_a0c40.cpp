// 1.13c D2Client+0xA0C40: walk a unit's skill list (unit+0xA8) and re-issue the skill-slot assignment for every entry whose
// skill record matches (class byte, hotkey slot). Register/stack arguments from the disassembly: EBX = unit, stack s0, s1, s2 (ret 0xC).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cA0c40Env {
    uint32_t (*ord10784)(uint32_t list);                                   // thunk C796: first node
    uint32_t (*ord10273)(uint32_t node);                                   // thunk C75A: next node
    int32_t  (*ord10601)(uint32_t node, uint32_t file, uint32_t line);     // thunk C42A: skill id of a node
    uint32_t (*f17f0)(int32_t id);                                         // EAX = id; pointer to the 0x...+2 slot record, or 0
    int32_t  (*ord10306)(uint32_t unit, uint32_t node, uint32_t one);      // thunk C718
    uint32_t (*ord11068)(int32_t id);                                      // thunk C70C
    int32_t  (*ord10304)(uint32_t node);                                   // thunk C7AE
    void     (*ord10886)(uint32_t unit, int32_t id, uint32_t s0);          // thunk C760
    uint32_t (*ord10708)(uint32_t unit, int32_t id, int32_t m1);           // thunk C4EA, ECX=unit EDX=id, stack -1
    void     (*ord10773)(uint32_t unit, int32_t id, uint32_t file, uint32_t line);  // thunk C706
    void     (*ord10418)(uint32_t unit);                                   // thunk C784
    void     (*fatal)(uint32_t code);
};
static uint32_t a0c_rd32(const uint8_t* p, size_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
extern "C" {
// dt = *sgptDataTables (+0xB98 skill rows, stride 0x23C, +0xBA0 count); unit = host pointer of the unit blob; ids are retail-side handles.
EMSCRIPTEN_KEEPALIVE void d2c_a0c40(const D2cA0c40Env* e, const uint8_t* dt, const uint8_t* unit, uint32_t unitH, uint32_t s0, uint32_t s1, uint32_t s2) {
    const uint32_t FILE_ = 0x6fb844b4;
    if (!unitH) { e->fatal(0x969); return; }
    if (!s0) { e->fatal(0x96a); return; }
    uint32_t list = a0c_rd32(unit, 0xa8);
    if (!list) { e->fatal(0x96c); return; }
    uint32_t cur = e->ord10784(list);
    while (cur) {
        uint32_t next = e->ord10273(cur);
        int32_t id = e->ord10601(cur, FILE_, 0x973);
        bool reload = false;   // retail's bad-id / null-record exits jump past the "cur = next" reload
        if (id >= 0 && id < (int32_t)a0c_rd32(dt, 0xba0)) {
            const uint8_t* rec = (const uint8_t*)(uintptr_t)a0c_rd32(dt, 0xb98) + (size_t)id * 0x23c;
            const uint8_t* c = (const uint8_t*)(uintptr_t)e->f17f0(id);
            if (c) {
                reload = true;
                if (e->ord10306(unitH, cur, 1) > 0 && e->ord11068(id) &&
                    (!s1 || (uint32_t)rec[0x1dc] == s1) &&
                    ((uint8_t)s2 == 4 || (int32_t)(int8_t)c[2] == (int32_t)(uint8_t)s2 + 1) &&
                    e->ord10304(cur) == -1) {
                    e->ord10886(unitH, id, s0);
                    uint32_t n = e->ord10708(unitH, id, -1);
                    if (n && e->ord10306(unitH, n, 1) == 0) e->ord10773(unitH, id, FILE_, 0x9a0);
                }
            }
        }
        if (reload) cur = next;
    }
    e->ord10418(unitH);
}
}
