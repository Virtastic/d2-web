// D2Client rva 0x884D0: "does unit U still owe the client its voice sample".
// The unit arrives in EDX (`test edx,edx` is the first instruction at 0x6FB384D0), ECX is never read.
// Both callees are injected; g = the pinned globals block (the list head at D2Client+0x11BF30 is g+0).
#include <cstddef>
#include <cstdint>

// The client sound-event list (head global D2Client+0x11BF30): node -> object -> unit.
struct D2Client884D0Node {
    uint32_t obj;                                        // [N+0x00] the sound-event record
    uint32_t next;                                       // [N+0x04] 0 ends the list
};
static_assert(offsetof(D2Client884D0Node, obj) == 0x00, "D2Client 0x884D0 node");
static_assert(offsetof(D2Client884D0Node, next) == 0x04, "D2Client 0x884D0 node");

struct D2Client884D0Obj {
    uint32_t unk00, unk04;
    uint32_t unit;                                       // [O+0x08] the unit the record belongs to
    uint32_t unk0C[8];                                   // [O+0x0C..0x2B]
    uint32_t c;                                          // [O+0x2C] pointer to the live sample handle
    uint32_t state;                                      // [O+0x30] 1 = the client is playing this unit's sample
};
static_assert(offsetof(D2Client884D0Obj, unit) == 0x08, "D2Client 0x884D0 object");
static_assert(offsetof(D2Client884D0Obj, c) == 0x2C, "D2Client 0x884D0 object");
static_assert(offsetof(D2Client884D0Obj, state) == 0x30, "D2Client 0x884D0 object");

struct D2Client884D0Callees {
    uint32_t (*isPlaying)(const D2Client884D0Obj* obj, uint32_t unit);   // D2Client+0x88080: [O+0x30] == 1 for this unit
    uint32_t (*soundPlaying)(uint32_t handle);                          // D2Sound ordinal 10054, thunk D2Client+0xD2B8
};

// 1.13c D2Client+0x884D0: 1 while U still owes the client its voice sample.
extern "C" uint32_t d2_client_884d0(uint8_t* g, uint32_t unit, const D2Client884D0Callees& k) {
    if (unit == 0) return 0;
    const uint32_t head = *(const uint32_t*)(g + 0x00);  // D2Client+0x11BF30
    for (const D2Client884D0Node* n = (const D2Client884D0Node*)(uintptr_t)head; n != nullptr;
         n = (const D2Client884D0Node*)(uintptr_t)n->next) {
        const D2Client884D0Obj* o = (const D2Client884D0Obj*)(uintptr_t)n->obj;
        if (o->unit != unit) continue;
        const uint32_t c = o->c;
        if (c == 0) return 0;                            // the C test precedes the state test
        if (k.isPlaying(o, unit) == 0) return 0;
        if (*(const uint32_t*)(uintptr_t)c == 0) return 0;
        if (k.soundPlaying(*(const uint32_t*)(uintptr_t)c) == 0) return 0;
        return 1;
    }
    return 0;
}
